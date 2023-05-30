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
#include "ATHTTPExamples.h"
#include "../ATCommands/ATHTTP.h"
#include "../ATCommands/ATPacketDomain.h"
#include <AdrasteaI/Adrastea.h>
#include "../ATCommands/ATEvent.h"
#include "AdrasteaExamples.h"

void Adrastea_ATHTTP_EventCallback(char *eventText);

static ATPacketDomain_Network_Registration_Status_t status = {
		.state = 0 };
static ATHTTP_Event_Result_t requestState = {
		.state = -1 };
void ATHTTPExample()
{

	if (!Adrastea_Init(115200, WE_FlowControl_NoFlowControl, WE_Parity_None, &Adrastea_ATHTTP_EventCallback, NULL))
	{
		return;
	}

	printf("*** Start of Adrastea ATHTTP example ***\r\n");

	WE_Delay(1000);

	bool ret = false;

	ret = ATPacketDomain_SetNetworkRegistrationResultCode(ATPacketDomain_Network_Registration_Result_Code_Enable_with_Location_Info);

	AdrasteaExamplesPrint("Set Network Registration Result Code", ret);

	while (status.state != ATPacketDomain_Network_Registration_State_Registered_Roaming)
	{
	}

	WE_Delay(1000);

	ret = ATHTTP_SetHTTPUnsolicitedNotificationEvents(ATHTTP_Event_All, 1);

	AdrasteaExamplesPrint("Set HTTP Unsolicited Notification Events", ret);

	ret = ATHTTP_ConfigureNodes(1, "http://captive.apple.com/", NULL, NULL);

	AdrasteaExamplesPrint("Configure Nodes", ret);

	ret = ATHTTP_ConfigureFormat(1, ATHTTP_Header_Presence_Disable, ATHTTP_Header_Presence_Disable);

	AdrasteaExamplesPrint("Configure Format", ret);

	ret = ATHTTP_ConfigureTimeout(1, 10000);

	AdrasteaExamplesPrint("Configure Timeout", ret);

	WE_Delay(2000);

	ret = ATHTTP_GET(1, "http://captive.apple.com/", ATHTTP_Header_Presence_Disable, NULL, 0);

	AdrasteaExamplesPrint("Get", ret);

	while (requestState.state != ATHTTP_Event_State_Success)
	{
	}

	WE_Delay(1000);

	ATHTTP_Response_t response;

	char responseBody[128];

	response.responseBody = responseBody;

	ret = ATHTTP_ReadResponse(1, ATHTTP_Data_Length_Max, &response);

	AdrasteaExamplesPrint("Read Response", ret);

	if (ret)
	{
		printf("Data Length: %d, Received Length: %d, Payload: %s\r\n", response.dataLength, response.receivedLength, response.responseBody);
	}
}

void Adrastea_ATHTTP_EventCallback(char *eventText)
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
	case ATEvent_HTTP_GET_Receive:
	{
		ATHTTP_ParseGETEvent(eventText, &requestState);
		break;
	}
	default:
		break;
	}
}
