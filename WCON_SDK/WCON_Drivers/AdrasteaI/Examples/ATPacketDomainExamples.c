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
#include "ATProprietaryExamples.h"
#include "../ATCommands/ATPacketDomain.h"
#include "../ATCommands/ATProprietary.h"
#include <AdrasteaI/Adrastea.h>
#include "../ATCommands/ATEvent.h"
#include "AdrasteaExamples.h"

void Adrastea_ATPacketDomain_EventCallback(char *eventText);

static ATPacketDomain_Network_Registration_Status_t status = {
		.state = 0 };

void ATPacketDomainExample()
{

	if (!Adrastea_Init(115200, WE_FlowControl_NoFlowControl, WE_Parity_None, &Adrastea_ATPacketDomain_EventCallback, NULL))
	{
		return;
	}

	printf("*** Start of Adrastea ATPacketDomain example ***\r\n");

	WE_Delay(1000);

	bool ret = false;

	ret = ATPacketDomain_SetNetworkRegistrationResultCode(ATPacketDomain_Network_Registration_Result_Code_Enable_with_Location_Info);

	AdrasteaExamplesPrint("Set Network Registration Result Code", ret);

	while (status.state != ATPacketDomain_Network_Registration_State_Registered_Roaming)
	{
	}

	WE_Delay(1000);

	ATPacketDomain_Network_Registration_Status_t statusRead;

	ret = ATPacketDomain_ReadNetworkRegistrationStatus(&statusRead);

	AdrasteaExamplesPrint("Read Network Registration Status", ret);

	if (ret)
	{
		printf("Result Code: %d, State: %d, TAC: %s, ECI: %s, AcT: %d\r\n", statusRead.resultCode, statusRead.state, statusRead.TAC, statusRead.ECI, statusRead.AcT);
	}

	ATPacketDomain_PDP_Context_t context = {
			.cid = 2,
			.pdpType = ATPacketDomain_PDP_Type_IPv4,
			.apnName = "web.vodafone.de" };

	ret = ATPacketDomain_DefinePDPContext(context);

	AdrasteaExamplesPrint("Define PDP Context", ret);

	ret = ATPacketDomain_ReadPDPContexts();

	AdrasteaExamplesPrint("Read PDP Contexts", ret);

	//TODO check this command

	ATPacketDomain_PDP_Context_CID_State_t pdpContextState = {
			.cid = 2,
			.state = ATPacketDomain_PDP_Context_State_Activated };

	ret = ATPacketDomain_SetPDPContextState(pdpContextState);

	AdrasteaExamplesPrint("Set PDP Context State", ret);

	ret = ATPacketDomain_ReadPDPContextsState();

	AdrasteaExamplesPrint("Read PDP Contexts State", ret);

}

void Adrastea_ATPacketDomain_EventCallback(char *eventText)
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
	case ATEvent_PacketDomain_PDP_Context:
	{
		ATPacketDomain_PDP_Context_t pdpContext;
		if (!ATPacketDomain_ParsePDPContextEvent(eventText, &pdpContext))
		{
			return;
		}
		printf("CID: %d, PDP Type: %d, APN Name: %s\r\n", pdpContext.cid, pdpContext.pdpType, pdpContext.apnName);
		break;
	}
	case ATEvent_PacketDomain_PDP_Context_State:
	{
		ATPacketDomain_PDP_Context_CID_State_t pdpContextState;
		if (!ATPacketDomain_ParsePDPContextStateEvent(eventText, &pdpContextState))
		{
			return;
		}
		printf("CID: %d, State: %d\r\n", pdpContextState.cid, pdpContextState.state);
		break;
	}
	default:
		break;
	}
}
