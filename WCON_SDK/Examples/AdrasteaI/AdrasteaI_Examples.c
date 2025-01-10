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
#include <stdio.h>
#include <AdrasteaI/AdrasteaI_Examples.h>
#include <AdrasteaI/ATDeviceExamples.h>
#include <AdrasteaI/ATSocketExamples.h>
#include <AdrasteaI/ATNetServiceExamples.h>
#include <AdrasteaI/ATSIMExamples.h>
#include <AdrasteaI/ATProprietaryExamples.h>
#include <AdrasteaI/ATPacketDomainExamples.h>
#include <AdrasteaI/ATGNSSExamples.h>
#include <AdrasteaI/ATMQTTExamples.h>
#include <AdrasteaI/ATHTTPExamples.h>
#include <AdrasteaI/ATSMSExamples.h>
#include <AdrasteaI/ATPowerExamples.h>
#if defined(STM32L073xx)
#include <global_L0xx.h>
#elif defined(STM32F401xE)
#include <global_F4xx.h>
#endif

/**
 * @brief Definition of the control pins
 */
AdrasteaI_Pins_t AdrasteaI_pins;

/**
 * @brief Definition of the uart
 */
WE_UART_t AdrasteaI_uart;

/**
 * @brief Runs Adrastea examples.
 *
 * Comment/uncomment lines in this function to start the desired example.
 */
void AdrasteaI_Examples()
{
	AdrasteaI_uart.baudrate = 115200;
	AdrasteaI_uart.flowControl = WE_FlowControl_NoFlowControl;
	AdrasteaI_uart.parity = WE_Parity_None;
	AdrasteaI_uart.uartInit = WE_UART1_Init;
	AdrasteaI_uart.uartDeinit = WE_UART1_DeInit;
	AdrasteaI_uart.uartTransmit = WE_UART1_Transmit;

	AdrasteaI_pins.AdrasteaI_Pin_Reset.port = (void*) GPIOA;
	AdrasteaI_pins.AdrasteaI_Pin_Reset.pin = GPIO_PIN_10;
	AdrasteaI_pins.AdrasteaI_Pin_WakeUp.port = (void*) GPIOA;
	AdrasteaI_pins.AdrasteaI_Pin_WakeUp.pin = GPIO_PIN_9;

	ATDeviceExample();
//    ATGNSSExample();
//    ATHTTPExample();
//    ATMQTTExample();
//    ATNetServiceExample();
//    ATPacketDomainExample();
//    ATPowerExample();
//    ATProprietaryExample();
//    ATSIMExample();
//    ATSMSExample();
//	ATSocketExample();

	return;
}

/**
 * @brief Prints the supplied string, prefixed with OK or NOK (depending on the success parameter).
 *
 * @param str String to print
 * @param success Variable indicating if action was ok
 */
void AdrasteaI_ExamplesPrint(char *str, bool success)
{
	WE_DEBUG_PRINT("%s%s\r\n", success ? "OK    " : "NOK   ", str);
}
