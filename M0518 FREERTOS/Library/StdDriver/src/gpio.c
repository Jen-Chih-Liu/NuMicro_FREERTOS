/**************************************************************************//**
 * @file     gpio.c
 * @version  V3.00
 * $Revision: 1 $
 * $Date: 14/10/03 1:24p $
 * @brief    M0518 series GPIO driver source file
 *
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include "M0518.h"

/** @addtogroup M0518_Device_Driver M0518 Device Driver
  @{
*/

/** @addtogroup M0518_GPIO_Driver GPIO Driver
  @{
*/

/** @addtogroup M0518_GPIO_EXPORTED_FUNCTIONS GPIO Exported Functions
  @{
*/

/**
 * @brief       Set GPIO operation mode
 *
 * @param[in]   port        GPIO port. It could be PA, PB, PC, PD, PE or PF.
 * @param[in]   u32PinMask  The single or multiple pins of specified GPIO port.
 *                          It could be BIT0 ~ BIT15 for PA and PB GPIO port.
 *                          It could be BIT0 ~ BIT3, BIT6 ~ BIT11, BIT14 and BIT15 for PC GPIO port.
 *                          It could be BIT6, BIT7, BIT14 and BIT15 for PD GPIO port.
 *                          It could be BIT5 for PE GPIO port.
 *                          It could be BIT0, BIT1 and BIT4 ~ BIT8 for PF GPIO port.
 * @param[in]   u32Mode     Operation mode. It could be \n
 *                          GPIO_PMD_INPUT, GPIO_PMD_OUTPUT, GPIO_PMD_OPEN_DRAIN, GPIO_PMD_QUASI.
 *
 * @return      None
 *
 * @details     This function is used to set specified GPIO operation mode.
 */
void GPIO_SetMode(GPIO_T *port, uint32_t u32PinMask, uint32_t u32Mode)
{
    uint32_t i;

    for(i = 0; i < GPIO_PIN_MAX; i++)
    {
        if(u32PinMask & (1 << i))
        {
            port->PMD = (port->PMD & ~(0x3 << (i << 1))) | (u32Mode << (i << 1));
        }
    }
}

/**
 * @brief       Enable GPIO interrupt
 *
 * @param[in]   port            GPIO port. It could be PA, PB, PC, PD, PE or PF.
 * @param[in]   u32Pin          The pin of specified GPIO port.
 *                              It could be 0 ~ 15 for PA and PB GPIO port.
 *                              It could be 0 ~ 3, 6 ~ 11, BIT14 and BIT15 for PC GPIO port.
 *                              It could be 6, 7, 14 and BIT15 for PD GPIO port.
 *                              It could be 5 for PE GPIO port.
 *                              It could be 0, 1 and 4 ~ 8 for PF GPIO port.
 * @param[in]   u32IntAttribs   The interrupt attribute of specified GPIO pin. It could be \n
 *                              GPIO_INT_RISING, GPIO_INT_FALLING, GPIO_INT_BOTH_EDGE, GPIO_INT_HIGH, GPIO_INT_LOW.
 *
 * @return      None
 *
 * @details     This function is used to enable specified GPIO pin interrupt.
 */
void GPIO_EnableInt(GPIO_T *port, uint32_t u32Pin, uint32_t u32IntAttribs)
{
    port->IMD |= (((u32IntAttribs >> 24) & 0xFFUL) << u32Pin);
    port->IEN |= ((u32IntAttribs & 0xFFFFFFUL) << u32Pin);
}


/**
 * @brief       Disable GPIO interrupt
 *
 * @param[in]   port      GPIO port. It could be PA, PB, PC, PD, PE or PF.
 * @param[in]   u32Pin    The pin of specified GPIO port.
 *                        It could be 0 ~ 15 for PA and PB GPIO port.
 *                        It could be 0 ~ 3, 6 ~ 11, BIT14 and BIT15 for PC GPIO port.
 *                        It could be 6, 7, 14 and BIT15 for PD GPIO port.
 *                        It could be 5 for PE GPIO port.
 *                        It could be 0, 1 and 4 ~ 8 for PF GPIO port.
 *
 * @return      None
 *
 * @details     This function is used to enable specified GPIO pin interrupt.
 */
void GPIO_DisableInt(GPIO_T *port, uint32_t u32Pin)
{
    port->IMD &= ~(1UL << u32Pin);
    port->IEN &= ~((0x00010001UL) << u32Pin);
}


/*@}*/ /* end of group M0518_GPIO_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group M0518_GPIO_Driver */

/*@}*/ /* end of group M0518_Device_Driver */

/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/
