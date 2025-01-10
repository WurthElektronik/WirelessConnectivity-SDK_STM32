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
#include <DaphnisI/ATCommands/P2P.h>
#include <DaphnisI/ATCommands/ATUserSettings.h>
#include <stdio.h>
#include <DaphnisI/DaphnisI_Examples.h>
#include <DaphnisI/DaphnisI_P2P_Throughput_Test.h>
#include <string.h>
#include <stdlib.h>

#if DAPHNISI_MIN_FW_VER >= FW(1,4,0)

#define RF_PROFILE				7
#define PACKET_TX_COUNT			100

static bool DaphnisI_P2P_RF_Profile_Check();
static bool DaphnisI_P2P_DC_Enforce_Check();

void DaphnisI_P2P_Throughput_Test()
{
	//Make sure to build the project using the "Release" build configuration

	DaphnisI_uart.baudrate = 115200;

	if (!DaphnisI_Init(&DaphnisI_uart, &DaphnisI_pins, NULL))
	{
		WE_DEBUG_PRINT("Initialization error\r\n");
		return;
	}

	if(!DaphnisI_P2P_RF_Profile_Check())
	{
		if(!DaphnisI_SetP2PRFProfileUS(RF_PROFILE))
		{
			WE_DEBUG_PRINT("Failed to set rf profile user setting.\r\n");
			return;
		}
	}

	if(!DaphnisI_P2P_DC_Enforce_Check())
	{
		if(!DaphnisI_SetP2PDutyCycleEnforceUS(false))
		{
			WE_DEBUG_PRINT("Failed to set dc enforce user setting.\r\n");
			return;
		}
	}

	if(!DaphnisI_PinReset())
	{
		WE_DEBUG_PRINT("Failed to reset module.\r\n");
		return;
	}

	uint8_t payload[DAPHNISI_P2P_MAX_PAYLOAD_SIZE];

	uint32_t total_time = 0;

	for(uint8_t i=0;i<PACKET_TX_COUNT;i++)
	{
		uint32_t current_time = WE_GetTick();
		if(!DaphnisI_P2P_TransmitBroadcast(payload, DAPHNISI_P2P_MAX_PAYLOAD_SIZE))
		{
			WE_DEBUG_PRINT("Failed to send payload.\r\n");
			return;
		}
		total_time += (WE_GetTick() - current_time);
	}

	WE_DEBUG_PRINT("Throughput of average %d packets in profile %d is %lf kb/s\r\n", PACKET_TX_COUNT, RF_PROFILE,
			(PACKET_TX_COUNT*DAPHNISI_P2P_MAX_PAYLOAD_SIZE*8.0)/total_time);
}

static bool DaphnisI_P2P_RF_Profile_Check()
{
	uint8_t rf_profile;

	if(!DaphnisI_GetP2PRFProfileRS(&rf_profile))
	{
		WE_DEBUG_PRINT("Failed to get rf profile runtime setting.\r\n");
		exit(-1);
	}

	return (rf_profile == RF_PROFILE);
}

static bool DaphnisI_P2P_DC_Enforce_Check()
{
	bool dc_enforce;

	if(!DaphnisI_GetP2PDutyCycleEnforceRS(&dc_enforce))
	{
		WE_DEBUG_PRINT("Failed to get dc enforce runtime setting.\r\n");
		exit(-1);
	}

	return (dc_enforce == false);
}

#endif
