/**************************************************************************//**
 * @file     sc.h
 * @version  V3.00
 * $Revision: 5 $
 * $Date: 15/05/12 1:52p $
 * @brief    NUC230_240 series Smartcard UART mode (SCUART) driver header file
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __SCUART_H__
#define __SCUART_H__

#ifdef __cplusplus
extern "C"
{
#endif


/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup SCUART_Driver SCUART Driver
  @{
*/

/** @addtogroup SCUART_EXPORTED_CONSTANTS SCUART Exported Constants
  @{
*/
#define SCUART_CHAR_LEN_5     (0x3ul << SC_UACTL_DATA_LEN_Pos)  /*!< Set SCUART word length to 5 bits */
#define SCUART_CHAR_LEN_6     (0x2ul << SC_UACTL_DATA_LEN_Pos)  /*!< Set SCUART word length to 6 bits */
#define SCUART_CHAR_LEN_7     (0x1ul << SC_UACTL_DATA_LEN_Pos)  /*!< Set SCUART word length to 7 bits */
#define SCUART_CHAR_LEN_8     (0)                               /*!< Set SCUART word length to 8 bits */

#define SCUART_PARITY_NONE    (SC_UACTL_PBDIS_Msk)              /*!< Set SCUART transfer with no parity   */
#define SCUART_PARITY_ODD     (SC_UACTL_OPE_Msk)                /*!< Set SCUART transfer with odd parity  */
#define SCUART_PARITY_EVEN    (0)                               /*!< Set SCUART transfer with even parity */

#define SCUART_STOP_BIT_1     (SC_CTL_SLEN_Msk)                 /*!< Set SCUART transfer with one stop bit  */
#define SCUART_STOP_BIT_2     (0)                               /*!< Set SCUART transfer with two stop bits */


/*@}*/ /* end of group SCUART_EXPORTED_CONSTANTS */


/** @addtogroup SCUART_EXPORTED_FUNCTIONS SCUART Exported Functions
  @{
*/

/* TX Macros */
/**
  * @brief Write Data to Tx data register.
  * @param[in] sc The base address of smartcard module.
  * @param[in] u8Data Data byte to transmit.
  * @return None
  * @details By writing data to THR register, the SC will send out an 8-bit data.
  * \hideinitializer
  */
#define SCUART_WRITE(sc, u8Data) ((sc)->THR = (u8Data))

/**
  * @brief Get TX FIFO empty flag status from register.
  * @param[in] sc The base address of smartcard module.
  * @return Transmit FIFO empty status.
  * @retval 0 Transmit FIFO is not empty.
  * @retval SC_TRSR_TX_EMPTY_F_Msk Transmit FIFO is empty.
  * @details When the last byte of TX buffer has been transferred to Transmitter Shift Register, hardware sets TX_EMPTY_F bit (SC_TRSR[9]) high.
  *          It will be cleared when writing data into THR(SC_THR[7:0]).
  * \hideinitializer
  */
#define SCUART_GET_TX_EMPTY(sc) ((sc)->TRSR & SC_TRSR_TX_EMPTY_F_Msk)

/**
  * @brief Get TX FIFO full flag status from register.
  * @param[in] sc The base address of smartcard module.
  * @retval 0 Transmit FIFO is not full.
  * @retval SC_TRSR_TX_FULL_F_Msk Transmit FIFO is full.
  * @details TX_FULL_F(SC_TRSR[10]) is set when TX pointer is equal to 4, otherwise is cleared by hardware.
  * \hideinitializer
  */
#define SCUART_GET_TX_FULL(sc) ((sc)->TRSR & SC_TRSR_TX_FULL_F_Msk)

/**
  * @brief Wait specified smartcard port transmission complete.
  * @param[in] sc The base address of smartcard module.
  * @return None
  * @details TX_ATV (SC_TRSR[31]) is cleared automatically when TX transfer is finished or the last byte transmission has completed.
  * @note This macro blocks until transmit complete.
  * \hideinitializer
  */
#define SCUART_WAIT_TX_EMPTY(sc) while((sc)->TRSR & SC_TRSR_TX_ATV_Msk)

/**
  * @brief Check specified smartcard port transmit FIFO is full or not.
  * @param[in] sc The base address of smartcard module.
  * @retval 0 Transmit FIFO is not full.
  * @retval 1 Transmit FIFO is full.
  * @details TX_FULL_F(SC_TRSR[10]) indicates TX buffer full or not.
  *          This is set when TX pointer is equal to 4, otherwise is cleared by hardware.
  * \hideinitializer
  */
