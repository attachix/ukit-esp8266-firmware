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
 * info.h
 *
 *  Created on: Jul 28, 2016
 *      Author: slavey
 */
#include "info.h"
#include <esp_spi_flash.h>

ManufacturerData* getManufacturerData()
{
	int size = sizeof(ManufacturerData);
	if(size % 4) { // align the size to 4 byte boundary
		size = ((size / 4) + 1) * 4;
	}
	uint8_t* buffer = new uint8_t[size];
	if(!buffer) {
		debugf("Unable to allocate memory for ManufacturerData buffer");
		return NULL;
	}

	int read = flashmem_read(buffer, MANUFACTURER_DATA_ADDR, size);
	if(read != size) {
		debugf("Unable to read ManufacturerData. Read only %d bytes.", read);
		delete[] buffer;

		return NULL;
	}

	return (ManufacturerData*)buffer;
}

DeviceData* getDeviceData()
{
	uint8_t buffer[100];

	int read = flashmem_read(buffer, DEVICE_DATA_ADDR, 100);
	if(read != 100) {
		debugf("Unable to read DeviceData. Got error: %d", read);
		delete[] buffer;

		return NULL;
	}

	DeviceData* data = (DeviceData*)malloc(sizeof(DeviceData));
	if(!data) {
		return NULL;
	}

	int algo = (int)buffer[0]; // max 256
	int size = (int)buffer[1]; // max 256

	//	debugf("Size: %d", size);

	// TODO: implement different password decryption schemes
	size_t bytesLength = std::min(sizeof(data->password), (unsigned int)size) - 1;
	memcpy(data->password, buffer + 2, bytesLength);
	data->password[bytesLength] = 0;

	//	debugf("Password: %s", data->password);

	return data;
}

#ifdef ENABLE_SSL

SslKeyCertPair getDeviceClientCert()
{
#include "ssl/private_key.h"
#include "ssl/cert.h"

	// TODO: Get the certificate data from Flash
	SslKeyCertPair clientCertKey;
	clientCertKey.assign(default_private_key, default_private_key_len, default_certificate, default_certificate_len);

	return clientCertKey;
}

#endif /* ENABLE_SSL */

char* loadPsk(int* keylen)
{
	SHA1_CTX sha_ctx;
	uint8_t* digest = (uint8_t*)malloc(SHA1_SIZE);
	if(!digest) {
		*keylen = 0;
		return NULL;
	}

	ManufacturerData* manufacturerData = getManufacturerData();
	DeviceData* deviceData = getDeviceData();

	if(!manufacturerData || !deviceData) {
		free(digest);
		*keylen = 0;
		return NULL;
	}

	char* buffer = (char*)malloc(100);
	sprintf(buffer, "%x%04x%04x%x%s%s", system_get_chip_id(), manufacturerData->id, manufacturerData->factory,
			spi_flash_get_id(), deviceData->password, manufacturerData->serial);

	//	debugf("Buffer: %s, size: %d", buffer, strlen(buffer));

	SHA1_Init(&sha_ctx);
	SHA1_Update(&sha_ctx, (uint8_t*)buffer, strlen(buffer));
	SHA1_Final(digest, &sha_ctx);

	free(buffer);

	delete[] manufacturerData;
	free(deviceData);

	char* hexdigest = (char*)malloc(2 * SHA1_SIZE + 1);
	if(!hexdigest) {
		free(digest);

		*keylen = 0;
		return NULL;
	}

	char* c = hexdigest;
	for(int i = 0; i < SHA1_SIZE; i++) {
		sprintf(c, "%02x", digest[i]);
		c += 2;
	}
	*c = '\0';
	*keylen = strlen(hexdigest);
	free(digest);

	return hexdigest;
}

/*
 * Get device's Unique Id
 * @return String - The Unique Id of the Device
 */
String getDevUID()
{
	char c[17];
	sprintf(c, "%x%x", system_get_chip_id(), spi_flash_get_id());
	return String(c);
}
