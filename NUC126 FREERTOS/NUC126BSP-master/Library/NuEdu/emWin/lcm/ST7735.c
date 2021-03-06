/*********************************************************************
*                 SEGGER Software GmbH                               *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2018  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.46 - Graphical user interface for embedded applications **
All  Intellectual Property rights in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product. This file may
only be used in accordance with the following terms:

The  software has  been licensed by SEGGER Software GmbH to Nuvoton Technology Corporation
at the address: No. 4, Creation Rd. III, Hsinchu Science Park, Taiwan
for the purposes  of  creating  libraries  for its
Arm Cortex-M and  Arm9 32-bit microcontrollers, commercialized and distributed by Nuvoton Technology Corporation
under  the terms and conditions  of  an  End  User
License  Agreement  supplied  with  the libraries.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Software GmbH
Licensed to:              Nuvoton Technology Corporation, No. 4, Creation Rd. III, Hsinchu Science Park, 30077 Hsinchu City, Taiwan
Licensed SEGGER software: emWin
License number:           GUI-00735
License model:            emWin License Agreement, signed February 27, 2018
Licensed platform:        Cortex-M and ARM9 32-bit series microcontroller designed and manufactured by or for Nuvoton Technology Corporation
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2018-03-26 - 2019-03-27
Contact to extend SUA:    sales@segger.com
----------------------------------------------------------------------
File        : LCDConf.c
Purpose     : Display controller configuration (single layer)
---------------------------END-OF-HEADER------------------------------
*/

#include <stddef.h>
#include <stdio.h>

#include "GUI.h"
#include "GUIDRV_FlexColor.h"

#include "NUC126.h"

#include "TouchPanel.h"

#include "lcm.h"

//
// Hardware related
//
#define SPI_LCD_PORT  SPI1

#define GPIO_SPI1_SS PD12
#define GPIOPORT_SPI1_SS PD
#define PINMASK_SPI1_SS BIT12

#define GPIO_SPI1_CLK PD15
#define GPIOPORT_SPI1_CLK PD
#define PINMASK_SPI1_CLK BIT15

#define GPIO_SPI1_MISO PD14
#define GPIOPORT_SPI1_MISO PD
#define PINMASK_SPI1_MISO BIT14

#define GPIO_SPI1_MOSI PD13
#define GPIOPORT_SPI1_MOSI PD
#define PINMASK_SPI1_MOSI BIT13

#define GPIO_LCM_DC PD6
#define GPIOPORT_LCM_DC PD
#define PINMASK_LCM_DC BIT6

#define GPIO_LCM_RESET PD1
#define GPIOPORT_LCM_RESET PD
#define PINMASK_LCM_RESET BIT1

#define SPI_CS_SET    GPIO_SPI1_SS = 1
#define SPI_CS_CLR    GPIO_SPI1_SS = 0

#define LCM_DC_SET    GPIO_LCM_DC = 1
#define LCM_DC_CLR    GPIO_LCM_DC = 0

#define LCM_RESET_SET GPIO_LCM_RESET = 1
#define LCM_RESET_CLR GPIO_LCM_RESET = 0

/*********************************************************************
*
*       _Read1
*/
U8 _Read1(void)
{
#if 1
    /* FIXME if panel supports read back feature */
    return 0;
#else
    LCM_DC_SET;
    SPI_CS_CLR;
    SPI_WRITE_TX(SPI_LCD_PORT, 0x00);
    SPI_READ_RX(SPI_LCD_PORT);
    SPI_CS_SET;
    return (SPI_READ_RX(SPI_LCD_PORT));
#endif
}

/*********************************************************************
*
*       _ReadM1
*/
void _ReadM1(U8 * pData, int NumItems)
{
#if 1
    /* FIXME if panel supports read back feature */
#else
    LCM_DC_SET;
    SPI_CS_CLR;
    while(NumItems--)
    {
        SPI_WRITE_TX(SPI_LCD_PORT, 0x00);
        while(SPI_IS_BUSY(SPI_LCD_PORT));
        *pData++ = SPI_READ_RX(SPI_LCD_PORT);
    }
    SPI_CS_SET;
#endif
}

/*********************************************************************
*
*       _Write0
*/
void _Write0(U8 Cmd)
{
    LCM_DC_CLR;
    
    SPI_WRITE_TX(SPI_LCD_PORT, Cmd);
    while(SPI_IS_BUSY(SPI_LCD_PORT));
}

/*********************************************************************
*
*       _Write1
*/
void _Write1(U8 Data)
{
    LCM_DC_SET;
    
    SPI_WRITE_TX(SPI_LCD_PORT, Data);
    while(SPI_IS_BUSY(SPI_LCD_PORT));
}

/*********************************************************************
*
*       _WriteM1
*/
void _WriteM1(U8 * pData, int NumItems)
{
    LCM_DC_SET;
    while(NumItems--)
    {
        while(SPI_LCD_PORT->STATUS & SPI_STATUS_TXFULL_Msk);
        SPI_WRITE_TX(SPI_LCD_PORT, *pData++);
    }
}

