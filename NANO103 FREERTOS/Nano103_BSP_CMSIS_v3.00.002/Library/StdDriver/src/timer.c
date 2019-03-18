/**************************************************************************//**
 * @file     timer.c
 * @version  V1.00
 * $Revision: 5 $
 * $Date: 16/02/24 9:24a $
 * @brief    Nano 103 TIMER driver source file
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "Nano103.h"

/** @addtogroup NANO103_Device_Driver NANO103 Device Driver
  @{
*/

/** @addtogroup NANO103_TIMER_Driver TIMER Driver
  @{
*/


/** @addtogroup NANO103_TIMER_EXPORTED_FUNCTIONS TIMER Exported Functions
  @{
*/

/**
  * @brief This API is used to configure timer to operate in specified mode
  *        and frequency. If timer cannot work in target frequency, a closest
  *        frequency will be chose and returned.
  * @param[in] timer The base address of Timer module
  * @param[in] u32Mode Operation mode. Possible options are
  *                 - \ref TIMER_ONESHOT_MODE
  *                 - \ref TIMER_PERIODIC_MODE
  *                 - \ref TIMER_TOGGLE_MODE
  *                 - \ref TIMER_CONTINUOUS_MODE
  * @param[in] u32Freq Target working frequency
  * @return Real Timer working frequency
  * @note After calling this API, Timer is \b NOT running yet. But could start timer running be calling
  *       \ref TIMER_Start macro or program registers directly
  */
uint32_t TIMER_Open(TIMER_T *timer, uint32_t u32Mode, uint32_t u32Freq)
{
    uint32_t u32Clk = TIMER_GetModuleClock(timer);
    uint32_t u32Cmpr = 0, u32Prescale = 0;

    // Fastest possible timer working freq is u32Clk / 2. While cmpr = 2, pre-scale = 0
    if(u32Freq > (u32Clk / 2)) {
        u32Cmpr = 2;
    } else {
        if(u32Clk > 0xFFFFFF) { // For Nano103, only needs to consider 36MHz at most
            u32Prescale = 1;
            u32Clk >>= 1;
        }
        u32Cmpr = u32Clk / u32Freq;
    }
    timer->CMP = u32Cmpr;
    timer->PRECNT = u32Prescale;
    timer->CTL = u32Mode;


    return(u32Clk / (u32Cmpr * (u32Prescale + 1)));
}

/**
  * @brief This API stops Timer counting and disable the Timer interrupt function
  * @param[in] timer The base address of Timer module
  * @return None
  */
void TIMER_Close(TIMER_T *timer)
{
    timer->CTL = 0;
    timer->INTEN = 0;
}

/**
  * @brief This API is used to create a delay loop for u32usec micro seconds
  * @param[in] timer The base address of Timer module
  * @param[in] u32Usec Delay period in micro seconds with 10 usec every step. Valid values are between 10~1000000 (10 micro second ~ 1 second)
  * @return None
  * @note This API overwrites the register setting of the timer used to count the delay time.
  * @note This API use polling mode. So there is no need to enable interrupt for the timer module used to generate delay
  */
void TIMER_Delay(TIMER_T *timer, uint32_t u32Usec)
{
    uint32_t u32Clk = TIMER_GetModuleClock(timer);
    uint32_t u32Prescale = 0, delay = SystemCoreClock / u32Clk;
    long long u64Cmpr;

    // Clear current timer configuration
    timer->CTL = 0;

    if(u32Clk == 10000) {         // min delay is 100us if timer clock source is LIRC 10k
        u32Usec = ((u32Usec + 99) / 100) * 100;
    } else {    // 10 usec every step
        u32Usec = ((u32Usec + 9) / 10) * 10;
    }

    if(u32Clk > 0xFFFFFF) { // For Nano103, only needs to consider 32MHz at most
        u32Prescale = 1;
        u32Clk >>= 1;
    }

    // u32Usec * u32Clk might overflow if using uint32_t
    u64Cmpr = (long long)u32Usec * (long long)u32Clk / (long long)1000000;

    timer->CMP = (uint32_t)u64Cmpr;
    timer->PRECNT = u32Prescale;
    timer->CTL = TIMER_CTL_CNTEN_Msk; // one shot mode

    // When system clock is faster than timer clock, it is possible timer active bit cannot set in time while we check it.
    // And the while loop below return immediately, so put a tiny delay here allowing timer start counting and raise active flag.
    for(; delay > 0; delay--) {
        __NOP();
    }

    while(timer->CTL & TIMER_CTL_ACTSTS_Msk);

}

