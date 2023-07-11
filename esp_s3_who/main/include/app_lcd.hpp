#pragma once

#include "screen_driver.h"

#include "__base__.hpp"
#include "app_camera.hpp"
#include "app_button.hpp"
#include "app_speech.hpp"

#define BOARD_LCD_MOSI 47       //SPI总线主机发从机引脚号MOSI                                    
#define BOARD_LCD_MISO -1       //SPI总线从机发主机引脚号MISO
#define BOARD_LCD_SCK 21        //SPI总线时钟引脚号
#define BOARD_LCD_CS 44         
#define BOARD_LCD_DC 43
#define BOARD_LCD_RST -1
#define BOARD_LCD_BL 48
#define BOARD_LCD_PIXEL_CLOCK_HZ (40 * 1000 * 1000)
#define BOARD_LCD_BK_LIGHT_ON_LEVEL 0
#define BOARD_LCD_BK_LIGHT_OFF_LEVEL !BOARD_LCD_BK_LIGHT_ON_LEVEL
#define BOARD_LCD_H_RES 240
#define BOARD_LCD_V_RES 240
#define BOARD_LCD_CMD_BITS 8
#define BOARD_LCD_PARAM_BITS 8
#define LCD_HOST SPI2_HOST
//LCD类,基于更新基类,框架类
class AppLCD : public Observer, public Frame
{
private:
    AppButton *key;         //私有属性按钮类
    AppSpeech *speech;      //私有属性语音识别类

public:
    scr_driver_t driver;    //定义一个显示屏数据类,类里面有很多信息
    bool switch_on;         //定义一个开关
    bool paper_drawn;       //定义一个画图片的开关
//LCD类的构造函数,初始化信息按钮类,语音识别类,输入队列,输出队列,以及一个帧信息的回调函数
    AppLCD(AppButton *key,
           AppSpeech *speech,
           QueueHandle_t xQueueFrameI = nullptr,
           QueueHandle_t xQueueFrameO = nullptr,
           void (*callback)(camera_fb_t *) = esp_camera_fb_return);
//定义一个画图函数
    void draw_wallpaper();
//定义一个画颜色函数
    void draw_color(int color);
//基类更新的的实现函数
    void update();
//创建TASK的函数
    void run();
};
