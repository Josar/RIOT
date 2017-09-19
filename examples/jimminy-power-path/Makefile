# name of your application
APPLICATION = jimminy-power-path

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 0

# Batterie charger and power path IC
USEMODULE += bq24298
# Battery Fuel Gauge
USEMODULE += lc709203f
# Step Down Converter 1.8-3.3V
USEMODULE += tps6274x

# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

#Add support for xtimer
USEMODULE += xtimer
FEATURES_REQUIRED += periph_timer

include $(RIOTBASE)/Makefile.include