void _Open_SPI(void)
{
    GPIO_SetMode(GPIOPORT_LCM_DC, PINMASK_LCM_DC, GPIO_MODE_OUTPUT);
    GPIO_SetMode(GPIOPORT_LCM_RESET, PINMASK_LCM_RESET, GPIO_MODE_OUTPUT);
    GPIO_SetMode(GPIOPORT_SPI1_SS, PINMASK_SPI1_SS, GPIO_MODE_OUTPUT); //cs pin for gpiod

    SYS->GPD_MFPH &= ~(SYS_GPD_MFPH_PD12MFP_Msk | SYS_GPD_MFPH_PD13MFP_Msk | SYS_GPD_MFPH_PD14MFP_Msk | SYS_GPD_MFPH_PD15MFP_Msk);
    SYS->GPD_MFPH |= (SYS_GPD_MFPH_PD12MFP_SPI1_SS | SYS_GPD_MFPH_PD13MFP_SPI1_MOSI | SYS_GPD_MFPH_PD15MFP_SPI1_CLK | SYS_GPD_MFPH_PD14MFP_SPI1_MISO);

    /* PD12/13/14/15 high slew rate */
    PD->SLEWCTL = 0xf << 12;
    
    
    /* Enable SPI1 */
    CLK_EnableModuleClock(SPI1_MODULE);
    CLK_SetModuleClock(SPI1_MODULE, CLK_CLKSEL2_SPI1SEL_PLL, 0);

    SPI_Open(SPI_LCD_PORT, SPI_MASTER, SPI_MODE_0, 8, 36000000);

    /* Disable auto SS function, control SS signal manually. */
    SPI_EnableAutoSS(SPI_LCD_PORT, SPI_SS, SPI_SS_ACTIVE_LOW);
    SPI_ENABLE(SPI_LCD_PORT);
}

/*********************************************************************
*
*       _InitController
*
* Purpose:
*   Initializes the display controller
*/
void _InitController(void)
{
    int i;
    static uint8_t s_InitOnce = 0;

    if(s_InitOnce == 0)
        s_InitOnce = 1;
    else
        return;

    _Open_SPI();

    LCM_RESET_SET;
    LCM_RESET_CLR;

    GUI_X_Delay(100);

    LCM_RESET_SET;

    GUI_X_Delay(120);
    _Write0(0x11);    //Exit Sleep
    GUI_X_Delay(120);
//------------------------------------------------------------------//
//-------------------Software Reset---------------------------------//

    _Write0(0xB1);
    _Write1(0x01);
    _Write1(0x2C);
    _Write1(0x2D);

    _Write0(0xB2);
    _Write1(0x01);
    _Write1(0x2C);
    _Write1(0x2D);

    _Write0(0xB3);
    _Write1(0x01);
    _Write1(0x2C);
    _Write1(0x2D);

    _Write1(0x01);
    _Write1(0x2C);
    _Write1(0x2D);


    _Write0(0xB4);  //Column inversion
    _Write1(0x07);
    //ST7735R Power Sequence
    _Write0(0xC0);
    _Write1(0xA2);
    _Write1(0x02);
    _Write1(0x84);
    _Write1(0xC1);
    _Write1(0xC5);
    _Write0(0xC2);
    _Write1(0x0A);
    _Write1(0x00);

    _Write0(0xC3);
    _Write1(0x8A);
    _Write1(0x2A);
    _Write1(0xC4);
    _Write1(0x8A);
    _Write1(0xEE);

    _Write0(0xC5); //VCOM
    _Write1(0x0E);

    _Write0(0x36); //MX, MY, RGB mode
    _Write1(0xC0);
    _Write1(0xC8); //??C8 ??08 A8

    //ST7735R Gamma Sequence
    _Write0(0xe0);
    _Write1(0x0f);
    _Write1(0x1a);
    _Write1(0x0f);
    _Write1(0x18);
    _Write1(0x2f);
    _Write1(0x28);
    _Write1(0x20);
    _Write1(0x22);
    _Write1(0x1f);
    _Write1(0x1b);
    _Write1(0x23);
    _Write1(0x37);
    _Write1(0x00);
    _Write1(0x07);
    _Write1(0x02);
    _Write1(0x10);

    _Write0(0xe1);
    _Write1(0x0f);
    _Write1(0x1b);
    _Write1(0x0f);
    _Write1(0x17);
    _Write1(0x33);
    _Write1(0x2c);
    _Write1(0x29);
    _Write1(0x2e);
    _Write1(0x30);
    _Write1(0x30);
    _Write1(0x39);
    _Write1(0x3f);
    _Write1(0x00);
    _Write1(0x07);
    _Write1(0x03);
    _Write1(0x10);

    _Write0(0x2a);
    _Write1(0x02);
    _Write1(0x00 + 2);
    _Write1(0x02);
    _Write1(0x7F + 2);

    _Write0(0x2b);
    _Write1(0x01);
    _Write1(0x00 + 1);
    _Write1(0x01);
    _Write1(0x9F + 1);

    _Write0(0xF0); //Enable test command
    _Write1(0x01);
    _Write0(0xF6); //Disable ram power save mode
    _Write1(0x00);

    _Write0(0x3A); //65k mode
    _Write1(0x05);

    _Write0(0x2c);
    for(i = 0; i < 0x5000; i++)
    {
        _Write1(0x00 >> 8);
        _Write1(0x00);
    }

    _Write0(0x29);    //Display on
}
