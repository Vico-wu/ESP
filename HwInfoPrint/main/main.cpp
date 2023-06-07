#include <stdio.h>
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"

// Add variable
const char *Hw = "Hardware";
const char *Sw = "Software";
esp_chip_info_t ChipInfo;
char *ChipType;
extern "C" void app_main(void)
{
    esp_chip_info(&ChipInfo);
    switch (ChipInfo.model)
    {
    case CHIP_ESP32:
        ChipType = "ESP32";
        break;
    case CHIP_ESP32S2:
        ChipType = "ESP32S2";
        break;
    case CHIP_ESP32S3:
        ChipType = "ESP32S3";
        break;
    case CHIP_ESP32C3:
        ChipType = "ESP32C3";
        break;
    case CHIP_ESP32H2:
        ChipType = "ESP32H2";
        break;
    default:
        ChipType = "UNKNOWN";
        break;
    }
    ESP_LOGI(Hw, "芯片型号 : %s", ChipType);
    ESP_LOGI(Hw, "CPU 核心数: %d", ChipInfo.cores);
    ESP_LOGI(Hw, "CPU 版本号: %d", ChipInfo.full_revision);
    ESP_LOGI(Hw, "Embedded Flash Memory: %s", ChipInfo.features & CHIP_FEATURE_EMB_FLASH ? "True" : "False");
    ESP_LOGI(Hw, "2.4GHz WiFi: %s", ChipInfo.features & CHIP_FEATURE_WIFI_BGN ? "True" : "False");
    ESP_LOGI(Hw, "Bluetooth Low Energy: %s", ChipInfo.features & CHIP_FEATURE_BLE ? "True" : "False");
    ESP_LOGI(Hw, "Bluetooth Classic: %s", ChipInfo.features & CHIP_FEATURE_BT ? "True" : "False");
    ESP_LOGI(Hw, "IEEE 802.15.4: %s", ChipInfo.features & CHIP_FEATURE_IEEE802154 ? "True" : "False");
    ESP_LOGI(Hw, "Embedded Psram: %s", ChipInfo.features & CHIP_FEATURE_EMB_PSRAM ? "True" : "False");
    ESP_LOGI(Hw, "Flash 容量: %d MBytes", spi_flash_get_chip_size()/(1024*1024));
    ESP_LOGI(Hw, "Flash 容量: %d MBits", spi_flash_get_chip_size()*8/(1024*1024));
    uint8_t mac[6];
    esp_efuse_mac_get_default(mac);
    ESP_LOGI(Hw,"MAC 地址是 : %02X-%02X-%02X-%02X-%02X-%02X",*mac,*(mac+1),*(mac+2),*(mac+3),*(mac+4),*(mac+5));
    ESP_LOGI(Sw, "IDF 版本号: %s", esp_get_idf_version());
}
