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
 * @file
 * @brief This file contains the platform dependent pin definition used in the Wireless Connectivity SDK.
 *
 * @details For any other platform, which is not STM32, please define the WE_XXX_Pin_t struct and WE_XXX_PIN macro
 * needed for your platform.
 *
 */

#ifndef GLOBAL_PLATFORM_TYPES_H_
#define GLOBAL_PLATFORM_TYPES_H_

#if defined(STM32L073xx)
#include "global_L0xx.h"
#elif defined(STM32F401xE)
#include "global_F4xx.h"
#endif

/**
 * @brief Priority for UART interrupts (used for communicating with radio module)
 */
#define WE_PRIORITY_UART_RX 0

/**
 * @brief Priority for (asynchronous) processing of data received from radio module.
 */
#define WE_PRIORITY_RX_DATA_PROCESSING 1

/**
 * @brief Configuration of a STM32 pin.
 */
typedef struct WE_STM32_Pin_t
{
    GPIO_TypeDef* port;
    uint32_t pin;
} WE_STM32_Pin_t;

#define WE_STM32_PIN(PORT_ID, PIN_ID) ((WE_STM32_Pin_t){.port = PORT_ID, .pin = PIN_ID})

/**
* @brief Initializes the platform (peripherals, flash interface, Systick, system clock, interrupts etc.)
*/
extern void WE_Platform_Init(void);

/**
* @brief Is called in case of a critical HAL error.
*/
extern void WE_Error_Handler(void);

#endif /* GLOBAL_PLATFORM_TYPES_H_ */
