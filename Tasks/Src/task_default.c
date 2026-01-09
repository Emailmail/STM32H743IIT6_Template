#include "task_default.h"

W9825G6KH_Instance *w9825g6kh;
void StartDefaultTask(void *argument)
{
    w9825g6kh = W9825G6KH_Register();
    W9825G6KH_Init(w9825g6kh);

    LCD_Init();
    vTaskDelay(pdMS_TO_TICKS(100));
    
    for (;;)
    {
        LCD_SetBackColor(LCD_RED);
        vTaskDelay(pdMS_TO_TICKS(100));
        LCD_SetBackColor(LCD_GREEN);
        vTaskDelay(pdMS_TO_TICKS(100));
        LCD_SetBackColor(LCD_BLUE);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