#define SCUART_IS_TX_FULL(sc) ((sc)->TRSR & SC_TRSR_TX_FULL_F_Msk ? 1 : 0)

/**
  * @brief Check specified smartcard port transmission is over.
  * @param[in] sc The base address of smartcard module.
  * @retval 0 Transmit is not complete.
  * @retval 1 Transmit complete.
  * @details TX_ATV (SC_TRSR[31]) is set by hardware when TX transfer is in active and the STOP bit of the last byte has been transmitted.
  * \hideinitializer
  */
#define SCUART_IS_TX_EMPTY(sc) ((sc)->TRSR & SC_TRSR_TX_ATV_Msk ? 0 : 1)


/* RX Macros */

/**
  * @brief Read Rx data register.
  * @param[in] sc The base address of smartcard module.
  * @return The oldest data byte in RX FIFO.
  * @details By reading RBR register, the SC will return an 8-bit received data.
  * \hideinitializer
  */
#define SCUART_READ(sc) ((sc)->RBR)

/**
  * @brief Get RX FIFO empty flag status from register.
  * @param[in] sc The base address of smartcard module.
  * @retval 0 Receive FIFO is not empty.
  * @retval SC_TRSR_RX_EMPTY_F_Msk Receive FIFO is empty.
  * @details When the last byte of Rx buffer has been read by CPU, hardware sets RX_EMPTY_F(SC_TRSR[1]) high.
  *          It will be cleared when SC receives any new data.
  * \hideinitializer
  */
#define SCUART_GET_RX_EMPTY(sc) ((sc)->TRSR & SC_TRSR_RX_EMPTY_F_Msk)


/**
  * @brief Get RX FIFO full flag status from register.
  * @param[in] sc The base address of smartcard module.
  * @retval 0 Receive FIFO is not full.
  * @retval SC_TRSR_TX_FULL_F_Msk Receive FIFO is full.
  * @details RX_FULL_F(SC_TRSR[2]) is set when RX pointer is equal to 4, otherwise it is cleared by hardware.
  * \hideinitializer
  */
#define SCUART_GET_RX_FULL(sc) ((sc)->TRSR & SC_TRSR_RX_FULL_F_Msk)

/**
  * @brief Check if receive data number in FIFO reach FIFO trigger level or not.
  * @param[in] sc The base address of smartcard module.
  * @retval 0 The number of bytes in receive FIFO is less than trigger level.
  * @retval 1 The number of bytes in receive FIFO equals or larger than trigger level.
  * @details RDA_IS(SC_ISR[0]) is used for received data reaching trigger level RX_FTRI_LEV (SC_CTL[7:6]) interrupt status flag.
  * @note If receive trigger level is \b not 1 byte, this macro return 0 does not necessary indicates there is no data in FIFO.
  * \hideinitializer
  */
#define SCUART_IS_RX_READY(sc) ((sc)->ISR & SC_ISR_RDA_IS_Msk ? 1 : 0)

/**
  * @brief Check specified smartcard port receive FIFO is full or not.
  * @param[in] sc The base address of smartcard module.
  * @retval 0 Receive FIFO is not full.
  * @retval 1 Receive FIFO is full.
  * @details RX_FULL_F(SC_TRSR[2]) is set when RX pointer is equal to 4, otherwise it is cleared by hardware.
  * \hideinitializer
  */
#define SCUART_IS_RX_FULL(sc) ((sc)->TRSR & SC_TRSR_RX_FULL_F_Msk ? 1 : 0)

/* Interrupt Macros */

/**
  * @brief Enable specified interrupts.
  * @param[in] sc The base address of smartcard module.
  * @param[in] u32Mask Interrupt masks to enable, a combination of following bits.
  *             - \ref SC_IER_RTMR_IE_Msk
  *             - \ref SC_IER_TERR_IE_Msk
  *             - \ref SC_IER_TBE_IE_Msk
  *             - \ref SC_IER_RDA_IE_Msk
  * @return None
  * @details The macro is used to enable receiver buffer time-out interrupt, transfer error interrupt,
  *          transmit buffer empty interrupt or receive data reach trigger level interrupt.
  * \hideinitializer
  */
#define SCUART_ENABLE_INT(sc, u32Mask) ((sc)->IER |= (u32Mask))

