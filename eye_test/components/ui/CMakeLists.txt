# SET(src_dirs ui
#     screens/ui_Screen1.c
#     ui.c
#     ui_helpers.c
#     images/ui_img_bg3_png.c
#     images/ui_img_whether_sun_png.c
#     images/ui_img_1733488461.c
#     images/ui_img_1532110541.c
#     images/ui_img_261581757.c
#     images/ui_img_820147422.c
#     images/ui_img_583279387.c
#     images/ui_img_2018655616.c
#     images/ui_img_273823832.c
#     images/ui_img_807905347.c
#     images/ui_img_58911356.c
#     images/ui_img_1022817823.c
#     images/ui_img_997974308.c
#     images/ui_img_83754871.c
#     images/ui_img_1165484050.c
#     fonts/ui_font_Jianti16.c
#     fonts/ui_font_Jianti24.c
#     fonts/ui_font_Jianti32.c
#     fonts/ui_font_Jianti48.c)

add_library(ui ${SOURCES})
set(src_dirs        "ui_font_Jianti16.c"
                    "ui_font_Jianti24.c"
                    "ui_font_Jianti32.c"
                    "ui_font_Jianti48.c"
                    "ui_Screen1.c"
                    "ui.c"
                    "ui_helpers.c"
                    "ui_img_bg3_png.c"
                    "ui_img_whether_sun_png.c"
                    "ui_img_1733488461.c"
                    "ui_img_1532110541.c"
                    "ui_img_261581757.c"
                    "ui_img_820147422.c"
                    "ui_img_583279387.c"
                    "ui_img_2018655616.c"
                    "ui_img_273823832.c"
                    "ui_img_807905347.c"
                    "ui_img_58911356.c"
                    "ui_img_1022817823.c"
                    "ui_img_997974308.c"
                    "ui_img_83754871.c"
                    "ui_img_1165484050.c")
set(include_dirs    "include")
idf_component_register(SRC ${src_dirs} 
                    INCLUDE_DIRS "include")