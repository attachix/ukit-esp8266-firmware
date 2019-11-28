/* * U:Kit ESP8266 Firmware - This is the 'smart' firmware for the U:Kit sensor kit
 * Copyright (C) 2016, 2018 Slavey Karadzhov <slav@attachix.com>
 *
 * This file is part of U:Kit ESP8266 Firmware.
 *
 * U:Kit ESP8266 Firmware is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * U:Kit ESP8266 Firmware is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with U:Kit ESP8266 Firmware.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "lwip/ip4_addr.h"
#include "queue.h"

// Macros
#ifndef ets_sprintf
#define ets_sprintf sprintf
#endif

#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

// Types
struct station_config {
	uint8_t ssid[32];
	uint8_t password[64];
	uint8_t bssid_set; // Note: If bssid_set is 1, station will just connect to the router
					   // with both ssid[] and bssid[] matched. Please check about this.
	uint8_t bssid[6];
};

struct station_info {
	STAILQ_ENTRY(station_info) next;

	uint8_t bssid[6];
	struct ip4_addr ip;
};

// Functions
bool wifi_station_set_config(struct station_config* config);
bool wifi_station_get_config(struct station_config* config);
void wifi_softap_free_station_info(void);
struct station_info* wifi_softap_get_station_info(void);

bool system_rtc_mem_write(uint8_t des_addr, const void* src_addr, uint16_t save_size);
bool system_rtc_mem_read(uint8_t src_addr, void* des_addr, uint16_t load_size);

#ifdef __cplusplus
}
#endif
