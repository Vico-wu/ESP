#ifndef _WIFI_MANAGER_H_
#define _WIFI_MANAGER_H_
/********************************************************************************************************/
#include "esp_event.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_manager.h"
#include "whether_sync.h"
#include "update_ui.h"
/********************************************************************************************************/
extern void wifi_event_handler(void *arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void *event_data);

extern void wifi_init_sta(void);
/********************************************************************************************************/

/********************************************************************************************************/
extern EventGroupHandle_t wifi_event_group;
/********************************************************************************************************/
#endif