#include "driver/gpio.h"

#include "app_button.hpp"
#include "app_camera.hpp"
#include "app_lcd.hpp"
#include "app_led.hpp"
#include "app_motion.hpp"
#include "app_speech.hpp"
#include "app_face.hpp"

extern "C" void app_main()
{   
    //xQueueFrame_0是camera 队列的 handle, 内存大小为2,camera 会一直向这个队列写入数据.
    //同时AppFace会一直从这个队列中获取数据,并经过二次处理之后输出给xQueueFrame_1
    QueueHandle_t xQueueFrame_0 = xQueueCreate(2, sizeof(camera_fb_t *));
    //xQueueFrame_1 是AppFace 队列的handle, AppFace 会一直向这个队列中写入经过AppFace 处理过的xQueueFrame_0中的数据
    //同时AppMotion 会从这个队列中获取数据,并经过二次梳理之后输出给xQueueFrame_2
    QueueHandle_t xQueueFrame_1 = xQueueCreate(2, sizeof(camera_fb_t *));
    //xQueueFrame_2 是AppMotion 队列的handle, AppMotin 会一直向这个队列中写入数据
    //同时AppLcd H会从这个队列红获取数据,并最终处理后在LCD中显示
    QueueHandle_t xQueueFrame_2 = xQueueCreate(2, sizeof(camera_fb_t *));

    AppButton *key = new AppButton();
    AppSpeech *speech = new AppSpeech();
    AppCamera *camera = new AppCamera(PIXFORMAT_RGB565, FRAMESIZE_240X240, 2, xQueueFrame_0);
    AppFace *face = new AppFace(key, speech, xQueueFrame_0, xQueueFrame_1);
    AppMotion *motion = new AppMotion(key, speech, xQueueFrame_1, xQueueFrame_2);
    AppLCD *lcd = new AppLCD(key, speech, xQueueFrame_2);
    AppLED *led = new AppLED(GPIO_NUM_3, key, speech);

    key->attach(face);
    key->attach(motion);
    key->attach(led);
    key->attach(lcd);

    speech->attach(face);
    speech->attach(motion);
    speech->attach(led);
    speech->attach(lcd);
    
    //创建lcd task
    lcd->run(); 
    //创建动作 task
    motion->run();
    //创建人脸识别 Task
    face->run();
    //创建camera task
    camera->run();
    //创建语音识别task
    speech->run();
    //创建按钮识别task
    key->run();
}
