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

/**
 * @file
 * @brief Calypso WLAN example.
 */
#include <Calypso/Examples/Calypso_WLAN_Example.h>
#include <stdio.h>
#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/ATCommands/ATWLAN.h>
#include <Calypso/Examples/Calypso_Examples.h>

/**
 * @brief WLAN example.
 */
void Calypso_WLAN_Example(void)
{
	printf("*** Start of Calypso ATWLAN example ***\r\n");

	if (!Calypso_Init(&Calypso_uart, &Calypso_pins, &Calypso_Examples_EventCallback))
	{
		printf("Initialization error\r\n");
		return;
	}

	Calypso_PinReset();

	Calypso_Examples_WaitForStartup(5000);

	bool ret = false;

	/* Get version info. This retrieves Calypso's firmware version (amongst other version info) and
	 * stores the firmware version in Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
	 * Calypso_firmwareVersionPatch for later use. */
	Calypso_ATDevice_Value_t deviceValue;
	ret = Calypso_ATDevice_Get(Calypso_ATDevice_GetId_General, Calypso_ATDevice_GetGeneral_Version, &deviceValue);
	Calypso_Examples_Print("Get device version", ret);

	/* WLAN station mode */
	ret = Calypso_ATWLAN_SetMode(Calypso_ATWLAN_SetMode_Station);
	Calypso_Examples_Print("Set WLAN station mode", ret);
	ret = Calypso_ATDevice_Restart(0);
	Calypso_Examples_Print("Restart network processor", ret);

	WE_Delay(1000);

	Calypso_ATWLAN_ScanEntry_t scanEntries[15];
	uint8_t numEntries;

	/* Scan for WLAN networks - the first scan always returns an error - the
	 * second scan should return the discovered networks */
	Calypso_ATWLAN_Scan(0, 15, scanEntries, &numEntries);

	WE_Delay(200);

	ret = Calypso_ATWLAN_Scan(0, 15, scanEntries, &numEntries);
	Calypso_Examples_Print("Scan WLAN networks", ret);

	WE_Delay(200);

	/* Connect to WLAN */
	Calypso_ATWLAN_ConnectionArguments_t connectArgs;
	memset(&connectArgs, 0, sizeof(connectArgs));
	strcpy(connectArgs.SSID, Calypso_Examples_wlanSSID);
	connectArgs.securityParams.securityType = Calypso_ATWLAN_SecurityType_WPA_WPA2;
	strcpy(connectArgs.securityParams.securityKey, Calypso_Examples_wlanKey);

	ret = Calypso_ATWLAN_Connect(connectArgs);
	Calypso_Examples_Print("Connect to WLAN", ret);

	WE_Delay(2000);

	ret = Calypso_ATWLAN_Disconnect();
	Calypso_Examples_Print("Disconnect from WLAN", ret);

	WE_Delay(500);

	/* Set connection policy to AUTO */
	ret = Calypso_ATWLAN_SetConnectionPolicy(Calypso_ATWLAN_PolicyConnection_Auto);
	Calypso_Examples_Print("Set connection policy to auto", ret);

	/* Add profile for the connected WLAN, reboot, then delete the profile */
	Calypso_ATWLAN_Profile_t profile = {
			0 };
	profile.connection = connectArgs;
	profile.priority = 15;
	uint8_t profileIndex = 0;
	ret = Calypso_ATWLAN_AddProfile(profile, &profileIndex);
	Calypso_Examples_Print("Add WLAN profile", ret);

	WE_Delay(2000);

	ret = Calypso_ATWLAN_Disconnect();
	Calypso_Examples_Print("Disconnect from WLAN", ret);

	WE_Delay(2000);

	ret = Calypso_ATDevice_Reboot();
	Calypso_Examples_Print("Reboot", ret);

	Calypso_Examples_WaitForStartup(5000);

	ret = Calypso_ATWLAN_GetProfile(profileIndex, &profile);
	Calypso_Examples_Print("Get WLAN profile", ret);

	WE_Delay(500);

	ret = Calypso_ATWLAN_DeleteProfile(profileIndex);
	Calypso_Examples_Print("Delete WLAN profile", ret);

	/* Get/set country code */
	Calypso_ATWLAN_Settings_t settings;
	ret = Calypso_ATWLAN_Get(Calypso_ATWLAN_SetID_General, Calypso_ATWLAN_SetGeneral_CountryCode, &settings);
	Calypso_Examples_Print("Get country code", ret);

	strcpy(settings.general.countryCode, "EU");
	ret = Calypso_ATWLAN_Set(Calypso_ATWLAN_SetID_General, Calypso_ATWLAN_SetGeneral_CountryCode, &settings);
	Calypso_Examples_Print("Set country code", ret);

	ret = Calypso_ATWLAN_Get(Calypso_ATWLAN_SetID_General, Calypso_ATWLAN_SetGeneral_CountryCode, &settings);
	Calypso_Examples_Print("Get country code", ret);
	printf("Country code: %s\r\n", settings.general.countryCode);

	/* Get scan parameters */
	ret = Calypso_ATWLAN_Get(Calypso_ATWLAN_SetID_General, Calypso_ATWLAN_SetGeneral_ScanParams, &settings);
	Calypso_Examples_Print("Get scan parameters", ret);

	/* Get/set station TX power */
	ret = Calypso_ATWLAN_Get(Calypso_ATWLAN_SetID_General, Calypso_ATWLAN_SetGeneral_StationTxPower, &settings);
	printf("STA TX POW: %d\r\n", settings.general.staTxPower);
	Calypso_Examples_Print("Get station TX power", ret);

	settings.general.staTxPower = 2;
	ret = Calypso_ATWLAN_Set(Calypso_ATWLAN_SetID_General, Calypso_ATWLAN_SetGeneral_StationTxPower, &settings);
	Calypso_Examples_Print("Set station TX power", ret);

	ret = Calypso_ATWLAN_Get(Calypso_ATWLAN_SetID_General, Calypso_ATWLAN_SetGeneral_StationTxPower, &settings);
	printf("STA TX POW: %d\r\n", settings.general.staTxPower);
	Calypso_Examples_Print("Get station TX power", ret);

	/* Get/set access point SSID */
	ret = Calypso_ATWLAN_Get(Calypso_ATWLAN_SetID_AccessPoint, Calypso_ATWLAN_SetAP_SSID, &settings);
	printf("AP SSID: %s\r\n", settings.ap.ssidConfig.ssid);
	Calypso_Examples_Print("Get AP SSID", ret);

	strcpy(settings.ap.ssidConfig.ssid, "Calypso-42");
	settings.ap.ssidConfig.appendMac = false;
	ret = Calypso_ATWLAN_Set(Calypso_ATWLAN_SetID_AccessPoint, Calypso_ATWLAN_SetAP_SSID, &settings);
	Calypso_Examples_Print("Set AP SSID", ret);

	ret = Calypso_ATWLAN_Get(Calypso_ATWLAN_SetID_AccessPoint, Calypso_ATWLAN_SetAP_SSID, &settings);
	printf("AP SSID: %s\r\n", settings.ap.ssidConfig.ssid);
	Calypso_Examples_Print("Get AP SSID", ret);

	/* Get/set access point maximum number of stations */
	ret = Calypso_ATWLAN_Get(Calypso_ATWLAN_SetID_AccessPoint, Calypso_ATWLAN_SetAP_MaxStations, &settings);
	printf("AP MAX stations: %d\r\n", settings.ap.maxStations);
	Calypso_Examples_Print("Get AP max. number of stations", ret);

	settings.ap.maxStations = 3;
	ret = Calypso_ATWLAN_Set(Calypso_ATWLAN_SetID_AccessPoint, Calypso_ATWLAN_SetAP_MaxStations, &settings);
	Calypso_Examples_Print("Set AP max. number of stations", ret);

	ret = Calypso_ATWLAN_Get(Calypso_ATWLAN_SetID_AccessPoint, Calypso_ATWLAN_SetAP_MaxStations, &settings);
	printf("AP MAX stations: %d\r\n", settings.ap.maxStations);
	Calypso_Examples_Print("Get AP max. number of stations", ret);

	/* Get/set access point security parameters */
	ret = Calypso_ATWLAN_Get(Calypso_ATWLAN_SetID_AccessPoint, Calypso_ATWLAN_SetAP_Security, &settings);
	printf("AP Security: %d\r\n", settings.ap.security);
	Calypso_Examples_Print("Get AP security params", ret);

	settings.ap.security = Calypso_ATWLAN_SecurityType_Open;
	ret = Calypso_ATWLAN_Set(Calypso_ATWLAN_SetID_AccessPoint, Calypso_ATWLAN_SetAP_Security, &settings);
	Calypso_Examples_Print("Set AP security params", ret);

	ret = Calypso_ATWLAN_Get(Calypso_ATWLAN_SetID_AccessPoint, Calypso_ATWLAN_SetAP_Security, &settings);
	printf("AP Security: %d\r\n", settings.ap.security);
	Calypso_Examples_Print("Get AP security params", ret);

	/* Get/set power management policy */
	Calypso_ATWLAN_PolicyPM_t pmPolicy;
	uint32_t maxSleepTimeMs;
	ret = Calypso_ATWLAN_GetPMPolicy(&pmPolicy, &maxSleepTimeMs);
	Calypso_Examples_Print("Get PM policy", ret);

	ret = Calypso_ATWLAN_SetPMPolicy(Calypso_ATWLAN_PolicyPM_LongSleep, 123);
	Calypso_Examples_Print("Set PM policy long sleep", ret);

	ret = Calypso_ATWLAN_GetPMPolicy(&pmPolicy, &maxSleepTimeMs);
	Calypso_Examples_Print("Get PM policy", ret);

	ret = Calypso_ATWLAN_SetPMPolicy(Calypso_ATWLAN_PolicyPM_Normal, 0);
	Calypso_Examples_Print("Set PM policy normal", ret);

	/* Get/set scan policy */
	ret = Calypso_ATWLAN_SetScanPolicy(Calypso_ATWLAN_PolicyScan_NoHiddenSSID, 1000);
	Calypso_Examples_Print("Set scan policy NoHiddenSSID", ret);

	ret = Calypso_ATWLAN_SetScanPolicy(Calypso_ATWLAN_PolicyScan_DisableScan, 0);
	Calypso_Examples_Print("Set scan policy DisableScan", ret);

	/* Get WLAN status */
	ret = Calypso_ATDevice_Get(Calypso_ATDevice_GetId_Status, Calypso_ATDevice_GetStatus_WLAN, &deviceValue);
	Calypso_Examples_Print("Get WLAN status", ret);
	char statusStr[256];
	Calypso_ATDevice_PrintStatusFlags(deviceValue.status, statusStr, sizeof(statusStr));
	printf("WLAN status flags: %s\r\n", statusStr);

	Calypso_Deinit();
}
