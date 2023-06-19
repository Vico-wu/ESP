#include "app_motion.hpp"

#include "esp_log.h"
#include "esp_camera.h"

#include "dl_image.hpp"

static const char TAG[] = "App/Motion";
//AppMotion 构造函数,类初始化
AppMotion::AppMotion(AppButton *key,
                     AppSpeech *speech,
                     QueueHandle_t queue_i,
                     QueueHandle_t queue_o,
                     void (*callback)(camera_fb_t *)) : Frame(queue_i, queue_o, callback),
                                                        key(key),
                                                        speech(speech),
                                                        switch_on(false) {}

void AppMotion::update()
{
    if (this->key->pressed > BUTTON_IDLE)//按钮被按下
    {
        if (this->key->pressed == BUTTON_MENU)//如果是菜单按钮被按下
        {
            this->switch_on = (this->key->menu == MENU_MOTION_DETECTION) ? true : false;//判定菜单是否等于MENU_MOTION_DETECTION,如果是switch_on =true
            ESP_LOGD(TAG, "%s", this->switch_on ? "ON" : "OFF");
        }
    }

    if (this->speech->command > COMMAND_NOT_DETECTED)//如果语音识别为检测到指令
    {
        if (this->speech->command >= MENU_STOP_WORKING && this->speech->command <= MENU_MOTION_DETECTION)//判定语音指令是否为停止工作
        {
            this->switch_on = (this->speech->command == MENU_MOTION_DETECTION) ? true : false;//如果语音识别指令为运动物体识别,则打开开关
            ESP_LOGD(TAG, "%s", this->switch_on ? "ON" : "OFF");
        }
    }
}

static void task(AppMotion *self)
{
    ESP_LOGD(TAG, "Start");
    while (true)
    {
        if (self->queue_i == nullptr)
            break;

        camera_fb_t *frame1 = NULL;
        camera_fb_t *frame2 = NULL;
        if (xQueueReceive(self->queue_i, &frame1, portMAX_DELAY))//如果队列中有信息
        {
            if (self->switch_on)//如果运动识别开关为打开状态
            {
                if (xQueueReceive(self->queue_i, &frame2, portMAX_DELAY))//如果队列中有信息
                {
                    //执行运动识别,检测步幅为8,激活检测阈值为15,返回值为激活的检测点数
                    uint32_t moving_point_number = dl::image::get_moving_point_number((uint16_t *)frame1->buf, (uint16_t *)frame2->buf, frame1->height, frame1->width, 8, 15);
                    if (moving_point_number > 50)//如果检测点数大于50,则认定为物体移动,并在图片中化矩形,矩形起始坐标左上角(0,0),矩形终点坐标(20,20),颜色未设置,默认值为0x00FF0000
                    {
                        ESP_LOGI(TAG, "Something moved!");
                        dl::image::draw_filled_rectangle((uint16_t *)frame1->buf, frame1->height, frame1->width, 0, 0, 20, 20);
                    }

                    self->callback(frame2);//执行回调函数,返回帧缓存地址,用于重用
                }
            }

            if (self->queue_o)//如果输出队列不为空,则发送队列
                xQueueSend(self->queue_o, &frame1, portMAX_DELAY);
            else
                self->callback(frame1);//获取当前帧的缓存地址
        }
    }
    ESP_LOGD(TAG, "Stop");//这两行代码永远不会执行
    vTaskDelete(NULL);//删除任务
}
// 创建Task
void AppMotion::run()
{
    xTaskCreatePinnedToCore((TaskFunction_t)task, TAG, 3 * 1024, this, 5, NULL, 0);
}