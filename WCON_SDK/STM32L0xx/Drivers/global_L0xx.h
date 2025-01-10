/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK for STM32:
 *
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE THAT WÜRTH ELEKTRONIK
 * EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY KIND RELATED TO, BUT NOT LIMITED
 * TO THE NON-INFRINGEMENT OF THIRD PARTIES’ INTELLECTUAL PROPERTY RIGHTS OR THE
 * MERCHANTABILITY OR FITNESS FOR YOUR INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT
 * WARRANT OR REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY PATENT
 * RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY RIGHT RELATING TO ANY
 * COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT IS USED. INFORMATION PUBLISHED BY
 * WÜRTH ELEKTRONIK EISOS REGARDING THIRD-PARTY PRODUCTS OR SERVICES DOES NOT CONSTITUTE A LICENSE
 * FROM WÜRTH ELEKTRONIK EISOS TO USE SUCH PRODUCTS OR SERVICES OR A WARRANTY OR ENDORSEMENT
 * THEREOF
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS DRIVER PACKAGE.
 *
 * COPYRIGHT (c) 2025 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief STM32L0xx platform driver.
 */

#ifndef GLOBAL_L0XX_H_INCLUDED
#define GLOBAL_L0XX_H_INCLUDED

#ifdef STM32L073xx

#include <stdbool.h>

#include "stm32l0xx_hal.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_dma.h"
#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_usart.h"

#include <global/global_types.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void WE_SystemClock_Config(void);

/**
 * @brief Initialize and start the UART.
 *
 * @param[in] baudrate Baud rate of the serial interface
 * @param[in] flowControl Enable/disable flow control
 * @param[in] parity Parity bit configuration
 * @param[in] rxByteHandlerP Pointer to the handle rx byte function inside the driver. (this function should be called by the ISR for uart on data reception)
 */
extern bool WE_UART1_Init(uint32_t baudrate, WE_FlowControl_t flowControl, WE_Parity_t parity, WE_UART_HandleRxByte_t *rxByteHandlerP);

/**
 * @brief Deinitialize and stop the UART.
 */
extern bool WE_UART1_DeInit();

/**
 * @brief Transmit data via UART.
 *
 * @param[in] data Pointer to data buffer (data to be sent)
 * @param[in] length Number of bytes to be sent
 */
extern bool WE_UART1_Transmit(const uint8_t *data, uint16_t length);

/**
 * @brief Initialize and start the UART.
 *
 * @param[in] baudrate Baud rate of the serial interface
 * @param[in] flowControl Enable/disable flow control
 * @param[in] parity Parity bit configuration
 * @param[in] rxByteHandlerP Pointer to the handle rx byte function inside the driver. (this function should be called by the ISR for uart on data reception)
 */
extern bool WE_UART4_Init(uint32_t baudrate, WE_FlowControl_t flowControl, WE_Parity_t parity, WE_UART_HandleRxByte_t* rxByteHandlerP);

/**
 * @brief Deinitialize and stop the UART.
 */
extern bool WE_UART4_DeInit();

/**
 * @brief Transmit data via UART.
 *
 * @param[in] data Pointer to data buffer (data to be sent)
 * @param[in] length Number of bytes to be sent
 */
extern bool WE_UART4_Transmit(const uint8_t *data, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif /* STM32L073xx */

#endif /* GLOBAL_L0XX_H_INCLUDED */