/**
  * @brief This API is used to enable timer capture function with specified mode and capture edge
  * @param[in] timer The base address of Timer module
  * @param[in] u32CapMode Timer capture mode. Could be
  *                 - \ref TIMER_CAPTURE_FREE_COUNTING_MODE
  *                 - \ref TIMER_CAPTURE_TRIGGER_COUNTING_MODE
  *                 - \ref TIMER_CAPTURE_COUNTER_RESET_MODE
  * @param[in] u32Edge Timer capture edge. Possible values are
  *                 - \ref TIMER_CAPTURE_FALLING_EDGE
  *                 - \ref TIMER_CAPTURE_RISING_EDGE
  *                 - \ref TIMER_CAPTURE_FALLING_THEN_RISING_EDGE
  *                 - \ref TIMER_CAPTURE_RISING_THEN_FALLING_EDGE
  * @return None
  * @note Timer frequency should be configured separately by using \ref TIMER_Open API, or program registers directly
  */
void TIMER_EnableCapture(TIMER_T *timer, uint32_t u32CapMode, uint32_t u32Edge)
{

    timer->CTL = (timer->CTL & ~(TIMER_CTL_CAPFUNCS_Msk |
                                 TIMER_CTL_CAPCNTMD_Msk |
                                 TIMER_CTL_CAPEDGE_Msk)) |
                 u32CapMode | u32Edge | TIMER_CTL_CAPEN_Msk;
}

/**
  * @brief This API is used to disable the Timer capture function
  * @param[in] timer The base address of Timer module
  * @return None
  */
void TIMER_DisableCapture(TIMER_T *timer)
{
    timer->CTL &= ~TIMER_CTL_CAPEN_Msk;

}

/**
  * @brief This function is used to enable the Timer counter function with specify detection edge
  * @param[in] timer The base address of Timer module
  * @param[in] u32Edge Detection edge of counter pin. Could be ether
  *             - \ref TIMER_COUNTER_RISING_EDGE, or
  *             - \ref TIMER_COUNTER_FALLING_EDGE
  * @return None
  * @note Timer compare value should be configured separately by using \ref TIMER_SET_CMP_VALUE macro or program registers directly
  */
void TIMER_EnableEventCounter(TIMER_T *timer, uint32_t u32Edge)
{
    timer->CTL = (timer->CTL & ~TIMER_CTL_CNTPHASE_Msk) | u32Edge;
    timer->CTL |= TIMER_CTL_EXTCNTEN_Msk;
}

/**
  * @brief This API is used to disable the Timer event counter function.
  * @param[in] timer The base address of Timer module
  * @return None
  */
void TIMER_DisableEventCounter(TIMER_T *timer)
{
    timer->CTL &= ~TIMER_CTL_EXTCNTEN_Msk;
}

/**
  * @brief This API is used to get the clock frequency of Timer
  * @param[in] timer The base address of Timer module
  * @return Timer clock frequency
  * @note This API cannot return correct clock rate if timer source is external clock input.
  */
uint32_t TIMER_GetModuleClock(TIMER_T *timer)
{
    uint32_t u32Src, u32Div;
    const uint32_t au32Clk[] = {__HXT, __LXT, __LIRC, 0};   // we don't know actual clock if external pin is clock source, set to 0 here

    if(timer == TIMER0) {
        u32Src = (CLK->CLKSEL1 & CLK_CLKSEL1_TMR0SEL_Msk) >> CLK_CLKSEL1_TMR0SEL_Pos;
        u32Div = (CLK->CLKDIV1 & CLK_CLKDIV1_TMR0DIV_Msk) >> CLK_CLKDIV1_TMR0DIV_Pos;
    } else if(timer == TIMER1) {
        u32Src = (CLK->CLKSEL1 & CLK_CLKSEL1_TMR1SEL_Msk) >> CLK_CLKSEL1_TMR1SEL_Pos;
        u32Div = (CLK->CLKDIV1 & CLK_CLKDIV1_TMR1DIV_Msk) >> CLK_CLKDIV1_TMR1DIV_Pos;
    } else if(timer == TIMER2) {
        u32Src = (CLK->CLKSEL2 & CLK_CLKSEL2_TMR2SEL_Msk) >> CLK_CLKSEL2_TMR2SEL_Pos;
        u32Div = (CLK->CLKDIV1 & CLK_CLKDIV1_TMR2DIV_Msk) >> CLK_CLKDIV1_TMR2DIV_Pos;
    } else {// Timer 3
        u32Src = (CLK->CLKSEL2 & CLK_CLKSEL2_TMR3SEL_Msk) >> CLK_CLKSEL2_TMR3SEL_Pos;
        u32Div = (CLK->CLKDIV1 & CLK_CLKDIV1_TMR3DIV_Msk) >> CLK_CLKDIV1_TMR3DIV_Pos;
    }
    u32Div++;
    if(u32Src < 4)
        return au32Clk[u32Src] / u32Div;
    else if(u32Src == 4) {
        /* HIRC Source Selection */
        if(CLK->CLKSEL0 & CLK_CLKSEL0_HIRCSEL_Msk) {
            /* Clock source from HIRC1 (36MHz) */
            return __HIRC36M / u32Div;
        } else {
            /* Clock source from HIRC0 (12MHz) */
            if(CLK->PWRCTL & CLK_PWRCTL_HIRC0FSEL_Msk)
                return __HIRC16M / u32Div;
            else
                return __HIRC12M / u32Div;
        }

    } else if(u32Src == 5) { // MIRC
        return __MIRC / u32Div;

    } else  // HCLK
        return CLK_GetHCLKFreq() / u32Div;

}

