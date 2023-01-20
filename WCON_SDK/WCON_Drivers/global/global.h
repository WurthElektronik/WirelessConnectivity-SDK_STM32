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
 * COPYRIGHT (c) 2022 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief This is the main header file of the WE Wireless Connectivity SDK for STM32.
 */

#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>

#ifdef STM32L073xx
#include "global_L0xx.h"
#endif

#ifdef STM32F401xE
#include "global_F4xx.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Driver version
 */
#define WE_WIRELESS_CONNECTIVITY_SDK_VERSION {1,4,0}

#if defined(WE_DEBUG) || defined(WE_DEBUG_INIT)
/* Redirect printf() to UART for testing/debugging purposes */
#include "debug.h"
#else
/* Ignore all occurrences of fprintf(), printf() and fflush() */
#define fprintf(...)
#define printf(...)
#define fflush(...)
#endif /* WE_DEBUG */


/**
 * @brief Priority for DMA interrupts (used for receiving data from radio module)
 */
#define WE_PRIORITY_DMA_RX 0

/**
 * @brief Priority for UART interrupts (used for communicating with radio module)
 */
#define WE_PRIORITY_UART_RX 0

/**
 * @brief Priority for (asynchronous) processing of data received from radio module.
 */
#define WE_PRIORITY_RX_DATA_PROCESSING 1

/**
 * @brief Priority for UART interface used for debugging.
 */
#define WE_PRIORITY_UART_DEBUG 2

/**
 * @brief Size of DMA receive buffer.
 */
#define WE_DMA_RX_BUFFER_SIZE 512


/**
 * @brief UART interface used for communicating with radio module.
 */
extern USART_TypeDef *WE_uartWireless;

/**
 * @brief Flow control setting used for WE_uartWireless.
 */
extern WE_FlowControl_t WE_uartFlowControl;

/**
 * @brief Parity setting used for WE_uartWireless.
 */
extern WE_Parity_t WE_uartParity;

/**
 * @brief Is set to true if using DMA to receive data from radio module.
 */
extern bool WE_dmaEnabled;

/**
 * @brief Buffer used for WE_dmaWirelessRx.
 */
extern uint8_t WE_dmaRxBuffer[WE_DMA_RX_BUFFER_SIZE];

/**
 * @brief DMA used for receiving data from radio module (used only if DMA is enabled).
 */
extern DMA_TypeDef *WE_dmaWirelessRx;

/**
 * @brief DMA stream used for receiving data from radio module (used only if DMA is enabled).
 */
extern uint32_t WE_dmaWirelessRxStream;

/**
 * @brief Last read position in DMA receive buffer (used only if DMA is enabled).
 * @see WE_CheckIfDmaDataAvailable()
 */
extern size_t WE_dmaLastReadPos;

/**
 * @brief Initializes the platform (peripherals, flash interface, Systick, system clock, interrupts etc.)
 */
extern void WE_Platform_Init(void);

/**
 * @brief Initialize and start the UART.
 *
 * @param[in] baudrate Baud rate of the serial interface
 * @param[in] flowControl Enable/disable flow control
 * @param[in] par Parity bit configuration
 * @param[in] dma Enables DMA for receiving data
 */
extern void WE_UART_Init(uint32_t baudrate,
                         WE_FlowControl_t flowControl,
                         WE_Parity_t par,
                         bool dma);

/**
 * @brief Deinitialize and stop the UART.
 */
extern void WE_UART_DeInit();

/**
 * @brief Handle a single byte received via UART.
 *
 * @param[in] receivedByte The received byte
 */
extern void WE_UART_HandleRxByte(uint8_t receivedByte);

/**
 * @brief Transmit data via UART.
 *
 * @param[in] data Pointer to data buffer (data to be sent)
 * @param[in] length Number of bytes to be sent
 */
extern void WE_UART_Transmit(const uint8_t *data, uint16_t length);

/**
 * @brief Is called in case of a critical HAL error.
 */
extern void WE_Error_Handler(void);

/**
 * @brief Configure the system clock.
 */
extern void WE_SystemClock_Config(void);

/**
 * @brief Request the 3 byte driver version
 *
 * @param[out] version Pointer to the 3 byte driver version.
 * @return true if request succeeded, false otherwise
 */
extern bool WE_GetDriverVersion(uint8_t *version);

/**
 * @brief Initialize GPIO pins.
 *
 * @param[in] pins Array of pins to configure. Entries that have port or pin number set to zero are ignored.
 * @param[in] numPins Number of elements in pins array.
 * @return true if request succeeded, false otherwise
 */
extern bool WE_InitPins(WE_Pin_t pins[], uint8_t numPins);

/**
 * @brief Deinitialize a pin.
 *
 * @param[in] pin Pin to be deinitialized
 * @return true if request succeeded, false otherwise
 */
extern bool WE_DeinitPin(WE_Pin_t pin);

/**
 * @brief Switch pin to output high/low
 *
 * @param[in] pin Output pin to be set
 * @param[in] out Output level to be set
 * @return true if request succeeded, false otherwise
 */
extern bool WE_SetPin(WE_Pin_t pin, WE_Pin_Level_t out);

/**
 * @brief Read current pin level.
 *
 * @param[in] pin Pin to be read
 * @return Current level of pin
 */
extern WE_Pin_Level_t WE_GetPinLevel(WE_Pin_t pin);

/**
 * @brief Sleep function.
 *
 * @param[in] sleepForMs Delay in milliseconds
 */
extern void WE_Delay(uint16_t sleepForMs);

/**
 * @brief Sleep function.
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @param[in] sleepForUsec Delay in microseconds
 */
extern void WE_DelayMicroseconds(uint32_t sleepForUsec);

/**
 * @brief Returns current tick value (in milliseconds).
 *
 * @return Current tick value (in milliseconds)
 */
extern uint32_t WE_GetTick();

/**
 * @brief Returns current tick value (in microseconds).
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @return Current tick value (in microseconds)
 */
extern uint32_t WE_GetTickMicroseconds();

/**
 * @brief Checks if the DMA buffer contains unread data and calls OnDmaDataReceived() if so.
 */
extern void WE_CheckIfDmaDataAvailable();

/**
 * @brief Triggers asynchronous execution of WE_CheckIfDmaDataAvailable().
 *
 * Is used in ISRs to trigger reading of bytes from DMA receive buffer, which may take quite
 * some time and can thus not be done directly in ISR.
 *
 * Asynchronous execution is implemented using pendable service interrupt (PendSV).
 */
extern void WE_CheckIfDmaDataAvailableAsync();

#ifdef __cplusplus
}
#endif

#endif /* GLOBAL_H_INCLUDED */
