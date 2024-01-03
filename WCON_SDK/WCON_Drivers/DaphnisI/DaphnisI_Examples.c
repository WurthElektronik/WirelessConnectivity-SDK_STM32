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
 * COPYRIGHT (c) 2023 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Main file for DaphnisI driver examples.
 *
 */
#include <global/global.h>
#include <DaphnisI/DaphnisI.h>
#include <DaphnisI/Commands/ATDevice.h>
#include <DaphnisI/Commands/LoRaWAN.h>
#include <DaphnisI/Commands/ATUserSettings.h>
#include <DaphnisI/Commands/ATEvent.h>
#include <stdio.h>

static void main_EventCallback(char *eventText);
static void OTAA_JoinExample(void);

static uint8_t rxDataBuffer[DAPHNISI_MAX_PAYLOAD_SIZE];
static DaphnisI_RxData_t rxData;

/*
 * State machine for Daphnis-I sdk example.
 * */
typedef enum
{
	DaphnisI_SM_Set_Up_Keys,
	DaphnisI_SM_Join_Network,
	DaphnisI_SM_Class_Change,
	DaphnisI_SM_Send_Data,
	DaphnisI_SM_Receive_Data,
	DaphnisI_SM_Idle,
	DaphnisI_SM_Error
} DaphnisI_SM_t;

static volatile DaphnisI_SM_t currentState = DaphnisI_SM_Idle;

/**
 * @brief Definition of the control pins
 */
DaphnisI_Pins_t DaphnisI_pins;

/**
 * @brief Definition of the uart
 */
WE_UART_t DaphnisI_uart;

/**
 * @brief The application's main function.
 */
void DaphnisI_Examples(void)
{
	DaphnisI_pins.DaphnisI_Pin_Reset.port = (void*) GPIOA;
	DaphnisI_pins.DaphnisI_Pin_Reset.pin = GPIO_PIN_10;
	DaphnisI_pins.DaphnisI_Pin_WakeUp.port = (void*) GPIOA;
	DaphnisI_pins.DaphnisI_Pin_WakeUp.pin = GPIO_PIN_5;
	DaphnisI_pins.DaphnisI_Pin_Boot.port = (void*) GPIOA;
	DaphnisI_pins.DaphnisI_Pin_Boot.pin = GPIO_PIN_7;
	DaphnisI_pins.DaphnisI_Pin_StatusInd0.port = (void*) GPIOB;
	DaphnisI_pins.DaphnisI_Pin_StatusInd0.pin = GPIO_PIN_8;
	DaphnisI_pins.DaphnisI_Pin_StatusInd1.port = (void*) GPIOB;
	DaphnisI_pins.DaphnisI_Pin_StatusInd1.pin = GPIO_PIN_9;

	rxData.data = rxDataBuffer;

	DaphnisI_uart.baudrate = DAPHNISI_DEFAULT_BAUDRATE;
	DaphnisI_uart.flowControl = WE_FlowControl_NoFlowControl;
	DaphnisI_uart.parity = WE_Parity_None;
	DaphnisI_uart.uartInit = WE_UART1_Init;
	DaphnisI_uart.uartDeinit = WE_UART1_DeInit;
	DaphnisI_uart.uartTransmit = WE_UART1_Transmit;

	OTAA_JoinExample();
}

/**
 * @brief In this example the module, that is already registered at TTN network, is joining the network
 * with the provided application EUI and application key. After joining, the module transmits a test string and is able
 * to receive data from the TTN server.
 */
static void OTAA_JoinExample()
{
	bool ret = false;

	if (false == DaphnisI_Init(&DaphnisI_uart, &DaphnisI_pins, main_EventCallback))
	{
		printf("Initialization error\r\n");
		return;
	}

	while (1)
	{
		switch (currentState)
		{
		case DaphnisI_SM_Set_Up_Keys:
		{
			ret = true;
			pEui_t appEUI = "00:00:00:00:00:00:00:00";
			pKey_t appKey = "71:42:A2:01:2B:43:80:AD:7E:79:E3:C0:16:5A:98:A8";
			pEui_t device_eui;
			ret &= DaphnisI_GetDeviceEUI(&device_eui); //read device EUI to enter on network server
			printf("Device EUI is %s\r\n", device_eui);
			ret &= DaphnisI_SetAPPEUI(appEUI);
			ret &= DaphnisI_SetKey(DaphnisI_KEY_APP, appKey);
			currentState = ret ? DaphnisI_SM_Join_Network : DaphnisI_SM_Error;
			break;
		}
		case DaphnisI_SM_Join_Network:
		{
			ret = true;
			ret &= DaphnisI_Join(DaphnisI_JoinMode_OTAA);
			currentState = ret ? DaphnisI_SM_Idle : DaphnisI_SM_Error;
			break;
		}
		case DaphnisI_SM_Send_Data:
		{
			ret = true;
			const char *payload = "Hello. I'm a test string";
			ret &= DaphnisI_Send(1, (const uint8_t*) payload, strlen(payload), false);
			currentState = ret ? DaphnisI_SM_Send_Data : DaphnisI_SM_Error;
			WE_Delay(60000); // transmit one packet each 60s
			break;
		}
		case DaphnisI_SM_Receive_Data:
		{
			printf("%i bytes received on on port %i: %s\r\n", rxData.dataLength, rxData.port, rxData.data);
			currentState = DaphnisI_SM_Send_Data;
			break;
		}
		case DaphnisI_SM_Error:
		{
			printf("Error occurred\r\n");
			return;
		}
		default:
			break;
		}
	}

	return;
}

/**
 * @brief Is called when an event notification has been received.
 *
 * Note that this function is called from an interrupt - code in this function
 * should thus be kept simple.
 *
 * Note in particular that it is not possible to send AT commands to Daphnis
 * from within this event handler.
 *
 * Also note that not all calls of this handler necessarily correspond to valid
 * events (i.e. events from DaphnisI_ATEvent_t). Some events might in fact be responses
 * to AT commands that are not included in DaphnisI_ATEvent_t.
 */
static void main_EventCallback(char *eventText)
{
	DaphnisI_ATEvent_t event;
	if (!DaphnisI_ATEvent_ParseEventType(&eventText, &event))
	{
		return;
	}

	switch (event)
	{
	case DaphnisI_ATEvent_Sysnotf:
	{
		currentState = DaphnisI_SM_Set_Up_Keys;
		break;
	}
	case DaphnisI_ATEvent_JoinState_Success:
	{
		currentState = DaphnisI_SM_Send_Data;
		break;
	}
	case DaphnisI_ATEvent_JoinState_Fail:
	{
		currentState = DaphnisI_SM_Error;
		break;
	}
	case DaphnisI_ATEvent_RxData:
	{
		currentState = DaphnisI_ATEvent_ParseRxData(&eventText, &rxData) ? DaphnisI_SM_Receive_Data : DaphnisI_SM_Error;
		break;
	}
	case DaphnisI_ATEvent_Class:
	case DaphnisI_ATEvent_BeaconInfo:
	case DaphnisI_ATEvent_RxInfo:
	case DaphnisI_ATEvent_TxConf:
	case DaphnisI_ATEvent_BeaconLost:
	case DaphnisI_ATEvent_BeaconNotReceived:
	case DaphnisI_ATEvent_ContextState_Stored:
	case DaphnisI_ATEvent_ContextState_Restored:
	default:
		break;
	}
}