/**
  * @brief This function is used to reset the Timer counter value
  * @param[in] timer The base address of Timer module
  * @return None
  */
void TIMER_ResetCounter(TIMER_T *timer)
{
    // A write with any value to TIMER CNT register will trigger timer counter reset
    timer->CNT = 0;
}

/**
  * @brief This function is used to enable the Timer frequency counter function
  * @param[in] timer The base address of Timer module. Can be \ref TIMER0 or \ref TIMER2
  * @param[in] u32DropCount Set the event needs to drop before starting to measure event
  *                         frequency. Valid value is between 0~0xFF
  * @param[in] u32Timeout Set the timeout value before stop the frequency counter. No matter timer has
  *                       sufficient sample count or not. The unit is timer clock and valid range is
  *                       between 2~0xFFFFFF. Input 0 or 1 means disable timeout function.
  * @param[in] u32EnableInt Enable interrupt assertion after capture complete or not. Valid values are TRUE and FALSE
  * @return None
  * @details This function is used to calculate input event frequency. After enable
  *          this function, a pair of timers, TIMER0 and TIMER1, or TIMER2 and TIMER3
  *          will be configured for this function. The mode used to calculate input
  *          event frequency is mentioned as "Inter Timer Trigger Mode" in Technical
  *          Reference Manual
  * @note If drop count is not 0, a valid timeout value (between 2~0xFFFFFF) must be provided.
  */
void TIMER_EnableFreqCounter(TIMER_T *timer,
                             uint32_t u32DropCount,
                             uint32_t u32Timeout,
                             uint32_t u32EnableInt)
{
    uint32_t mode = TIMER_CTL_INTRTGEN_Msk;
    TIMER_T *t;    // store the timer base to configure compare value

    if(u32DropCount != 0 || u32Timeout >= 2)
        mode |= TIMER_CTL_INTRTGMD_Msk;
    if(u32Timeout < 2)
        u32Timeout = 0xFFFFFF;

    timer->ECTL = u32DropCount << TIMER_ECTL_EVNTDPCNT_Pos;
    t = (timer == TIMER0) ? TIMER1 : TIMER3;

    t->CMP = u32Timeout;
    t->INTEN = u32EnableInt ? TIMER_INTEN_CAPIEN_Msk : 0;
    timer->CTL = mode | TIMER_CTL_CNTEN_Msk;

    return;
}
/**
  * @brief This function is used to disable the Timer frequency counter function.
  * @param[in] timer The base address of Timer module
  * @return None
  */
void TIMER_DisableFreqCounter(TIMER_T *timer)
{
    timer->CTL &= ~(TIMER_CTL_INTRTGEN_Msk | TIMER_CTL_INTRTGMD_Msk);
}

/**
  * @brief This function is used to select the interrupt source used to trigger other modules.
  * @param[in] timer The base address of Timer module
  * @param[in] u32Src Selects the interrupt source to trigger other modules. Could be:
  *              - \ref TIMER_TIMEOUT_TRIGGER
  *              - \ref TIMER_CAPTURE_TRIGGER
  * @return None
  */
void TIMER_SetTriggerSource(TIMER_T *timer, uint32_t u32Src)
{
    timer->CTL = (timer->CTL & ~TIMER_CTL_TRGSSEL_Msk) | u32Src;
}

/**
  * @brief This function is used to set modules trigger by timer interrupt
  * @param[in] timer The base address of Timer module
  * @param[in] u32Mask The mask of modules (ADC ,PDMA and PWM) trigger by timer. Is the combination of
  *             - \ref TIMER_CTL_TRGPWM_Msk, and
  *             - \ref TIMER_CTL_TRGPDMA_Msk, and
  *             - \ref TIMER_CTL_TRGADC_Msk
  * @return None
  */
void TIMER_SetTriggerTarget(TIMER_T *timer, uint32_t u32Mask)
{
    timer->CTL = (timer->CTL & ~(TIMER_CTL_TRGPWM_Msk | TIMER_CTL_TRGPDMA_Msk | TIMER_CTL_TRGADC_Msk)) | u32Mask;
}

/*@}*/ /* end of group NANO103_TIMER_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group NANO103_TIMER_Driver */

/*@}*/ /* end of group NANO103_Device_Driver */

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/
