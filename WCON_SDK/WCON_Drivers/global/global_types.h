/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK:
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
 * @file global_types.h
 * @brief Contains type definitions used in the Wireless Connectivity SDK.
 */

#ifndef GLOBAL_TYPES_H_INCLUDED
#define GLOBAL_TYPES_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Combines major, minor, and patch version numbers into a single 32-bit integer.
 *
 * @param[in] ma: Major
 * @param[in] mi: Minor
 * @param[in] pa: Patch
 *
 * @return int that represents the firmware version.
 */

#define FW(ma, mi, pa) ((ma << 16) | (mi << 8) | pa)

/**
 * @brief Marks the pin as input or output pin.
 */
typedef enum WE_Pin_Type_t
{
    WE_Pin_Type_Input = (uint8_t)0, /**< Input pin */
    WE_Pin_Type_Output = (uint8_t)1 /**< Output pin */
} WE_Pin_Type_t;

/**
 * @brief Defines pin pull type.
 */
typedef enum WE_Pin_PullType_t
{
    WE_Pin_PullType_No = (uint8_t)0,  /**< No pull up / pull down */
    WE_Pin_PullType_Up = (uint8_t)1,  /**< Pull up */
    WE_Pin_PullType_Down = (uint8_t)2 /**< Pull down */
} WE_Pin_PullType_t;

/**
 * @brief Defines the level of a pin (high/low).
 */
typedef enum WE_Pin_Level_t
{
    WE_Pin_Level_Low = (uint8_t)0, /**< Pin level LOW */
    WE_Pin_Level_High = (uint8_t)1 /**< Pin level HIGH */
} WE_Pin_Level_t;

/**
 * @brief Configuration of one pin.
 */
typedef struct WE_Pin_t
{
    void* pin_def;      /**< Pointer to pin definition (platform agnostic) */
    WE_Pin_Type_t type; /**< Pin type */
    union
    {
        WE_Pin_PullType_t input_pull; /**< Initial value of input pull type */
        WE_Pin_Level_t output;        /**< Initial value of output */
    } initial_value;                  /**< Initial value of the pin */
} WE_Pin_t;

/**
 * @brief UART flow control configuration.
 */
typedef enum WE_FlowControl_t
{
    WE_FlowControl_NoFlowControl = (uint8_t)0, /**< Flow control disabled */
    WE_FlowControl_RTSOnly = (uint8_t)1,       /**< Flow control using RTS only */
    WE_FlowControl_CTSOnly = (uint8_t)2,       /**< Flow control using CTS only */
    WE_FlowControl_RTSAndCTS = (uint8_t)3,     /**< Flow control using RTS and CTS */
} WE_FlowControl_t;

/**
 * @brief UART parity configuration.
 */
typedef enum WE_Parity_t
{
    WE_Parity_None, /**< No parity */
    WE_Parity_Odd,  /**< Odd parity */
    WE_Parity_Even  /**< Even parity */
} WE_Parity_t;

/**
 * @brief Handle one or several bytes received via UART.
 *
 * @param[in] received_bytesP: Pointer to the buffer of bytes received via UART
 * @param[in] length: Number of received bytes
 *
 * @return None
 */
typedef void (*WE_UART_HandleRxByte_t)(uint8_t* received_bytesP, size_t length);

/**
 * @brief Initialization and start of the UART using the provided settings (baud rate, flow control,...).
 * 
 * @param[in] baudrate: Baud rate
 * @param[in] flow_control: The type of flow control
 * @param[in] parity: The type of parity
 * @param[in] RXbyte_handlerP: Function pointer to the byte handler that will be called whenever bytes are received
 *
 * @return True in case initialization succeeded, false otherwise
 */
typedef bool (*WE_UART_Init_t)(uint32_t baudrate, WE_FlowControl_t flow_control, WE_Parity_t parity, WE_UART_HandleRxByte_t* RXbyte_handlerP);

/**
 * @brief De-initialization of the UART.
 *
 * @return True in case de-initialization succeeded, false otherwise
 */
typedef bool (*WE_UART_DeInit_t)();

/**
 * @brief Transmit data via UART.
 *
 * @param[in] dataP: Pointer to buffer of data to be sent via UART
 * @param[in] length: Number of bytes to be sent
 *
 * @return True in case UART data transmission succeeded, false otherwise
 */
typedef bool (*WE_UART_Transmit_t)(const uint8_t* dataP, uint16_t length);

/**
 * @brief Used to store pointers to UART functions and configuration.
 */
typedef struct WE_UART_t
{
    WE_UART_Init_t uartInit;         /**< Pointer to UART initialization function */
    WE_UART_DeInit_t uartDeinit;     /**< Pointer to UART de-initialization function */
    WE_UART_Transmit_t uartTransmit; /**< Pointer to UART transmit function */
    uint32_t baudrate;               /**< UART baud rate configuration */
    WE_FlowControl_t flowControl;    /**< UART flow control setting */
    WE_Parity_t parity;              /**< UART parity configuration */
} WE_UART_t;

#endif /* GLOBAL_TYPES_H_INCLUDED */
