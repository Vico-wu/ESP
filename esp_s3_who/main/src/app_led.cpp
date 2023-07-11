#include "app_led.hpp"

#include "esp_log.h"

const static char TAG[] = "App/LED";

typedef enum
{
    LED_ALWAYS_OFF = 0,     //常关模式
    LED_ALWAYS_ON = 1,      //常开模式
    LED_OFF_1S = 2,         
    LED_OFF_2S = 3,         
    LED_OFF_4S = 4,
    LED_ON_1S = 5,
    LED_ON_2S = 6,
    LED_ON_4S = 7,
    LED_BLINK_1S = 8,       //1S闪烁模式
    LED_BLINK_2S = 9,       //2S闪烁模式
    LED_BLINK_4S = 10,      //4S闪烁模式
} led_mode_t;
//构造函数初始化IO,按钮类,语音识别类
AppLED::AppLED(const gpio_num_t pin, AppButton *key, AppSpeech *sr) : pin(pin), key(key), sr(sr)
{
    // initialize GPIO
    gpio_config_t gpio_conf;                    //实例化GPIOS 结构体 
    gpio_conf.mode = GPIO_MODE_OUTPUT_OD;       //配置引脚模式为开漏输出
    gpio_conf.pull_up_en = GPIO_PULLUP_ENABLE;  //使能输出端的上拉电阻

    gpio_conf.intr_type = GPIO_INTR_DISABLE;    //取消激活GPIO中断
    gpio_conf.pin_bit_mask = 1LL << this->pin;  //设定位掩码,将掩码设置为将当前PIN引脚号左移对应位
    gpio_config(&gpio_conf);                    //初始化GPIO

    gpio_set_level(this->pin, 0);               //设定当前引脚的输出电平,0 是低电平, 1是高电平,当前设置为低电平输出
}
//基类更新函数的实现代码
void AppLED::update()
{
    //设定LED的工作模式,类型为自定义,初始化LED工作模式为常关闭
    led_mode_t mode = LED_ALWAYS_OFF;

    // 获取按键状态,判定按键是否被按下,如果按键被按下,LED的状态修改为1S闪烁模式
    if (this->key->pressed)
    {
        mode = LED_BLINK_1S;
    }
    // 如果语音识别模块检测到,并且指令为什么都没检测到,LED切换为1S闪烁模式,如果不是什么都没检测到,LED就切换为常亮模式
    else if (this->sr->detected)
    {
        if (this->sr->command > COMMAND_NOT_DETECTED)
            mode = LED_BLINK_1S;
        else
            mode = LED_ALWAYS_ON;
    }

    // 基于获取的LED的不同的工作模式设定LED灯的闪烁状态
    switch (mode)
    {
    case LED_ALWAYS_OFF:
        gpio_set_level(this->pin, 0);
        break;
    case LED_ALWAYS_ON:
        gpio_set_level(this->pin, 1);
        break;
    case LED_OFF_1S:
        gpio_set_level(this->pin, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(this->pin, 1);
        break;
    case LED_OFF_2S:
        gpio_set_level(this->pin, 0);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        gpio_set_level(this->pin, 1);
        break;
    case LED_OFF_4S:
        gpio_set_level(this->pin, 0);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        gpio_set_level(this->pin, 1);
        break;
    case LED_ON_1S:
        gpio_set_level(this->pin, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(this->pin, 0);
        break;
    case LED_ON_2S:
        gpio_set_level(this->pin, 1);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        gpio_set_level(this->pin, 0);
        break;
    case LED_ON_4S:
        gpio_set_level(this->pin, 1);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        gpio_set_level(this->pin, 0);
        break;
    case LED_BLINK_1S:
        for (int i = 0; i < 2; ++i)
        {
            gpio_set_level(this->pin, 1);
            vTaskDelay(250 / portTICK_PERIOD_MS);
            gpio_set_level(this->pin, 0);
            vTaskDelay(250 / portTICK_PERIOD_MS);
        }
        break;
    case LED_BLINK_2S:
        for (int i = 0; i < 4; ++i)
        {
            gpio_set_level(this->pin, 1);
            vTaskDelay(250 / portTICK_PERIOD_MS);
            gpio_set_level(this->pin, 0);
            vTaskDelay(250 / portTICK_PERIOD_MS);
        }
        break;
    case LED_BLINK_4S:
        for (int i = 0; i < 8; ++i)
        {
            gpio_set_level(this->pin, 1);
            vTaskDelay(250 / portTICK_PERIOD_MS);
            gpio_set_level(this->pin, 0);
            vTaskDelay(250 / portTICK_PERIOD_MS);
        }
        break;

    default:
        break;
    }
}