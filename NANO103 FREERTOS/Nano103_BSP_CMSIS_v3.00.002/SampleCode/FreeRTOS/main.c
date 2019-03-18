/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 2 $
 * $Date: 15/12/11 3:38p $
 * @brief    Show hard fault information when hard fault happened.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano103.h"


/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"


void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable External XTAL (4~24 MHz) */
    //CLK->PWRCTL &= ~CLK_PWRCTL_HXT_EN_Msk;
    CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk; // HXT Enabled

    /* Waiting for 12MHz clock ready */
    CLK_WaitClockReady( CLK_STATUS_HXTSTB_Msk);

    /* Switch HCLK clock source to XTAL */
    CLK->CLKSEL0 = (CLK->CLKSEL0 &~ CLK_CLKSEL0_HCLKSEL_Msk) | (CLK_CLKSEL0_HCLKSEL_HXT);

    /* Enable IP clock */
    CLK->APBCLK |= CLK_APBCLK_UART0_EN; // UART0 Clock Enable

    /* Select IP clock source */
    CLK->CLKSEL1 &= ~CLK_CLKSEL1_UART0SEL_Msk;
    CLK->CLKSEL1 |= (0x0 << CLK_CLKSEL1_UART0SEL_Pos);// Clock source from external 12 MHz crystal clock

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD and TXD  */
    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB0MFP_Msk|SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |=  (SYS_GPB_MFPL_PB0MFP_UART0_RXD|SYS_GPB_MFPL_PB1MFP_UART0_TXD);

    /* Lock protected registers */
    SYS_LockReg();
}


void UART0_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    UART_Open(UART0, 115200);
}


void prvSetupHardware(void)
{
    SYS_Init();
    UART0_Init();
}


void Task0(void *pvParameters)
{
    printf("Task0 running ...\n");

    for(;;)
    {   
        printf("Task0\n");
        vTaskDelay(100);
    }
}


void Task1(void *pvParameters)
{
    printf("Task1 running ...\n");

    for(;;)
    {   
        printf("Task1\n");
        vTaskDelay(100);
    }
}


int main(void)
{
    prvSetupHardware();

    xTaskCreate(Task0, (const char *)"Task0", 100, NULL, 1, NULL);
    xTaskCreate(Task1, (const char *)"Task1", 100, NULL, 2, NULL);
    vTaskStartScheduler();

    while(1);
}




