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
#include <stdio.h>
#include <AdrasteaI/Examples/ATNetServiceExamples.h>
#include <AdrasteaI/ATCommands/ATNetService.h>
#include <AdrasteaI/ATCommands/ATPacketDomain.h>
#include <AdrasteaI/ATCommands/ATEvent.h>
#include <AdrasteaI/AdrasteaI.h>
#include <AdrasteaI/Examples/AdrasteaI_Examples.h>

void AdrasteaI_ATNetService_EventCallback(char *eventText);

static AdrasteaI_ATPacketDomain_Network_Registration_Status_t status = {
		0 };

/**
 * @brief This example connects to the cellular network and checks the quality of the connection
 *
 */
void ATNetServiceExample()
{
	printf("*** Start of Adrastea-I ATNetService example ***\r\n");

	if (!AdrasteaI_Init(&AdrasteaI_uart, &AdrasteaI_pins, &AdrasteaI_ATNetService_EventCallback))
	{
		printf("Initialization error\r\n");
		return;
	}

	bool ret = AdrasteaI_ATPacketDomain_SetNetworkRegistrationResultCode(AdrasteaI_ATPacketDomain_Network_Registration_Result_Code_Enable_with_Location_Info);
	AdrasteaI_ExamplesPrint("Set Network Registration Result Code", ret);
	while (status.state != AdrasteaI_ATPacketDomain_Network_Registration_State_Registered_Roaming)
	{
		WE_Delay(10);
	}

	ret = AdrasteaI_ATNetService_ReadOperators();
	AdrasteaI_ExamplesPrint("Read Operators", ret);

	ret = AdrasteaI_ATNetService_SetPLMNReadFormat(AdrasteaI_ATNetService_PLMN_Format_Numeric);
	AdrasteaI_ExamplesPrint("Set PLMN Read Format", ret);

	AdrasteaI_ATNetService_PLMN_t plmn;
	ret = AdrasteaI_ATNetService_ReadPLMN(&plmn);
	AdrasteaI_ExamplesPrint("Read PLMN", ret);
	if (ret)
	{
		printf("Selection Mode: %d, Format: %d, Operator Numeric: %lu\r\n", plmn.selectionMode, plmn.format, (unsigned long) plmn.operator.operatorNumeric);
	}

	AdrasteaI_ATNetService_Signal_Quality_t sq;
	AdrasteaI_ATNetService_ReadSignalQuality(&sq);
	AdrasteaI_ExamplesPrint("Read Signal Quality", ret);
	if (ret)
	{
		printf("RSSI: %d, BER: %d\r\n", sq.rssi, sq.ber);
	}

	AdrasteaI_ATNetService_Extended_Signal_Quality_t esq;
	ret = AdrasteaI_ATNetService_ReadExtendedSignalQuality(&esq);
	AdrasteaI_ExamplesPrint("Read Extended Signal Quality", ret);
	if (ret)
	{
		printf("RXLEV: %d, BER: %d, RSCP: %d, ECNO: %d, RSRQ: %d, RSRP: %d\r\n", esq.rxlev, esq.ber, esq.rscp, esq.ecno, esq.rsrq, esq.rsrp);
	}

	AdrasteaI_ATNetService_Power_Saving_Mode_t psm = {
			.state = AdrasteaI_ATNetService_Power_Saving_Mode_State_Enable,
			.activeTime = {
					.activeTimeValues = {
							.value = 20,
							.unit = AdrasteaI_ATNetService_Power_Saving_Mode_Active_Time_Unit_1m } },
			.periodicTAU = {
					.periodicTAUValues = {
							.value = 0,
							.unit = AdrasteaI_ATNetService_Power_Saving_Mode_Periodic_TAU_Deactivated } } };
	ret = AdrasteaI_ATNetService_SetPowerSavingMode(psm);
	AdrasteaI_ExamplesPrint("Set Power Saving Mode", ret);
	psm.state = AdrasteaI_ATNetService_Power_Saving_Mode_State_Disable;

	ret = AdrasteaI_ATNetService_SetPowerSavingMode(psm);
	AdrasteaI_ExamplesPrint("Set Power Saving Mode", ret);
}

void AdrasteaI_ATNetService_EventCallback(char *eventText)
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
	case AdrasteaI_ATEvent_NetService_Operator_Read:
	{
		AdrasteaI_ATNetService_Operator_t operator;
		if (!AdrasteaI_ATNetService_ParseOperatorReadEvent(eventText, &operator))
		{
			return;
		}
		break;
	}
	default:
		break;
	}
}
