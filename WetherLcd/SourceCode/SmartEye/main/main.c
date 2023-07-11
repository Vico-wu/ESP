#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_manager.h"
#include "time_sync.h"
#include "whether_sync.h"
#include "wifi_manager.h"
#include "ui.h"
#include "bsp/esp32_s3_eye.h"
#include "update_ui.h"
#include "ui_manager.h"
/********************************************************************************************************/
#define TAG "MAIN"
/********************************************************************************************************/
void app_lvgl_display(void)
{
    bsp_display_lock(0);
    ui_init();
    bsp_display_unlock();
}
void printTaskList(void)
{
    char taskListBuffer[1024];

    vTaskList(taskListBuffer);
    printf("**********************************************\n");
    printf("Task         State   Prio    Stack     Num\n");
    printf("**********************************************\n");
    printf("Task List:\n%s\n", taskListBuffer);
    printf("**********************************************\n");
}
void printRunTimeStats(void)
{
    char runTimeStatsBuffer[1024];

    vTaskGetRunTimeStats(runTimeStatsBuffer);

    printf("Run Time Stats:\n%s\n", runTimeStatsBuffer);
}
/********************************************************************************************************/
void app_main(void)
{
    /* Initialize display and LVGL */
    bsp_display_start();
    /* Turn on display backlight */
    bsp_display_backlight_on();
    /* Add and show objects on display */
    app_lvgl_display();
    ESP_ERROR_CHECK(nvs_flash_init());
    store_wifi_config_in_nvs("Vodafone-A0E8", "rUAHFabBCDMqdET8");
    wifi_init_sta();
    ESP_LOGI(TAG, "Example initialization done.");
    
    
    printTaskList();
    printRunTimeStats();
}
/********************************************************************************************************/