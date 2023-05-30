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
#include <AdrasteaI/Adrastea.h>
#include "../ATCommands/ATEvent.h"
#include "../ATCommands/ATProprietary.h"
#include "AdrasteaExamples.h"

void Adrastea_ATProprietary_EventCallback(char *eventText);

static ATPacketDomain_Network_Registration_Status_t status = {
		.state = 0 };

void ATProprietaryExample()
{

	if (!Adrastea_Init(115200, WE_FlowControl_NoFlowControl, WE_Parity_None, &Adrastea_ATProprietary_EventCallback, NULL))
	{
		return;
	}

	printf("*** Start of Adrastea ATProprietary example ***\r\n");

	WE_Delay(1000);

	bool ret = false;

	ret = ATPacketDomain_SetNetworkRegistrationResultCode(ATPacketDomain_Network_Registration_Result_Code_Enable_with_Location_Info);

	AdrasteaExamplesPrint("Set Network Registration Result Code", ret);

	while (status.state != ATPacketDomain_Network_Registration_State_Registered_Roaming)
	{
	}

	WE_Delay(1000);

	ret = ATProprietary_Ping(ATProprietary_IP_Addr_Format_IPv4, "8.8.8.8", ATProprietary_Ping_Packet_Count_Invalid, ATProprietary_Ping_Packet_Size_Invalid, ATProprietary_Ping_Timeout_Invalid);

	AdrasteaExamplesPrint("Ping", ret);

	ret = ATProprietary_ResolveDomainName(1, "www.google.com", ATProprietary_IP_Addr_Format_IPv4);

	AdrasteaExamplesPrint("Resolve Domain Name", ret);

	ATProprietary_Network_Attachment_State_t networkAttachmentState;

	ret = ATProprietary_ReadNetworkAttachmentState(&networkAttachmentState);

	AdrasteaExamplesPrint("Read Network Attachment State", ret);

	if (ret)
	{
		printf("Attachment State: %d\r\n", networkAttachmentState);
	}

	ATProprietary_PIN_PUK_Attempts_t attempts;

	ret = ATProprietary_ReadRemainingPINPUKAttempts(&attempts);

	AdrasteaExamplesPrint("Read Remaining PIN PUK Attempts", ret);

	if (ret)
	{
		printf("Remaining Attempts PIN: %d, PUK: %d, PIN2: %d, PUK2: %d\r\n", attempts.pinAttempts, attempts.pukAttempts, attempts.pin2Attempts, attempts.puk2Attempts);
	}

	ATProprietary_RAT_Status_t ratStatus;

	ret = ATProprietary_ReadRATStatus(&ratStatus);

	AdrasteaExamplesPrint("Read RAT Status", ret);

	if (ret)
	{
		printf("RAT Type: %d, RAT Mode: %d, RAT Source: %d\r\n", ratStatus.rat, ratStatus.mode, ratStatus.source);
	}

	memset(&status, -1, sizeof(ATPacketDomain_Network_Registration_Status_t));

	ret = ATProprietary_SwitchToRATWithoutFullReboot(ATProprietary_RAT_NB_IOT, ATProprietary_RAT_Storage_Non_Persistant, ATProprietary_RAT_Source_Invalid);

	AdrasteaExamplesPrint("Switch To RAT Without Full Reboot", ret);

	while (status.state != ATPacketDomain_Network_Registration_State_Registered_Roaming)
	{
	}

	WE_Delay(1000);

	ret = ATProprietary_ReadRATStatus(&ratStatus);

	AdrasteaExamplesPrint("Read RAT Status", ret);

	if (ret)
	{
		printf("RAT Type: %d, RAT Mode: %d, RAT Source: %d\r\n", ratStatus.rat, ratStatus.mode, ratStatus.source);
	}

	ATProprietary_PDN_Parameters_t pdnParamters;

	ret = ATProprietary_ReadPDNParameters(&pdnParamters);

	AdrasteaExamplesPrint("Read PDN Parameters", ret);

	if (ret)
	{
		printf("Session ID: %d, APN Name: %s, IP Format: %d\r\n", pdnParamters.sessionID, pdnParamters.apnName, pdnParamters.ipFormat);
	}

	ATProprietary_File_Names_List_t filesnamesList;

	ret = ATProprietary_ListCredentials(&filesnamesList);

	AdrasteaExamplesPrint("List Credentials", ret);

	if (ret)
	{
		for (uint8_t i = 0; i < filesnamesList.count; i++)
		{
			printf("Filename: %s\r\n", filesnamesList.filenames[i]);

			ret = ATProprietary_DeleteCredential(filesnamesList.filenames[i]);

			AdrasteaExamplesPrint("Delete Credential", ret);
		}
		free(filesnamesList.filenames);
	}

	ret =
			ATProprietary_WriteCredential("DigiCertRootCA.crt", ATProprietary_Credential_Format_Certificate, "-----BEGIN CERTIFICATE-----\
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

	AdrasteaExamplesPrint("Write Credential", ret);

	ATProprietary_TLS_Profile_ID_List_t tlsProfilesIDList;

	ret = ATProprietary_ListTLSProfiles(&tlsProfilesIDList);

	AdrasteaExamplesPrint("List TLS Profiles IDs", ret);

	if (ret)
	{
		for (uint8_t i = 0; i < tlsProfilesIDList.count; i++)
		{
			printf("SSL/TLS Profile ID: %d\r\n", tlsProfilesIDList.profileIDs[i]);

			ret = ATProprietary_DeleteTLSProfile(tlsProfilesIDList.profileIDs[i]);

			AdrasteaExamplesPrint("Delete TLS Profile", ret);
		}
		free(tlsProfilesIDList.profileIDs);
	}

	ret = ATProprietary_AddTLSProfile(3, "DigiCertRootCA.crt", ".", NULL, NULL, NULL, NULL);

	AdrasteaExamplesPrint("Add TLS Profile", ret);
}

void Adrastea_ATProprietary_EventCallback(char *eventText)
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
	case ATEvent_Proprietary_Ping_Result:
	{
		ATProprietary_Ping_Result_t pingResult;
		if (!ATProprietary_ParsePingResultEvent(eventText, &pingResult))
		{
			return;
		}
		printf("Ping ID: %d, Address: %s, TTL: %d, RTT: %d\r\n", pingResult.id, pingResult.addr, pingResult.ttl, pingResult.rtt);
		break;
	}
	case ATEvent_Proprietary_Domain_Name_Resolve:
	{
		ATProprietary_Domain_Name_Resolve_Result_t dnsResult;
		if (!ATProprietary_ParseResolveDomainNameEvent(eventText, &dnsResult))
		{
			return;
		}
		printf("Address Format: %d, IP Address: %s\r\n", dnsResult.format, dnsResult.addr);
		break;
	}
	default:
		break;
	}
}
