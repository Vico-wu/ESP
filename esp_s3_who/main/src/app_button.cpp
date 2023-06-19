#include "app_button.hpp"

#include <stdio.h>
#include <stdlib.h>

#include "driver/adc_common.h"
#include "esp_log.h"
#include "esp_adc_cal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ADC Channels
#define ADC1_EXAMPLE_CHAN0 ADC1_CHANNEL_0
// ADC Attenuation
#define ADC_EXAMPLE_ATTEN ADC_ATTEN_DB_11
// ADC Calibration
#if CONFIG_IDF_TARGET_ESP32
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_VREF
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32C3
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32S3
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP_FIT
#endif

#define PRESS_INTERVAL 500000

static const char *TAG = "App/Button";
//Button 构造函数,初始化key_config,并设定每个按钮的功能以及最小电压和最大电压,将按钮状态修改为IDLE
AppButton::AppButton() : key_configs({{BUTTON_MENU, 2800, 3000}, {BUTTON_PLAY, 2250, 2450}, {BUTTON_UP, 300, 500}, {BUTTON_DOWN, 850, 1050}}),
                         pressed(BUTTON_IDLE)
{
    //配置ADC1模块的位宽
    ESP_ERROR_CHECK(adc1_config_width((adc_bits_width_t)ADC_WIDTH_BIT_DEFAULT));
    //配置 ADC1 通道的增益，增益值越大，测量的电压范围越小，但精度越高。增益值的选择应根据具体应用场景和所测量的电压范围来确定
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_EXAMPLE_CHAN0, ADC_EXAMPLE_ATTEN));
}

static void task(AppButton *self)
{
    //获取系统启动以来的时间，以微秒为单位
    int64_t backup_time = esp_timer_get_time();
    int64_t last_time = esp_timer_get_time();

    uint8_t menu_count = 0;

    while (true)
    {
        uint32_t voltage = adc1_get_raw(ADC1_EXAMPLE_CHAN0);//获取ADC1通道的原始测量值
        backup_time = esp_timer_get_time();//获取系统启动以来的时间，以微秒为单位
        for (auto key_config : self->key_configs)
        {
            if ((voltage >= key_config.min) && (voltage <= key_config.max))// 电压介于最小值或者最大值之间
            {
                if (((backup_time - last_time) > PRESS_INTERVAL))//按键去抖动,500ms
                {
                    self->pressed = key_config.key; //将符合电压区间的按钮赋值给pressed
                    ESP_LOGI(TAG, "Button[%d] is clicked", self->pressed); //打印输出哪个按钮被按下了

                    if (self->pressed == BUTTON_MENU)
                    {
                        self->menu++;
                        self->menu %= (MENU_MOTION_DETECTION + 1); //求模运算,限定menu 只能等于0 1 2 3,这里应该使用的轮询机制
                    }

                    last_time = backup_time;//更新上次的时间
                    self->notify();

                    self->pressed = BUTTON_IDLE; // 所有操作完成后,清空按钮按下的状态
                    break;
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
//创建任务
void AppButton::run()
{
    xTaskCreatePinnedToCore((TaskFunction_t)task, TAG, 3 * 1024, this, 5, NULL, 0);
}
