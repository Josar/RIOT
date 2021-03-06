/*
 * Copyright (c) 2015-2017 Ken Bannister. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gcoap
 * @{
 *
 * @file
 * @brief       GNRC's implementation of CoAP protocol
 *
 * Runs a thread (_pid) to manage request/response messaging.
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 */

#include <errno.h>
#include "net/gcoap.h"
#include "random.h"
#include "thread.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* Internal functions */
static void *_event_loop(void *arg);
static void _listen(sock_udp_t *sock);
static ssize_t _well_known_core_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len);
static ssize_t _write_options(coap_pkt_t *pdu, uint8_t *buf, size_t len);
static size_t _handle_req(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                                         sock_udp_ep_t *remote);
static ssize_t _finish_pdu(coap_pkt_t *pdu, uint8_t *buf, size_t len);
static void _expire_request(gcoap_request_memo_t *memo);
static bool _endpoints_equal(const sock_udp_ep_t *ep1, const sock_udp_ep_t *ep2);
static void _find_req_memo(gcoap_request_memo_t **memo_ptr, coap_pkt_t *pdu,
                           const sock_udp_ep_t *remote);
static void _find_resource(coap_pkt_t *pdu, coap_resource_t **resource_ptr,
                                            gcoap_listener_t **listener_ptr);
static int _find_observer(sock_udp_ep_t **observer, sock_udp_ep_t *remote);
static int _find_obs_memo(gcoap_observe_memo_t **memo, sock_udp_ep_t *remote,
                                                       coap_pkt_t *pdu);
static void _find_obs_memo_resource(gcoap_observe_memo_t **memo,
                                   const coap_resource_t *resource);

/* Internal variables */
const coap_resource_t _default_resources[] = {
    { "/.well-known/core", COAP_GET, _well_known_core_handler },
};

static gcoap_listener_t _default_listener = {
    (coap_resource_t *)&_default_resources[0],
    sizeof(_default_resources) / sizeof(_default_resources[0]),
    NULL
};

static gcoap_state_t _coap_state = {
    .listeners   = &_default_listener,
};

static kernel_pid_t _pid = KERNEL_PID_UNDEF;
static char _msg_stack[GCOAP_STACK_SIZE];
static sock_udp_t _sock;


/* Event/Message loop for gcoap _pid thread. */
static void *_event_loop(void *arg)
{
    msg_t msg_rcvd, msg_queue[GCOAP_MSG_QUEUE_SIZE];
    (void)arg;

    msg_init_queue(msg_queue, GCOAP_MSG_QUEUE_SIZE);

    sock_udp_ep_t local;
    memset(&local, 0, sizeof(sock_udp_ep_t));
    local.family = AF_INET6;
    local.netif  = SOCK_ADDR_ANY_NETIF;
    local.port   = GCOAP_PORT;

    int res = sock_udp_create(&_sock, &local, NULL, 0);
    if (res < 0) {
        DEBUG("gcoap: cannot create sock: %d\n", res);
        return 0;
    }

    while(1) {
        res = msg_try_receive(&msg_rcvd);

        if (res > 0) {
            switch (msg_rcvd.type) {
                case GCOAP_MSG_TYPE_TIMEOUT:
                    _expire_request((gcoap_request_memo_t *)msg_rcvd.content.ptr);
                    break;
                case GCOAP_MSG_TYPE_INTR:
                    /* next _listen() timeout will account for open requests */
                    break;
                default:
                    break;
            }
        }

        _listen(&_sock);
    }

    return 0;
}

