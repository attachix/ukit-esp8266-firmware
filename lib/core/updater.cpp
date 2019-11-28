//
// U:Kit ESP8266 Firmware - This is the 'smart' firmware for the U:Kit sensor kit
// Copyright (C) 2016, 2018 Slavey Karadzhov <slav@attachix.com>
//
// This file is part of U:Kit ESP8266 Firmware.
//
// U:Kit ESP8266 Firmware is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// U:Kit ESP8266 Firmware is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with U:Kit ESP8266 Firmware.  If not, see <http://www.gnu.org/licenses/>.
//

#include "updater.h"
#include "tsb.h"
#include "misc.h"
#include <esp_spi_flash.h>

FwUpdater::FwUpdater(int cpu1ResetPin)
{
	this->cpu1ResetPin = cpu1ResetPin;
	rboot_get_rtc_data(&rtc);

	bootConfig = rboot_get_config();
}

FwUpdater::~FwUpdater()
{
	delete baseRequest;
	delete httpClient;
	delete otaUpdater;
	baseRequest = NULL;
	httpClient = NULL;
	otaUpdater = NULL;
}

bool FwUpdater::update(const String& url, const DeviceInfo& info, bool optionalUpdates /* = false */)
{
	return check(url, info, RequestCompletedDelegate(&FwUpdater::checkCallback, this), optionalUpdates);
}

bool FwUpdater::check(const String& url, const DeviceInfo& info, RequestCompletedDelegate onCheckCallback,
					  bool optionalUpdates /* = false */)
{
	if(httpClient == NULL) {
		httpClient = new HttpClient();
	}

	HttpRequest* request;
	if(baseRequest != NULL) {
		request = baseRequest->clone();
	} else {
		request = httpClient->createRequest(url);
	}

	request->setURL(URL(url));

	HttpHeaders requestHeaders;
	if(optionalUpdates) {
		requestHeaders["x-fw-debug"] = "1";
	}
	requestHeaders["x-fw-did"] = String(info.id);
	requestHeaders["x-fw-v1"] = String(info.fw1v);
	requestHeaders["x-fw-v2"] = String(info.fw2v);
	requestHeaders["Connection"] = "keep-alive";
	request->setHeaders(requestHeaders);

	request->onRequestComplete(onCheckCallback);

	return httpClient->send(request);
}

int FwUpdater::checkCallback(HttpConnection& connection, bool successful)
{
	debug_d("after Check Callback");

	if(!successful) {
		debug_e("Update failed. Response Code: %d", connection.getResponse()->code);
		return -1;
	}

	UpdateURL* urls = new UpdateURL();
	Vector<String> lines;
	Vector<String> parts;

	// type(FW1 | FW2)|download-url|version|min-version.
	String body = connection.getResponse()->getBody();
	splitString(body, '\n', lines);

	for(unsigned i = 0; i < lines.count(); i++) {
		String line = lines[i];
		splitString(line, '|', parts);
		FwType type = (FwType)parts[0].toInt();
		switch(type) {
		case FwType::TYPE_FW1:
		case FwType::TYPE_FW2:
			(*urls)[type] = parts[1];
			break;

		default:
			debugf("Invalid type: %d", type);
			break;
		}
	}

	lines.clear();
	parts.clear();

	if(httpClient != NULL) {
		delete httpClient;
		httpClient = NULL;
	}

	doUpdate(urls);

	return 0;
}

bool FwUpdater::doUpdate(UpdateURL* urls)
{
	if(!urls->count()) {
		return false;
	}

	debug_d("Updating...");

	// need a clean object, otherwise if run before and failed will not run again
	if(otaUpdater)
		delete otaUpdater;
	otaUpdater = new RbootHttpUpdater();

#ifdef ENABLE_SSL
	otaUpdater->setBaseRequest(baseRequest);
#endif

#if ENABLE_OTA_TSB
	// NOTICE: The MCU1 ROM must be the first to download!
	if(urls->contains(FwType::TYPE_FW1)) {
		fw |= (int)FwType::TYPE_FW1;
		otaUpdater->addItem(TSB_CURRENT_ROM_ADDR, (*urls)[FwType::TYPE_FW1]);
	}
#endif

	if(urls->contains(FwType::TYPE_FW2)) {
		uint8_t slot = bootConfig.current_rom;
		if(slot == 0) {
			slot = 1;
		} else {
			slot = 0;
		}

		fw |= (int)FwType::TYPE_FW2;
		otaUpdater->addItem(bootConfig.roms[slot], (*urls)[FwType::TYPE_FW2]);
	}

	// and/or set a callback (called on failure or success without switching requested)
	otaUpdater->setCallback(OtaUpdateDelegate(&FwUpdater::updateCallBack, this));

	// start update
	otaUpdater->start();

	if(urls != NULL) {
		delete urls;
	}

	return true;
}

