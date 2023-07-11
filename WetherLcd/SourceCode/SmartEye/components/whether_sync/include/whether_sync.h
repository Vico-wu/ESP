#ifndef _WHETHER_SYNC_H_
#define _WHETHER_SYNC_H_

#include "esp_http_client.h"
#include "cJSON.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"
void whether_sync(void *prt);
void whether_sync_run(void);
typedef struct weather_t
{
    char city[30];
    char tempHigh[5];
    char tempLow[5];
    char weather_day[20];
    char weather_night[20];
    char weathertomorrow[20];
    char huimidity[5];
    char wind_scale[5];
    char wind_speed[10];
    int code_day;
    int code_night;
} weather_ts;
extern weather_ts weather_s;
#endif