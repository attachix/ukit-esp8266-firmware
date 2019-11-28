## Local build configuration
## Parameters configured here will override default and ENV values.
## Uncomment and change examples:

ENABLE_HTTP_SERVER_MULTIPART := 1
export COMPONENT_SEARCH_DIRS := $(CURDIR)/components

ifeq ($(SMING_ARCH),Host)
	COMPONENT_DEPENDS := esp-mock
endif

## Add your source directories here separated by space
COMPONENT_SRCDIRS := app lib/core
COMPONENT_INCDIRS := include lib/core
COMPONENT_CFLAGS := 
CS_DIR := app lib/core

# Emulate both serial ports
ENABLE_HOST_UARTID := 0 1

## Com port speed
COM_SPEED_SERIAL ?= 9200

## Configure flash parameters (for ESP12-E and other new boards):
# SPI_MODE = dio

## SPIFFS options
#DISABLE_SPIFFS = 1
SPIFF_FILES = web/build

#### overridable rBoot options ####
## use rboot build mode
RBOOT_ENABLED ?= 1
## enable big flash support (for multiple roms, each in separate 1mb block of flash)
RBOOT_BIG_FLASH ?= 1
RBOOT_RTC_ENABLED = 1
# Boot via the GPIO will run the factory default ROM.
RBOOT_GPIO_ENABLED = 1
# `RBOOT_GPIO_SKIP_ENABLED` and `RBOOT_GPIO_ENABLED` cannot be used at the same time.
RBOOT_GPIO_SKIP_ENABLED = 0
# 3rd MB
RBOOT_ROM1_ADDR=0x202000
# 4th MB
RBOOT_ROM2_ADDR=0x302000

# Don't print rBoot hello messages
RBOOT_SILENT = 1 

## two rom mode (where two roms sit in the same 1mb block of flash)
#RBOOT_TWO_ROMS  ?= 1
## size of the flash chip
SPI_SIZE        ?= 4M
## output file for first rom (.bin will be appended)
#RBOOT_ROM_0     ?= rom0
## input linker file for first rom
#RBOOT_LD_0      ?= rom0.ld
## these next options only needed when using two rom mode
#RBOOT_ROM_1     ?= rom1
#RBOOT_LD_1      ?= rom1.ld
SPIFF_START_OFFSET ?= $(RBOOT_SPIFFS_0)
## size of the spiffs to create
SPIFF_SIZE      ?= 512000
## option to completely disable spiffs
#DISABLE_SPIFFS  = 1
## flash offsets for spiffs, set if using two rom mode or not on a 4mb flash
## (spiffs location defaults to the mb after the rom slot on 4mb flash)
#RBOOT_SPIFFS_0  ?= 0x100000
#RBOOT_SPIFFS_1  ?= 0x300000
## esptool2 path
#ESPTOOL2        ?= esptool2

# [Memory Map]

# TSB Settings (Update them only here according to the updated flash map)
TSB_DEFAULT_MARK_ADDR= 0x300000
TSB_CURRENT_MARK_ADDR= 0x301000
TSB_DEFAULT_ROM_ADDR = 0x3f0000
TSB_CURRENT_ROM_ADDR = 0x3f4000

# Device Info Settings
MANUFACTURER_DATA_ADDR = 0x00000ea0
DEVICE_DATA_ADDR=0x200000

# Application data address
APP_DATA_ADDR=0x1fe000

# Crash Data address
CRASH_DATA_ADDR=0x1fd000

# [Flash Chunks]
ALL_CHUNKS = 0x02000=$(CURRENT_DIR)/$(RELEASE_DIR)/rom0.bin \
						 $(RBOOT_ROM2_ADDR)=$(CURRENT_DIR)/$(RELEASE_DIR)/rom0-default.bin \
						 $(SPIFF_START_OFFSET)=$(SPIFF_BIN_OUT) \
						 $(TSB_DEFAULT_ROM_ADDR)=firmware/rom1.bin \
						 0x0=$(CURRENT_DIR)/$(RELEASE_DIR)/rboot.bin

# [Sming Compile Settings] #

# SSL settings
ENABLE_SSL=1

# Custom Heap Settings
ENABLE_CUSTOM_HEAP ?= 0 

ENABLE_CUSTOM_PHY ?= 0

# Use custom LWIP
ENABLE_CUSTOM_LWIP=1
ifeq ($(SMING_ARCH),Host)
   ENABLE_CUSTOM_LWIP=2
endif
# ENABLE_ESPCONN=1