/* Listen for an incoming CoAP message. */
static void _listen(sock_udp_t *sock)
{
    coap_pkt_t pdu;
    uint8_t buf[GCOAP_PDU_BUF_SIZE];
    sock_udp_ep_t remote;
    gcoap_request_memo_t *memo = NULL;
    uint8_t open_reqs = gcoap_op_state();

    ssize_t res = sock_udp_recv(sock, buf, sizeof(buf),
                                open_reqs > 0 ? GCOAP_RECV_TIMEOUT : SOCK_NO_TIMEOUT,
                                &remote);
    if (res <= 0) {
#if ENABLE_DEBUG
        if (res < 0 && res != -ETIMEDOUT) {
            DEBUG("gcoap: udp recv failure: %d\n", res);
        }
#endif
        return;
    }

    res = coap_parse(&pdu, buf, res);
    if (res < 0) {
        DEBUG("gcoap: parse failure: %d\n", (int)res);
        /* If a response, can't clear memo, but it will timeout later. */
        return;
    }

    if (pdu.hdr->code == COAP_CODE_EMPTY) {
        DEBUG("gcoap: empty messages not handled yet\n");
        return;

    /* incoming request */
    } else if (coap_get_code_class(&pdu) == COAP_CLASS_REQ) {
        if (coap_get_type(&pdu) == COAP_TYPE_NON
                || coap_get_type(&pdu) == COAP_TYPE_CON) {
            size_t pdu_len = _handle_req(&pdu, buf, sizeof(buf), &remote);
            if (pdu_len > 0) {
                sock_udp_send(sock, buf, pdu_len, &remote);
            }
        }
        else {
            DEBUG("gcoap: illegal request type: %u\n", coap_get_type(&pdu));
            return;
        }
    }

    /* incoming response */
    else {
        _find_req_memo(&memo, &pdu, &remote);
        if (memo) {
            xtimer_remove(&memo->response_timer);
            memo->state = GCOAP_MEMO_RESP;
            memo->resp_handler(memo->state, &pdu, &remote);
            memo->state = GCOAP_MEMO_UNUSED;
        }
    }
}

/*
 * Main request handler: generates response PDU in the provided buffer.
 *
 * Caller must finish the PDU and send it.
 *
 * return length of response pdu, or < 0 if can't handle
 */
static size_t _handle_req(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                                         sock_udp_ep_t *remote)
{
    coap_resource_t *resource;
    gcoap_listener_t *listener;
    sock_udp_ep_t *observer    = NULL;
    gcoap_observe_memo_t *memo = NULL;
    gcoap_observe_memo_t *resource_memo = NULL;

    _find_resource(pdu, &resource, &listener);
    if (resource == NULL) {
        return gcoap_response(pdu, buf, len, COAP_CODE_PATH_NOT_FOUND);
    }
    else {
        /* used below to ensure a memo not already recorded for the resource */
        _find_obs_memo_resource(&resource_memo, resource);
    }

    if (coap_get_observe(pdu) == COAP_OBS_REGISTER) {
        int empty_slot = _find_obs_memo(&memo, remote, pdu);
        /* record observe memo */
        if (memo == NULL) {
            if (empty_slot >= 0 && resource_memo == NULL) {

                int obs_slot = _find_observer(&observer, remote);
                /* cache new observer */
                if (observer == NULL) {
                    if (obs_slot >= 0) {
                        observer = &_coap_state.observers[obs_slot];
                        memcpy(observer, remote, sizeof(sock_udp_ep_t));
                    } else {
                        DEBUG("gcoap: can't register observer\n");
                    }
                }
                if (observer != NULL) {
                    memo = &_coap_state.observe_memos[empty_slot];
                }
            }
            if (memo == NULL) {
                coap_clear_observe(pdu);
                DEBUG("gcoap: can't register observe memo\n");
            }
        }
        if (memo != NULL) {
            memo->observer  = observer;
            memo->resource  = resource;
            memo->token_len = coap_get_token_len(pdu);
            if (memo->token_len) {
                memcpy(&memo->token[0], pdu->token, memo->token_len);
            }
            DEBUG("gcoap: Registered observer for: %s\n", memo->resource->path);
            /* generate initial notification value */
            uint32_t now       = xtimer_now_usec();
            pdu->observe_value = (now >> GCOAP_OBS_TICK_EXPONENT) & 0xFFFFFF;
        }

    } else if (coap_get_observe(pdu) == COAP_OBS_DEREGISTER) {
        _find_obs_memo(&memo, remote, pdu);
        /* clear memo, and clear observer if no other memos */
        if (memo != NULL) {
            DEBUG("gcoap: Deregistering observer for: %s\n", memo->resource->path);
            memo->observer = NULL;
            memo           = NULL;
            _find_obs_memo(&memo, remote, NULL);
            if (memo == NULL) {
                _find_observer(&observer, remote);
                if (observer != NULL) {
                    observer->family = AF_UNSPEC;
                }
            }
        }
        coap_clear_observe(pdu);

    } else if (coap_has_observe(pdu)) {
        /* bogus request; don't respond */
        DEBUG("gcoap: Observe value unexpected: %" PRIu32 "\n", coap_get_observe(pdu));
        return -1;
    }

    ssize_t pdu_len = resource->handler(pdu, buf, len);
    if (pdu_len < 0) {
        pdu_len = gcoap_response(pdu, buf, len,
                                 COAP_CODE_INTERNAL_SERVER_ERROR);
    }
    return pdu_len;
}

