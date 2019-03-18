/****************************************************************************
 * @file     main.c
 * @version  V2.00
 * @brief    To utilize emWin library to demonstrate  widgets feature.
 *
 * @note
 * Copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>

#include "NUC126.h"


/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"


/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       SYS_Init
*/
void SYS_Init(void)
{
    /* Unlock write-protected registers */
    SYS_UnlockReg();

    /* Enable Internal RC 22.1184 MHz clock */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

#ifndef CRYSTAL_LESS
    /* Enable HXT */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);

    /* Waiting for external XTAL clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Set core clock */
    CLK_SetCoreClock(72000000);

    /* Use HIRC as UART clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UARTSEL_HIRC, CLK_CLKDIV0_UART(1));
#else
    /* Enable Internal RC 48MHz clock */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRC48EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRC48STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC48, CLK_CLKDIV0_HCLK(1));

    /* Use HIRC as UART clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UARTSEL_HIRC, CLK_CLKDIV0_UART(1));
#endif

    /* Enable module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Set PA multi-function pins for UART0 RXD and TXD */
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA3MFP_Msk)) | SYS_GPA_MFPL_PA3MFP_UART0_RXD;
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA2MFP_Msk)) | SYS_GPA_MFPL_PA2MFP_UART0_TXD;

    SYS_LockReg();
}


void Task0(void *pvParameters)
{
    printf("Task0 init ...\n");
    //GPIO_SetMode(PC, BIT9, GPIO_MODE_OUTPUT);

    for(;;)
    {   
		printf("Task0 running ...\n");
        //GPIO_TOGGLE(PC9);
        vTaskDelay(500);
    }
}


void Task1(void *pvParameters)
{
    printf("Task1 init ...\n");
    //GPIO_SetMode(PC, BIT9, GPIO_MODE_OUTPUT);

    for(;;)
    {   
		printf("Task1 running ...\n");
        //GPIO_TOGGLE(PC9);
        vTaskDelay(500);
    }
}


int main(void)
{
    SYS_Init();
    UART_Open(UART0, 115200);
    
    printf("FreeRTOS Demo \n");
    
    xTaskCreate(Task0, (const char *)"Task0", 128, NULL, 1, NULL);
    xTaskCreate(Task1, (const char *)"Task1", 128, NULL, 2, NULL);

    vTaskStartScheduler();
    while(1);
}

/*************************** End of file ****************************/
