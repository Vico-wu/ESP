SET(SOURCES screens/ui_Screen1.c
    ui.c
    ui_helpers.c
    images/ui_img_bg3_png.c
    images/ui_img_day_sunny_1000_png.c
    images/ui_img_bg0_png.c
    fonts/ui_font_Jianti16.c
    fonts/ui_font_Jianti24.c
    fonts/ui_font_Jianti32.c
    fonts/ui_font_Jianti48.c)

add_library(ui ${SOURCES})
