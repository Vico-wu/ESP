#ifndef _TIME_SYNC_C_
#define _TIME_SYNC_C_C

#include <stdio.h>
#include "time_sync.h"
/********************************************************************************************************/
#define TAG "TIME_SYNC"
/********************************************************************************************************/
static void time_sync_notification_cb(struct timeval *tv);
static void initialize_sntp(void);
static void set_timezone(const char *timezone);
/********************************************************************************************************/
void obtain_time(const char *timezone)
{
    set_timezone(timezone);
    initialize_sntp();
    // wait for time to be set
    int retry = 0;
    const int retry_count = 30;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count)
    {
        ESP_LOGI("Time", "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
/********************************************************************************************************/
static void initialize_sntp(void)
{
    ESP_LOGI("Time", "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    sntp_init();
}
/********************************************************************************************************/
static void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI("Time", "Notification of a time synchronization event");
}
/********************************************************************************************************/
static void set_timezone(const char *timezone)
{
    if (setenv("TZ", timezone, 1) < 0)
    {
        ESP_LOGE(TAG, "Failed to set timezone");
    }
    tzset(); // 更新时区设置
}
/********************************************************************************************************/
#endif