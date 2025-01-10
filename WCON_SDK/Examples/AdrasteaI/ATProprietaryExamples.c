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
#include <AdrasteaI/ATProprietaryExamples.h>
#include <AdrasteaI/ATCommands/ATPacketDomain.h>
#include <AdrasteaI/AdrasteaI.h>
#include <AdrasteaI/ATCommands/ATEvent.h>
#include <AdrasteaI/ATCommands/ATProprietary.h>
#include <AdrasteaI/AdrasteaI_Examples.h>

void AdrasteaI_ATProprietary_EventCallback(char *eventText);

static AdrasteaI_ATPacketDomain_Network_Registration_Status_t status = {
		.state = 0 };

void ATProprietaryExample()
{
	WE_DEBUG_PRINT("*** Start of Adrastea-I ATProprietary example ***\r\n");

	if (!AdrasteaI_Init(&AdrasteaI_uart, &AdrasteaI_pins, &AdrasteaI_ATProprietary_EventCallback))
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

	ret = AdrasteaI_ATProprietary_Ping(AdrasteaI_ATProprietary_IP_Addr_Format_IPv4, "8.8.8.8", AdrasteaI_ATProprietary_Ping_Packet_Count_Invalid, AdrasteaI_ATProprietary_Ping_Packet_Size_Invalid, AdrasteaI_ATProprietary_Ping_Timeout_Invalid);
	AdrasteaI_ExamplesPrint("Ping", ret);

	ret = AdrasteaI_ATProprietary_ResolveDomainName(1, "www.google.com", AdrasteaI_ATProprietary_IP_Addr_Format_IPv4);
	AdrasteaI_ExamplesPrint("Resolve Domain Name", ret);

	AdrasteaI_ATProprietary_Network_Attachment_State_t networkAttachmentState;
	ret = AdrasteaI_ATProprietary_ReadNetworkAttachmentState(&networkAttachmentState);
	AdrasteaI_ExamplesPrint("Read Network Attachment State", ret);
	if (ret)
	{
		WE_DEBUG_PRINT("Attachment State: %d\r\n", networkAttachmentState);
	}

	AdrasteaI_ATProprietary_PIN_PUK_Attempts_t attempts;
	ret = AdrasteaI_ATProprietary_ReadRemainingPINPUKAttempts(&attempts);
	AdrasteaI_ExamplesPrint("Read Remaining PIN PUK Attempts", ret);
	if (ret)
	{
		WE_DEBUG_PRINT("Remaining Attempts PIN: %d, PUK: %d, PIN2: %d, PUK2: %d\r\n", attempts.pinAttempts, attempts.pukAttempts, attempts.pin2Attempts, attempts.puk2Attempts);
	}

	AdrasteaI_ATProprietary_RAT_Status_t ratStatus;
	ret = AdrasteaI_ATProprietary_ReadRATStatus(&ratStatus);
	AdrasteaI_ExamplesPrint("Read RAT Status", ret);
	if (ret)
	{
		WE_DEBUG_PRINT("RAT Type: %d, RAT Mode: %d, RAT Source: %d\r\n", ratStatus.rat, ratStatus.mode, ratStatus.source);
	}

	memset(&status, -1, sizeof(AdrasteaI_ATPacketDomain_Network_Registration_Status_t));
	ret = AdrasteaI_ATProprietary_SwitchToRATWithoutFullReboot(AdrasteaI_ATProprietary_RAT_NB_IOT, AdrasteaI_ATProprietary_RAT_Storage_Non_Persistant, AdrasteaI_ATProprietary_RAT_Source_Invalid);
	AdrasteaI_ExamplesPrint("Switch To RAT Without Full Reboot", ret);
	while (status.state != AdrasteaI_ATPacketDomain_Network_Registration_State_Registered_Roaming)
	{
		WE_Delay(10);
	}

	ret = AdrasteaI_ATProprietary_ReadRATStatus(&ratStatus);
	AdrasteaI_ExamplesPrint("Read RAT Status", ret);
	if (ret)
	{
		WE_DEBUG_PRINT("RAT Type: %d, RAT Mode: %d, RAT Source: %d\r\n", ratStatus.rat, ratStatus.mode, ratStatus.source);
	}

	AdrasteaI_ATProprietary_PDN_Parameters_t pdnParamters;
	ret = AdrasteaI_ATProprietary_ReadPDNParameters(&pdnParamters);
	AdrasteaI_ExamplesPrint("Read PDN Parameters", ret);
	if (ret)
	{
		WE_DEBUG_PRINT("Session ID: %d, APN Name: %s, IP Format: %d\r\n", pdnParamters.sessionID, pdnParamters.apnName, pdnParamters.ipFormat);
	}

	AdrasteaI_ATProprietary_File_Names_List_t filesnamesList;
	ret = AdrasteaI_ATProprietary_ListCredentials(&filesnamesList);
	AdrasteaI_ExamplesPrint("List Credentials", ret);
	if (ret)
	{
		for (uint8_t i = 0; i < filesnamesList.count; i++)
		{
			WE_DEBUG_PRINT("Filename: %s\r\n", filesnamesList.filenames[i]);

			ret = AdrasteaI_ATProprietary_DeleteCredential(filesnamesList.filenames[i]);
			AdrasteaI_ExamplesPrint("Delete Credential", ret);
		}
		free(filesnamesList.filenames);
	}

	ret =
			AdrasteaI_ATProprietary_WriteCredential("DigiCertRootCA.crt", AdrasteaI_ATProprietary_Credential_Format_Certificate, "-----BEGIN CERTIFICATE-----\
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\
-----END CERTIFICATE-----");
	AdrasteaI_ExamplesPrint("Write Credential", ret);

	AdrasteaI_ATProprietary_TLS_Profile_ID_List_t tlsProfilesIDList;
	ret = AdrasteaI_ATProprietary_ListTLSProfiles(&tlsProfilesIDList);
	AdrasteaI_ExamplesPrint("List TLS Profiles IDs", ret);
	if (ret)
	{
		for (uint8_t i = 0; i < tlsProfilesIDList.count; i++)
		{
			WE_DEBUG_PRINT("SSL/TLS Profile ID: %d\r\n", tlsProfilesIDList.profileIDs[i]);

			ret = AdrasteaI_ATProprietary_DeleteTLSProfile(tlsProfilesIDList.profileIDs[i]);

			AdrasteaI_ExamplesPrint("Delete TLS Profile", ret);
		}
		free(tlsProfilesIDList.profileIDs);
	}

	ret = AdrasteaI_ATProprietary_AddTLSProfile(3, "DigiCertRootCA.crt", ".", "", "", "", "");
	AdrasteaI_ExamplesPrint("Add TLS Profile", ret);
}

void AdrasteaI_ATProprietary_EventCallback(char *eventText)
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
	case AdrasteaI_ATEvent_Proprietary_Ping_Result:
	{
		AdrasteaI_ATProprietary_Ping_Result_t pingResult;
		if (!AdrasteaI_ATProprietary_ParsePingResultEvent(eventText, &pingResult))
		{
			return;
		}
		WE_DEBUG_PRINT("Ping ID: %d, Address: %s, TTL: %d, RTT: %d\r\n", pingResult.id, pingResult.addr, pingResult.ttl, pingResult.rtt);
		break;
	}
	case AdrasteaI_ATEvent_Proprietary_Domain_Name_Resolve:
	{
		AdrasteaI_ATProprietary_Domain_Name_Resolve_Result_t dnsResult;
		if (!AdrasteaI_ATProprietary_ParseResolveDomainNameEvent(eventText, &dnsResult))
		{
			return;
		}
		WE_DEBUG_PRINT("Address Format: %d, IP Address: %s\r\n", dnsResult.format, dnsResult.addr);
		break;
	}
	default:
		break;
	}
}
