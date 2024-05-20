# name of your application
APPLICATION = home-task-2

# If no BOARD is found in the environment, use this default:
# BOARD ?= nucleo-l152re
BOARD ?= blackpill-stm32f103c8

USEMODULE += periph_gpio
USEMODULE += periph_i2c
USEMODULE += bme280_i2c	
USEMODULE += stdio_uart
USEMODULE += dht
USEMODULE += shell_commands

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
