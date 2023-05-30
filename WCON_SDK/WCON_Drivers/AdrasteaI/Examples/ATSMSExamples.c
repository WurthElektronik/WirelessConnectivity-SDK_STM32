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
#include "ATSMSExamples.h"
#include "../ATCommands/ATSMS.h"
#include "../ATCommands/ATPacketDomain.h"
#include <AdrasteaI/Adrastea.h>
#include "../ATCommands/ATEvent.h"
#include "../ATCommands/ATProprietary.h"
#include "AdrasteaExamples.h"

void Adrastea_ATSMS_EventCallback(char *eventText);

static ATPacketDomain_Network_Registration_Status_t status = {
		.state = 0 };

void ATSMSExample()
{

	if (!Adrastea_Init(115200, WE_FlowControl_NoFlowControl, WE_Parity_None, &Adrastea_ATSMS_EventCallback, NULL))
	{
		return;
	}

	printf("*** Start of Adrastea ATSMS example ***\r\n");

	WE_Delay(1000);

	bool ret = false;

	ret = ATPacketDomain_SetNetworkRegistrationResultCode(ATPacketDomain_Network_Registration_Result_Code_Enable_with_Location_Info);

	AdrasteaExamplesPrint("Set Network Registration Result Code", ret);

	while (status.state != ATPacketDomain_Network_Registration_State_Registered_Roaming)
	{
	}

	WE_Delay(1000);

	ret = ATSMS_SetSMSUnsolicitedNotificationEvents(ATCommon_Event_State_Enable);

	AdrasteaExamplesPrint("Set SMS Unsolicited Notification Events", ret);

	ATSMS_Message_Index_t msgIdx;

	ret = ATSMS_WriteMessageToStorage("+491638975759", ATSMS_Address_Type_International_Number, "Test Message", &msgIdx);

	AdrasteaExamplesPrint("Write Message To Storage", ret);

	ret = ATSMS_ListMessages(ATSMS_Message_State_All);

	AdrasteaExamplesPrint("List Messages", ret);

	ATSMS_Message_Storage_Usage_t storageUsage;

	ret = ATSMS_ReadMessageStorageUsage(&storageUsage);

	AdrasteaExamplesPrint("Read Message Storage Usage", ret);

	if (ret)
	{
		printf("Read & Delete Storage: \r\n");
		printf("Location: %d, Messages Used Count: %d, Messages Max Count: %d\r\n", storageUsage.readDeleteStorageUsage.storageLocation, storageUsage.readDeleteStorageUsage.usedMessages, storageUsage.readDeleteStorageUsage.maxMessages);
		printf("Write & Send Storage: \r\n");
		printf("Location: %d, Messages Used Count: %d, Messages Max Count: %d\r\n", storageUsage.writeSendStorageUsage.storageLocation, storageUsage.writeSendStorageUsage.usedMessages, storageUsage.writeSendStorageUsage.maxMessages);
		printf("Receive Storage: \r\n");
		printf("Location: %d, Messages Used Count: %d, Messages Max Count: %d\r\n", storageUsage.receiveStorageUsage.storageLocation, storageUsage.receiveStorageUsage.usedMessages, storageUsage.receiveStorageUsage.maxMessages);
	}

	ATSMS_Message_Reference_t msg1ID, msg2ID;

	ret = ATSMS_SendMessage("+491638975759", ATSMS_Address_Type_International_Number, "Test Message Direct", &msg1ID);

	AdrasteaExamplesPrint("Send Message", ret);

	if (ret)
	{
		printf("Message Reference: %d\r\n", msg1ID);
	}

	ret = ATSMS_SendMessageFromStorage(msgIdx, &msg2ID);

	AdrasteaExamplesPrint("Send Message From Storage", ret);

	if (ret)
	{
		printf("Message Reference: %d\r\n", msg2ID);
	}

	ret = ATSMS_DeleteAllMessages();

	AdrasteaExamplesPrint("Delete All Messages", ret);
}

void Adrastea_ATSMS_EventCallback(char *eventText)
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
	case ATEvent_SMS_Read_Message:
	{
		ATSMS_Message_t message;
		ATSMS_ParseReadMessageEvent(eventText, &message);
		break;
	}
	case ATEvent_SMS_List_Messages:
	{
		ATSMS_Message_t message;
		if (!ATSMS_ParseListMessagesEvent(eventText, &message))
		{
			return;
		}
		printf("Message Index: %d, Message State: %d, Address: %s, Payload: %s\r\n", message.messageIndex, message.messageState, message.address, message.payload);
		break;
	}
	case ATEvent_SMS_Message_Received:
	{
		ATSMS_Message_Received_Result_t result;
		ATSMS_ParseMessageReceivedEvent(eventText, &result);
		break;
	}
	case ATEvent_SMS_Error:
	{
		ATSMS_Error_t errorCode;
		ATSMS_ParseSMSErrorEvent(eventText, &errorCode);
		break;
	}
	default:
		break;
	}
}
