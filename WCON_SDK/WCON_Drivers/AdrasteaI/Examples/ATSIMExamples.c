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

#include "stdio.h"
#include "ATSIMExamples.h"
#include "../ATCommands/ATSIM.h"
#include "../ATCommands/ATPacketDomain.h"
#include <AdrasteaI/Adrastea.h>
#include "../ATCommands/ATEvent.h"
#include "../ATCommands/ATProprietary.h"
#include "AdrasteaExamples.h"

void Adrastea_ATSIM_EventCallback(char *eventText);

static ATPacketDomain_Network_Registration_Status_t status = {
		.state = 0 };

void ATSIMExample()
{

	if (!Adrastea_Init(115200, WE_FlowControl_NoFlowControl, WE_Parity_None, &Adrastea_ATSIM_EventCallback, NULL))
	{
		return;
	}

	printf("*** Start of Adrastea ATSIM example ***\r\n");

	WE_Delay(1000);

	bool ret = false;

	ret = ATPacketDomain_SetNetworkRegistrationResultCode(ATPacketDomain_Network_Registration_Result_Code_Enable_with_Location_Info);

	AdrasteaExamplesPrint("Set Network Registration Result Code", ret);

	while (status.state != ATPacketDomain_Network_Registration_State_Registered_Roaming)
	{
	}

	WE_Delay(1000);

	ATSIM_IMSI_t imsi;

	ret = ATSIM_RequestInternationalMobileSubscriberIdentity(&imsi);

	AdrasteaExamplesPrint("Request International Mobile Subscriber Identity", ret);

	if (ret)
	{
		printf("IMSI: %s\r\n", imsi);
	}

	ATSIM_ICCID_t iccid;

	ret = ATSIM_RequestIntegratedCircuitCardIdentifier(&iccid);

	AdrasteaExamplesPrint("Request Integrated Circuit Card Identifier", ret);

	if (ret)
	{
		printf("ICCID: %s\r\n", iccid);
	}

	ATSIM_PIN_Status_t pinStatus;

	ret = ATSIM_ReadPinStatus(&pinStatus);

	AdrasteaExamplesPrint("Read Pin Status", ret);

	if (ret)
	{
		printf("Pin Status: %d\r\n", pinStatus);
	}

	ret = ATSIM_SetFacilityLock(ATSIM_Facility_SC, ATSIM_Lock_Mode_Lock, "2912");

	AdrasteaExamplesPrint("Set Facility Lock", ret);

	ATSIM_Lock_Status_t lockStatus;

	ret = ATSIM_ReadFacilityLock(ATSIM_Facility_SC, &lockStatus);

	AdrasteaExamplesPrint("Read Facility Lock", ret);

	if (ret)
	{
		printf("Lock Status: %d\r\n", lockStatus);
	}

	ret = ATSIM_SetFacilityLock(ATSIM_Facility_SC, ATSIM_Lock_Mode_Unlock, "2912");

	AdrasteaExamplesPrint("Set Facility Unlock", ret);

	lockStatus = -1;

	ret = ATSIM_ReadFacilityLock(ATSIM_Facility_SC, &lockStatus);

	AdrasteaExamplesPrint("Read Facility Lock", ret);

	if (ret)
	{
		printf("Lock Status: %d\r\n", lockStatus);
	}

	ATSIM_Restricted_Access_Response_t ras;

	char repsonse[256];

	ras.responseRead = repsonse;

	ras.responseReadMaxBufferSize = 256;

	ATSIM_RestrictedSIMAccess(ATSIM_Restricted_Access_Command_Status, 0, 0, 0, 0, NULL, &ras);

	AdrasteaExamplesPrint("Restricted SIM Access", ret);

	if (ret)
	{
		printf("SW1 : %d, SW2 : %d, Response : %s\r\n", ras.sw1, ras.sw2, ras.responseRead);
	}
}

void Adrastea_ATSIM_EventCallback(char *eventText)
{
	ATEvent_t event;
	ATEvent_ParseEventType(&eventText, &event);

	switch (event)
	{
	case ATEvent_PacketDomain_Network_Registration_Status:
	{
		ATPacketDomain_ParseNetworkRegistrationStatusEvent(eventText, &status);
		break;
	}
	default:
		break;
	}
}
