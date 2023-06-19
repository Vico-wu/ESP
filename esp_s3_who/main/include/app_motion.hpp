#pragma once

#include "__base__.hpp"
#include "app_camera.hpp"
#include "app_button.hpp"
#include "app_speech.hpp"
//创建动作类,将Observer以及Frame作为父类
class AppMotion : public Observer, public Frame
{
private:
    AppButton *key;     //动作类的私有属性有按键类
    AppSpeech *speech;  //动作类的私有属性有语音识别类

public:
    bool switch_on;     //动作的开关属性
//构造函数,信息与人脸类一样,具有回调函数
    AppMotion(AppButton *key,
              AppSpeech *speech,
              QueueHandle_t queue_i = nullptr,
              QueueHandle_t queue_o = nullptr,
              void (*callback)(camera_fb_t *) = esp_camera_fb_return);

    void update();//虚函数的实现

    void run();//创建主Task
};
