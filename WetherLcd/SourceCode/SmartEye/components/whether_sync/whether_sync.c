#ifndef _WHETHER_SYNC_C_
#define _WHETHER_SYNC_C_

#include <stdlib.h>
#include <stdio.h>
#include "whether_sync.h"
/********************************************************************************************************/
#define TAG "WHETHER_SYNC"
#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048
#define HOST "api.seniverse.com" // 网址
#define HTTP_PORT "80"
#define API_KEY "Sk3fyxgYjI1ZQyyFy" // API密钥
#define LOCATION "wuxi"             // 地点
#define LANGUAGE "zh-Hans"          // 简体中文
#define TEMPERATURE_UNIT "c"
weather_ts weather_s;
//"http://api.seniverse.com/v3/weather/daily.json?key=Sk3fyxgYjI1ZQyyFy&location=wuxi&language=zh-Hans&unit=c&start=0&days=3"
/********************************************************************************************************/
void whether_sync(void *prt)
{
    ESP_LOGI(TAG, "whether_sync task was created.");

    while (1)
    {
        ESP_LOGI(TAG, "Sync Wether.");
        char *buffer = malloc(MAX_HTTP_OUTPUT_BUFFER);
        ESP_LOGI(TAG, "Stack pointer address is %08X", buffer);
        esp_http_client_config_t config = {
            .url = "http://" HOST "/v3/weather/daily.json?key=" API_KEY "&location=" LOCATION
                   "&language=" LANGUAGE "&unit=" TEMPERATURE_UNIT "&start=0&days=3",
        };
        esp_http_client_handle_t client = esp_http_client_init(&config);
        esp_http_client_set_method(client, HTTP_METHOD_GET);
        ESP_LOGI(TAG, "Finished Init http");
        // esp_err_t err = esp_http_client_perform(client);
        esp_err_t err = esp_http_client_open(client, 0);

        if (err == ESP_OK)
        {
            ESP_LOGI(TAG, "Finished Open URL");
            int content_length = esp_http_client_fetch_headers(client);
            ESP_LOGI(TAG, "Current got wether data length is %d", content_length);
            if (content_length > 0)
            {
                int data_read = esp_http_client_read_response(client, buffer, MAX_HTTP_OUTPUT_BUFFER);
                ESP_LOGI(TAG, "Current read wether data is %d", data_read);
                if (data_read >= 0)
                {
                    // buffer[data_read] = 0; // Ensure null terminated string
                    ESP_LOGI(TAG, "Prepare to parsing JSON");
                    cJSON *root = cJSON_Parse(buffer);
                    cJSON *results = cJSON_GetObjectItem(root, "results");
                    cJSON *first_result = cJSON_GetArrayItem(results, 0);

                    cJSON *location = cJSON_GetObjectItem(first_result, "location");
                    cJSON *city = cJSON_GetObjectItem(location, "name");
                    strcpy(weather_s.city , city->valuestring);
                    ESP_LOGI(TAG, "current city: %s", city->valuestring);

                    cJSON *daily = cJSON_GetObjectItem(first_result, "daily");
                    cJSON *today = cJSON_GetArrayItem(daily, 0);

                    cJSON *today_temphigh = cJSON_GetObjectItem(today, "high");
                    strcpy(weather_s.tempHigh , today_temphigh->valuestring);
                    ESP_LOGI(TAG, "today temperature of high: %s degree", today_temphigh->valuestring);

                    cJSON *today_templow = cJSON_GetObjectItem(today, "low");
                    strcpy(weather_s.tempLow , today_templow->valuestring);
                    ESP_LOGI(TAG, "today temperature of low: %s degree", today_templow->valuestring);

                    cJSON *day_weather = cJSON_GetObjectItem(today, "text_day");
                    strcpy(weather_s.weather_day , day_weather->valuestring);
                    ESP_LOGI(TAG, "The weather of day: %s", day_weather->valuestring);

                    cJSON *code_day_weather = cJSON_GetObjectItem(today, "code_day");
                    weather_s.code_day = atoi(code_day_weather->valuestring);
                    ESP_LOGI(TAG, "The weather of day: %d", weather_s.code_day);

                    cJSON *night_weather = cJSON_GetObjectItem(today, "text_night");
                    strcpy(weather_s.weather_night , night_weather->valuestring);               
                    ESP_LOGI(TAG, "The weather of night: %s", night_weather->valuestring);

                    cJSON *code_night_weather = cJSON_GetObjectItem(today, "code_night");
                    weather_s.code_night = atoi(code_night_weather->valuestring);
                    ESP_LOGI(TAG, "The weather of day: %d", weather_s.code_night);

                    cJSON_Delete(root);
                }
                else
                {
                    printf("Failed to read response data.\n");
                }
            }
        }
        else
        {
            printf("HTTP GET request failed: %s\n", esp_err_to_name(err));
        }
        esp_http_client_cleanup(client);
        free(buffer);

        vTaskDelay(1000 * 3600 / portTICK_PERIOD_MS);
    }
}
void whether_sync_run(void)
{
    xTaskCreate(whether_sync, TAG, 1024 * 10, NULL, 1, NULL);
}
/********************************************************************************************************/
#endif