# Preserve heap by disabling command executor
ENABLE_CMD_EXECUTOR=0

MQTT_NO_COMPAT=1

export ENABLE_SSL
export ENABLE_CUSTOM_LWIP
export ENABLE_ESPCONN
export ENABLE_CMD_EXECUTOR
export ENABLE_CUSTOM_PHY
export MQTT_NO_COMPAT
export ENABLE_HTTP_SERVER_MULTIPART
# Always use strict compilation
export STRICT ?= 1

# Should be enabled after the first phase

#Modes
RELEASE ?= 0
TEST_RELEASE ?=1

# Deployment Settings
REMOTE_USER=remote
REMOTE_HOST=attachix.com
REMOTE_FOLDER=/home/remote/public-roms/
REMOTE_OPTIONS="-P 10022"

# Debug - hide completely debug messages
#DEBUG=1 
# Do not show any visual indication
#STEALTH_MODE=1 
# Allow test commands to be used
# TEST_MODE=1

ifeq ($(RELEASE), 1)
	COMPONENT_CFLAGS +=-DRELEASE=1 -DSTEALTH_MODE=1
else ifeq ($(TEST_RELEASE), 1)
	COMPONENT_CFLAGS += -DTEST_MODE=1
else 
	DEBUG_VERBOSE_LEVEL = 3
	export DEBUG_VERBOSE_LEVEL
	COMPONENT_CFLAGS +=-DDEBUG=1 -DTEST_MODE=1
endif

# Experimental Features
ENABLE_EXPERIMENTAL ?= 1

ENABLE_JSVM ?= 0 # allow JerryScript VM if experimental mode is set
ENABLE_OTA  ?= 1 # allow OverTheAir updates
ENABLE_OTA_TSB ?= 1 # Allow updates of CPU1 over the air using TSB bootloader
ENABLE_OTA_DEFAULT_ROM ?= 0

COMPONENT_CFLAGS +=-DTSB_DEFAULT_MARK_ADDR=$(TSB_DEFAULT_MARK_ADDR) -DTSB_CURRENT_MARK_ADDR=$(TSB_CURRENT_MARK_ADDR) \
			  -DTSB_DEFAULT_ROM_ADDR=$(TSB_DEFAULT_ROM_ADDR) -DTSB_CURRENT_ROM_ADDR=$(TSB_CURRENT_ROM_ADDR) \
			  -DMANUFACTURER_DATA_ADDR=$(MANUFACTURER_DATA_ADDR) -DDEVICE_DATA_ADDR=$(DEVICE_DATA_ADDR) \
			  -DAPP_DATA_ADDR=$(APP_DATA_ADDR) -DCRASH_DATA_ADDR=$(CRASH_DATA_ADDR)

ifeq ($(ENABLE_OTA_DEFAULT_ROM), 1)
	COMPONENT_CFLAGS +=-DRBOOT_GPIO_ENABLED=1 -DBOOT_GPIO_ENABLED=1 -DRBOOT_ROM1_ADDR=$(RBOOT_ROM1_ADDR) \
				  -DRBOOT_ROM2_ADDR=$(RBOOT_ROM2_ADDR) -DRBOOT_SILENT=$(RBOOT_SILENT)
endif

ifeq ($(ENABLE_EXPERIMENTAL), 1)
	COMPONENT_CFLAGS +=-DENABLE_JSVM=$(ENABLE_JSVM) -DENABLE_OTA=$(ENABLE_OTA) -DENABLE_OTA_TSB=$(ENABLE_OTA_TSB) \
				  -DENABLE_OTA_DEFAULT_ROM=$(ENABLE_OTA_DEFAULT_ROM)
endif

ifeq ($(ENABLE_JSVM),1)
	COMPONENT_DEPENDS += jsvm
else
$(info "(!) JSVM is in experimental phase. Use ENABLE_JSVM=1 to turn it on.")
endif

ifeq ($(RELEASE), 1)
	# this requires also recompilation of Sming with the SMING_RELEASE flag.
	export SMING_RELEASE=1
endif

COMPONENT_VARS 			:= ENABLE_JSVM ENABLE_OTA ENABLE_OTA_TSB ENABLE_OTA_DEFAULT_ROM
RELINK_VARS         := ENABLE_JSVM
CONFIG_VARS  				:= ENABLE_JSVM

export ENABLE_JSVM
export ENABLE_OTA
export ENABLE_OTA_TSB
export ENABLE_OTA_DEFAULT_ROM