/*
 * Searches listener registrations for the resource matching the path in a PDU.
 *
 * param[out] resource_ptr -- found resource
 * param[out] listener_ptr -- listener for found resource
 */
static void _find_resource(coap_pkt_t *pdu, coap_resource_t **resource_ptr,
                                            gcoap_listener_t **listener_ptr)
{
    unsigned method_flag = coap_method2flag(coap_get_code_detail(pdu));

    /* Find path for CoAP msg among listener resources and execute callback. */
    gcoap_listener_t *listener = _coap_state.listeners;
    while (listener) {
        coap_resource_t *resource = listener->resources;
        for (size_t i = 0; i < listener->resources_len; i++) {
            if (i) {
                resource++;
            }
            if (! (resource->methods & method_flag)) {
                continue;
            }

            int res = strcmp((char *)&pdu->url[0], resource->path);
            if (res > 0) {
                continue;
            }
            else if (res < 0) {
                /* resources expected in alphabetical order */
                break;
            }
            else {
                *resource_ptr = resource;
                *listener_ptr = listener;
                return;
            }
        }
        listener = listener->next;
    }
    /* resource not found */
    *resource_ptr = NULL;
    *listener_ptr = NULL;
}

/*
 * Finishes handling a PDU -- write options and reposition payload.
 *
 * Returns the size of the PDU within the buffer, or < 0 on error.
 */
static ssize_t _finish_pdu(coap_pkt_t *pdu, uint8_t *buf, size_t len)
{
    ssize_t hdr_len = _write_options(pdu, buf, len);
    DEBUG("gcoap: header length: %i\n", (int)hdr_len);

    if (hdr_len > 0) {
        /* move payload over unused space after options */
        if (pdu->payload_len) {
            memmove(buf + hdr_len, pdu->payload, pdu->payload_len);
        }

        return hdr_len + pdu->payload_len;
    }
    else {
        return -1;      /* generic failure code */
    }
}

/*
 * Finds the memo for an outstanding request within the _coap_state.open_reqs
 * array. Matches on remote endpoint and token.
 *
 * memo_ptr[out] -- Registered request memo, or NULL if not found
 * src_pdu[in] -- PDU for token to match
 * remote[in] -- Remote endpoint to match
 */
