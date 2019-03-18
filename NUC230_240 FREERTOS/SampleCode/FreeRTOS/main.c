/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * @brief    A project template for M031 MCU.
 *
 * Copyright (C) 2017 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NUC230_240.h"
/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"


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


int main()
{
    xTaskCreate(Task0, (const char *)"Task0", 100, NULL, 1, NULL);
    xTaskCreate(Task1, (const char *)"Task1", 100, NULL, 2, NULL);
    vTaskStartScheduler();
    /* Got no where to go, just loop forever */
    while(1);
}

/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/
