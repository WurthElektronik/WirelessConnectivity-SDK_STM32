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
 * @brief DaphnisI examples.
 *
 * Comment/uncomment lines in DaphnisI_Examples() to start the desired example.
 */

#ifndef DAPHNISI_EXAMPLES_H_INCLUDED
#define DAPHNISI_EXAMPLES_H_INCLUDED

#include <DaphnisI/DaphnisI.h>

#ifdef __cplusplus
extern "C"
{
#endif

    extern DaphnisI_Pins_t DaphnisI_pins;

    extern WE_UART_t DaphnisI_uart;

    extern void DaphnisI_Examples(void);

#ifdef __cplusplus
}
#endif

#endif /* DAPHNISI_EXAMPLES_H_INCLUDED */
