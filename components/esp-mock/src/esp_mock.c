#include "esp_mock.h"
#include <string.h>

bool wifi_station_set_config(struct station_config* config)
{
	return true;
}

bool wifi_station_get_config(struct station_config* config)
{
	return true;
}

void wifi_softap_free_station_info(void)
{
}

struct station_info* wifi_softap_get_station_info(void)
{
	struct station_info* info = (struct station_info*)malloc(sizeof(struct station_info));
	memcpy(info->bssid,"1234", 5);
	info->ip.addr = 17672384;

	return info;
}

bool system_rtc_mem_write(uint8_t des_addr, const void* src_addr, uint16_t save_size)
{
	return true;
}

bool system_rtc_mem_read(uint8_t src_addr, void* des_addr, uint16_t load_size)
{
	return true;
}
