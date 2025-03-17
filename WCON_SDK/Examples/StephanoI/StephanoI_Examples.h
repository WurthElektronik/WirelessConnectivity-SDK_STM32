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
 * @brief StephanoI examples.
 *
 * Comment/uncomment lines in StephanoI_Examples() to start the desired example.
 */

#ifndef STEPHANOI_EXAMPLES_H_INCLUDED
#define STEPHANOI_EXAMPLES_H_INCLUDED

#include <StephanoI/ATCommands/ATEvent.h>
#include <StephanoI/StephanoI.h>
#include <global/global.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

    extern StephanoI_Pins_t StephanoI_pins;
    extern WE_UART_t StephanoI_uart;

    extern bool StephanoI_Examples_startupEventReceived;

    extern void StephanoI_Examples(void);
    extern void StephanoI_Examples_Print(char* str, bool success);
    extern bool StephanoI_Examples_WaitForStartup(uint32_t timeoutMs);
    extern void StephanoI_Examples_EventCallback(char* eventText);

#ifdef __cplusplus
}
#endif

#endif /* STEPHANOI_EXAMPLES_H_INCLUDED */