static void _find_req_memo(gcoap_request_memo_t **memo_ptr, coap_pkt_t *src_pdu,
                           const sock_udp_ep_t *remote)
{
    *memo_ptr = NULL;
    /* no need to initialize struct; we only care about buffer contents below */
    coap_pkt_t memo_pdu_data;
    coap_pkt_t *memo_pdu = &memo_pdu_data;
    unsigned cmplen      = coap_get_token_len(src_pdu);

    for (int i = 0; i < GCOAP_REQ_WAITING_MAX; i++) {
        if (_coap_state.open_reqs[i].state == GCOAP_MEMO_UNUSED)
            continue;

        gcoap_request_memo_t *memo = &_coap_state.open_reqs[i];
        if (memo->send_limit == GCOAP_SEND_LIMIT_NON) {
            memo_pdu->hdr = (coap_hdr_t *) &memo->msg.hdr_buf[0];
        }
        else {
            memo_pdu->hdr = (coap_hdr_t *) memo->msg.data.pdu_buf;
        }

        if (coap_get_token_len(memo_pdu) == cmplen) {
            memo_pdu->token = &memo_pdu->hdr->data[0];
            if ((memcmp(src_pdu->token, memo_pdu->token, cmplen) == 0)
                    && _endpoints_equal(&memo->remote_ep, remote)) {
                *memo_ptr = memo;
                break;
            }
        }
    }
}

/* Calls handler callback on receipt of a timeout message. */
static void _expire_request(gcoap_request_memo_t *memo)
{
    coap_pkt_t req;

    DEBUG("coap: received timeout message\n");
    if (memo->state == GCOAP_MEMO_WAIT) {
        memo->state = GCOAP_MEMO_TIMEOUT;
        /* Pass response to handler */
        if (memo->resp_handler) {
            req.hdr = (coap_hdr_t *)&memo->msg.hdr_buf[0];   /* for reference */
            memo->resp_handler(memo->state, &req, NULL);
        }
        memo->state = GCOAP_MEMO_UNUSED;
    }
    else {
        /* Response already handled; timeout must have fired while response */
        /* was in queue. */
    }
}

/*
 * Handler for /.well-known/core. Lists registered handlers, except for
 * /.well-known/core itself.
 */
static ssize_t _well_known_core_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len)
{
   /* write header */
    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    int plen = gcoap_get_resource_list(pdu->payload, (size_t)pdu->payload_len,
                                      COAP_FORMAT_LINK);
    /* response content */
    return gcoap_finish(pdu, (size_t)plen, COAP_FORMAT_LINK);
}

/*
 * Creates CoAP options and sets payload marker, if any.
 *
 * Returns length of header + options, or -EINVAL on illegal path.
 */
static ssize_t _write_options(coap_pkt_t *pdu, uint8_t *buf, size_t len)
{
    uint8_t last_optnum = 0;
    (void)len;

    uint8_t *bufpos = buf + coap_get_total_hdr_len(pdu);  /* position for write */

    /* Observe for notification or registration response */
    if (coap_get_code_class(pdu) == COAP_CLASS_SUCCESS && coap_has_observe(pdu)) {
        uint32_t nval  = htonl(pdu->observe_value);
        uint8_t *nbyte = (uint8_t *)&nval;
        unsigned i;
        /* find address of non-zero MSB; max 3 bytes */
        for (i = 1; i < 4; i++) {
            if (*(nbyte+i) > 0) {
                break;
            }
        }
        bufpos += coap_put_option(bufpos, last_optnum, COAP_OPT_OBSERVE,
                                                       nbyte+i, 4-i);
        last_optnum = COAP_OPT_OBSERVE;
    }

    /* Uri-Path for request */
    if (coap_get_code_class(pdu) == COAP_CLASS_REQ) {
        size_t url_len = strlen((char *)pdu->url);
        if (url_len) {
            if (pdu->url[0] != '/') {
                DEBUG("gcoap: _write_options: path does not start with '/'\n");
                return -EINVAL;
            }
            bufpos += coap_put_option_uri(bufpos, last_optnum, (char *)pdu->url,
                                          COAP_OPT_URI_PATH);
            last_optnum = COAP_OPT_URI_PATH;
        }
    }

    /* Content-Format */
    if (pdu->content_type != COAP_FORMAT_NONE) {
        bufpos += coap_put_option_ct(bufpos, last_optnum, pdu->content_type);
        last_optnum = COAP_OPT_CONTENT_FORMAT;
    }

    /* Uri-query for requests */
    if (coap_get_code_class(pdu) == COAP_CLASS_REQ) {
        bufpos += coap_put_option_uri(bufpos, last_optnum, (char *)pdu->qs,
                                      COAP_OPT_URI_QUERY);
        /* uncomment when further options are added below ... */
        /* last_optnum = COAP_OPT_URI_QUERY; */
    }

    /* write payload marker */
    if (pdu->payload_len) {
        *bufpos++ = GCOAP_PAYLOAD_MARKER;
    }
    return bufpos - buf;
}

