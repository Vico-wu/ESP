#ifndef _NVS_MANAGER_H_
#define _NVS_MANAGER_H_

#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"



extern void store_wifi_config_in_nvs(const char *ssid, const char *password);

extern void get_wifi_config_from_nvs(char *ssid,
                                     size_t ssid_size,
                                     char *password,
                                     size_t password_size);

#endif