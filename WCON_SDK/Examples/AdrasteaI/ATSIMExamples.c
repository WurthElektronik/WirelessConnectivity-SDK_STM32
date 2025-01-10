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
#include <AdrasteaI/ATSIMExamples.h>
#include <AdrasteaI/ATCommands/ATSIM.h>
#include <AdrasteaI/ATCommands/ATPacketDomain.h>
#include <AdrasteaI/AdrasteaI.h>
#include <AdrasteaI/ATCommands/ATEvent.h>
#include <AdrasteaI/ATCommands/ATProprietary.h>
#include <AdrasteaI/AdrasteaI_Examples.h>

void AdrasteaI_ATSIM_EventCallback(char *eventText);

static AdrasteaI_ATPacketDomain_Network_Registration_Status_t status = {
		.state = 0 };

void ATSIMExample()
{
	WE_DEBUG_PRINT("*** Start of Adrastea-I ATSIM example ***\r\n");

	if (!AdrasteaI_Init(&AdrasteaI_uart, &AdrasteaI_pins, &AdrasteaI_ATSIM_EventCallback))
	{
		WE_DEBUG_PRINT("Initialization error\r\n");
		return;
	}

	bool ret = AdrasteaI_ATPacketDomain_SetNetworkRegistrationResultCode(AdrasteaI_ATPacketDomain_Network_Registration_Result_Code_Enable_with_Location_Info);
	AdrasteaI_ExamplesPrint("Set Network Registration Result Code", ret);
	while (status.state != AdrasteaI_ATPacketDomain_Network_Registration_State_Registered_Roaming)
	{
		WE_Delay(10);
	}

	AdrasteaI_ATSIM_IMSI_t imsi;
	ret = AdrasteaI_ATSIM_RequestInternationalMobileSubscriberIdentity(&imsi);
	AdrasteaI_ExamplesPrint("Request International Mobile Subscriber Identity", ret);
	if (ret)
	{
		WE_DEBUG_PRINT("IMSI: %s\r\n", imsi);
	}

	AdrasteaI_ATSIM_ICCID_t iccid;
	ret = AdrasteaI_ATSIM_RequestIntegratedCircuitCardIdentifier(&iccid);
	AdrasteaI_ExamplesPrint("Request Integrated Circuit Card Identifier", ret);
	if (ret)
	{
		WE_DEBUG_PRINT("ICCID: %s\r\n", iccid);
	}

	AdrasteaI_ATSIM_PIN_Status_t pinStatus;
	ret = AdrasteaI_ATSIM_ReadPinStatus(&pinStatus);
	AdrasteaI_ExamplesPrint("Read Pin Status", ret);
	if (ret)
	{
		WE_DEBUG_PRINT("Pin Status: %d\r\n", pinStatus);
	}

	ret = AdrasteaI_ATSIM_SetFacilityLock(AdrasteaI_ATSIM_Facility_SC, AdrasteaI_ATSIM_Lock_Mode_Lock, "2912");
	AdrasteaI_ExamplesPrint("Set Facility Lock", ret);

	AdrasteaI_ATSIM_Lock_Status_t lockStatus;
	ret = AdrasteaI_ATSIM_ReadFacilityLock(AdrasteaI_ATSIM_Facility_SC, &lockStatus);
	AdrasteaI_ExamplesPrint("Read Facility Lock", ret);
	if (ret)
	{
		WE_DEBUG_PRINT("Lock Status: %d\r\n", lockStatus);
	}

	ret = AdrasteaI_ATSIM_SetFacilityLock(AdrasteaI_ATSIM_Facility_SC, AdrasteaI_ATSIM_Lock_Mode_Unlock, "2912");
	AdrasteaI_ExamplesPrint("Set Facility Unlock", ret);

	lockStatus = -1;
	ret = AdrasteaI_ATSIM_ReadFacilityLock(AdrasteaI_ATSIM_Facility_SC, &lockStatus);
	AdrasteaI_ExamplesPrint("Read Facility Lock", ret);
	if (ret)
	{
		WE_DEBUG_PRINT("Lock Status: %d\r\n", lockStatus);
	}

	AdrasteaI_ATSIM_Restricted_Access_Response_t ras;
	char response[256];
	ras.responseRead = response;
	ras.responseReadMaxBufferSize = sizeof(response);
	AdrasteaI_ATSIM_RestrictedSIMAccess(AdrasteaI_ATSIM_Restricted_Access_Command_Status, 0, 0, 0, 0, NULL, &ras);
	AdrasteaI_ExamplesPrint("Restricted SIM Access", ret);
	if (ret)
	{
		WE_DEBUG_PRINT("SW1 : %d, SW2 : %d, Response : %s\r\n", ras.sw1, ras.sw2, ras.responseRead);
	}
}

void AdrasteaI_ATSIM_EventCallback(char *eventText)
{
	AdrasteaI_ATEvent_t event;
	if (false == AdrasteaI_ATEvent_ParseEventType(&eventText, &event))
	{
		return;
	}

	switch (event)
	{
	case AdrasteaI_ATEvent_PacketDomain_Network_Registration_Status:
	{
		AdrasteaI_ATPacketDomain_ParseNetworkRegistrationStatusEvent(eventText, &status);
		break;
	}
	default:
		break;
	}
}
