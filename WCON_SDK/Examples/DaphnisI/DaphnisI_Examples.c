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
 * @brief Main file for DaphnisI driver examples.
 *
 */
#include <DaphnisI/DaphnisI_Examples.h>
#include <DaphnisI/DaphnisI_LoRaWAN_Example.h>
#include <DaphnisI/DaphnisI_Local_GPIO_Example.h>
#include <DaphnisI/DaphnisI_P2P_Example.h>
#include <DaphnisI/DaphnisI_P2P_Throughput_Test.h>
#include <DaphnisI/DaphnisI_Remote_GPIO_Example.h>
#include <global/global.h>
#include <global_platform_types.h>
#include <stdio.h>
/**
 * @brief Definition of the control pins
 */
DaphnisI_Pins_t DaphnisI_pins = {
    .DaphnisI_Pin_Reset = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_10)),
    .DaphnisI_Pin_WakeUp = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_5)),
    .DaphnisI_Pin_Boot = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_7)),
};

/**
 * @brief Definition of the uart
 */
WE_UART_t DaphnisI_uart;

/**
 * @brief The application's main function.
 */
void DaphnisI_Examples(void)
{

    DaphnisI_uart.baudrate = DAPHNISI_DEFAULT_BAUDRATE;
    DaphnisI_uart.flowControl = WE_FlowControl_NoFlowControl;
    DaphnisI_uart.parity = WE_Parity_None;
    DaphnisI_uart.uartInit = WE_UART1_Init;
    DaphnisI_uart.uartDeinit = WE_UART1_DeInit;
    DaphnisI_uart.uartTransmit = WE_UART1_Transmit;

    DaphnisI_OTAA_JoinExample();
    //	DaphnisI_P2P_Send();
    //	DaphnisI_Local_GPIO_Example();
    //	DaphnisI_Remote_GPIO_Example();
    //	DaphnisI_P2P_Throughput_Test();
}