/**
  * @brief Disable specified interrupts.
  * @param[in] sc The base address of smartcard module.
  * @param[in] u32Mask Interrupt masks to disable, a combination of following bits.
  *             - \ref SC_IER_RTMR_IE_Msk
  *             - \ref SC_IER_TERR_IE_Msk
  *             - \ref SC_IER_TBE_IE_Msk
  *             - \ref SC_IER_RDA_IE_Msk
  * @return None
  * @details The macro is used to disable receiver buffer time-out interrupt, transfer error interrupt,
  *          transmit buffer empty interrupt or receive data reach trigger level interrupt.
  * \hideinitializer
  */
#define SCUART_DISABLE_INT(sc, u32Mask) ((sc)->IER &= ~(u32Mask))

/**
  * @brief Get specified interrupt flag/status.
  * @param[in] sc The base address of smartcard module.
  * @param[in] u32Type Interrupt flag/status to check, could be one of following value:
  *             - \ref SC_ISR_RTMR_IS_Msk
  *             - \ref SC_ISR_TERR_IS_Msk
  *             - \ref SC_ISR_TBE_IS_Msk
  *             - \ref SC_ISR_RDA_IS_Msk
  * @return The status of specified interrupt.
  * @retval 0 Specified interrupt does not happened.
  * @retval 1 Specified interrupt happened.
  * @details The macro is used to get receiver buffer time-out interrupt status, transfer error interrupt status,
  *          transmit buffer empty interrupt status or receive data reach interrupt status.
  * \hideinitializer
  */
#define SCUART_GET_INT_FLAG(sc, u32Type) ((sc)->ISR & (u32Type) ? 1 : 0)

/**
  * @brief Clear specified interrupt flag/status.
  * @param[in] sc The base address of smartcard module.
  * @param[in] u32Type Interrupt flag/status to clear, could be the combination of following values:
  *             - \ref SC_ISR_RTMR_IS_Msk
  *             - \ref SC_ISR_TERR_IS_Msk
  *             - \ref SC_ISR_TBE_IS_Msk
  * @return None
  * @details The macro is used to clear receiver buffer time-out interrupt flag, transfer error interrupt flag or
  *          transmit buffer empty interrupt flag.
  * \hideinitializer
  */
#define SCUART_CLR_INT_FLAG(sc, u32Type) ((sc)->ISR = (u32Type))

/**
  * @brief Get receive error flag/status.
  * @param[in] sc The base address of smartcard module.
  * @return Current receive error status, could one of following errors:
  * @retval SC_TRSR_RX_EPA_F_Msk Parity error.
  * @retval SC_TRSR_RX_EFR_F_Msk Frame error.
  * @retval SC_TRSR_RX_EBR_F_Msk Break error.
  * @details The macro is used to get receiver parity error status, receiver frame error status or
  *          receiver break error status.
  * \hideinitializer
  */
#define SCUART_GET_ERR_FLAG(sc) ((sc)->TRSR & (SC_TRSR_RX_EPA_F_Msk | SC_TRSR_RX_EFR_F_Msk | SC_TRSR_RX_EBR_F_Msk))

/**
  * @brief Clear specified receive error flag/status.
  * @param[in] sc The base address of smartcard module.
  * @param[in] u32Mask Receive error flag/status to clear, combination following values:
  *             - \ref SC_TRSR_RX_EPA_F_Msk
  *             - \ref SC_TRSR_RX_EFR_F_Msk
  *             - \ref SC_TRSR_RX_EBR_F_Msk
  * @return None
  * @details The macro is used to clear receiver parity error flag, receiver frame error flag or
  *          receiver break error flag.
  * \hideinitializer
  */
#define SCUART_CLR_ERR_FLAG(sc, u32Mask) ((sc)->TRSR = (u32Mask))

void SCUART_Close(SC_T* sc);
uint32_t SCUART_Open(SC_T* sc, uint32_t u32baudrate);
uint32_t SCUART_Read(SC_T* sc, uint8_t *pu8RxBuf, uint32_t u32ReadBytes);
uint32_t SCUART_SetLineConfig(SC_T* sc, uint32_t u32Baudrate, uint32_t u32DataWidth, uint32_t u32Parity, uint32_t  u32StopBits);
void SCUART_SetTimeoutCnt(SC_T* sc, uint32_t u32TOC);
void SCUART_Write(SC_T* sc, uint8_t *pu8TxBuf, uint32_t u32WriteBytes);

/*@}*/ /* end of group SCUART_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group SCUART_Driver */

/*@}*/ /* end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif //__SCUART_H__

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
