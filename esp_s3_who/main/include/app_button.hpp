#pragma once

#include <vector>

#include "__base__.hpp"

typedef enum
{
    BUTTON_IDLE = 0,
    BUTTON_MENU,
    BUTTON_PLAY,
    BUTTON_UP,
    BUTTON_DOWN
} button_name_t;

typedef struct
{
    button_name_t key; /**< button index on the channel */
    int min;           /**< min voltage in mv corresponding to the button */
    int max;           /**< max voltage in mv corresponding to the button */
} key_config_t;
//继承了主题类,具有容器列表,可以实现列表元素的添加或者删除,容器列表的数据类型又是基类Observer,所以可以实现纯虚函数update
class AppButton : public Subject
{
public:
    std::vector<key_config_t> key_configs;
    button_name_t pressed;

    uint8_t menu;

    AppButton();
    ~AppButton();

    void run();
};
