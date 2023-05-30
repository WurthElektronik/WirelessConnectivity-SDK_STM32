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
#include "ATMQTTExamples.h"
#include "../ATCommands/ATMQTT.h"
#include "../ATCommands/ATPacketDomain.h"
#include <AdrasteaI/Adrastea.h>
#include "../ATCommands/ATEvent.h"
#include "AdrasteaExamples.h"

void Adrastea_ATMQTT_EventCallback(char *eventText);

static ATPacketDomain_Network_Registration_Status_t status = {
		.state = 0 };
static ATMQTT_Connection_Result_t conResult = {
		.resultCode = -1 };
static ATMQTT_Subscription_Result_t subResult = {
		.resultCode = -1 };

void ATMQTTExample()
{

	if (!Adrastea_Init(115200, WE_FlowControl_NoFlowControl, WE_Parity_None, &Adrastea_ATMQTT_EventCallback, NULL))
	{
		return;
	}

	printf("*** Start of Adrastea ATMQTT example ***\r\n");

	WE_Delay(1000);

	bool ret = false;

	ret = ATPacketDomain_SetNetworkRegistrationResultCode(ATPacketDomain_Network_Registration_Result_Code_Enable_with_Location_Info);

	AdrasteaExamplesPrint("Set Network Registration Result Code", ret);

	while (status.state != ATPacketDomain_Network_Registration_State_Registered_Roaming)
	{
	}

	WE_Delay(1000);

	ret = ATMQTT_SetMQTTUnsolicitedNotificationEvents(ATMQTT_Event_All, 1);

	AdrasteaExamplesPrint("MQTT Unsolicited Notification Events", ret);

	ret = ATMQTT_ConfigureNodes(1, "adrastea_module_23", "test.mosquitto.org", NULL, NULL);

	AdrasteaExamplesPrint("Configure Nodes", ret);

	ret = ATMQTT_ConfigureProtocol(1, 1200, 1);

	AdrasteaExamplesPrint("Configure Protocol", ret);

	ret = ATMQTT_Connect(1);

	AdrasteaExamplesPrint("Connect", ret);

	while (conResult.resultCode != ATMQTT_Event_Result_Code_Success)
	{
	}

	ret = ATMQTT_Publish(1, 0, 0, "adrtopic", "241", 3);

	AdrasteaExamplesPrint("Publish", ret);

	ret = ATMQTT_Subscribe(1, ATMQTT_QoS_Exactly_Once, "adrtopic");

	AdrasteaExamplesPrint("Subscribe", ret);

	while (subResult.resultCode != ATMQTT_Event_Result_Code_Success)
	{
	}
}

void Adrastea_ATMQTT_EventCallback(char *eventText)
{
	ATEvent_t event;
	ATEvent_ParseEventType(&eventText, &event);

	switch (event)
	{
	case ATEvent_MQTT_Connection_Confirmation:
	{
		ATMQTT_ParseConnectionConfirmationEvent(eventText, &conResult);
		break;
	}
	case ATEvent_MQTT_Subscription_Confirmation:
	{
		ATMQTT_ParseSubscriptionConfirmationEvent(eventText, &subResult);
		break;
	}
	case ATEvent_MQTT_Publication_Received:
	{
		ATMQTT_Publication_Received_Result_t result;
		char payload[128];
		result.payload = payload;
		result.payloadMaxBufferSize = 128;
		if (!ATMQTT_ParsePublicationReceivedEvent(eventText, &result))
		{
			return;
		}
		printf("Connection ID: %d, Message ID: %d, Topic Name: %s, Payload Size: %d, Payload: %s\r\n", result.connID, result.msgID, result.topicName, result.payloadSize, result.payload);
		break;
	}
	case ATEvent_PacketDomain_Network_Registration_Status:
	{
		ATPacketDomain_ParseNetworkRegistrationStatusEvent(eventText, &status);
		break;
	}
	default:
		break;
	}
}
