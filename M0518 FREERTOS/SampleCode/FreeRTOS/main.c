/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * @brief    A project template for M031 MCU.
 *
 * Copyright (C) 2017 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "M0518.h"
/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"


/*
 * This is a template project for M031 series MCU. Users could based on this project to create their
 * own application without worry about the IAR/Keil project settings.
 *
 * This template application uses external crystal as HCLK source and configures UART0 to print out
 * "Hello World", users may need to do extra system configuration based on their system design.
 */
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
    //SYS_Init();

    /* Init UART0 to 115200-8n1 for print message */
   // UART_Open(UART0, 115200);

    /* Connect UART to PC, and open a terminal tool to receive following message */
  //  printf("Hello World\n");
    xTaskCreate(Task0, (const char *)"Task0", 100, NULL, 1, NULL);
    xTaskCreate(Task1, (const char *)"Task1", 100, NULL, 2, NULL);
    vTaskStartScheduler();
    /* Got no where to go, just loop forever */
    while(1);
}

/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/
