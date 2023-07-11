#include "app_face.hpp"

#include <list>

#include "esp_log.h"
#include "esp_camera.h"

#include "dl_image.hpp"
#include "fb_gfx.h"

#include "who_ai_utils.hpp"

static const char TAG[] = "App/Face";

#define RGB565_MASK_RED 0xF800
#define RGB565_MASK_GREEN 0x07E0
#define RGB565_MASK_BLUE 0x001F

#define FRAME_DELAY_NUM 16
//猜测应该是在摄像头信息上添加字符串内容并设定字符串颜色
static void rgb_print(camera_fb_t *fb, uint32_t color, const char *str)
{
    fb_gfx_print(fb, (fb->width - (strlen(str) * 14)) / 2, 10, color, str);
}
//后面为可变参数
static int rgb_printf(camera_fb_t *fb, uint32_t color, const char *format, ...)
{
    char loc_buf[64];
    char *temp = loc_buf;
    int len;
    //创建可变参数列表
    va_list arg;
    //创建可变参数列表副本
    va_list copy;
    //初始化向量指针
    va_start(arg, format);
    va_copy(copy, arg);
    //格式化字符串
    len = vsnprintf(loc_buf, sizeof(loc_buf), format, arg);
    //清理可变参数列表副本
    va_end(copy);
    if (len >= sizeof(loc_buf))
    {
        //如果长度过大,动态申请内存空间,如果无法申请,函数返回
        temp = (char *)malloc(len + 1);
        if (temp == NULL)
        {
            return 0;
        }
    }
    //格式化字符串
    vsnprintf(temp, len + 1, format, arg);
    //清理可变参数列表
    va_end(arg);
    //在摄像头图像中添加字符串,位置为固定
    rgb_print(fb, color, temp);
    //如果长度大于64,释放动态内存
    if (len > 64)
    {
        free(temp);
    }
    return len;
}
//app构造函数的实现部分,初始化Frame,初始化按钮信息,初始化语音识别信息,初始化人脸识别,初始化人脸识别状态,将Swt 设置为关闭状态
//人脸识别第一个为低于阈值被过滤,第二个为高于阈值被过滤,第三个为保留前几个最高分项,第四个为调整图像输出比例
AppFace::AppFace(AppButton *key,
                 AppSpeech *speech,
                 QueueHandle_t queue_i,
                 QueueHandle_t queue_o,
                 void (*callback)(camera_fb_t *)) : Frame(queue_i, queue_o, callback),
                                                    key(key),
                                                    speech(speech),
                                                    detector(0.3F, 0.3F, 10, 0.3F),
                                                    detector2(0.4F, 0.3F, 10),
                                                    state(FACE_IDLE),
                                                    switch_on(false)
{
#if CONFIG_MFN_V1
#if CONFIG_S8
    this->recognizer = new FaceRecognition112V1S8();
#elif CONFIG_S16
    this->recognizer = new FaceRecognition112V1S16();
#endif
#endif
    //设定人脸识别信息到flash,或者从flash中读取人脸信息
    this->recognizer->set_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "fr");
    //设定来自flash的注册ID
    this->recognizer->set_ids_from_flash();
}

