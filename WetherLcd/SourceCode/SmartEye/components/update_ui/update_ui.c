#include <stdio.h>
#include "update_ui.h"
#define TAG "UI UPDATE"
void update_ui(void *prt)
{
    ESP_LOGI(TAG, "update_ui task was created");
    time_t now;
    struct tm timeinfo;
    char year[3];  // 年份的后两位需要2个字符 + 1个空字符
    char month[3]; // 月份需要2个字符 + 1个空字符
    char date[3];  // 日期需要2个字符 + 1个空字符
    char Hour[3];
    char Min[3];
    char Sec[3];
    char *WeekDays[] = {"日", "一", "二", "三", "四", "五", "六"};

    while (1)
    {

        time(&now);
        localtime_r(&now, &timeinfo);
        int fullYear = timeinfo.tm_year + 1900;
        int lastTwoDigitsOfYear = fullYear % 100;
        snprintf(year, sizeof(year), "%02d", lastTwoDigitsOfYear);
        snprintf(month, sizeof(month), "%02d", timeinfo.tm_mon + 1);
        snprintf(date, sizeof(date), "%02d", timeinfo.tm_mday);
        snprintf(Hour, sizeof(Hour), "%02d", timeinfo.tm_hour);
        snprintf(Min, sizeof(Min), "%02d", timeinfo.tm_min);
        snprintf(Sec, sizeof(Sec), "%02d", timeinfo.tm_sec);

        lvgl_port_lock(0);
        lv_label_set_text(ui_LableTimeYear, year);
        lv_label_set_text(ui_LableTimeMonth, month);
        lv_label_set_text(ui_LableTimeDate, date);
        lv_label_set_text(ui_LableHour, Hour);
        lv_label_set_text(ui_LableMin, Min);
        lv_label_set_text(ui_LableSec, Sec);
        // lv_label_set_text(ui_LableWeek, "四");
        if (timeinfo.tm_wday >= 0 && timeinfo.tm_wday <= 6)
        {
            const char *CurrentWeekDay = WeekDays[timeinfo.tm_wday];
            lv_label_set_text(ui_LableWeek, CurrentWeekDay);
        }
        else
        {
            // 输出错误信息或者进行其他错误处理
            ESP_LOGE("Error", "Invalid week day");
        }
        lvgl_port_unlock();
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
void update_sntp(void *ptr)
{
    ESP_LOGI(TAG, "update_sntp task was created");
    while (1)
    {
        obtain_time("CST-8");
        vTaskDelay(1000 * 3600 / portTICK_PERIOD_MS);
    }
}
void changeWeatherIcon(int index)
{
    if (weather_s.weather_day)
    {
        switch (index)
        {
        default:
            lv_img_set_src(ui_ImgWhether, &ui_img_710175435);
            break;
        case 0:
            lv_img_set_src(ui_ImgWhether, &ui_img_1596713459);
            break;
        case 1:
            lv_img_set_src(ui_ImgWhether, &ui_img_1768248264);
            break;
        case 2:
            lv_img_set_src(ui_ImgWhether, &ui_img_157055255);
            break;
        case 3:
            lv_img_set_src(ui_ImgWhether, &ui_img_712580050);
            break;
        case 4:
            lv_img_set_src(ui_ImgWhether, &ui_img_1936254497);
            break;
        case 5:
            lv_img_set_src(ui_ImgWhether, &ui_img_1006248924);
            break;
        case 6:
            lv_img_set_src(ui_ImgWhether, &ui_img_604944085);
            break;
        case 7:
            lv_img_set_src(ui_ImgWhether, &ui_img_49419290);
            break;
        case 8:
            lv_img_set_src(ui_ImgWhether, &ui_img_72714779);
            break;
        case 9:
            lv_img_set_src(ui_ImgWhether, &ui_img_1002720352);
            break;
        case 10:
            lv_img_set_src(ui_ImgWhether, &ui_img_1720731786);
            break;
        case 11:
            lv_img_set_src(ui_ImgWhether, &ui_img_1720837355);
            break;
        case 12:
            lv_img_set_src(ui_ImgWhether, &ui_img_1720799304);
            break;
        case 13:
            lv_img_set_src(ui_ImgWhether, &ui_img_1720905129);
            break;
        case 14:
            lv_img_set_src(ui_ImgWhether, &ui_img_1720875278);
            break;
        case 15:
            lv_img_set_src(ui_ImgWhether, &ui_img_1720980847);
            break;
        case 16:
            lv_img_set_src(ui_ImgWhether, &ui_img_1720942796);
            break;
        case 17:
            lv_img_set_src(ui_ImgWhether, &ui_img_1721048621);
            break;
        case 18:
            lv_img_set_src(ui_ImgWhether, &ui_img_1721018770);
            break;
        case 19:
            lv_img_set_src(ui_ImgWhether, &ui_img_1721124339);
            break;
        case 20:
            lv_img_set_src(ui_ImgWhether, &ui_img_1793650149);
            break;
        case 21:
            lv_img_set_src(ui_ImgWhether, &ui_img_1793755718);
            break;
        case 22:
            lv_img_set_src(ui_ImgWhether, &ui_img_1793717667);
            break;
        case 23:
            lv_img_set_src(ui_ImgWhether, &ui_img_1793823492);
            break;
        case 24:
            lv_img_set_src(ui_ImgWhether, &ui_img_1793793641);
            break;
        case 25:
            lv_img_set_src(ui_ImgWhether, &ui_img_1793899210);
            break;
        case 26:
            lv_img_set_src(ui_ImgWhether, &ui_img_1793861159);
            break;
        case 27:
            lv_img_set_src(ui_ImgWhether, &ui_img_1793966984);
            break;
        case 28:
            lv_img_set_src(ui_ImgWhether, &ui_img_1793937133);
            break;
        case 29:
            lv_img_set_src(ui_ImgWhether, &ui_img_1794042702);
            break;
        case 30:
            lv_img_set_src(ui_ImgWhether, &ui_img_775672896);
            break;
        case 31:
            lv_img_set_src(ui_ImgWhether, &ui_img_775567327);
            break;
        case 32:
            lv_img_set_src(ui_ImgWhether, &ui_img_775605378);
            break;
        case 33:
            lv_img_set_src(ui_ImgWhether, &ui_img_775499553);
            break;
        case 34:
            lv_img_set_src(ui_ImgWhether, &ui_img_775529404);
            break;
        case 35:
            lv_img_set_src(ui_ImgWhether, &ui_img_775423835);
            break;
        case 36:
            lv_img_set_src(ui_ImgWhether, &ui_img_775461886);
            break;
        case 37:
            lv_img_set_src(ui_ImgWhether, &ui_img_775356061);
            break;
        case 38:
            lv_img_set_src(ui_ImgWhether, &ui_img_775385912);
            break;
        }
    }
}
void update_weather(void *ptr)
{
    time_t now;
    struct tm timeinfo;
    while (1)
    {
        time(&now);
        localtime_r(&now, &timeinfo);
        lvgl_port_lock(0);
        if (timeinfo.tm_hour >= 6 && timeinfo.tm_hour < 18)
        {
            lv_label_set_text(ui_LableWhether, weather_s.weather_day);
            changeWeatherIcon(weather_s.code_day);
        }
        else
        {
            lv_label_set_text(ui_LableWhether, weather_s.weather_night);
            changeWeatherIcon(weather_s.code_night);
        }
        lv_label_set_text(ui_LableTempLo, weather_s.tempLow);
        lv_label_set_text(ui_LableTempHi, weather_s.tempHigh);
        lvgl_port_unlock();
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
void update_ui_run(void)
{
    xTaskCreate(update_sntp, "SNTP SYNC", 1024 * 5, NULL, 1, NULL);
    xTaskCreate(update_ui, "UPDATE UI", 1024 * 5, NULL, 1, NULL);
    xTaskCreate(update_weather, "UPDATE WEATHER", 1024 * 10, NULL, 1, NULL);
}