static bool _endpoints_equal(const sock_udp_ep_t *ep1, const sock_udp_ep_t *ep2)
{
    if (ep1->family != ep2->family) {
        return false;
    }
    if (ep1->port != ep2->port) {
        return false;
    }

    switch (ep1->family) {
    case AF_INET6:
        return memcmp(&ep1->addr.ipv6[0], &ep2->addr.ipv6[0], 16) == 0;
    case AF_INET:
        return ep1->addr.ipv4_u32 == ep2->addr.ipv4_u32;
    }
    return false;
}

/*
 * Find registered observer for a remote address and port.
 *
 * observer[out] -- Registered observer, or NULL if not found
 * remote[in] -- Endpoint to match
 *
 * return Index of empty slot, suitable for registering new observer; or -1
 *        if no empty slots. Undefined if observer found.
 */
static int _find_observer(sock_udp_ep_t **observer, sock_udp_ep_t *remote)
{
    int empty_slot = -1;
    *observer      = NULL;
    for (unsigned i = 0; i < GCOAP_OBS_CLIENTS_MAX; i++) {

        if (_coap_state.observers[i].family == AF_UNSPEC) {
            empty_slot = i;
        }
        else if (_endpoints_equal(&_coap_state.observers[i], remote)) {
            *observer = &_coap_state.observers[i];
            break;
        }
    }
    return empty_slot;
}

/*
 * Find registered observe memo for a remote address and token.
 *
 * memo[out] -- Registered observe memo, or NULL if not found
 * remote[in] -- Endpoint for address to match
 * pdu[in] -- PDU for token to match, or NULL to match only on remote address
 *
 * return Index of empty slot, suitable for registering new memo; or -1 if no
 *        empty slots. Undefined if memo found.
 */
static int _find_obs_memo(gcoap_observe_memo_t **memo, sock_udp_ep_t *remote,
                                                       coap_pkt_t *pdu)
{
    int empty_slot = -1;
    *memo          = NULL;

    sock_udp_ep_t *remote_observer = NULL;
    _find_observer(&remote_observer, remote);

    for (unsigned i = 0; i < GCOAP_OBS_REGISTRATIONS_MAX; i++) {
        if (_coap_state.observe_memos[i].observer == NULL) {
            empty_slot = i;
            continue;
        }

        if (_coap_state.observe_memos[i].observer == remote_observer) {
            if (pdu == NULL) {
                *memo = &_coap_state.observe_memos[i];
                break;
            }

            if (_coap_state.observe_memos[i].token_len == coap_get_token_len(pdu)) {
                unsigned cmplen = _coap_state.observe_memos[i].token_len;
                if (cmplen &&
                        memcmp(&_coap_state.observe_memos[i].token[0], &pdu->token[0],
                                                                       cmplen) == 0) {
                    *memo = &_coap_state.observe_memos[i];
                    break;
                }
            }
        }
    }
    return empty_slot;
}

/*
 * Find registered observe memo for a resource.
 *
 * memo[out] -- Registered observe memo, or NULL if not found
 * resource[in] -- Resource to match
 */
