idf_component_register(SRCS "update_ui.c"
                    INCLUDE_DIRS "include" 
                    REQUIRES ui time_sync whether_sync espressif__esp_lvgl_port)

