/**************************************************************************//**
 * @file     emWinTask.c
 * @version  V1.00
 * @brief    emWin display text demo
 *
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
 
#include "GUIDEMO.h"
#include <stdio.h>
#include "stdint.h"
 

void emWinTask(void *pvParameters)
{
    uint8_t u8Count = 0;
    
    printf("emWinTask running ...\n");
    GUI_Init();
    GUI_SetBkColor(GUI_BLACK);
    GUI_Clear();
    GUI_SetFont(GUI_FONT_8X16X2X2);
    GUI_DispStringAt("emWin+FreeRTOS", 45,30); 
    
    GUI_SetFont(GUI_FONT_8X16X1X2);
    GUI_DispStringAt("Task 1: emWin", 100,90); 
    GUI_DispStringAt("Task 2: Led", 100,130); 
    
    while(1)
    {
        u8Count++;
        GUI_DispStringAt("Time: ", 100,170);
        GUI_DispDec(u8Count, 3); 
        vTaskDelay(1000);
    }
}

/*************************** End of file ****************************/