static void _find_obs_memo_resource(gcoap_observe_memo_t **memo,
                                   const coap_resource_t *resource)
{
    *memo = NULL;
    for (int i = 0; i < GCOAP_OBS_REGISTRATIONS_MAX; i++) {
        if (_coap_state.observe_memos[i].observer != NULL
                && _coap_state.observe_memos[i].resource == resource) {
            *memo = &_coap_state.observe_memos[i];
            break;
        }
    }
}

/*
 * gcoap interface functions
 */

kernel_pid_t gcoap_init(void)
{
    if (_pid != KERNEL_PID_UNDEF) {
        return -EEXIST;
    }
    _pid = thread_create(_msg_stack, sizeof(_msg_stack), THREAD_PRIORITY_MAIN - 1,
                            THREAD_CREATE_STACKTEST, _event_loop, NULL, "coap");

    mutex_init(&_coap_state.lock);
    /* Blank lists so we know if an entry is available. */
    memset(&_coap_state.open_reqs[0], 0, sizeof(_coap_state.open_reqs));
    memset(&_coap_state.observers[0], 0, sizeof(_coap_state.observers));
    memset(&_coap_state.observe_memos[0], 0, sizeof(_coap_state.observe_memos));
    /* randomize initial value */
    atomic_init(&_coap_state.next_message_id, (unsigned)random_uint32());

    return _pid;
}

void gcoap_register_listener(gcoap_listener_t *listener)
{
    /* Add the listener to the end of the linked list. */
    gcoap_listener_t *_last = _coap_state.listeners;
    while (_last->next) {
        _last = _last->next;
    }

    listener->next = NULL;
    _last->next = listener;
}

int gcoap_req_init(coap_pkt_t *pdu, uint8_t *buf, size_t len, unsigned code,
                                                              char *path) {
    (void)len;

    pdu->hdr = (coap_hdr_t *)buf;
    memset(pdu->url, 0, NANOCOAP_URL_MAX);
    memset(pdu->qs, 0, NANOCOAP_QS_MAX);

    /* generate token */
#if GCOAP_TOKENLEN
    uint8_t token[GCOAP_TOKENLEN];
    for (size_t i = 0; i < GCOAP_TOKENLEN; i += 4) {
        uint32_t rand = random_uint32();
        memcpy(&token[i],
               &rand,
               (GCOAP_TOKENLEN - i >= 4) ? 4 : GCOAP_TOKENLEN - i);
    }
    uint16_t msgid = (uint16_t)atomic_fetch_add(&_coap_state.next_message_id, 1);
    ssize_t hdrlen = coap_build_hdr(pdu->hdr, COAP_TYPE_NON, &token[0], GCOAP_TOKENLEN,
                                    code, msgid);
#else
    uint16_t msgid = (uint16_t)atomic_fetch_add(&_coap_state.next_message_id, 1);
    ssize_t hdrlen = coap_build_hdr(pdu->hdr, COAP_TYPE_NON, NULL, GCOAP_TOKENLEN,
                                    code, msgid);
#endif

    if (hdrlen > 0) {
        /* Reserve some space between the header and payload to write options later */
        pdu->payload      = buf + coap_get_total_hdr_len(pdu) + strlen(path)
                                                              + GCOAP_REQ_OPTIONS_BUF;
        /* Payload length really zero at this point, but we set this to the available
         * length in the buffer. Allows us to reconstruct buffer length later. */
        pdu->payload_len  = len - (pdu->payload - buf);
        pdu->content_type = COAP_FORMAT_NONE;

        memcpy(&pdu->url[0], path, strlen(path));
        return 0;
    }
    else {
        /* reason for negative hdrlen is not defined, so we also are vague */
        return -1;
    }
}

ssize_t gcoap_finish(coap_pkt_t *pdu, size_t payload_len, unsigned format)
{
    /* reconstruct full PDU buffer length */
    size_t len = pdu->payload_len + (pdu->payload - (uint8_t *)pdu->hdr);

    pdu->content_type = format;
    pdu->payload_len  = payload_len;
    return _finish_pdu(pdu, (uint8_t *)pdu->hdr, len);
}

