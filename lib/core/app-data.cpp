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
 * app-data.c
 *
 *  Created on: Feb 11, 2016
 *      Author: slavey
 */

#include "app-data.h"
#include <esp_spi_flash.h>

AppData::AppData(uint32_t startAddress)
{
	this->startAddress = startAddress;
}

bool AppData::load()
{
	uint8_t buffer[sizeof(AppDataType)];

	int read = flashmem_read(buffer, startAddress, sizeof(AppDataType));
	if(read != sizeof(AppDataType)) {
		debugf("Unable to read application data. Read only %d bytes", read);

		return false;
	}

	const AppDataType* tempData = (AppDataType*)buffer;
	if(tempData->magic != DATA_MAGIC) {
		debugf("AppData: Invalid magic!");

		return false;
	}

	memcpy(&data, buffer, sizeof(AppDataType));

	return true;
}

bool AppData::save()
{
	flashmem_erase_sector(flashmem_get_sector_of_address(startAddress));
	int written = flashmem_write(&data, startAddress, sizeof(AppDataType));
	if(written != sizeof(AppDataType)) {
		debugf("Unable to write application data. Wrote only %d bytes.", written);

		return false;
	}

	return true;
}

bool AppData::migrate()
{
	if(data.version < 2) {
		// Add here the needed migrations...
	}

	return false;
}
