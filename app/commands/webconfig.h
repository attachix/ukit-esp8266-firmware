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

#include <MultipartParser/HttpMultipartResource.h>
#include <Data/Stream/FileStream.h>

#define DNS_PORT 53
#define MAC_SIZE 6

static uint8_t authorizedMacs[5][MAC_SIZE] = {0};

/**
 * Starts a web configure procedure
 */
void cmdModeSmartConfig(char command, char* params);

namespace Commands
{
namespace WebConfig
{
HttpServer* webServer = nullptr;
DnsServer* dnsServer = nullptr;

#define WEB_CMD_MODE_SMOKE "A"
#define WEB_CMD_MODE_ALARM "B"
#define WEB_CMD_MODE_SMART_ALARM "D"
#define WEB_CMD_FIRMWARE_UPDATE "F"
#define WEB_CMD_MODE_SMART_PROTECTION "H"
#define WEB_CMD_MODE_CANCEL "X"

/**
 * Starts or Stops
 *  - Web Server
 * 	- Captive DNS portal
 *
 *
 * @param bool flag - true to start, false otherwise
 */
void toggleServices(bool start);

void onIndex(HttpRequest& request, HttpResponse& response);
void onFile(HttpRequest& request, HttpResponse& response);
int onCommand(HttpServerConnection& connection, HttpRequest& request, HttpResponse& response);
#if ENABLE_JSVM == 1
void onWasm(HttpRequest& request, HttpResponse& response);
int onUpload(HttpServerConnection& connection, HttpRequest& request, HttpResponse& response);
void fileUploadMapper(HttpFiles& files);
#endif

/* Implementation */
void toggleServices(bool start)
{
	if(!start) {
		debugf("Stopping DNS Captive portal...");
		dnsServer->stop();
		delete dnsServer;
		dnsServer = nullptr;

		debugf("Stopping Web Server...");
		if(webServer) {
			webServer->shutdown();
			webServer = nullptr;
		}
	} else {
		debugf("Starting Web Server...");
		delete webServer;
		webServer = new HttpServer();
		webServer->listen(80);
		webServer->paths.set("/", onIndex);
		webServer->paths.set("/cmd", onCommand);
		webServer->paths.setDefault(onFile);

#if ENABLE_JSVM == 1
		webServer->paths.set("/jsc.wasm", onWasm);
		HttpMultipartResource* uploadResouce = new HttpMultipartResource(fileUploadMapper, onUpload);
		webServer->paths.set("/update", uploadResouce);
#endif

		// TODO: there is problem happening if the WIFI connection happens AFTER the WIFI AP is already established
		//		 take care to handle also such situations...
		debugf("Starting Captive Web Portal on IP: %s", WifiAccessPoint.getIP().toString().c_str());
		delete dnsServer;
		dnsServer = new DnsServer();
		dnsServer->start(DNS_PORT, "*", WifiAccessPoint.getIP());
	}
}

void onIndex(HttpRequest& request, HttpResponse& response)
{
	response.sendFile("index.html");
}

void onFile(HttpRequest& request, HttpResponse& response)
{
	String file = request.uri.Path;
	if(file[0] == '/')
		file = file.substring(1);

	if(file[0] == '.')
		response.code = HTTP_STATUS_FORBIDDEN;
	else {
		response.setCache(86400, true);
		response.sendFile(file);
	}
}

/**
 * 	@brief Limit the request based on the MAC address of the user extracted from the assigned IP.
 * 	@param const IpAddress& ip The current IP. From the IP we can get the MAC address
 * 	@return bool - true if the user is authorized, false otherwise
 */
bool isAuthorized(const IpAddress& ip)
{
	uint8_t foundMac[6] = {0};
	station_info* stationList = wifi_softap_get_station_info();
	debugf("Remote IP:  %s", ip.toString().c_str());

	while(stationList != NULL) {
		debugf("bssid: " MACSTR " .", MAC2STR(stationList->bssid));
		if(ip == IpAddress(stationList->ip)) {
			debugf("This is our guy!");
			memcpy(foundMac, stationList->bssid, sizeof(stationList->bssid));
			break;
		}

		stationList = STAILQ_NEXT(stationList, next);
	}
	wifi_softap_free_station_info();

	if(*foundMac) {
		// TODO: keep a list of authorized MAC addresses.
		//		 Allow the user only if his/her IP is in the allowed list
		unsigned max = sizeof(authorizedMacs) / MAC_SIZE; // a MAC address is 6 bytes
		for(unsigned i = 0; i < max; i++) {
			if(memcmp(authorizedMacs[i], foundMac, MAC_SIZE) == 0) {
				return true;
			}
		}

		return true; // TODO: remove that line once the actual storing of authorized MACs is ready.
	}

	return false;
}

int onCommand(HttpServerConnection& connection, HttpRequest& request, HttpResponse& response)
{
	if(request.method != HTTP_POST) {
		return 0;
	}

	if(!isAuthorized(connection.getRemoteIp())) {
		response.code = HTTP_STATUS_FORBIDDEN;
		return 0;
	}

	String command = request.getPostParameter("cmd");
	if(command == WEB_CMD_MODE_SMOKE || command == WEB_CMD_MODE_ALARM || command == WEB_CMD_MODE_SMART_ALARM ||
	   command == WEB_CMD_MODE_SMART_PROTECTION) {
		Serial.write(command.c_str(), command.length());
		return 0;
	}

	if(command == WEB_CMD_FIRMWARE_UPDATE) {
		OtaUpdate(UPDATE_URL);
		return 0;
	}

	Serial.write('X');

	return 0;
}

#if ENABLE_JSVM == 1
void fileUploadMapper(HttpFiles& files)
{
	files["mainjs"] = new FileStream(MAIN_JS_FILE, eFO_CreateNewAlways | eFO_WriteOnly);
}

int onUpload(HttpServerConnection& connection, HttpRequest& request, HttpResponse& response)
{
	String body = "{\"status\": \"ok\"}";
	if(response.isSuccess()) {
		body = "{\"status\": \"ok\"}";
	}
	response.headers[HTTP_HEADER_CONTENT_TYPE] = "application/json";
	response.sendString(body);
	return 0;
}


void onWasm(HttpRequest& request, HttpResponse& response)
{
	onFile(request, response);
	response.headers[HTTP_HEADER_CONTENT_TYPE] = "application/wasm";
}
#endif

void onReady()
{
	toggleServices(true);
}

} // namespace WebConfig
} // namespace Commands

void cmdModeSmartConfig(char command, char* params)
{
	WifiAccessPoint.enable(true, false); // enable, don't persist in device configuration
	System.onReady(Commands::WebConfig::onReady);
}
