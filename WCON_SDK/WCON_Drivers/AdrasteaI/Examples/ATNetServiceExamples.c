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
#include "ATNetServiceExamples.h"
#include "../ATCommands/ATNetService.h"
#include "../ATCommands/ATPacketDomain.h"
#include "../ATCommands/ATEvent.h"
#include "../Adrastea.h"
#include "AdrasteaExamples.h"

void Adrastea_ATNetService_EventCallback(char *eventText);

static ATPacketDomain_Network_Registration_Status_t status = {
		0 };

void ATNetServiceExample()
{

	if (!Adrastea_Init(115200, WE_FlowControl_NoFlowControl, WE_Parity_None, &Adrastea_ATNetService_EventCallback, NULL))
	{
		return;
	}

	printf("*** Start of Adrastea ATNetService example ***\r\n");

	WE_Delay(1000);

	bool ret = false;

	ret = ATPacketDomain_SetNetworkRegistrationResultCode(ATPacketDomain_Network_Registration_Result_Code_Enable_with_Location_Info);

	AdrasteaExamplesPrint("Set Network Registration Result Code", ret);

	while (status.state != ATPacketDomain_Network_Registration_State_Registered_Roaming)
	{
	}

	WE_Delay(1000);

	ret = ATNetService_ReadOperators();

	AdrasteaExamplesPrint("Read Operators", ret);

	ret = ATNetService_SetPLMNReadFormat(ATNetService_PLMN_Format_Numeric);

	AdrasteaExamplesPrint("Set PLMN Read Format", ret);

	ATNetService_PLMN_t plmn;

	ret = ATNetService_ReadPLMN(&plmn);

	AdrasteaExamplesPrint("Read PLMN", ret);

	if (ret)
	{
		printf("Selection Mode: %d, Format: %d, Operator Numeric: %lu\r\n", plmn.selectionMode, plmn.format, (unsigned long) plmn.operator.operatorNumeric);
	}

	ATNetService_Signal_Quality_t sq;

	ATNetService_ReadSignalQuality(&sq);

	AdrasteaExamplesPrint("Read Signal Quality", ret);

	if (ret)
	{
		printf("RSSI: %d, BER: %d\r\n", sq.rssi, sq.ber);
	}

	ATNetService_Extended_Signal_Quality_t esq;

	ret = ATNetService_ReadExtendedSignalQuality(&esq);

	AdrasteaExamplesPrint("Read Extended Signal Quality", ret);

	if (ret)
	{
		printf("RXLEV: %d, BER: %d, RSCP: %d, ECNO: %d, RSRQ: %d, RSRP: %d\r\n", esq.rxlev, esq.ber, esq.rscp, esq.ecno, esq.rsrq, esq.rsrp);
	}

	ATNetService_Power_Saving_Mode_t psm = {
			.state = ATNetService_Power_Saving_Mode_State_Enable,
			.activeTime = {
					.activeTimeValues = {
							.value = 20,
							.unit = ATNetService_Power_Saving_Mode_Active_Time_Unit_1m } },
			.periodicTAU = {
					.periodicTAUValues = {
							.value = 0,
							.unit = ATNetService_Power_Saving_Mode_Periodic_TAU_Deactivated } } };

	ret = ATNetService_SetPowerSavingMode(psm);

	AdrasteaExamplesPrint("Set Power Saving Mode", ret);

	psm.state = ATNetService_Power_Saving_Mode_State_Disable;

	ret = ATNetService_SetPowerSavingMode(psm);

	AdrasteaExamplesPrint("Set Power Saving Mode", ret);

}

void Adrastea_ATNetService_EventCallback(char *eventText)
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
	case ATEvent_NetService_Operator_Read:
	{
		ATNetService_Operator_t operator;
		if (!ATNetService_ParseOperatorReadEvent(eventText, &operator))
		{
			return;
		}
		break;
	}
	default:
		break;
	}
}
