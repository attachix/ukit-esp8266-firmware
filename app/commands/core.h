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

// Commands
#define PROTOCOL_READY (char)0x5a

enum ProtocolCommand {
	PROTOCOL_COMMAND_CONFIG = 0x30,
	PROTOCOL_COMMAND_SMOKE,
	PROTOCOL_COMMAND_TEMPERATURE,
	PROTOCOL_COMMAND_MOTION,
	PROTOCOL_COMMAND_BATTERY
};

/**
 * Smoke event
 */
void cmdEventSmoke(char command, char* params);

/*
 * General Event
 */
void cmdEvent(char command, char* params);

bool persistWifiConfig();

#if ENABLE_OTA == 1
void checkOTAConnection();
#endif /* ENABLE_OTA == 1  */

/* Implementation */

Timer wifiConnectionTimer;

void cmdEventSmoke(char command, char* params)
{
	char value[25] = {0};
	ets_sprintf(value, "%d|%d", params[0], params[1]);

	sendAndHalt(String("dev/") + DEVICE_TYPE + "/" + deviceId + "/smoke", value, String(PROTOCOL_READY));
}

void cmdEvent(char command, char* params)
{
	HashMap<char, String> commandTopic;
	commandTopic[PROTOCOL_COMMAND_MOTION] = "motion";
	commandTopic[PROTOCOL_COMMAND_TEMPERATURE] = "temperature";
	commandTopic[PROTOCOL_COMMAND_BATTERY] = "battery";

	if(!commandTopic.contains(command)) {
		debugf("Cannot handle this command: %x", command);
		return;
	}

	char value[5] = {0};
	ets_sprintf(value, "%d", params[0]);

	sendAndHalt(String("dev/") + DEVICE_TYPE + "/" + deviceId + "/" + commandTopic[command], value,
				String(PROTOCOL_READY));
}

// Helper functions

bool persistWifiConfig()
{
	station_config config = {0};
	bool cfgreaded = wifi_station_get_config(&config);
	if(!cfgreaded) {
		debugf("Can't read station configuration!");
		return false;
	}

	wifi_station_set_config(&config);
	return true;
}
