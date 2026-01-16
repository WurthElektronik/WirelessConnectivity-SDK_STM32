/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK:
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

/**
 * @file ATWifi.c
 * @brief AT commands for Wifi functionality.
 */

#include <StephanoI/ATCommands/ATWifi.h>
#include <StephanoI/StephanoI.h>
#include <global/ATCommands.h>

bool StephanoI_ATWifi_Init(bool enable)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWINIT=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, enable ? 1 : 0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_GetInit(bool* enableP)
{
    char responsebuffer[5];
    if (!StephanoI_SendRequest("AT+CWINIT?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseInit(responsebuffer, enableP);
}

bool StephanoI_ATWifi_SetMode(StephanoI_ATWifiMode_t mode, bool autoconnect)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWMODE=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)mode, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, autoconnect ? (uint32_t)1 : (uint32_t)0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_GetMode(StephanoI_ATWifiMode_t* modeP)
{
    char responsebuffer[2];
    if (!StephanoI_SendRequest("AT+CWMODE?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseMode(responsebuffer, modeP);
}

bool StephanoI_ATWifi_GetState(StephanoI_ATWiFi_State_t* stateP)
{
    char responsebuffer[sizeof(StephanoI_ATWiFi_State_t) + 1];
    if (!StephanoI_SendRequest("AT+CWSTATE?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseState(responsebuffer, stateP);
}

bool StephanoI_ATWifi_SetCountryCode(StephanoI_ATWifiCountryCode_t t)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWCOUNTRY=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)t.policy, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, t.country_code, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)t.start_channel, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)t.total_channel_count, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_GetCountryCode(StephanoI_ATWifiCountryCode_t* t)
{
    char responsebuffer[sizeof(StephanoI_ATWifiCountryCode_t) + 1];
    if (!StephanoI_SendRequest("AT+CWCOUNTRY?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseCountryCode(responsebuffer, t);
}

bool StephanoI_ATWifi_SetHostname(StephanoI_ATWiFi_Hostname_t name)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWHOSTNAME=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, name, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_GetHostname(StephanoI_ATWiFi_Hostname_t* nameP)
{
    char responsebuffer[sizeof(StephanoI_ATWiFi_Hostname_t) + 1];
    if (!StephanoI_SendRequest("AT+CWHOSTNAME?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseHostname(responsebuffer, nameP);
}

bool StephanoI_ATWifi_Station_ScanAPOptions(uint16_t print_mask, int8_t rssi, uint16_t authmode_mask)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWLAPOPT=1,");
    if (!ATCommand_AppendArgumentInt(pRequestCommand, print_mask, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, rssi, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, authmode_mask, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_WifiScan), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_Station_ScanAPs(char* ssid)
{
    char* pRequestCommand = AT_commandBuffer;

    if (ssid == NULL)
    {
        strcpy(pRequestCommand, "AT+CWLAP\r\n");
    }
    else
    {
        strcpy(pRequestCommand, "AT+CWLAP=");
        if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, ssid, ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_WifiScan), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_Station_Connect(char* ssid, char* password)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWJAP=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, ssid, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, password, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_WifiConnect), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_Station_ReConnectConfiguration(uint16_t interval_s, uint16_t repeat_count)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWRECONNCFG=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, interval_s, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, repeat_count, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_Station_ConnectWPA2Enterprise(char* ssid, StephanoI_ATWifiWPA2Method_t method, char* identity, char* username, char* password, StephanoI_ATWPA2SecurityFlags_t security)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWJEAP=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, ssid, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)method, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, identity, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, username, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, password, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)security, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_WifiConnect), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_Station_Disconnect()
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWQAP\r\n");

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_Station_AutoConnect(bool autoconnect)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWAUTOCONN=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, autoconnect ? (uint32_t)1 : (uint32_t)0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_Station_GetIP()
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPSTA?\r\n");

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_Station_SetMAC(StephanoI_ATWiFi_MAC_t mac)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPSTAMAC=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, mac, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_Station_GetMAC(StephanoI_ATWiFi_MAC_t* t)
{
    char responsebuffer[sizeof(StephanoI_ATWiFi_MAC_t) + 1];
    if (!StephanoI_SendRequest("AT+CIPSTAMAC?\r\n"))
    {
        return false;
    }

    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseGetMAC(responsebuffer, t);
}

bool StephanoI_ATWifi_Station_SetProtocol(StephanoI_ATWifiProtocolFlags_t prot)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWSTAPROTO=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)prot, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_Station_GetProtocol(StephanoI_ATWifiProtocolFlags_t* protP)
{
    char responsebuffer[sizeof(StephanoI_ATWifiProtocolFlags_t) + 1];
    if (!StephanoI_SendRequest("AT+CWSTAPROTO?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseProtocol(responsebuffer, protP);
}

bool StephanoI_ATWifi_AP_SetMAC(StephanoI_ATWiFi_MAC_t mac)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPAPMAC=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, mac, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_AP_GetMAC(StephanoI_ATWiFi_MAC_t* t)
{
    char responsebuffer[sizeof(StephanoI_ATWiFi_MAC_t) + 1];
    if (!StephanoI_SendRequest("AT+CIPAPMAC?\r\n"))
    {
        return false;
    }

    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseGetMAC(responsebuffer, t);
}

bool StephanoI_ATWifi_AP_SetConfiguration(StephanoI_ATWiFi_APConfiguration_t config)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWSAP=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, config.ssid, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, config.pwd, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, config.channel, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, config.ecn, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, config.max_connections, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, config.ssid_hidden ? 1 : 0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_FactoryReset), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_AP_GetConfiguration(StephanoI_ATWiFi_APConfiguration_t* configP)
{
    char responsebuffer[sizeof(StephanoI_ATWiFi_APConfiguration_t) + 1];
    if (!StephanoI_SendRequest("AT+CWSAP?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseAPConfiguration(responsebuffer, configP);
}

bool StephanoI_ATWifi_AP_GetConnectedStations()
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWLIF\r\n");

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_AP_DisconnectStation(StephanoI_ATWiFi_MAC_t mac)
{
    char* pRequestCommand = AT_commandBuffer;

    if (mac == NULL)
    {
        strcpy(pRequestCommand, "AT+CWQIF\r\n");
    }
    else
    {
        strcpy(pRequestCommand, "AT+CWQIF=");

        if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, mac, ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_AP_SetProtocol(StephanoI_ATWifiProtocolFlags_t prot)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWAPPROTO=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)prot, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_AP_GetProtocol(StephanoI_ATWifiProtocolFlags_t* protP)
{
    char responsebuffer[sizeof(StephanoI_ATWifiProtocolFlags_t) + 1];
    if (!StephanoI_SendRequest("AT+CWAPPROTO?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseProtocol(responsebuffer, protP);
}

bool StephanoI_ATWifi_EnableDHCP(bool enable, StephanoI_ATWifiDHCPFlags_t mode)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWDHCP=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, enable ? (uint32_t)1 : (uint32_t)0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)mode, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_SocketPing), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_GetDHCPState(uint8_t* stateP)
{
    char responsebuffer[5];
    if (!StephanoI_SendRequest("AT+CWDHCP?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseGetDHCPState(responsebuffer, stateP);
}

bool StephanoI_ATWifi_AP_SetIPrangeConfiguration(bool enable, StephanoI_ATWiFi_IPRange_t range)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CWDHCPS=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, enable ? (uint32_t)1 : (uint32_t)0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, range.lease_time, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, range.start_ip, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, range.end_ip, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_AP_GetIPrangeConfiguration(StephanoI_ATWiFi_IPRange_t* rangeP)
{
    char responsebuffer[sizeof(StephanoI_ATWiFi_IPRange_t) + 5];
    if (!StephanoI_SendRequest("AT+CWDHCPS?\r\n"))
    {
        return false;
    }
    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATWiFi_ParseGetIPrangeConfiguration(responsebuffer, rangeP);
}

bool StephanoI_ATWifi_AP_GetIP()
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPAP?\r\n");

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_AP_SetIP(char* ip, char* gateway, char* netmask)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+CIPAP=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, ip, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, gateway, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, netmask, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_General), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWifi_EnableMDNS(bool enable, char* host_name, char* service_name, uint16_t port)
{
    char* pRequestCommand = AT_commandBuffer;

    if (enable)
    {
        strcpy(pRequestCommand, "AT+MDNS=");

        if (!ATCommand_AppendArgumentInt(pRequestCommand, enable ? (uint32_t)1 : (uint32_t)0, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, host_name, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, service_name, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentInt(pRequestCommand, port, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }
    else
    {
        strcpy(pRequestCommand, "AT+MDNS=0\r\n");
    }
    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_SocketPing), StephanoI_CNFStatus_Success);
}

bool StephanoI_ATWiFi_ParseMode(char* EventArgumentsP, StephanoI_ATWifiMode_t* t)
{
    char* argumentsP = EventArgumentsP;

    return ATCommand_GetNextArgumentInt(&argumentsP, t, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
}

bool StephanoI_ATWiFi_ParseScan(char* EventArgumentsP, StephanoI_ATWiFi_Scan_t* t)
{
    /*ignore first '(' */
    char* argumentsP = EventArgumentsP + 1;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->enc), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(&argumentsP, t->ssid, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->ssid)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->rssi), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&argumentsP, t->MAC, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->MAC)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->channel), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->freq_offset), ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->freqcal_val), ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->pairwise_cipher), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->group_cipher), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->bgn), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->wps_flags), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ')'))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATWiFi_ParseState(char* EventArgumentsP, StephanoI_ATWiFi_State_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->state), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(&argumentsP, t->ssid, ATCOMMAND_STRING_TERMINATE, sizeof(t->ssid)))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATWiFi_ParseGetConnectedIPs(char* EventArgumentsP, StephanoI_ATWiFi_GetConnectedIPs_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentString(&argumentsP, t->IP, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->IP)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&argumentsP, t->MAC, ATCOMMAND_STRING_TERMINATE, sizeof(t->MAC)))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATWiFi_ParseGetIP(char* EventArgumentsP, StephanoI_ATWiFi_GetIP_t* t)
{
    char* argumentsP = EventArgumentsP;
    char type[16];
    if (!ATCommand_GetNextArgumentString(&argumentsP, type, ':', sizeof(type)))
    {
        return false;
    }
    if (0 == strcmp(type, "ip"))
    {
        if (!ATCommand_GetNextArgumentString(&argumentsP, t->IP, ATCOMMAND_STRING_TERMINATE, sizeof(t->IP)))
        {
            return false;
        }
    }
    else if (0 == strcmp(type, "gateway"))
    {
        if (!ATCommand_GetNextArgumentString(&argumentsP, t->gateway, ATCOMMAND_STRING_TERMINATE, sizeof(t->gateway)))
        {
            return false;
        }
    }
    else if (0 == strcmp(type, "netmask"))
    {
        if (!ATCommand_GetNextArgumentString(&argumentsP, t->netmask, ATCOMMAND_STRING_TERMINATE, sizeof(t->netmask)))
        {
            return false;
        }
    }
    else if (0 == strcmp(type, "ip6ll"))
    {
        if (!ATCommand_GetNextArgumentString(&argumentsP, t->ip6ll, ATCOMMAND_STRING_TERMINATE, sizeof(t->ip6ll)))
        {
            return false;
        }
    }
    else if (0 == strcmp(type, "ip6gl"))
    {
        if (!ATCommand_GetNextArgumentString(&argumentsP, t->ip6gl, ATCOMMAND_STRING_TERMINATE, sizeof(t->ip6gl)))
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool StephanoI_ATWiFi_ParseGetMAC(char* EventArgumentsP, StephanoI_ATWiFi_MAC_t* t)
{
    char* argumentsP = EventArgumentsP;

    return ATCommand_GetNextArgumentString(&argumentsP, *t, ATCOMMAND_STRING_TERMINATE, sizeof(StephanoI_ATWiFi_MAC_t));
}

bool StephanoI_ATWiFi_ParseStationhasConnected(char* EventArgumentsP, StephanoI_ATWiFi_MAC_t* t) { return StephanoI_ATWiFi_ParseGetMAC(EventArgumentsP, t); }

bool StephanoI_ATWiFi_ParseAPAssignIP(char* EventArgumentsP, StephanoI_ATWiFi_APAssignIP_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentString(&argumentsP, t->MAC, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->MAC)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(&argumentsP, t->ip, ATCOMMAND_STRING_TERMINATE, sizeof(t->MAC)))
    {
        return false;
    }

    return true;
}

bool StephanoI_ATWiFi_ParseCountryCode(char* EventArgumentsP, StephanoI_ATWifiCountryCode_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->policy), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(&argumentsP, t->country_code, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->country_code)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->start_channel), ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->total_channel_count), ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    return true;
}

