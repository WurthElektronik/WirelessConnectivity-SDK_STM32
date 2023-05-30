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
#include "../ATCommands/ATSocket.h"
#include "../ATCommands/ATPacketDomain.h"
#include <AdrasteaI/Adrastea.h>
#include "../ATCommands/ATEvent.h"
#include "AdrasteaExamples.h"

void Adrastea_ATSocket_EventCallback(char *eventText);

static ATPacketDomain_Network_Registration_Status_t status = {
		.state = 0 };
static ATSocket_ID_t dataReceivedsocketID = ATSocket_ID_Invalid;
static bool dataReceived = false;

void ATSocketExample()
{

	if (!Adrastea_Init(115200, WE_FlowControl_NoFlowControl, WE_Parity_None, &Adrastea_ATSocket_EventCallback, NULL))
	{
		return;
	}

	printf("*** Start of Adrastea ATSocket example ***\r\n");

	WE_Delay(1000);

	bool ret = false;

	ret = ATPacketDomain_SetNetworkRegistrationResultCode(ATPacketDomain_Network_Registration_Result_Code_Enable_with_Location_Info);

	AdrasteaExamplesPrint("Set Network Registration Result Code", ret);

	while (status.state != ATPacketDomain_Network_Registration_State_Registered_Roaming)
	{
	}

	WE_Delay(1000);

	ret = ATSocket_SetSocketUnsolicitedNotificationEvents(ATSocket_Event_All, ATCommon_Event_State_Enable);

	AdrasteaExamplesPrint("Set Socket Unsolicited Notification Events", ret);

	ATSocket_ID_t socketID;

	ret = ATSocket_AllocateSocket(1, ATSocket_Type_TCP, ATSocket_Behaviour_Open_Connection, "52.43.121.77", 9001, 0, 0, 0, ATSocket_IP_Addr_Format_IPv4, &socketID);

	AdrasteaExamplesPrint("Allocate Socket", ret);

	if (ret)
	{
		printf("Socket ID: %d\r\n", socketID);
	}

	ret = ATSocket_ActivateSocket(1, ATCommon_Session_ID_Invalid);

	AdrasteaExamplesPrint("Activate Socket", ret);

	char Payload[256];

	ATSocket_Data_Read_t dataRead;

	dataRead.data = Payload;

	while (dataReceived == false)
	{
	}

	ret = ATSocket_ReceiveFromSocket(dataReceivedsocketID, &dataRead, sizeof(Payload));

	AdrasteaExamplesPrint("Receive From Socket", ret);

	if (ret)
	{
		printf("Socket ID: %d, Data length: %d, Payload: %s\r\n", dataRead.socketID, dataRead.dataLength, dataRead.data);
	}

}

void Adrastea_ATSocket_EventCallback(char *eventText)
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
	case ATEvent_Socket_Data_Received:
	{
		ATSocket_ParseDataReceivedEvent(eventText, &dataReceivedsocketID);
		dataReceived = true;
		break;
	}
	case ATEvent_Socket_Sockets_Read:
	{
		ATSocket_Read_Result_t readResult;
		ATSocket_ParseSocketsReadEvent(eventText, &readResult);
		break;
	}
	default:
		break;
	}
}