AppFace::~AppFace()
{
    delete this->recognizer;
}
//update 函数的实现
void AppFace::update()
{
    // 获取当前按钮状态
    if (this->key->pressed > BUTTON_IDLE)
    {
        //如果菜单按钮被按下
        if (this->key->pressed == BUTTON_MENU)
        {
            //人脸识别状态切换为IDLE
            this->state = FACE_IDLE;
            //如果按键功能当前为人脸识别,则打开开关
            this->switch_on = (this->key->menu == MENU_FACE_RECOGNITION) ? true : false;
            //打印诊断信息,人脸识别开关是否打开
            ESP_LOGD(TAG, "%s", this->switch_on ? "ON" : "OFF");
        }
        //如果Play 按钮被按下,则切换为人脸识别状态
        else if (this->key->pressed == BUTTON_PLAY)
        {
            this->state = FACE_RECOGNIZE;
        }
        //如果up按钮被按下,则切换为注册状态
        else if (this->key->pressed == BUTTON_UP)
        {
            this->state = FACE_ENROLL;
        }
        //如果down 按钮被按下,则切换为删除状态
        else if (this->key->pressed == BUTTON_DOWN)
        {
            this->state = FACE_DELETE;
        }
    }

    // 解析语音识别,如果检测检测到指令
    if (this->speech->command > COMMAND_NOT_DETECTED)
    {
        //如果当前指令为停止工作
        if (this->speech->command >= MENU_STOP_WORKING && this->speech->command <= MENU_MOTION_DETECTION)
        {
            this->state = FACE_IDLE;    //人脸识别状态切换到空闲
            this->switch_on = (this->speech->command == MENU_FACE_RECOGNITION) ? true : false; //如果语音识别指令为人脸识别,则打开开关
            ESP_LOGD(TAG, "%s", this->switch_on ? "ON" : "OFF");    //打印当前开关状态
        }
        else if (this->speech->command == ACTION_ENROLL)    //如果语音识别为注册动作
        {
            this->state = FACE_ENROLL;                      //切换人脸识别状态为注册人脸
        }
        else if (this->speech->command == ACTION_RECOGNIZE) //如果语音识别为识别动作
        {
            this->state = FACE_RECOGNIZE;                   //切换人脸识别状态为识别人脸
        }
        else if (this->speech->command == ACTION_DELETE)    //如果隐隐识别为删除动作
        {
            this->state = FACE_DELETE;                      //切换人脸识别状态为删除人脸
        }
    }
    ESP_LOGD(TAG, "Human face recognition state = %d", this->state);//debug模式打印人脸识别状态
}
//人脸识别主TASK
static void task(AppFace *self)
{
    ESP_LOGD(TAG, "Start");//debug 模式打印状态
    camera_fb_t *frame = nullptr; //注册摄像头结构体指针,并赋值为空指针

    while (true)
    {
        if (self->queue_i == nullptr) //如果类的输入队列为空,返回
            break;

        if (xQueueReceive(self->queue_i, &frame, portMAX_DELAY))//如果获取到队列内容,并赋值给摄像头结构体
        {
            if (self->switch_on)//如果人脸识别开关为打开状态
            {
                //调用库函数将识别结果赋值给detect_candidates
                std::list<dl::detect::result_t> &detect_candidates = self->detector.infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3});
                std::list<dl::detect::result_t> &detect_results = self->detector2.infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_candidates);

                if (detect_results.size())
                {
                    // print_detection_result(detect_results);
                    //调用库函数
                    draw_detection_result((uint16_t *)frame->buf, frame->height, frame->width, detect_results);
                }

                if (self->state)
                {
                    if (detect_results.size() == 1)
                    {
                        if (self->state == FACE_ENROLL) //如果当前人脸识别状态为注册人脸,则调用库函数注册人脸
                        {
                            self->recognizer->enroll_id((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_results.front().keypoint, "", true);
                            ESP_LOGI(TAG, "Enroll ID %d", self->recognizer->get_enrolled_ids().back().id);
                        }
                        else if (self->state == FACE_RECOGNIZE)//如果当前人脸识别状态为人脸识别,则条用库函数识别人脸
                        {
                            self->recognize_result = self->recognizer->recognize((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_results.front().keypoint);
                            // print_detection_result(detect_results);
                            ESP_LOGD(TAG, "Similarity: %f", self->recognize_result.similarity);//在debug 模式下打印识别率
                            if (self->recognize_result.id > 0)
                                ESP_LOGI(TAG, "Match ID: %d", self->recognize_result.id);//如果识别结果ID大于0,则打印当前ID
                            else
                                ESP_LOGI(TAG, "Match ID: %d", self->recognize_result.id);//否则打印当前识别结果的ID值
                        }
                    }

                    if (self->state == FACE_DELETE)//如果当前人脸识别状态为删除人脸,则调用函数删除人脸
                    {
                        vTaskDelay(10);
                        self->recognizer->delete_id(true);
                        ESP_LOGI(TAG, "%d IDs left", self->recognizer->get_enrolled_id_num());//打印删除的人脸ID
                    }

                    self->state_previous = self->state; //将当前人脸识别状态赋值到上次人脸识别状态中.
                    self->state = FACE_IDLE;            //将当前人脸识别状态清空为等待
                    self->frame_count = FRAME_DELAY_NUM;
                }

                // Write result on several frames of image
                if (self->frame_count)
                {
                    switch (self->state_previous)//获取上次人脸识别状态
                    {
                    case FACE_DELETE://如果上次人脸识别状态为删除人脸,在显示屏中打印被删除的人脸ID
                        rgb_printf(frame, RGB565_MASK_RED, "%d IDs left", self->recognizer->get_enrolled_id_num());
                        break;

                    case FACE_RECOGNIZE://如果上次人脸识别状态为识别人脸,在显示屏中显示识别的人脸ID
                        if (self->recognize_result.id > 0)//如果识别结果大于0,证明识别到人脸,打印识别到的人脸ID
                            rgb_printf(frame, RGB565_MASK_GREEN, "ID %d Similarity:%f", self->recognize_result.id,self->recognize_result.similarity);
                        else
                            rgb_print(frame, RGB565_MASK_RED, "who ?");//如果为识别到匹配的人脸结果,打印为who
                        break;

                    case FACE_ENROLL://如果上次的人脸识别状态为注册人脸,打印注册ID number
                        rgb_printf(frame, RGB565_MASK_BLUE, "Enroll: ID %d", self->recognizer->get_enrolled_ids().back().id);
                        break;

                    default:
                        break;
                    }

                    self->frame_count--;
                }
            }

            if (self->queue_o)//如果输出队列不为空,则发送队列
                xQueueSend(self->queue_o, &frame, portMAX_DELAY);
            else//如果为空,则执行callback function,返回帧缓存地址,用于重用
                self->callback(frame);
        }
    }
    ESP_LOGD(TAG, "Stop");//debug 模式大打印停止
    vTaskDelete(NULL);
}

void AppFace::run()
{
    xTaskCreatePinnedToCore((TaskFunction_t)task, TAG, 5 * 1024, this, 5, NULL, 1);
}