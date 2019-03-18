/**************************************************************************//**
 * @file     adc.c
 * @version  V1.00
 * $Revision: 2 $
 * $Date: 15/12/03 1:29p $
 * @brief    Nano 103 ADC driver source file
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "Nano103.h"

/** @addtogroup NANO103_Device_Driver NANO103 Device Driver
  @{
*/

/** @addtogroup NANO103_ADC_Driver ADC Driver
  @{
*/


/** @addtogroup NANO103_ADC_EXPORTED_FUNCTIONS ADC Exported Functions
  @{
*/

/**
  * @brief This API configures ADC module to be ready for convert the input from selected channel
  * @param[in] adc Base address of ADC module
  * @param[in] u32InputMode Input mode (single-end/differential). Valid values are:
  *                 - \ref ADC_INPUT_MODE_SINGLE_END
  *                 - \ref ADC_INPUT_MODE_DIFFERENTIAL
  * @param[in] u32OpMode Operation mode (single/single cycle/continuous). Valid values are:
  *                 - \ref ADC_OPERATION_MODE_SINGLE
  *                 - \ref ADC_OPERATION_MODE_SINGLE_CYCLE
  *                 - \ref ADC_OPERATION_MODE_CONTINUOUS
  * @param[in] u32ChMask Channel enable bit. Each bit corresponds to a input channel. Bit 0 is channel 0, bit 1 is channel 1...
  * @return  None
  * @note This API does not turn on ADC power nor does trigger ADC conversion
  */
void ADC_Open(ADC_T *adc,
              uint32_t u32InputMode,
              uint32_t u32OpMode,
              uint32_t u32ChMask)
{

    ADC->CTL = (ADC->CTL & ~ADC_CTL_DIFF_Msk) | u32InputMode;
    ADC->CTL = (ADC->CTL & ~ADC_CTL_ADMD_Msk) | u32OpMode;
    ADC->CTL = (ADC->CTL & ~ADC_CTL_REFSEL_Msk);
    ADC->CHEN  = u32ChMask;
    return;
}

/**
  * @brief Disable ADC module
  * @param[in] adc Base address of ADC module
  * @return None
  */
void ADC_Close(ADC_T *adc)
{
    SYS->IPRST2 |= SYS_IPRST2_ADCRST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_ADCRST_Msk;
    return;

}

/**
  * @brief Configure the hardware trigger condition and enable hardware trigger
  * @param[in] adc Base address of ADC module
  * @param[in] u32Source Decides the hardware trigger source. Valid values are:
  *                 - \ref ADC_TRIGGER_BY_EXT_PIN
  *                 - \ref ADC_TRIGGER_BY_PWM
  * @param[in] u32Param While ADC trigger by external pin, this parameter
  *                     is used to set trigger condition. Valid values are:
  *                 - \ref ADC_LOW_LEVEL_TRIGGER
  *                 - \ref ADC_HIGH_LEVEL_TRIGGER
  *                 - \ref ADC_FALLING_EDGE_TRIGGER
  *                 - \ref ADC_RISING_EDGE_TRIGGER
  * @return None
  */
void ADC_EnableHWTrigger(ADC_T *adc,
                         uint32_t u32Source,
                         uint32_t u32Param)
{
    ADC->CTL &= ~(ADC_CTL_HWTRGSEL_Msk | ADC_CTL_HWTRGCOND_Msk | ADC_CTL_HWTRGEN_Msk);
    if(u32Source == ADC_TRIGGER_BY_EXT_PIN) {
        ADC->CTL |= u32Source | u32Param | ADC_CTL_HWTRGEN_Msk;
    } else {
        ADC->CTL |= u32Source | ADC_CTL_HWTRGEN_Msk;
    }

    return;
}

/**
  * @brief Disable hardware trigger ADC function.
  * @param[in] adc Base address of ADC module
  * @return None
  */
void ADC_DisableHWTrigger(ADC_T *adc)
{
    ADC->CTL &= ~(ADC_CTL_HWTRGSEL_Msk | ADC_CTL_HWTRGCOND_Msk | ADC_CTL_HWTRGEN_Msk);
    return;
}

