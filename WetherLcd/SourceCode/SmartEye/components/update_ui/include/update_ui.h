#ifndef _UPDATE_UI_H_
#define _UPDATE_UI_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ui.h"
#include <time.h>
#include "time_sync.h"
#include "whether_sync.h"
#include "esp_lvgl_port.h"

void update_sntp(void *ptr);
void update_ui(void *ptr);
void update_weather(void *ptr);
void update_ui_run(void);
#endif
