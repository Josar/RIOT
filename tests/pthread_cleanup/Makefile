include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-mega2560 waspmote-pro arduino-uno arduino-duemilanove \
                   jiminy-mega256rfr2
# arduino mega2560 uno duemilanove jiminy-mega256rfr2: unknown type name: clockid_t

USEMODULE += pthread

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
