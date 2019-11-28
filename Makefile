# Important parameters check.
# We need to make sure SMING_HOME and ESP_HOME variables are set.
# You can use Makefile-user.mk in each project or use enviromental variables to set it globally.

ifndef SMING_HOME
$(error SMING_HOME is not set. Please configure it as an environment variable, or in Makefile-user.mk)
endif

# Include application Makefile
include $(SMING_HOME)/project.mk

PATH:=./node_modules/.bin/:$(PATH)
export PATH

CURRENT_DIR := $(dir $(abspath $(firstword $(MAKEFILE_LIST))))

GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always --tags)
GIT_BRANCH  := $(shell git rev-parse --abbrev-ref HEAD)
SMING_VERSION := $(shell git -C $(SMING_HOME) log -n 1 --pretty=format:"%h")

ifeq ($(ENABLE_OTA),1)
	FEATURES += OTA
endif
ifeq ($(ENABLE_JSVM),1)
	FEATURES += JSVM
endif

DATE_VERSION :=$(shell date +'%Y.%m.%d.%H')

USER_CFLAGS +=-DFW_VERSION="\"$(DATE_VERSION)-$(GIT_VERSION):$(SMING_VERSION)@$(GIT_BRANCH):$(FEATURES)\""

ifeq ($(TEST_BUZZ), 1)
	USER_CFLAGS += -DTEST_BUZZ=1
endif

ifeq ($(TEST_BLINK), 1)
	USER_CFLAGS += -DTEST_BLINK=1
endif

ifeq ($(TEST_AUTO_OTA), 1)
	USER_CFLAGS += -DTEST_AUTO_OTA=1
endif

rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
CS_FILES := $(call rwildcard,$(patsubst %,%/*,$(CS_DIR)),*.cpp *.h *.c)

### Deployment And Release Targets ###
RELEASE_DIR=$(OUT_BASE)/releases

ifneq ($(DISABLE_SPIFFS), 1)
	EXTRA_OPTS += --spiffs=$(CURRENT_DIR)/$(RELEASE_DIR)/spiff_rom.bin
endif

ifeq ($(FLASH_INIT_DATA), 1) 
	EXTRA_OPTS += --init=$(SDK_BASE)/bin/esp_init_data_default.bin
endif

##@Firmware
../mcu1/build/main.hex:
	$(Q) $(MAKE) -C ../mcu1/

firmware/rom1.bin: ../mcu1/build/main.hex ## Generate rom1
	$(Q) hex2bin.py $< $@

firmware/rom1-debug.bin: ../mcu1/build/main.hex ## Generate debug rom1
	$(Q) hex2bin.py $< $@

##@Deployment
.PHONY: deploy1 
deploy1: firmware/rom1.bin firmware/rom1-debug.bin ## Deploy firmware for MCU1
	$(Q) scp $(REMOTE_OPTIONS) firmware/rom1.bin ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_FOLDER}/rom1.bin
	$(Q) scp $(REMOTE_OPTIONS) firmware/rom1-debug.bin ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_FOLDER}/rom1-debug.bin

.PHONY: deploy2
deploy2: ## Deploy firmware for MCU2
	$(Q) $(MAKE) RELEASE=0 TEST_RELEASE=1 
	$(Q) cp $(OUT_BASE)/firmware/rom0.bin firmware/rom0-9600.bin
	$(Q) scp $(REMOTE_OPTIONS) firmware/rom0-9600.bin ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_FOLDER}/rom2.bin
	$(Q) $(MAKE) clean
	$(Q) $(MAKE) RELEASE=0 TEST_RELEASE=0 
	$(Q) cp $(OUT_BASE)/firmware/rom0.bin firmware/rom0-9600-debug.bin
	$(Q) scp $(REMOTE_OPTIONS) firmware/rom0-9600-debug.bin ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_FOLDER}/rom2-debug.bin

.PHONY: deploy
deploy: deploy1 deploy2 ## Deploy firmware for MCU1 and MCU2

##@Flashing
flashchunks: firmware/rom1.bin $(SPIFF_BIN_OUT) ##Flash all chunks
	 $(call WriteFlash, $(ALL_CHUNKS))
	
##@Releases
release-clean:
	$(Q) rm -rf $(RELEASE_DIR)

$(RELEASE_DIR): 
	$(Q) mkdir -p $(RELEASE_DIR)
	
$(RELEASE_DIR)/rom0.bin: $(RELEASE_DIR) $(SPIFF_BIN_OUT)
	$(Q) $(MAKE) config-clean clean
	$(Q) $(MAKE) all SMING_ARCH=Esp8266 
	$(Q) cp $(SPIFF_BIN_OUT) $(RELEASE_DIR)/spiff_rom.bin
	$(Q) cp $(OUT_BASE)/firmware/rom0.bin $(RELEASE_DIR)/rom0.bin

$(RELEASE_DIR)/rom0-default.bin: $(RELEASE_DIR)
	$(Q) $(MAKE) config-clean clean
	$(Q) $(MAKE) all SMING_ARCH=Esp8266 ENABLE_OTA_DEFAULT_ROM=1
	$(Q) cp $(OUT_BASE)/firmware/rboot.bin $(RELEASE_DIR)/rboot.bin
	$(Q) cp $(OUT_BASE)/firmware/rom0.bin $(RELEASE_DIR)/rom0-default.bin

release: $(RELEASE_DIR) $(RELEASE_DIR)/rom0-default.bin $(RELEASE_DIR)/rom0.bin firmware/rom1.bin
	python ../tools/flasher.py --addr-rom0=0x02000 --addr-romD=$(RBOOT_ROM2_ADDR) \
				--addr-spiffs=$(SPIFF_START_OFFSET) --addr-device=$(DEVICE_DATA_ADDR) \
				--addr-romX=$(TSB_DEFAULT_ROM_ADDR) --addr-mark=$(TSB_DEFAULT_MARK_ADDR) \
				--config=$(SDK_BASE)/bin/blank.bin \
				--rom0=$(CURRENT_DIR)/$(RELEASE_DIR)/rom0.bin --romD=$(CURRENT_DIR)/$(RELEASE_DIR)/rom0-default.bin \
			    --romX=firmware/rom1.bin --boot=$(CURRENT_DIR)/$(RELEASE_DIR)/rboot.bin $(EXTRA_OPTS)
		
pre-update: $(OUT_BASE)/firmware/rom0.bin
	$(Q) python ../tools/romupdater.py -a 0x02000 -n $< -o $<.last
	 	
update:  pre-update
	$(Q) cp -f $(OUT_BASE)/firmware/rom0.bin $(OUT_BASE)/firmware/rom0.bin.last 
	

firmware:
	$(Q) mkdir -p firmware

#test-ota-mcu1: $(RELEASE_DIR) $(BUILD_BASE) firmware
#	$(vecho) "Flash AVR-APP with TEST_BUZZ=1"
#	$(vecho) "Compile AVR-APP with TEST_BLINK=1 TEST_AUTO_OTA=1"
#	$(Q) touch ../mcu1/src/main.asm
#	$(Q) make -C ../mcu1/src/ TEST_BLINK=1 TEST_AUTO_OTA=1
#	$(Q) make -C ../mcu1/src/ ota-app
#	$(vecho) "Compile AVR-APP with TEST_BLINK=1 and TEST_BUZZ=1"
#	$(Q) touch ../mcu1/src/main.asm 
#	$(Q) make -C ../mcu1/src/ build/main.bin TEST_BLINK=1 TEST_BUZZ=1
#	$(Q) cp ../mcu1/src/build/main.bin firmware/rom1.bin
#	$(vecho) "compile AVR-APP with TEST_BUZZ=1" 
#	$(Q) touch ../mcu1/src/main.asm 
#	$(Q) make -C ../mcu1/src/ build/main.bin TEST_BUZZ=1
#	$(Q) scp $(REMOTE_OPTIONS) ../mcu1/src/build/main.bin ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_FOLDER}/rom1.bin 
#	
#test-ota-mcu2: $(RELEASE_DIR) $(BUILD_BASE) firmware
#	$(vecho) "Compile ESP-APP with TEST_BUZZ and send it to remote server"
#	$(Q) make clean
#	$(Q) make TEST_BUZZ=1
#	$(Q) scp $(REMOTE_OPTIONS) $(OUT_BASE)/firmware/rom0.bin ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_FOLDER}/rom0.bin
#	$(vecho) "Compile ESP-APP with TEST_BLINK=1 and TEST_AUTO_OTA=1 and save it to releases"
#	$(Q) make clean
#	$(Q) make TEST_BLINK=1 TEST_AUTO_OTA=1
#	$(Q) cp $(OUT_BASE)/firmware/rom0.bin $(RELEASE_DIR)/rom0.bin
#	$(vecho) "Compile ESP-APP with TEST_BLINK and TEST_BUZZ and save it to releases as factory default"
#	$(Q) $(MAKE) clean
#	$(Q) $(MAKE)  ENABLE_OTA_DEFAULT_ROM=1 TEST_BUZZ=1 TEST_BLINK=1
#	$(Q) cp $(OUT_BASE)/firmware/rboot.bin $(RELEASE_DIR)/rboot.bin
#	$(Q) cp $(OUT_BASE)/firmware/rom0.bin $(RELEASE_DIR)/rom0-default.bin
#	$(vecho) "Prepare MCU2 for flashing. Press enter when ready"; read line
#	$(Q) $(MAKE) release
#	
#test-ota: test-ota-mcu1 test-ota-mcu2

##@Web development targets
WEB_DEV_DIR := web/dev
WEB_BUILD_DIR := web/build
NODEJS ?= node

.PHONY: web-pack
web-pack: ##Packing all web resources
	$(Q) rm -rf $(WEB_BUILD_DIR)
	$(Q) gulp
	$(Q) date +'%a, %d %b %Y %H:%M:%S GMT' -u > $(WEB_BUILD_DIR)/.lastModified
	
$(WEB_BUILD_DIR)/jsc.wasm.gz: $(WEB_DEV_DIR)/jsc.wasm ##Gzipping the WebAssembly file (gulp cannot do it right!?)
	$(Q) cp $< $(WEB_BUILD_DIR)/.
	$(Q) gzip $(WEB_BUILD_DIR)/jsc.wasm

$(WEB_BUILD_DIR)/main.js.snap: $(WEB_DEV_DIR)/main.js ##Create compiled main.js file.
	$(Q) mkdir -p $(dir $@)
	$(Q) $(NODEJS) $(WEB_DEV_DIR)/jsc.js -o $@ $<

$(SPIFF_BIN_OUT): web-pack $(WEB_BUILD_DIR)/main.js.snap $(WEB_BUILD_DIR)/jsc.wasm.gz 

##@Coding Style Targets
.PHONY: cs
cs: ## Apply coding style rules to application code
	$(Q) for FILE in $(CS_FILES); do \
		clang-format -i -style=file $$FILE; \
	done