bool StephanoI_ATWiFi_ParseHostname(char* EventArgumentsP, StephanoI_ATWiFi_Hostname_t* t)
{
    char* argumentsP = EventArgumentsP;
    return ATCommand_GetNextArgumentString(&argumentsP, *t, ATCOMMAND_STRING_TERMINATE, sizeof(StephanoI_ATWiFi_Hostname_t));
}

bool StephanoI_ATWiFi_ParseProtocol(char* EventArgumentsP, StephanoI_ATWifiProtocolFlags_t* t)
{
    char* argumentsP = EventArgumentsP;
    return ATCommand_GetNextArgumentInt(&argumentsP, t, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
}

bool StephanoI_ATWiFi_ParseInit(char* EventArgumentsP, bool* t)
{
    char* argumentsP = EventArgumentsP;
    uint8_t val;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &val, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    *t = (val == (uint8_t)0x01);
    return true;
}

bool StephanoI_ATWiFi_ParseAPConfiguration(char* EventArgumentsP, StephanoI_ATWiFi_APConfiguration_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, t->ssid, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->ssid)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(&argumentsP, t->pwd, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->pwd)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->channel), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->ecn), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->max_connections), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    uint8_t dummy;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &dummy, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    t->ssid_hidden = (dummy == 0x01);
    return true;
}

bool StephanoI_ATWiFi_ParseGetDHCPState(char* EventArgumentsP, uint8_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, t, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    return true;
}

bool StephanoI_ATWiFi_ParseGetIPrangeConfiguration(char* EventArgumentsP, StephanoI_ATWiFi_IPRange_t* t)
{
    char* argumentsP = EventArgumentsP;

    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->lease_time), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(&argumentsP, t->start_ip, ATCOMMAND_ARGUMENT_DELIM, sizeof(t->start_ip)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(&argumentsP, t->end_ip, ATCOMMAND_STRING_TERMINATE, sizeof(t->end_ip)))
    {
        return false;
    }
    return true;
}
