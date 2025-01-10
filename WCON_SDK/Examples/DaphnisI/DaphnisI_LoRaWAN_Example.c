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
#include <global/global.h>
#include <DaphnisI/DaphnisI.h>
#include <DaphnisI/ATCommands/ATDevice.h>
#include <DaphnisI/ATCommands/LoRaWAN.h>
#include <DaphnisI/ATCommands/ATUserSettings.h>
#include <stdio.h>
#include <DaphnisI/DaphnisI_Examples.h>
#include <DaphnisI/DaphnisI_LoRaWAN_Example.h>
#include <string.h>

static void DaphnisI_OTAA_EventCallback(DaphnisI_ATEvent_t event, char *eventText);

static uint8_t rxDataBuffer[DAPHNISI_LORAWAN_MAX_PAYLOAD_SIZE];
static DaphnisI_RxData_t rxData;

/*
 * State machine for Daphnis-I sdk example.
 * */
typedef enum
{
	DaphnisI_LoRaWAN_SM_Set_Up_Keys,
	DaphnisI_LoRaWAN_SM_Join_Network,
	DaphnisI_LoRaWAN_SM_Send_Data,
	DaphnisI_LoRaWAN_SM_Receive_Data,
	DaphnisI_LoRaWAN_SM_Idle,
	DaphnisI_LoRaWAN_SM_Error
} DaphnisI_LoRaWAN_SM_t;

static volatile DaphnisI_LoRaWAN_SM_t currentState = DaphnisI_LoRaWAN_SM_Idle;

/**
 * @brief In this example the module, that is already registered at TTN network, is joining the network
 * with the provided application EUI and application key. After joining, the module transmits a test string and is able
 * to receive data from the TTN server.
 */
void DaphnisI_OTAA_JoinExample()
{
	bool ret = false;

	rxData.data = rxDataBuffer;

	if (!DaphnisI_Init(&DaphnisI_uart, &DaphnisI_pins, DaphnisI_OTAA_EventCallback))
	{
		WE_DEBUG_PRINT("Initialization error\r\n");
		return;
	}

	while (1)
	{
		switch (currentState)
		{
		case DaphnisI_LoRaWAN_SM_Set_Up_Keys:
		{
			ret = true;
			DaphnisI_EUI_t appEUI = {0};
			DaphnisI_Key_t appKey = {0xA8, 0x98, 0x5A, 0x16, 0xC0, 0xE3, 0x79, 0x7E, 0xAD, 80, 0x43, 0x2B, 0x01, 0xA2, 0x42, 0x71};
			DaphnisI_EUI_t device_eui;
			ret &= DaphnisI_LoRaWAN_GetDeviceEUI(&device_eui); //read device EUI to enter on network server
			WE_DEBUG_PRINT("Device EUI is ");
			for(uint8_t i=0; i<DAPHNISI_LORAWAN_EUI_LENGTH-1; i++)
			{
				WE_DEBUG_PRINT("%02X:", device_eui[i]);
			}
			WE_DEBUG_PRINT("%02X\r\n", device_eui[DAPHNISI_LORAWAN_EUI_LENGTH-1]);
			ret &= DaphnisI_LoRaWAN_SetAPPEUI(appEUI);
			ret &= DaphnisI_LoRaWAN_SetKey(DaphnisI_KeyType_APP, appKey);
			currentState = ret ? DaphnisI_LoRaWAN_SM_Join_Network : DaphnisI_LoRaWAN_SM_Error;
			break;
		}
		case DaphnisI_LoRaWAN_SM_Join_Network:
		{
			ret = true;
			ret &= DaphnisI_LoRaWAN_Join(DaphnisI_JoinMode_OTAA);
			currentState = ret ? DaphnisI_LoRaWAN_SM_Idle : DaphnisI_LoRaWAN_SM_Error;
			break;
		}
		case DaphnisI_LoRaWAN_SM_Send_Data:
		{
			ret = true;
			const char *payload = "Hello. I'm a test string";
			ret &= DaphnisI_LoRaWAN_Send(1, (const uint8_t*) payload, strlen(payload), false);
			currentState = ret ? DaphnisI_LoRaWAN_SM_Send_Data : DaphnisI_LoRaWAN_SM_Error;
			WE_Delay(60000); // transmit one packet each 60s
			break;
		}
		case DaphnisI_LoRaWAN_SM_Receive_Data:
		{
			WE_DEBUG_PRINT("%i bytes received on on port %i: %s\r\n", rxData.dataLength, rxData.port, rxData.data);
			currentState = DaphnisI_LoRaWAN_SM_Send_Data;
			break;
		}
		case DaphnisI_LoRaWAN_SM_Error:
		{
			WE_DEBUG_PRINT("Error occurred\r\n");
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
static void DaphnisI_OTAA_EventCallback(DaphnisI_ATEvent_t event, char *eventText)
{
	switch (event)
	{
	case DaphnisI_ATEvent_StartUp:
	{
		currentState = DaphnisI_LoRaWAN_SM_Set_Up_Keys;
		break;
	}
	case DaphnisI_ATEvent_LoRaWAN_JoinState_Success:
	{
		currentState = DaphnisI_LoRaWAN_SM_Send_Data;
		break;
	}
	case DaphnisI_ATEvent_LoRaWAN_JoinState_Fail:
	{
		currentState = DaphnisI_LoRaWAN_SM_Error;
		break;
	}
	case DaphnisI_ATEvent_LoRaWAN_RxData:
	{
		currentState = DaphnisI_LoRaWAN_ParseRxDataEvent(&eventText, &rxData) ? DaphnisI_LoRaWAN_SM_Receive_Data : DaphnisI_LoRaWAN_SM_Error;
		break;
	}
	default:
		break;
	}
}
