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
#include <DaphnisI/ATCommands/P2P.h>
#include <DaphnisI/ATCommands/ATUserSettings.h>
#include <stdio.h>
#include <DaphnisI/DaphnisI_Examples.h>
#include <DaphnisI/DaphnisI_P2P_Example.h>

#if DAPHNISI_MIN_FW_VER >= FW(1,4,0)

static void DaphnisI_P2P_EventCallback(DaphnisI_ATEvent_t event, char *eventText);

static uint8_t rxDataBuffer[DAPHNISI_P2P_MAX_PAYLOAD_SIZE];
static DaphnisI_P2P_RxData_t rxData;

/**
 * @brief In this example the module, that is already registered at TTN network, is joining the network
 * with the provided application EUI and application key. After joining, the module transmits a test string and is able
 * to receive data from the TTN server.
 */
void DaphnisI_P2P_Send()
{
	bool ret = false;

	rxData.data = rxDataBuffer;

	if (!DaphnisI_Init(&DaphnisI_uart, &DaphnisI_pins, DaphnisI_P2P_EventCallback))
	{
		WE_DEBUG_PRINT("Initialization error\r\n");
		return;
	}

	DaphnisI_Device_Address_t dest_address = {0x05, 0x2e, 0x7d, 0xb1};

	uint8_t payload[] = {0x12,0x34};

	while(1)
	{
		if(!DaphnisI_P2P_TransmitUnicastExtended(dest_address, payload, sizeof(payload)))
		{
			WE_DEBUG_PRINT("Data transmission failed\r\n");
			return;
		}

		WE_Delay(500);
	}
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
static void DaphnisI_P2P_EventCallback(DaphnisI_ATEvent_t event, char *eventText)
{
	switch (event)
	{
	case DaphnisI_ATEvent_P2P_TxTime:
	{
		uint32_t toa;
		if(!DaphnisI_P2P_ParseTXTimeEvent(&eventText, &toa))
		{
			return;
		}
		break;
	}
	case DaphnisI_ATEvent_P2P_RxData:
	{
		if(!DaphnisI_P2P_ParseRXDataEvent(&eventText, &rxData))
		{
			return;
		}
		break;
	}
	default:
		break;
	}
}

#endif
