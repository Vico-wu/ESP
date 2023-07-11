#ifndef _NVS_MANAGER_C_
#define _NVS_MANAGER_C_

#include <stdio.h>
#include "nvs_manager.h"
/********************************************************************************************************/
#define TAG "NVS"
#define NVS_NAMESPACE "wifi_config"
/********************************************************************************************************/
void store_wifi_config_in_nvs(const char *ssid, const char *password)
{
    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "ssid", ssid));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "password", password));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
}
/********************************************************************************************************/
void get_wifi_config_from_nvs(char *ssid, size_t ssid_size, char *password, size_t password_size)
{
    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle));
    size_t len;
    ESP_ERROR_CHECK(nvs_get_str(nvs_handle, "ssid", NULL, &len));
    ESP_ERROR_CHECK(nvs_get_str(nvs_handle, "ssid", ssid, &len));
    ESP_ERROR_CHECK(nvs_get_str(nvs_handle, "password", NULL, &len));
    ESP_ERROR_CHECK(nvs_get_str(nvs_handle, "password", password, &len));
    nvs_close(nvs_handle);
}
/********************************************************************************************************/
#endif