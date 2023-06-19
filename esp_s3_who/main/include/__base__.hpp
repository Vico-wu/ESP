#pragma once

#include <list>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_camera.h"

typedef enum
{
    COMMAND_TIMEOUT = -2,       //超时
    COMMAND_NOT_DETECTED = -1,  //未检测到

    MENU_STOP_WORKING = 0,      //停止工作
    MENU_DISPLAY_ONLY = 1,      //只显示
    MENU_FACE_RECOGNITION = 2,  //人脸识别
    MENU_MOTION_DETECTION = 3,  //运动识别

    ACTION_ENROLL = 4,
    ACTION_DELETE = 5,          //删除
    ACTION_RECOGNIZE = 6
} command_word_t;

// 纯虚函数,基类 用于更新,类名称译为观察者
class Observer
{
public:
    virtual void update() = 0;
};

// 主题类, 属性是一个容器,用于存储元素的双向链表。它可以在任意位置插入或删除元素，具有高效的插入和删除操作。
class Subject
{
private:
    std::list<Observer *> observers;//以基类Observer作为数据类型创建列表容器,需要实现update

public:
    // 尾部插入元素
    void attach(Observer *observer)
    {
        this->observers.push_back(observer);
    }
    // 删除满足条件为observer的所有元素
    void detach(Observer *observer)
    {
        this->observers.remove(observer);
    }
    //将容器中的所有元素都删除,使其变为空列表
    void detach_all()
    {
        this->observers.clear();
    }
    //对每个元素observer 执行update, update 的实现需要在其子类中实现代码
    void notify()
    {
        for (auto observer : this->observers)
            observer->update();
    }
};
//框架类,公共属性有 两个队列句柄,以及一个回调函数
//构造函数将队列句柄以及回调函数初始化
//set_io 函数用于将句柄设置为输入和输出
class Frame
{
public:
    QueueHandle_t queue_i;
    QueueHandle_t queue_o;
    void (*callback)(camera_fb_t *);

    Frame(QueueHandle_t queue_i = nullptr,
          QueueHandle_t queue_o = nullptr,
          void (*callback)(camera_fb_t *) = nullptr) : queue_i(queue_i),
                                                       queue_o(queue_o),
                                                       callback(callback) {}

    void set_io(QueueHandle_t queue_i, QueueHandle_t queue_o)
    {
        this->queue_i = queue_i;
        this->queue_o = queue_o;
    }
};
