#include "app_lcd.hpp"

#include <string.h>

#include "esp_log.h"
#include "esp_camera.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "logo_en_240x240_lcd.h"

#include "app_custlogo.hpp"
static const char TAG[] = "App/LCD";
//LCD构造函数的实现,初始化按钮类,语音识别类,输入,输出队列,默认开关设置为关闭,初始化基于类Frame的构造函数
AppLCD::AppLCD(AppButton *key,
               AppSpeech *speech,
               QueueHandle_t queue_i,
               QueueHandle_t queue_o,
               void (*callback)(camera_fb_t *)) : Frame(queue_i, queue_o, callback),
                                                  key(key),
                                                  speech(speech),
                                                  switch_on(false)
{
    do //do{}while(0) 循环在这段作用域用于加入break关键字,当初始化执行过程中出现故障码,可以跳出当前作用域.
    {
        spi_config_t bus_conf = {
            .miso_io_num = (gpio_num_t)BOARD_LCD_MISO,
            .mosi_io_num = (gpio_num_t)BOARD_LCD_MOSI,
            .sclk_io_num = (gpio_num_t)BOARD_LCD_SCK,
            .max_transfer_sz = 2 * 240 * 240 + 10,
        };
        spi_bus_handle_t spi_bus = spi_bus_create(SPI2_HOST, &bus_conf);

        scr_interface_spi_config_t spi_lcd_cfg = {
            .spi_bus = spi_bus,
            .pin_num_cs = BOARD_LCD_CS,
            .pin_num_dc = BOARD_LCD_DC,
            .clk_freq = 40 * 1000000,
            .swap_data = 0,
        };

        scr_interface_driver_t *iface_drv;
        scr_interface_create(SCREEN_IFACE_SPI, &spi_lcd_cfg, &iface_drv);
        if (ESP_OK != scr_find_driver(SCREEN_CONTROLLER_ST7789, &this->driver))
        {
            ESP_LOGE(TAG, "screen find failed");
            break;
        }

        scr_controller_config_t lcd_cfg = {
            .interface_drv = iface_drv,
            .pin_num_rst = BOARD_LCD_RST,
            .pin_num_bckl = BOARD_LCD_BL,
            .rst_active_level = 0,
            .bckl_active_level = 0,
            .width = 240,
            .height = 240,
            .offset_hor = 0,
            .offset_ver = 0,
            .rotate = (scr_dir_t)0,
        };

        if (ESP_OK != this->driver.init(&lcd_cfg))
        {
            ESP_LOGE(TAG, "screen initialize failed");
            break;
        }

        scr_info_t lcd_info;
        this->driver.get_info(&lcd_info);
        ESP_LOGI(TAG, "Screen name:%s | width:%d | height:%d", lcd_info.name, lcd_info.width, lcd_info.height);

        this->draw_color(0x000000);
        vTaskDelay(pdMS_TO_TICKS(500));
        this->draw_wallpaper();
        vTaskDelay(pdMS_TO_TICKS(1000));
    } while (0);
}
//画图片函数
void AppLCD::draw_wallpaper()
{
    //计算需要的内存大小,并动态申请内存,如果申请成功,返回内存地址,  MALLOC_CAP_SPIRAM意为申请外部RAM,   MALLOC_CAP_8BIT意为分配的内存执行8bit对齐
    uint16_t *pixels = (uint16_t *)heap_caps_malloc((logo_en_240x240_lcd_width * logo_en_240x240_lcd_height) * sizeof(uint16_t), MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    if (NULL == pixels)
    {
        ESP_LOGE(TAG, "Memory for bitmap is not enough");
        return;
    }
    //将乐鑫LOGO图片信息拷贝到刚刚申请的内存中去
    // memcpy(pixels, logo_en_240x240_lcd, (logo_en_240x240_lcd_width * logo_en_240x240_lcd_height) * sizeof(uint16_t));
    //调用乐鑫官网的库填充像素点.
    // this->driver.draw_bitmap(0, 0, logo_en_240x240_lcd_width, logo_en_240x240_lcd_height, (uint16_t *)pixels);
    memcpy(pixels, gImage_aaa, (logo_en_240x240_lcd_width * logo_en_240x240_lcd_height) * sizeof(uint16_t));
    this->driver.draw_bitmap(0, 0, logo_en_240x240_lcd_width, logo_en_240x240_lcd_height, (uint16_t *)pixels);
    //释放刚刚申请的用于存储图片的动态内存
    heap_caps_free(pixels);

    this->paper_drawn = true;
}
//画颜色函数
void AppLCD::draw_color(int color)
{
    //实例化屏幕信息结构体为lcd_info
    scr_info_t lcd_info;
    //调用乐鑫官方的库获取屏幕信息
    this->driver.get_info(&lcd_info);
    //动态申请内存并返回内存指针,不明白为什么只有宽度信息
    uint16_t *buffer = (uint16_t *)malloc(lcd_info.width * sizeof(uint16_t));
    if (buffer)//如果内存申请成功
    {
        //向刚刚申请的动态内存中填充颜色信息,这样就生成了一行的屏幕填充数组
        for (size_t i = 0; i < lcd_info.width; i++)
        {
            buffer[i] = color;
        }
        //将刚刚的一行的屏幕填充数组采用for循环的形式填充其他行,从上往下一行一行填充
        for (int y = 0; y < lcd_info.height; y++)
        {
            this->driver.draw_bitmap(0, y, lcd_info.width, 1, buffer);
        }
        //释放刚刚申请的动态内存
        free(buffer);
    }
    else
    {
        ESP_LOGE(TAG, "Memory for bitmap is not enough");
    }
}
//基于基类虚函数更新的代码实现部分
void AppLCD::update()
{
    if (this->key->pressed > BUTTON_IDLE)   //如果按钮已经被按下
    {
        if (this->key->pressed == BUTTON_MENU)  //如果按下的是菜单按钮,如果菜单不是停止工作,就将开关设置为启动
        {
            this->switch_on = (this->key->menu == MENU_STOP_WORKING) ? false : true;
            ESP_LOGD(TAG, "%s", this->switch_on ? "ON" : "OFF");
        }
    }
    //如果语音识别结果为识别到指令,如果指令为只显示,或者人脸识别,或者运动检测,那么将开关设置为true
    if (this->speech->command > COMMAND_NOT_DETECTED)
    {
        if (this->speech->command >= MENU_STOP_WORKING && this->speech->command <= MENU_MOTION_DETECTION)
        {
            this->switch_on = (this->speech->command == MENU_STOP_WORKING) ? false : true;
            ESP_LOGD(TAG, "%s", this->switch_on ? "ON" : "OFF");
        }
    }
    //如果开关关闭,那么画图开关将被关闭
    if (this->switch_on == false)
    {
        this->paper_drawn = false;
    }
}

static void task(AppLCD *self)
{
    ESP_LOGD(TAG, "Start");
    //创建一个camera实例
    camera_fb_t *frame = nullptr;
    while (true)
    {
        if (self->queue_i == nullptr)
            break;
        //接受队列
        if (xQueueReceive(self->queue_i, &frame, portMAX_DELAY))
        {
            //如果开关状态为打开,画图片,图片继承于摄像头内容
            if (self->switch_on)
                self->driver.draw_bitmap(0, 0, frame->width, frame->height, (uint16_t *)frame->buf);
            //或者如果开关未开打,如果画图开关为关闭状态,画LOGO
            else if (self->paper_drawn == false)
                self->draw_wallpaper();
            //如果输出队列不为空,则将输出队列发送至队列
            if (self->queue_o)
                xQueueSend(self->queue_o, &frame, portMAX_DELAY);
            else
                //否则获取当前图像指针地址
                self->callback(frame);
        }
    }
    ESP_LOGD(TAG, "Stop");
    self->draw_wallpaper();
    vTaskDelete(NULL);
}

void AppLCD::run()
{
    xTaskCreatePinnedToCore((TaskFunction_t)task, TAG, 2 * 1024, this, 5, NULL, 1);
}