void FwUpdater::updateCallBack(RbootHttpUpdater& client, bool result)
{
	debug_d("In OTA callback...");

	if(!result) {
		debug_e("Firmware update failed!");
		return;
	}

	if(bitsSet(fw, (int)FwType::TYPE_FW2)) {
		uint8_t slot = bootConfig.current_rom;
		if(slot == 0) {
			slot = 1;
		} else {
			slot = 0;
		}

		if(!bitsSet(fw, (int)FwType::TYPE_FW1)) { // if only FW2 is updated
			// set to boot new rom and then reboot
			debug_d("FW2 updated, setting temporary ROM to %d...", slot);
			rboot_set_temp_rom(slot);
			System.restart();
		}

		rboot_set_current_rom(slot);
	}

	if(bitsSet(fw, (int)FwType::TYPE_FW1)) {
		RbootHttpUpdaterItem item = client.getItem(0); // the fist item is always the MCU1
		FwUpdateMark mark;
		mark.size = item.size;
		mark.minVersion = 0; // TODO: hard-coded value...
		storeMark(mark);
	}

	// Inform about RESET
	Serial.write('F'); // this will inform the other side to RESET

	// after the last command CPU1 should be reset causing also CPU2 to power off.

	// we should not be here but ...
	if(bitsSet(fw, (int)FwType::TYPE_FW2)) {
		System.restart();
	}
}

void FwUpdater::setBaseRequest(HttpRequest* request)
{
	baseRequest = request;
}

/**
 * Returns if there is a pending update that is not yet applied.
 * Basically it checks if we are using temporary ROM and the PENDING_UPDATE flag is set
 */
bool FwUpdater::pending()
{
#if ENABLE_OTA_DEFAULT_ROM == 1
	if(bootConfig.mode == MODE_GPIO_ROM) {
		debug_d("************** RUNNING DEFAULT BOOT ROM ********************");
#if ENABLE_OTA_TSB
		return true;
#endif
	}

	return false;
#else

#if ENABLE_OTA_TSB
	FwUpdateMark mark = readMark();

	return (mark.size > 0);
#else
	return false;
#endif

#endif
}

bool FwUpdater::postUpdate(const String& password /*  = "" */)
{
#if ENABLE_OTA_TSB
	uint32_t markAddr = TSB_CURRENT_MARK_ADDR;
	uint32_t romAddr = TSB_CURRENT_ROM_ADDR;

#if ENABLE_OTA_DEFAULT_ROM == 1
	if(bootConfig.mode == MODE_GPIO_ROM) {
		markAddr = TSB_DEFAULT_MARK_ADDR;
		romAddr = TSB_DEFAULT_ROM_ADDR;
	}
#endif

	FwUpdateMark mark = readMark(markAddr);

	//	debug_d("Mark size: %d", mark.size);
	//	Serial.printf("Mark size: %d", mark.size);

	if(mark.size < 1) {
		return false;
	}

	// TODO: Check minVersion
	uint8_t* firmwareData = readFwData(mark.size, romAddr);
	if(!firmwareData) {
		debug_e("Unable to read the FW1 data");

		return false;
	}

	Tsb tsb(&Serial);
	if(!tsb.connect(password)) {
		debug_e("Unable to communicate with TSB");
		//		logInfo("Unable to communicate with TSB");
		delete[] firmwareData;

		return false;
	}

	logInfo("Connected to TSB");

	size_t bytesWritten = tsb.writeFlash(firmwareData, mark.size);

#ifdef REMOTE_DEBUG
	char value[250] = {0};
	ets_sprintf(value, "Finished: s=%d, wrtn=%d", mark.size, bytesWritten);
	logInfo(String(value));
#endif

	if(bytesWritten != mark.size) {
		debug_e("Unable to write the new flash data!");
		delete[] firmwareData;

		return false;
	}

	// IDEA: Count the unsuccessful attempts and clear the mark if more than X failures.
	if(rtc.last_rom != bootConfig.gpio_rom) {
		debug_d("Clearing the mark...");
		clearMark();
	}

	//	logInfo("TSB: Finish");

	delete[] firmwareData;

	tsb.run();

	return true;
#endif

	return true;
}

bool FwUpdater::storeMark(FwUpdateMark mark, uint32_t startAddr /*= TSB_CURRENT_MARK_ADDR */)
{
	int markSize = sizeof(FwUpdateMark);

	// calculate checksum
	mark.checksum = mark.size ^ mark.minVersion;

	flashmem_erase_sector(flashmem_get_sector_of_address(startAddr));
	int written = flashmem_write(&mark, startAddr, markSize);

	debug_d("Writing flash data. Wrote %d bytes.", written);

	return (written == markSize);
}

FwUpdateMark FwUpdater::readMark(uint32_t startAddr /* = TSB_CURRENT_MARK_ADDR */)
{
	FwUpdateMark mark;
	int markSize = sizeof(FwUpdateMark); // IMPORTANT - the size must always be aligned to the 4th byte boundary

	flashmem_read(&mark, startAddr, markSize);

	if(mark.checksum != (mark.size ^ mark.minVersion)) {
		mark.size = 0;
		mark.minVersion = 0;
		mark.checksum = 0;
	}

	return mark;
}

bool FwUpdater::clearMark()
{
	FwUpdateMark mark;
	mark.size = 0;
	mark.minVersion = 0;
	mark.checksum = 0;

	return storeMark(mark);
}
uint8_t* FwUpdater::readFwData(int size, uint32_t startAddr)
{
	if(size % 4) { // align the size to 4 byte boundary
		size = ((size / 4) + 1) * 4;
	}
	uint8_t* data = new uint8_t[size];

	if(!data) {
		debug_e("Unable to allocate memory for FW1");
		return NULL;
	}

	int read = flashmem_read(data, startAddr, size);
	if(read != size) {
		debug_e("FwUpdater::readFwData read only %d bytes", read);
		delete[] data;

		return NULL;
	}

	return data;
}