size_t gcoap_req_send(const uint8_t *buf, size_t len, const ipv6_addr_t *addr,
                      uint16_t port, gcoap_resp_handler_t resp_handler)
{
    sock_udp_ep_t remote;

    remote.family = AF_INET6;
    remote.netif  = SOCK_ADDR_ANY_NETIF;
    remote.port   = port;

    memcpy(&remote.addr.ipv6[0], &addr->u8[0], sizeof(addr->u8));

    return gcoap_req_send2(buf, len, &remote, resp_handler);
}

size_t gcoap_req_send2(const uint8_t *buf, size_t len,
                       const sock_udp_ep_t *remote,
                       gcoap_resp_handler_t resp_handler)
{
    gcoap_request_memo_t *memo = NULL;
    assert(remote != NULL);
    assert(resp_handler != NULL);

    /* Find empty slot in list of open requests. */
    mutex_lock(&_coap_state.lock);
    for (int i = 0; i < GCOAP_REQ_WAITING_MAX; i++) {
        if (_coap_state.open_reqs[i].state == GCOAP_MEMO_UNUSED) {
            memo = &_coap_state.open_reqs[i];
            memo->state = GCOAP_MEMO_WAIT;
            break;
        }
    }
    mutex_unlock(&_coap_state.lock);

    if (memo) {
        memo->send_limit = GCOAP_SEND_LIMIT_NON;
        memcpy(&memo->msg.hdr_buf[0], buf, GCOAP_HEADER_MAXLEN);
        memcpy(&memo->remote_ep, remote, sizeof(sock_udp_ep_t));
        memo->resp_handler = resp_handler;

        size_t res = sock_udp_send(&_sock, buf, len, remote);

        if (res && (GCOAP_NON_TIMEOUT > 0)) {
            /* interrupt sock listening (to set a listen timeout) */
            msg_t mbox_msg;
            mbox_msg.type          = GCOAP_MSG_TYPE_INTR;
            mbox_msg.content.value = 0;
            if (mbox_try_put(&_sock.reg.mbox, &mbox_msg)) {
                /* start response wait timer */
                memo->timeout_msg.type        = GCOAP_MSG_TYPE_TIMEOUT;
                memo->timeout_msg.content.ptr = (char *)memo;
                xtimer_set_msg(&memo->response_timer, GCOAP_NON_TIMEOUT,
                                                      &memo->timeout_msg, _pid);
            }
            else {
                memo->state = GCOAP_MEMO_UNUSED;
                DEBUG("gcoap: can't wake up mbox; no timeout for msg\n");
            }
        }
        else if (!res) {
            memo->state = GCOAP_MEMO_UNUSED;
            DEBUG("gcoap: sock send failed: %d\n", (int)res);
        }
        return res;
    } else {
        DEBUG("gcoap: dropping request; no space for response tracking\n");
        return 0;
    }
}

int gcoap_resp_init(coap_pkt_t *pdu, uint8_t *buf, size_t len, unsigned code)
{
    if (coap_get_type(pdu) == COAP_TYPE_CON) {
        coap_hdr_set_type(pdu->hdr, COAP_TYPE_ACK);
    }
    coap_hdr_set_code(pdu->hdr, code);

    /* Reserve some space between the header and payload to write options later */
    pdu->payload      = buf + coap_get_total_hdr_len(pdu) + GCOAP_RESP_OPTIONS_BUF;
    /* Payload length really zero at this point, but we set this to the available
     * length in the buffer. Allows us to reconstruct buffer length later. */
    pdu->payload_len  = len - (pdu->payload - buf);
    pdu->content_type = COAP_FORMAT_NONE;

    return 0;
}

