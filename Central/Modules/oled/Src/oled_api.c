#include <rtos_common.h>
#include <logging_api.h>
#include "oled_api.h"
#include "oled.h"

CONFIGURE_TASK(oled, osPriorityAboveNormal, 2048, oled_task, NULL);

static void oled_task(void *argument) {
    ssd1306_Init();
    CONSOLE_LOG("[CENTRAL][OLED] Oled inited!");

    while (1) { // test example
        ssd1306_Fill(White);
        ssd1306_UpdateScreen();
        CONSOLE_LOG("[CENTRAL][OLED] Oled filled white!");
        osDelay(5000);
        ssd1306_Fill(Black);
        ssd1306_UpdateScreen();
        CONSOLE_LOG("[CENTRAL][OLED] Oled filled black!");
        osDelay(5000);
    }
}