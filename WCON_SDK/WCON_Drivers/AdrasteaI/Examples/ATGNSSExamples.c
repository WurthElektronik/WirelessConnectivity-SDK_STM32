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
#include "../ATCommands/ATGNSS.h"
#include "../ATCommands/ATDevice.h"
#include "../ATCommands/ATPacketDomain.h"
#include <AdrasteaI/Adrastea.h>
#include "../ATCommands/ATEvent.h"
#include "AdrasteaExamples.h"

void Adrastea_ATGNSS_EventCallback(char *eventText);

static ATPacketDomain_Network_Registration_Status_t status = {
		.state = 0 };

static ATGNSS_Satellite_Count_t satelliteQueryCount = 0, satelliteQueryEventCount = 0;

void ATGNSSExample()
{

	if (!Adrastea_Init(115200, WE_FlowControl_NoFlowControl, WE_Parity_None, &Adrastea_ATGNSS_EventCallback, NULL))
	{
		return;
	}

	printf("*** Start of Adrastea ATGNSS example ***\r\n");

	WE_Delay(1000);

	bool ret = false;

	ret = ATPacketDomain_SetNetworkRegistrationResultCode(ATPacketDomain_Network_Registration_Result_Code_Enable_with_Location_Info);

	AdrasteaExamplesPrint("Set Network Registration Result Code", ret);

	while (status.state != ATPacketDomain_Network_Registration_State_Registered_Roaming)
	{
	}

	WE_Delay(1000);

	ret = ATGNSS_DownloadCEPFile(ATGNSS_CEP_Number_of_Days_1Day);

	AdrasteaExamplesPrint("Download CEP File", ret);

	ATGNSS_CEP_Status_t cepStatus;

	ret = ATGNSS_QueryCEPFileStatus(&cepStatus);

	AdrasteaExamplesPrint("Query CEP Status", ret);

	if (ret)
	{
		printf("Validity: %d, Remaining Days: %d, Hours: %d, Minutes:%d\r\n", cepStatus.validity, cepStatus.remDays, cepStatus.remHours, cepStatus.remMinutes);
	}

	WE_Delay(1000);

	ret = ATDevice_SetPhoneFunctionality(ATDevice_Phone_Functionality_Min, ATDevice_Phone_Functionality_Reset_Do_Not_Reset);

	AdrasteaExamplesPrint("Set Phone Functionality", ret);

	ret = ATGNSS_StartGNSS(ATGNSS_Start_Mode_Cold);

	AdrasteaExamplesPrint("Start GNSS", ret);

	ATGNSS_Satellite_Systems_t satSystems = {
			.systems = {
					.GPS = ATGNSS_Runtime_Mode_State_Set,
					.GLONASS = ATGNSS_Runtime_Mode_State_Set } };

	ret = ATGNSS_SetSatelliteSystems(satSystems);

	AdrasteaExamplesPrint("Set Satellite Systems", ret);

	ATGNSS_Fix_t fix;

	while (1)
	{
		WE_Delay(5000);
		satelliteQueryCount = 0;
		satelliteQueryEventCount = 0;
		ret = ATGNSS_QueryGNSSSatellites(&satelliteQueryCount);

		AdrasteaExamplesPrint("Query GNSS Satellites", ret);

		printf("Satellites Count: %d\r\n", satelliteQueryCount);

		while (satelliteQueryCount != satelliteQueryEventCount)
		{
		}

		ret = ATGNSS_QueryGNSSFix(ATGNSS_Fix_Relavancy_Current, &fix);

		AdrasteaExamplesPrint("Query GNSS Fix", ret);

		if (ret && fix.fixType != ATGNSS_Fix_Type_No_Fix)
		{
			printf("Fix Latitude: %f, Longitude: %f, Altitude: %f\r\n", fix.latitude, fix.longitude, fix.altitude);
		}
	}
}

void Adrastea_ATGNSS_EventCallback(char *eventText)
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
	case ATEvent_GNSS_Satalite_Query:
	{
		ATGNSS_Satellite_t satellite;
		if (!ATGNSS_ParseSatelliteQueryEvent(eventText, &satellite))
		{
			return;
		}
		printf("PRN: %d, Elevation: %d, Azimuth: %d, SNR: %d\r\n", satellite.prn, satellite.elevation, satellite.azimuth, satellite.snr);
		satelliteQueryEventCount++;
		break;
	}
	default:
		break;
	}
}