int gcoap_obs_init(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                                  const coap_resource_t *resource)
{
    gcoap_observe_memo_t *memo = NULL;

    _find_obs_memo_resource(&memo, resource);
    if (memo == NULL) {
        /* Unique return value to specify there is not an observer */
        return GCOAP_OBS_INIT_UNUSED;
    }

    pdu->hdr       = (coap_hdr_t *)buf;
    uint16_t msgid = (uint16_t)atomic_fetch_add(&_coap_state.next_message_id, 1);
    ssize_t hdrlen = coap_build_hdr(pdu->hdr, COAP_TYPE_NON, &memo->token[0],
                                    memo->token_len, COAP_CODE_CONTENT, msgid);

    if (hdrlen > 0) {
        uint32_t now       = xtimer_now_usec();
        pdu->observe_value = (now >> GCOAP_OBS_TICK_EXPONENT) & 0xFFFFFF;

        /* Reserve some space between the header and payload to write options later */
        pdu->payload       = buf + coap_get_total_hdr_len(pdu) + GCOAP_OBS_OPTIONS_BUF;
        /* Payload length really zero at this point, but we set this to the available
         * length in the buffer. Allows us to reconstruct buffer length later. */
        pdu->payload_len   = len - (pdu->payload - buf);
        pdu->content_type  = COAP_FORMAT_NONE;

        return GCOAP_OBS_INIT_OK;
    }
    else {
        /* reason for negative hdrlen is not defined, so we also are vague */
        return GCOAP_OBS_INIT_ERR;
    }
}

size_t gcoap_obs_send(const uint8_t *buf, size_t len,
                      const coap_resource_t *resource)
{
    gcoap_observe_memo_t *memo = NULL;

    _find_obs_memo_resource(&memo, resource);

    if (memo) {
        return sock_udp_send(&_sock, buf, len, memo->observer);
    }
    else {
        return 0;
    }
}

uint8_t gcoap_op_state(void)
{
    uint8_t count = 0;
    for (int i = 0; i < GCOAP_REQ_WAITING_MAX; i++) {
        if (_coap_state.open_reqs[i].state != GCOAP_MEMO_UNUSED) {
            count++;
        }
    }
    return count;
}

int gcoap_get_resource_list(void *buf, size_t maxlen, uint8_t cf)
{
    assert(cf == COAP_CT_LINK_FORMAT);
#ifndef DEVELHELP
    (void)cf;
#endif

    /* skip the first listener, gcoap itself (we skip /.well-known/core) */
    gcoap_listener_t *listener = _coap_state.listeners->next;

    char *out = (char *)buf;
    size_t pos = 0;

    /* write payload */
    while (listener) {
        coap_resource_t *resource = listener->resources;

        for (unsigned i = 0; i < listener->resources_len; i++) {
            size_t path_len = strlen(resource->path);
            if (out) {
                /* only add new resources if there is space in the buffer */
                if ((pos + path_len + 3) > maxlen) {
                    break;
                }
                if (pos) {
                    out[pos++] = ',';
                }
                out[pos++] = '<';
                memcpy(&out[pos], resource->path, path_len);
                pos += path_len;
                out[pos++] = '>';
            }
            else {
                pos += (pos) ? 3 : 2;
                pos += path_len;
            }
            ++resource;
        }

        listener = listener->next;
    }

    return (int)pos;
}

int gcoap_add_qstring(coap_pkt_t *pdu, const char *key, const char *val)
{
    size_t qs_len = strlen((char *)pdu->qs);
    size_t key_len = strlen(key);
    size_t val_len = (val) ? (strlen(val) + 1) : 0;

    /* make sure if url_len + the new query string fit into the url buffer */
    if ((qs_len + key_len + val_len + 2) >= NANOCOAP_QS_MAX) {
        return -1;
    }

    pdu->qs[qs_len++] = '&';
    memcpy(&pdu->qs[qs_len], key, key_len);
    qs_len += key_len;
    if (val) {
        pdu->qs[qs_len++] = '=';
        memcpy(&pdu->qs[qs_len], val, val_len);
        qs_len += val_len;
    }
    pdu->qs[qs_len] = '\0';

    return (int)qs_len;
}

/** @} */
