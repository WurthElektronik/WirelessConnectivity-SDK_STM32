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
#include <AdrasteaI/ATHTTPExamples.h>
#include <AdrasteaI/ATCommands/ATHTTP.h>
#include <AdrasteaI/ATCommands/ATPacketDomain.h>
#include <AdrasteaI/AdrasteaI.h>
#include <AdrasteaI/ATCommands/ATEvent.h>
#include <AdrasteaI/AdrasteaI_Examples.h>

void AdrasteaI_ATHTTP_EventCallback(char *eventText);

static AdrasteaI_ATPacketDomain_Network_Registration_Status_t status = {
		.state = 0 };
static AdrasteaI_ATHTTP_Event_Result_t requestState = {
		.state = -1 };

/**
 * @brief This example connects to the cellular network and access the content of a web site
 *
 */
void ATHTTPExample()
{
	WE_DEBUG_PRINT("*** Start of Adrastea-I ATHTTP example ***\r\n");

	if (!AdrasteaI_Init(&AdrasteaI_uart, &AdrasteaI_pins, &AdrasteaI_ATHTTP_EventCallback))
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

	ret = AdrasteaI_ATHTTP_SetHTTPUnsolicitedNotificationEvents(AdrasteaI_ATHTTP_Event_All, 1);
	AdrasteaI_ExamplesPrint("Set HTTP Unsolicited Notification Events", ret);

	ret = AdrasteaI_ATHTTP_ConfigureNodes(1, "http://captive.apple.com/", "", "");
	AdrasteaI_ExamplesPrint("Configure Nodes", ret);

	ret = AdrasteaI_ATHTTP_ConfigureFormat(1, AdrasteaI_ATHTTP_Header_Presence_Disable, AdrasteaI_ATHTTP_Header_Presence_Disable);
	AdrasteaI_ExamplesPrint("Configure Format", ret);

	ret = AdrasteaI_ATHTTP_ConfigureTimeout(1, 10000);
	AdrasteaI_ExamplesPrint("Configure Timeout", ret);

	ret = AdrasteaI_ATHTTP_GET(1, "http://captive.apple.com/", AdrasteaI_ATHTTP_Header_Presence_Disable, NULL, 0);
	AdrasteaI_ExamplesPrint("Get", ret);

	while (requestState.state != AdrasteaI_ATHTTP_Event_State_Success)
	{
		WE_Delay(10);
	}

	AdrasteaI_ATHTTP_Response_t response;
	char responseBody[128];
	response.responseBody = responseBody;
	ret = AdrasteaI_ATHTTP_ReadResponse(1, sizeof(responseBody), &response);
	AdrasteaI_ExamplesPrint("Read Response", ret);
	if (ret)
	{
		WE_DEBUG_PRINT("Data Length: %d, Received Length: %d, Payload: %s\r\n", response.dataLength, response.receivedLength, response.responseBody);
	}
}

void AdrasteaI_ATHTTP_EventCallback(char *eventText)
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
	case AdrasteaI_ATEvent_HTTP_GET_Receive:
	{
		AdrasteaI_ATHTTP_ParseGETEvent(eventText, &requestState);
		break;
	}
	default:
		break;
	}
}
