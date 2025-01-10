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

#include <StephanoI/StephanoI_Examples.h>
#include <StephanoI/StephanoI.h>
#include <StephanoI/StephanoI_Device_Example.h>
#include <StephanoI/StephanoI_BluetoothLE_Example.h>
#include <StephanoI/StephanoI_Wifi_Example.h>

#include <StephanoI/ATCommands/ATEvent.h>

#if defined(STM32L073xx)
#include <global_L0xx.h>
#elif defined(STM32F401xE)
#include <global_F4xx.h>
#endif

/**
 * @brief Is set to true when a startup event is received
 */
bool StephanoI_Examples_startupEventReceived = false;

/**
 * @brief Definition of the uart
 */
WE_UART_t StephanoI_uart;

/**
 * @brief Definition of the control pins
 */
StephanoI_Pins_t StephanoI_pins;

/**
 * @brief Waits for the startup event to be received.
 */
bool StephanoI_Examples_WaitForStartup(uint32_t timeoutMs)
{
	uint32_t t0 = WE_GetTick();
	StephanoI_Examples_startupEventReceived = false;
	while (false == StephanoI_Examples_startupEventReceived && (WE_GetTick() - t0) < timeoutMs)
	{
	}
	return StephanoI_Examples_startupEventReceived;
}

/**
 * @brief Runs StephanoI examples.
 *
 * Comment/uncomment lines in this function to start the desired example.
 */
void StephanoI_Examples(void)
{
	StephanoI_pins.StephanoI_Pin_Reset.port = (void*) GPIOA;
	StephanoI_pins.StephanoI_Pin_Reset.pin = GPIO_PIN_10;
	StephanoI_pins.StephanoI_Pin_Wakeup.port = (void*) GPIOA;
	StephanoI_pins.StephanoI_Pin_Wakeup.pin = GPIO_PIN_5;

	StephanoI_uart.baudrate = STEPHANOI_DEFAULT_BAUDRATE;
	StephanoI_uart.flowControl = WE_FlowControl_NoFlowControl;
	StephanoI_uart.parity = WE_Parity_None;
	StephanoI_uart.uartInit = WE_UART1_Init;
	StephanoI_uart.uartDeinit = WE_UART1_DeInit;
	StephanoI_uart.uartTransmit = WE_UART1_Transmit;

	/* The example to be started can be picked from the list below by calling the corresponding function. */

	StephanoI_Device_Example();
	//StephanoI_DeviceSleep_Example();
	//StephanoI_BluetoothLE_Peripheral_Advertising_Example();
	//StephanoI_BluetoothLE_Peripheral_Data_Example();
	//StephanoI_BluetoothLE_Central_Data_Example();
	//StephanoI_BluetoothLE_Central_ThroughputData_Example();

	//StephanoI_Wifi_AP_Example();
	//StephanoI_Wifi_Station_Example();
	//StephanoI_Wifi_MQTT_Example();
	//StephanoI_Wifi_HTTP_Example();
	//StephanoI_Wifi_Provisioning_Example();
	//StephanoI_Wifi_OTA_Example();

	//StephanoI_TCP_ClientThroughput_Example();
	//StephanoI_TCP_Client_Example();
	//StephanoI_UDP_Client_Example();
	//StephanoI_UDP_ClientThroughput_Example();
	//StephanoI_TCP_Server_Example();
	//StephanoI_SSL_Server_Example();

	return;
}

/**
 * @brief Is called when an event notification has been received.
 *
 * Note that this function is called from an interrupt - code in this function
 * should thus be kept simple.
 *
 * Note in particular that it is not possible to send AT commands to StephanoI
 * from within this event handler.
 *
 * Also note that not all calls of this handler necessarily correspond to valid
 * events (i.e. events from StephanoI_ATEvent_t). Some events might in fact be responses
 * to AT commands that are not included in StephanoI_ATEvent_t.
 */
void StephanoI_Examples_EventCallback(char *eventText)
{
	StephanoI_ATEvent_t event;
	if (false == StephanoI_ATEvent_ParseEventType(&eventText, &event))
	{
		return;
	}

	switch (event)
	{
	case StephanoI_ATEvent_Startup:
		StephanoI_Examples_startupEventReceived = true;
		break;
	default:
		break;
	}
}

/**
 * @brief Prints the supplied string, prefixed with OK or NOK (depending on the success parameter).
 *
 * @param str String to print
 * @param success Variable indicating if action was ok
 */
void StephanoI_Examples_Print(char *str, bool success)
{
	WE_DEBUG_PRINT("%s%s\r\n", success ? "OK    " : "NOK   ", str);
}