/**
  * @brief Config and enable timer trigger
  * @param[in] adc Base address of ADC module
  * @param[in] u32Source Decides which timer trigger ADC. Valid values are: 0 ~ 3
  * @param[in] u32PDMACnt When timer event occurred, PDMA will transfer u32PDMACnt+1 ADC result
  * @return None
  */
void ADC_EnableTimerTrigger(ADC_T *adc,
                            uint32_t u32Source,
                            uint32_t u32PDMACnt)
{
    ADC->CTL &= ~(ADC_CTL_TMPDMACNT_Msk | ADC_CTL_TMSEL_Msk);
    ADC->CTL |= (u32PDMACnt << ADC_CTL_TMPDMACNT_Pos) | (u32Source << ADC_CTL_TMSEL_Pos) | ADC_CTL_TMTRGMOD_Msk;

    return;
}

/**
  * @brief Disable timer trigger ADC function.
  * @param[in] adc Base address of ADC module
  * @return None
  */
void ADC_DisableTimerTrigger(ADC_T *adc)
{
    ADC->CTL &= ~ADC_CTL_TMTRGMOD_Msk;

    return;
}

/**
  * @brief Configure the hardware trigger condition and enable hardware trigger
  * @param[in] adc Base address of ADC module
  * @param[in] u32ChNum The channel number
  * @param[in] u32SampleTime Decides the extend sampling counter. Valid values are 0 ~ 15
  * @return None
  */
void ADC_SetExtraSampleTime(ADC_T *adc,
                            uint32_t u32ChNum,
                            uint32_t u32SampleTime)
{

    if (u32ChNum < 8)
        ADC->EXTSMPT0 = (ADC->EXTSMPT0 & ~(ADC_EXTSMPT0_EXTSMPT_CH0_Msk << (u32ChNum * 4))) | (u32SampleTime << (u32ChNum * 4));
    else
        ADC->EXTSMPT1 = (ADC->EXTSMPT1 & ~ADC_EXTSMPT1_EXTSMPT_INTCH_Msk) | (u32SampleTime << ADC_EXTSMPT1_EXTSMPT_INTCH_Pos);
}

/**
  * @brief Enable the interrupt(s) selected by u32Mask parameter.
  * @param[in] adc Base address of ADC module
  * @param[in] u32Mask  The combination of interrupt status bits listed below. Each bit
  *                     corresponds to a interrupt status. This parameter decides which
  *                     interrupts will be enabled.
  *                     - \ref ADC_ADF_INT
  *                     - \ref ADC_CMP0_INT
  *                     - \ref ADC_CMP1_INT
  * @return None
  */
void ADC_EnableInt(ADC_T *adc, uint32_t u32Mask)
{
    if(u32Mask & ADC_ADF_INT)
        ADC->CTL |= ADC_CTL_ADCIEN_Msk;
    if(u32Mask & ADC_CMP0_INT)
        ADC->CMP0 |= ADC_CMP0_ADCMPIE_Msk;
    if(u32Mask & ADC_CMP1_INT)
        ADC->CMP1 |= ADC_CMP1_ADCMPIE_Msk;

    return;
}

/**
  * @brief Disable the interrupt(s) selected by u32Mask parameter.
  * @param[in] adc Base address of ADC module
  * @param[in] u32Mask  The combination of interrupt status bits listed below. Each bit
  *                     corresponds to a interrupt status. This parameter decides which
  *                     interrupts will be disabled.
  *                     - \ref ADC_ADF_INT
  *                     - \ref ADC_CMP0_INT
  *                     - \ref ADC_CMP1_INT
  * @return None
  */
void ADC_DisableInt(ADC_T *adc, uint32_t u32Mask)
{
    if(u32Mask & ADC_ADF_INT)
        ADC->CTL &= ~ADC_CTL_ADCIEN_Msk;
    if(u32Mask & ADC_CMP0_INT)
        ADC->CMP0 &= ~ADC_CMP0_ADCMPIE_Msk;
    if(u32Mask & ADC_CMP1_INT)
        ADC->CMP1 &= ~ADC_CMP1_ADCMPIE_Msk;

    return;
}



/*@}*/ /* end of group NANO103_ADC_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group NANO103_ADC_Driver */

/*@}*/ /* end of group NANO103_Device_Driver */

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/
