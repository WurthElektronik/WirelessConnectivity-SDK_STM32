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

#include "global_types.h"

#ifdef __cplusplus
extern "C" {
#endif


extern void WE_SystemClock_Config(void);

extern void WE_UART_Init(uint32_t baudrate,
                         WE_FlowControl_t fc,
                         WE_Parity_t par,
                         bool dma);
extern void WE_UART_DeInit();
extern void WE_DMA_Init();
extern void WE_DMA_DeInit();


#ifdef __cplusplus
}
#endif


#endif /* STM32L073xx */

#endif /* GLOBAL_L0XX_H_INCLUDED */
