include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-mega2560 waspmote-pro arduino-uno arduino-duemilanove
# arduino-mega2560: builds locally but breaks travis (possibly because of
# differences in the toolchain)

# The MSP boards don't feature round(), exp(), and log(), which are used in the unittests
BOARD_BLACKLIST += chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1 \
                   jiminy-mega256rfr2

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031 nucleo32-f042 nucleo32-l031

USEMODULE += libfixmath-unittests

ifneq (,$(filter native,$(BOARD)))
  export LINKFLAGS += -lm
endif

include $(RIOTBASE)/Makefile.include

test:
    tests/01-run.py
