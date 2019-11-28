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

/*
 * crash.cpp
 *
 *  Created on: Jul 28, 2016
 *      Author: slavey
 */
#include "crash.h"
#include <esp_spi_flash.h>

bool loadCrashData(CrashDataType* data)
{
	uint8_t buffer[sizeof(CrashDataType)];

	int read = flashmem_read(buffer, CRASH_DATA_ADDR, sizeof(CrashDataType));
	if(read != sizeof(CrashDataType)) {
		debugf("Unable to read application data. Read only %d bytes", read);

		return false;
	}

	const CrashDataType* tempData = (CrashDataType*)buffer;
	if(tempData->magic != CRASH_DATA_MAGIC) {
		debugf("CrashData: Invalid magic!");

		return false;
	}

	memcpy(data, buffer, sizeof(CrashDataType));

	return true;
}

bool saveCrashData(CrashDataType* data)
{
	flashmem_erase_sector(flashmem_get_sector_of_address(CRASH_DATA_ADDR));
	int written = flashmem_write(data, CRASH_DATA_ADDR, sizeof(CrashDataType));
	if(written != sizeof(CrashDataType)) {
		debugf("Unable to write crash data. Wrote only %d bytes.", written);

		return false;
	}

	return true;
}

void custom_crash_callback(struct rst_info* rst_info, uint32_t start, uint32_t end)
{
	CrashDataType data;
	uint8_t slot = 0;
	uint32_t pos = 0;
	uint8_t index = 0;

	rboot_rtc_data rtc;
	rboot_get_rtc_data(&rtc);
	slot = rtc.last_rom;

	if(rtc.last_mode == MODE_TEMP_ROM) {
		return;
	}

	loadCrashData(&data);
	data.rom[slot].count++;
	data.rom[slot].reason = (*rst_info);
	data.rom[slot].ms = millis();

	for(pos = start; pos < end; pos += 0x10) {
		uint32_t* values = (uint32_t*)(pos);

		// rough indicator: stack frames usually have SP saved as the second word
		// bool looksLikeStackFrame = (values[2] == pos + 0x10);
		data.rom[slot].sf[index++] = pos;
		data.rom[slot].sf[index++] = values[0];
		data.rom[slot].sf[index++] = values[1];
		data.rom[slot].sf[index++] = values[2];
		data.rom[slot].sf[index++] = values[3];

		if(index == (CRASH_MAX_STACK_FRAME_LINES * 5)) {
			break;
		}
	}

	saveCrashData(&data);

	debugf("*** Crash Information Stored! ****");
}
