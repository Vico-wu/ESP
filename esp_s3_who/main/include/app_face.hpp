#pragma once

#include "sdkconfig.h"

#include "human_face_detect_msr01.hpp"
#include "human_face_detect_mnp01.hpp"
#include "face_recognition_tool.hpp"
#if CONFIG_MFN_V1
#if CONFIG_S8
#include "face_recognition_112_v1_s8.hpp"
#elif CONFIG_S16
#include "face_recognition_112_v1_s16.hpp"
#endif
#endif

#include "__base__.hpp"
#include "app_camera.hpp"
#include "app_button.hpp"
#include "app_speech.hpp"

typedef enum
{
    FACE_IDLE = 0,          //空闲状态
    FACE_ENROLL = 1,        //注册人脸
    FACE_RECOGNIZE = 2,     //识别人脸
    FACE_DELETE = 3,        //删除人脸
} face_action_t;
//创建人脸类,将Observer以及Frame作为父类
class AppFace : public Observer, public Frame
{
private:
    AppButton *key;     //人脸类的私有属性有按键类
    AppSpeech *speech;  //人脸类的私有属性有语音识别类

public:
    HumanFaceDetectMSR01 detector;          //公共属性有HumanFaceDetectMSR01 人脸识别类,这是一个静态库文件,无法查看源码,只有接口文件
    HumanFaceDetectMNP01 detector2;         //公共属性有HumanFaceDetectMSR01 人脸识别类,这是一个静态库文件,无法查看源码,只有接口文件

#if CONFIG_MFN_V1                           //人脸识别模型配置档,默认只有MFN_V1 模型,无其他选项
#if CONFIG_S8                               //人脸识别模型配置档,默认选择S8,意思为8位量化,尚不清楚区别
    FaceRecognition112V1S8 *recognizer;     //输入尺寸112*112*3
#elif CONFIG_S16                            //人脸识别模型配置档,默认选择S8,意思为8位量化,尚不清楚区别
    FaceRecognition112V1S16 *recognizer;    //输入尺寸112*112*3
#endif
#endif

    face_info_t recognize_result;   // 人脸信息识别结果结构体,内容有ID 名字,以及浮点类型的相似度
    face_action_t state;            // 人脸识别状态
    face_action_t state_previous;   // 上次的人脸识别状态

    bool switch_on;

    uint8_t frame_count;
    //人脸识别构造函数,构造函数初始化了 队列输入输出以及camera 的回调函数
    AppFace(AppButton *key,
            AppSpeech *speech,
            QueueHandle_t queue_i = nullptr,
            QueueHandle_t queue_o = nullptr,
            void (*callback)(camera_fb_t *) = esp_camera_fb_return);
    ~AppFace();
    //update 的代码实现,继承 Observer
    void update();
    //创建人脸识别Task
    void run();
};
