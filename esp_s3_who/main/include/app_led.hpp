#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "app_button.hpp"
#include "app_speech.hpp"

class AppLED : public Observer
{
private:
    const gpio_num_t pin;       //LED引脚号的枚举
    AppButton *key;             //按钮类
    AppSpeech *sr;              //语音识别类

public:
    AppLED(const gpio_num_t pin, AppButton *key, AppSpeech *sr);//构造函数,输入IO号,输入按钮类,输入语音识别类

    void update();//基类虚函数的实现
};
