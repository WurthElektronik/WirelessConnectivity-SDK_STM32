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

/**
 * @file
 * @brief Calypso network applications example.
 */
#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/ATCommands/ATNetApp.h>
#include <Calypso/ATCommands/ATNetCfg.h>
#include <Calypso/ATCommands/ATWLAN.h>
#include <Calypso/Calypso_Examples.h>
#include <Calypso/Calypso_NetApp_Example.h>
#include <stdio.h>

static void Calypso_NetApp_Example_EventCallback(char* eventText);

/* Used to keep track of number of expected ping responses remaining */
static uint8_t nrOfPingResponsesRemaining = 0;

void Calypso_NetApp_Example(void)
{
    WE_APP_PRINT("*** Start of Calypso ATNetApp example ***\r\n");

    bool ret = false;

    if (!Calypso_Init(&Calypso_uart, &Calypso_pins, &Calypso_NetApp_Example_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    Calypso_PinReset();

    Calypso_Examples_WaitForStartup(5000);

    WE_Delay(1000);

    /* Get version info. This retrieves Calypso's firmware version (amongst other version info) and
     * stores the firmware version in Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
     * Calypso_firmwareVersionPatch for later use. */
    Calypso_ATDevice_Value_t deviceValue;
    ret = Calypso_ATDevice_Get(Calypso_ATDevice_GetId_General, Calypso_ATDevice_GetGeneral_Version, &deviceValue);
    Calypso_Examples_Print("Get device version", ret);

    Calypso_ATWLAN_Disconnect();

    /* Set IPv4 address method DHCP */
    Calypso_ATNetCfg_IPv4Config_t newIpV4Config = {0};
    newIpV4Config.method = Calypso_ATNetCfg_IPv4Method_Dhcp;
    ret = Calypso_ATNetCfg_SetIPv4AddressStation(&newIpV4Config);
    Calypso_Examples_Print("Set IPv4 DHCP", ret);

    /* No automatic connection */
    ret = Calypso_ATWLAN_SetConnectionPolicy(Calypso_ATWLAN_PolicyConnection_None);
    Calypso_Examples_Print("Set no WLAN connection policy", ret);

    /* WLAN station mode */
    ret = Calypso_ATWLAN_SetMode(Calypso_ATWLAN_SetMode_Station);
    Calypso_Examples_Print("Set WLAN station mode", ret);

    ret = Calypso_ATDevice_Restart(0);
    Calypso_Examples_Print("Restart network processor", ret);

    WE_Delay(1000);

    /* Connect to WLAN */
    Calypso_ATWLAN_ConnectionArguments_t connectArgs;
    memset(&connectArgs, 0, sizeof(connectArgs));
    strcpy(connectArgs.SSID, Calypso_Examples_wlanSSID);
    connectArgs.securityParams.securityType = Calypso_ATWLAN_SecurityType_WPA_WPA2;
    strcpy(connectArgs.securityParams.securityKey, Calypso_Examples_wlanKey);

    ret = Calypso_ATWLAN_Connect(connectArgs);
    Calypso_Examples_Print("Connect to WLAN", ret);

    WE_Delay(2000);

    /* Get IPv4 configuration (station) */
    Calypso_ATNetCfg_IPv4Config_t ipV4Config;
    ret = Calypso_ATNetCfg_GetIPv4AddressStation(&ipV4Config);
    Calypso_Examples_Print("Get IPv4 config", ret);
    if (ret)
    {
        WE_APP_PRINT("*** Station IPv4 configuration ***\r\n");
        WE_APP_PRINT("IPv4 address: %s\r\n", ipV4Config.ipAddress);
        WE_APP_PRINT("Subnet mask: %s\r\n", ipV4Config.subnetMask);
        WE_APP_PRINT("Gateway address: %s\r\n", ipV4Config.gatewayAddress);
        WE_APP_PRINT("DNS address: %s\r\n", ipV4Config.dnsAddress);
    }

    /* Host / IP lookup example */
    Calypso_ATNetApp_GetHostByNameResult_t lookupResult;
    ret = Calypso_ATNetApp_GetHostByName("www.google.com", Calypso_ATSocket_Family_INET, &lookupResult);
    Calypso_Examples_Print("Get host by name", ret);
    WE_APP_PRINT("IP lookup result: host=\"%s\", IP=\"%s\"\r\n", lookupResult.hostName, lookupResult.hostAddress);

    /* SNTP client example */
    ret = Calypso_ATNetApp_StartApplications(Calypso_ATNetApp_Application_SntpClient);
    Calypso_Examples_Print("Start SNTP client", ret);

    Calypso_ATNetApp_OptionValue_t value;
    memset(&value, 0, sizeof(value));
    value.sntp.updateInterval = 1;
    ret = Calypso_ATNetApp_Set(Calypso_ATNetApp_Application_SntpClient, Calypso_ATNetApp_SntpOption_UpdateInterval, &value);
    Calypso_Examples_Print("Set SNTP update interval", ret);

    memset(&value, 0, sizeof(value));
    value.sntp.timeZone = 60;
    ret = Calypso_ATNetApp_Set(Calypso_ATNetApp_Application_SntpClient, Calypso_ATNetApp_SntpOption_TimeZone, &value);
    Calypso_Examples_Print("Set SNTP time zone", ret);

    memset(&value, 0, sizeof(value));
    strcpy(value.sntp.servers[0], "pool.ntp.org");
    strcpy(value.sntp.servers[1], "europe.pool.ntp.org");
    strcpy(value.sntp.servers[2], "");
    ret = Calypso_ATNetApp_Set(Calypso_ATNetApp_Application_SntpClient, Calypso_ATNetApp_SntpOption_Servers, &value);
    Calypso_Examples_Print("Set SNTP servers", ret);

    WE_Delay(2000);

    ret = Calypso_ATNetApp_UpdateTime();
    Calypso_Examples_Print("Update device time", ret);

    ret = Calypso_ATDevice_Get(Calypso_ATDevice_GetId_General, Calypso_ATDevice_GetGeneral_Time, &deviceValue);
    Calypso_Examples_Print("Get device time", ret);
    if (ret)
    {
        WE_APP_PRINT("date(dd:mm:yy): %u.%u.%u time(hh:mm:ss): %u:%u:%u\r\n", deviceValue.general.time.day, deviceValue.general.time.month, deviceValue.general.time.year, deviceValue.general.time.hour, deviceValue.general.time.minute, deviceValue.general.time.second);
    }

    ret = Calypso_ATNetApp_StopApplications(Calypso_ATNetApp_Application_SntpClient);
    Calypso_Examples_Print("Disable SNTP client", ret);

    WE_Delay(500);

    /* Ping the gateway. Ping response is reported in the form of events. */
    Calypso_ATNetApp_PingParameters_t pingParams;
    pingParams.family = Calypso_ATSocket_Family_INET;
    strcpy(pingParams.destination, ipV4Config.gatewayAddress);
    pingParams.size = 8;
    pingParams.delayMs = 500;
    pingParams.timeoutMs = 1000;
    pingParams.maxNumberOfPings = 4;
    pingParams.mode = Calypso_ATNetApp_PingMode_ReportAfterEveryPing;
    nrOfPingResponsesRemaining = pingParams.maxNumberOfPings;
    ret = Calypso_ATNetApp_Ping(&pingParams);
    Calypso_Examples_Print("Ping gateway", ret);

    /* Wait for all ping responses requested above to arrive */
    while (nrOfPingResponsesRemaining > 0)
    {
    }

    Calypso_Deinit();
}

/**
 * @brief Is called when an event notification has been received.
 *
 * Note that this function is called from an interrupt - code in this function
 * should thus be kept simple.
 *
 * Note in particular that it is not possible to send AT commands to Calypso
 * from within this event handler.
 *
 * Also note that not all calls of this handler necessarily correspond to valid
 * events (i.e. events from Calypso_ATEvent_t). Some events might in fact be responses
 * to AT commands that are not included in Calypso_ATEvent_t.
 */
void Calypso_NetApp_Example_EventCallback(char* eventText)
{
    Calypso_Examples_EventCallback(eventText);

    Calypso_ATEvent_t event;
    if (false == Calypso_ATEvent_ParseEventType(&eventText, &event))
    {
        return;
    }

    switch (event)
    {
        case Calypso_ATEvent_Ping:
        {
            Calypso_ATEvent_Ping_t pingEvent;
            if (Calypso_ATEvent_ParsePingEvent(&eventText, &pingEvent))
            {
                WE_APP_PRINT("Ping event - "
                             "Packets received: %d, "
                             "packets sent: %d, "
                             "roundtrip time: %d ms\r\n",
                             pingEvent.packetsSent, pingEvent.packetsReceived, pingEvent.roundTripTimeMs);
            }
            if (nrOfPingResponsesRemaining > 0)
            {
                nrOfPingResponsesRemaining--;
            }
            break;
        }

        case Calypso_ATEvent_Startup:
        case Calypso_ATEvent_WakeUp:
        case Calypso_ATEvent_SocketTxFailed:
        case Calypso_ATEvent_SocketAsyncEvent:
        case Calypso_ATEvent_SocketTCPConnect:
        case Calypso_ATEvent_SocketTCPAccept:
        case Calypso_ATEvent_SocketRcvd:
        case Calypso_ATEvent_SocketRcvdFrom:
        case Calypso_ATEvent_WlanP2PConnect:
        case Calypso_ATEvent_WlanP2PDisconnect:
        case Calypso_ATEvent_WlanP2PClientAdded:
        case Calypso_ATEvent_WlanP2PClientRemoved:
        case Calypso_ATEvent_WlanP2PDevFound:
        case Calypso_ATEvent_WlanP2PRequest:
        case Calypso_ATEvent_WlanP2PConnectFail:
        case Calypso_ATEvent_GeneralResetRequest:
        case Calypso_ATEvent_GeneralError:
        case Calypso_ATEvent_WlanConnect:
        case Calypso_ATEvent_WlanDisconnect:
        case Calypso_ATEvent_WlanStaAdded:
        case Calypso_ATEvent_WlanStaRemoved:
        case Calypso_ATEvent_WlanProvisioningStatus:
        case Calypso_ATEvent_WlanProvisioningProfileAdded:
        case Calypso_ATEvent_NetappIP4Acquired:
        case Calypso_ATEvent_NetappIP6Acquired:
        case Calypso_ATEvent_NetappIPCollision:
        case Calypso_ATEvent_NetappDHCPv4_leased:
        case Calypso_ATEvent_NetappDHCPv4_released:
        case Calypso_ATEvent_NetappIPv4Lost:
        case Calypso_ATEvent_NetappDHCPIPv4AcquireTimeout:
        case Calypso_ATEvent_NetappIPv6Lost:
        case Calypso_ATEvent_MQTTOperation:
        case Calypso_ATEvent_MQTTRecv:
        case Calypso_ATEvent_MQTTDisconnect:
        case Calypso_ATEvent_FileListEntry:
        case Calypso_ATEvent_HTTPGet:
        case Calypso_ATEvent_CustomGPIO:
        case Calypso_ATEvent_CustomHTTPPost:
        case Calypso_ATEvent_FatalErrorDeviceAbort:
        case Calypso_ATEvent_FatalErrorDriverAbort:
        case Calypso_ATEvent_FatalErrorSyncLost:
        case Calypso_ATEvent_FatalErrorNoCmdAck:
        case Calypso_ATEvent_FatalErrorCmdTimeout:
        default:
            break;
    }
}
