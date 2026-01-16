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
 * @brief StephanoI Wifi example.
 */

#include <StephanoI/ATCommands/ATDevice.h>
#include <StephanoI/ATCommands/ATSocket.h>
#include <StephanoI/ATCommands/ATWifi.h>
#include <StephanoI/StephanoI_Examples.h>
#include <StephanoI/StephanoI_Wifi_Example.h>
#include <inttypes.h>

/**
 * @brief Contains information on last Wifi State event (if any)
 */
StephanoI_ATWiFi_State_t StephanoI_Examples_Wifi_State = {.state = StephanoI_ATWifiState_NotConnected};

StephanoI_ATWebserver_Response_t StephanoI_Examples_Webserver_State = 0;

/**
 * @brief Contains information if multiple sockets are enabled
 */
bool multiple_sockets_enabled = false;

/**
 * @brief Contains information if there is a client connection request
 */
bool client_request = false;

/**
 * @brief Is called when an event notification has been received.
 *
 * Note that this function is called from an interrupt - code in this function
 * should thus be kept simple.
 *
 * Note in particular that it is not possible to send AT commands to StephanoI
 * from within this event handler.
 *
 * Also note that not all calls of this handler necessarily correspond to valid
 * events (i.e. events from StephanoI_ATEvent_t). Some events might in fact be responses
 * to AT commands that are not included in StephanoI_ATEvent_t.
 */
void StephanoI_Wifi_Examples_EventCallback(char* eventText)
{
    char* eventText_original = eventText;
    StephanoI_ATEvent_t event;
    if (false == StephanoI_ATEvent_ParseEventType(&eventText, &event))
    {
        return;
    }

    switch (event)
    {
        case StephanoI_ATEvent_Socket_GetMultiple:
        {
            if (StephanoI_ATSocket_ParseGetMultiple(eventText, &multiple_sockets_enabled))
            {
                if (multiple_sockets_enabled)
                {
                    WE_APP_PRINT("Multiple sockets enabled\r\n");
                }
                else
                {
                    WE_APP_PRINT("Multiple sockets disabled\r\n");
                }
            }
        }
        break;
        case StephanoI_ATEvent_Socket_Receive:
        {
            StephanoI_ATSocket_Receive_t received;
            if (StephanoI_ATSocket_ParseReceive(eventText, multiple_sockets_enabled, &received))
            {
                WE_APP_PRINT("Socket received event received: 0x");
                for (uint32_t i = 0; i < received.length; i++)
                {
                    WE_APP_PRINT("%02X", received.data[i]);
                }
                WE_APP_PRINT("\r\n");
            }
        }
        break;
        case StephanoI_ATEvent_Socket_Ping:
        {
            StephanoI_ATSocket_Ping_t ping;
            if (StephanoI_ATSocket_ParsePing(eventText, &ping))
            {
                WE_APP_PRINT("Socket ping event received.\r\n"
                             "Time: %lu ms\r\n",
                             (uint32_t)ping);
            }
        }
        break;
        case StephanoI_ATEvent_Socket_DomainResolved:
        {
            StephanoI_ATSocket_DomainResolved_t domain;
            if (StephanoI_ATSocket_ParseDomainResolved(eventText, &domain))
            {
                WE_APP_PRINT("Socket domain resolved event received.\r\n"
                             "IP: %s\r\n",
                             domain);
            }
        }
        break;
        case StephanoI_ATEvent_Wifi_State:
        {
            if (StephanoI_ATWiFi_ParseState(eventText, &StephanoI_Examples_Wifi_State))
            {
                WE_APP_PRINT("Wifi state event received.\r\n"
                             "State: %d\r\n"
                             "SSID: %s\r\n",
                             (int)StephanoI_Examples_Wifi_State.state, StephanoI_Examples_Wifi_State.ssid);
            }
        }
        break;
        case StephanoI_ATEvent_Wifi_Scan:
        {
            StephanoI_ATWiFi_Scan_t StephanoI_Examples_Wifi_Scan;
            if (StephanoI_ATWiFi_ParseScan(eventText, &StephanoI_Examples_Wifi_Scan))
            {
                WE_APP_PRINT("Wifi scan event received.\r\n"
                             "SSID: %s\r\n"
                             "Encryption: %d\r\n"
                             "RSSI: %d\r\n"
                             "Channel: %d\r\n",
                             StephanoI_Examples_Wifi_Scan.ssid, StephanoI_Examples_Wifi_Scan.enc, StephanoI_Examples_Wifi_Scan.rssi, StephanoI_Examples_Wifi_Scan.channel);
            }
        }
        break;
        case StephanoI_ATEvent_Wifi_IPAquired:
        case StephanoI_ATEvent_Wifi_Connected:
        case StephanoI_ATEvent_Wifi_Disconnected:
        {
        }
        break;
        case StephanoI_ATEvent_Wifi_GetStationIP:
        case StephanoI_ATEvent_Wifi_GetAPIP:
        {
            StephanoI_ATWiFi_GetIP_t StephanoI_Examples_Wifi_IP;
            if (StephanoI_ATWiFi_ParseGetIP(eventText, &StephanoI_Examples_Wifi_IP))
            {
                WE_APP_PRINT("Wifi getIP event received.\r\n"
                             "IP: %s\r\n"
                             "Gateway: %s\r\n"
                             "Netmask: %s\r\n",
                             StephanoI_Examples_Wifi_IP.IP, StephanoI_Examples_Wifi_IP.gateway, StephanoI_Examples_Wifi_IP.netmask);
            }
        }
        break;
        case StephanoI_ATEvent_Wifi_GetConnectedIPs:
        {
            StephanoI_ATWiFi_GetConnectedIPs_t t;
            if (StephanoI_ATWiFi_ParseGetConnectedIPs(eventText, &t))
            {
                WE_APP_PRINT("Wifi connected IPs event received.\r\n"
                             "MAC: %s\r\n"
                             "IP: %s\r\n",
                             t.MAC, t.IP);
            }
        }
        break;
        case StephanoI_ATEvent_Wifi_StationhasConnected:
        {
            StephanoI_ATWiFi_MAC_t t;
            if (StephanoI_ATWiFi_ParseStationhasConnected(eventText, &t))
            {
                WE_APP_PRINT("Wifi station has connected event received.\r\n"
                             "MAC: %s\r\n",
                             t);
            }
        }
        break;
        case StephanoI_ATEvent_Wifi_APAssignIP:
        {
            StephanoI_ATWiFi_APAssignIP_t t;
            if (StephanoI_ATWiFi_ParseAPAssignIP(eventText, &t))
            {
                WE_APP_PRINT("Wifi AP assign IP event received.\r\n"
                             "MAC: %s\r\n"
                             "IP: %s\r\n",
                             t.MAC, t.ip);
            }
        }
        break;
        case StephanoI_ATEvent_Webserver_Response:
        {
            if (StephanoI_ATWebserver_ParseResponse(eventText, &StephanoI_Examples_Webserver_State))
            {
                WE_APP_PRINT("Webserver response event received.\r\n"
                             "Response: %d\r\n",
                             StephanoI_Examples_Webserver_State);
            }
        }
        break;
        case StephanoI_ATEvent_Socket_Connected:
        {
            client_request = true;
        }
        break;
        case StephanoI_ATEvent_MQTT_SubscriptionReceive:
        {
            StephanoI_ATMQTT_ReceiveSubscriptions_t data;
            if (StephanoI_ATMQTT_ParseReceiveSubscriptions(eventText, &data))
            {
                WE_APP_PRINT("MQTT data received: ");
                for (uint32_t i = 0; i < data.length; i++)
                {
                    WE_APP_PRINT("%c", data.data[i]);
                }
                WE_APP_PRINT("\r\n");
                free(data.data);
            }
        }
        break;
        case StephanoI_ATEvent_MQTT_Connected:
        case StephanoI_ATEvent_MQTT_ConnectionInfo:
        case StephanoI_ATEvent_HTTP_Client:
        case StephanoI_ATEvent_HTTP_Get:
        case StephanoI_ATEvent_HTTP_GetSize:
        {
        }
        break;
        default:
        {
            StephanoI_Examples_EventCallback(eventText_original);
        }
        break;
    }
}

/**
 * @brief This example starts a Wifi station, connects to a configured AP, reads the assigned IP and disconnects
 */
void StephanoI_Wifi_Station_Example()
{
    WE_APP_PRINT("*** Start of StephanoI Wifi station example ***\r\n");
    char target_network_ssid[WIFI_SSID_STRINGLEN] = "FRITZ!Box LTE";
    char target_network_password[WIFI_PWD_STRINGLEN] = "2314-5467-3428";

    if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_Wifi_Examples_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    bool ret = StephanoI_PinReset();
    StephanoI_Examples_Print("PinReset", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);

#if 1
    ret = StephanoI_ATDevice_Restore();
    StephanoI_Examples_Print("Restore", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);
#endif

    ret = StephanoI_ATSocket_SetMultiple(true);
    StephanoI_Examples_Print("Set multiple sockets enabled", ret);
    ret = StephanoI_ATSocket_GetMultiple(&multiple_sockets_enabled);
    StephanoI_Examples_Print("Get multiple sockets enabled", ret);

    StephanoI_ATWifiCountryCode_t countrycode = {.policy = StephanoI_ATWifiCountryCodePolicy_Keep, .country_code = "DE", .start_channel = StephanoI_ATWifiChannel_1, .total_channel_count = 13};
    ret = StephanoI_ATWifi_SetCountryCode(countrycode);
    StephanoI_Examples_Print("Set country code", ret);

    ret = StephanoI_ATWifi_SetMode(StephanoI_ATWifiMode_Station, false);
    StephanoI_Examples_Print("Wifi Station on", ret);

    bool IPv6enable;
    ret = StephanoI_ATSocket_GetEnableIPv6(&IPv6enable);
    StephanoI_Examples_Print("IPv6 enable", ret);
    WE_APP_PRINT("IPv6 %s enabled\r\n", IPv6enable ? "is" : "is not");

    uint8_t max_connections;
    ret = StephanoI_ATSocket_GetTCPSSLServerMaxConnections(&max_connections);
    StephanoI_Examples_Print("Get max connections", ret);
    WE_APP_PRINT("%d\r\n", max_connections);

    ret = StephanoI_ATSocket_SetTCPSSLServerMaxConnections(2);
    StephanoI_Examples_Print("Set max connections", ret);

    StephanoI_ATWiFi_Hostname_t name;
    ret = StephanoI_ATWifi_GetHostname(&name);
    StephanoI_Examples_Print("Get host name", ret);
    strcpy(name, "Stephano-I station");
    ret = StephanoI_ATWifi_SetHostname(name);
    StephanoI_Examples_Print("Set host name", ret);

    ret = StephanoI_ATWifi_Station_ReConnectConfiguration(5, 100);
    StephanoI_Examples_Print("Set reconnection settings", ret);

#if 1
    ret = StephanoI_ATWifi_Station_ScanAPOptions(ATWifiScanPrintOptions_ecn | ATWifiScanPrintOptions_ssid | ATWifiScanPrintOptions_rssi | ATWifiScanPrintOptions_mac | ATWifiScanPrintOptions_channel, -100, StephanoI_ATWifiScanAuthOptions_all);
    StephanoI_Examples_Print("Set scan options", ret);
#endif

    ret = StephanoI_ATWifi_Station_ScanAPs(NULL);
    StephanoI_Examples_Print("Scan for APs", ret);

    StephanoI_ATWiFi_State_t state;
    ret = StephanoI_ATWifi_GetState(&state);
    StephanoI_Examples_Print("Get Wifi state", ret);
    WE_Delay(500);
    while (StephanoI_Examples_Wifi_State.state != StephanoI_ATWifiState_ConnectednIP)
    {
        if ((StephanoI_Examples_Wifi_State.state == StephanoI_ATWifiState_Disconnected) || (StephanoI_Examples_Wifi_State.state == StephanoI_ATWifiState_NotConnected))
        {
            ret = StephanoI_ATWifi_Station_Connect(target_network_ssid, target_network_password);
            StephanoI_Examples_Print("Connect to AP", ret);
            WE_Delay(2500);
        }
        ret = StephanoI_ATWifi_GetState(&state);
        StephanoI_Examples_Print("Get Wifi state", ret);
        WE_Delay(500);
    }

    ret = StephanoI_ATWifi_Station_GetIP();
    StephanoI_Examples_Print("Get IP", ret);

#if 0
    ret = StephanoI_ATSocket_SetDNSServerInformation(true, "8.8.8.8");//dns.google.com;
    StephanoI_Examples_Print("Set manual DNS", ret);
#endif

    StephanoI_ATSocket_DomainResolved_t domain;
    ret = StephanoI_ATSocket_ResolveDomain("www.we-online.de", &domain);
    StephanoI_Examples_Print("Resolve domain", ret);

    StephanoI_ATSocket_Ping_t ping;
    ret = StephanoI_ATSocket_Ping("www.google.de", &ping);
    StephanoI_Examples_Print("Ping", ret);
    WE_APP_PRINT("%d ms\r\n", (int)ping);
    WE_Delay(1000);

    ret = StephanoI_ATSocket_EnableSNTPServer(true, 2, "cn.ntp.org.cn");
    StephanoI_Examples_Print("SNTP Server enabled", ret);
    uint32_t time;
    ret = StephanoI_ATSocket_GetSNTPTimeInterval(&time);
    StephanoI_Examples_Print("get SNTP time interval", ret);
    WE_APP_PRINT("%d s\r\n", (int)time);
    ret = StephanoI_ATSocket_SetSNTPTimeInterval(time / 2);
    StephanoI_Examples_Print("set SNTP time interval", ret);
    WE_Delay(1000);
    uint8_t count = 0;
    while (count++ < 10)
    {
        StephanoI_ATSocket_SNTP_Time_t time;
        ret = StephanoI_ATSocket_ReadSNTPTime(&time);
        StephanoI_Examples_Print("StephanoI_ATSocket_ReadSNTPTime", ret);
        WE_APP_PRINT("%s\r\n", time);
        WE_Delay(5000);
    }

    ret = StephanoI_ATWifi_Station_Disconnect();
    StephanoI_Examples_Print("Disconnect from AP", ret);
}

/**
 * @brief This example starts an AP and periodically prints the connected stations
 */
void StephanoI_Wifi_AP_Example()
{
    WE_APP_PRINT("*** Start of StephanoI Wifi AP example ***\r\n");

    if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_Wifi_Examples_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    bool ret = StephanoI_PinReset();
    StephanoI_Examples_Print("PinReset", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);

#if 1
    ret = StephanoI_ATDevice_Restore();
    StephanoI_Examples_Print("Restore", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);
#endif

    bool mult;
    ret = StephanoI_ATSocket_GetMultiple(&mult);
    StephanoI_Examples_Print("Multiple sockets enabled", ret);

    ret = StephanoI_ATWifi_SetMode(StephanoI_ATWifiMode_AP, false);
    StephanoI_Examples_Print("AP on", ret);

    StephanoI_ATWiFi_Hostname_t name;
    ret = StephanoI_ATWifi_GetHostname(&name);
    StephanoI_Examples_Print("Get host name", ret);
    WE_APP_PRINT("%s\r\n", name);

    strcpy(name, "Stephano-I");
    ret = StephanoI_ATWifi_SetHostname(name);
    StephanoI_Examples_Print("Set host name", ret);

    StephanoI_ATWifiCountryCode_t countrycode;
    ret = StephanoI_ATWifi_GetCountryCode(&countrycode);
    StephanoI_Examples_Print("Get country code", ret);
    WE_APP_PRINT("%s\r\n", countrycode.country_code);

    countrycode.policy = StephanoI_ATWifiCountryCodePolicy_Keep;
    strcpy(countrycode.country_code, "DE");
    countrycode.start_channel = 1;
    countrycode.total_channel_count = 13;
    ret = StephanoI_ATWifi_SetCountryCode(countrycode);
    StephanoI_Examples_Print("Set country code", ret);
    WE_Delay(500);

    StephanoI_ATWiFi_MAC_t mac = {"18:fe:35:98:d3:7b"};
    ret = StephanoI_ATWifi_AP_SetMAC(mac);
    StephanoI_Examples_Print("Set MAC", ret);
    WE_Delay(500);

    StephanoI_ATWiFi_MAC_t t;
    ret = StephanoI_ATWifi_AP_GetMAC(&t);
    StephanoI_Examples_Print("Get MAC", ret);
    WE_APP_PRINT("%s\r\n", t);
    WE_Delay(500);

    StephanoI_ATWifiProtocolFlags_t prot;
    ret = StephanoI_ATWifi_AP_GetProtocol(&prot);
    StephanoI_Examples_Print("Get protocol", ret);
    WE_Delay(500);

    ret = StephanoI_ATWifi_AP_SetProtocol(StephanoI_ATWifiProtocolFlags_80211b | StephanoI_ATWifiProtocolFlags_80211g | StephanoI_ATWifiProtocolFlags_80211n);
    StephanoI_Examples_Print("Set protocol", ret);
    WE_Delay(500);

    ret = StephanoI_ATWifi_EnableDHCP(true, StephanoI_ATWifiDHCPFlags_SoftAP);
    StephanoI_Examples_Print("Enable DHCP", ret);
    WE_Delay(500);

    StephanoI_ATWiFi_IPRange_t IPrange;
    ret = StephanoI_ATWifi_AP_GetIPrangeConfiguration(&IPrange);
    StephanoI_Examples_Print("Get IP range", ret);
    WE_Delay(500);

    IPrange.lease_time = 3;
    strcpy(IPrange.start_ip, "192.168.4.10");
    strcpy(IPrange.end_ip, "192.168.4.15");
    ret = StephanoI_ATWifi_AP_SetIPrangeConfiguration(true, IPrange);
    StephanoI_Examples_Print("Set IP range", ret);
    WE_Delay(500);

    ret = StephanoI_ATWifi_AP_GetIP();
    StephanoI_Examples_Print("Get own IP", ret);
    WE_Delay(500);

    StephanoI_ATWiFi_APConfiguration_t ap_config = {.ssid = "myAP", .pwd = "myPassword", .channel = StephanoI_ATWifiChannel_8, .ecn = StephanoI_ATWifiEncryptionMethod_WPA2PSK, .max_connections = 10, .ssid_hidden = false};
    ret = StephanoI_ATWifi_AP_SetConfiguration(ap_config);
    StephanoI_Examples_Print("Configure AP", ret);

    ret = StephanoI_ATWifi_AP_GetConfiguration(&ap_config);
    StephanoI_Examples_Print("Read AP configuration", ret);
    WE_Delay(500);

    StephanoI_ATWiFi_State_t state;
    ret = StephanoI_ATWifi_GetState(&state);
    StephanoI_Examples_Print("Get Wifi state", ret);

    uint8_t dhcp_state = 0;
    ret = StephanoI_ATWifi_GetDHCPState(&dhcp_state);
    StephanoI_Examples_Print("Get DHCP state", ret);
    WE_Delay(500);

    while (1)
    {
        ret = StephanoI_ATWifi_AP_GetConnectedStations();
        StephanoI_Examples_Print("Get connected stations", ret);
        ret = StephanoI_ATWifi_GetState(&state);
        StephanoI_Examples_Print("Get Wifi state", ret);
        WE_Delay(2500);
    }
}

/**
 * @brief This example connects to a configured AP, connects to a MQTT broker and publishes and subcribes to topics
 */
void StephanoI_Wifi_MQTT_Example()
{
    WE_APP_PRINT("*** Start of StephanoI MQTT example ***\r\n");
    char target_network_ssid[WIFI_SSID_STRINGLEN] = "FRITZ!Box LTE";
    char target_network_password[WIFI_PWD_STRINGLEN] = "2314-5467-3428";
    char MQTT_test_publish_topic[] = "/WE_eiSmart/test1";
    char MQTT_test_subscribe_topic[] = "/WE_eiSmart/test2";
    char MQTT_test_server_url[] = "test.mosquitto.org";
    uint16_t MQTT_test_server_port = 1884;
    char MQTT_username[] = "rw";
    char MQTT_password[] = "readwrite";
    char MQTT_ID[] = "Stephano-I_publisher";

    if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_Wifi_Examples_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    bool ret = StephanoI_PinReset();
    StephanoI_Examples_Print("PinReset", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);

#if 1
    ret = StephanoI_ATDevice_Restore();
    StephanoI_Examples_Print("Restore", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);
#endif

    bool mult;
    ret = StephanoI_ATSocket_GetMultiple(&mult);
    StephanoI_Examples_Print("Multiple sockets enabled", ret);

    ret = StephanoI_ATWifi_SetMode(StephanoI_ATWifiMode_Station, false);
    StephanoI_Examples_Print("Wifi Station on", ret);

    StephanoI_ATWifiCountryCode_t countrycode = {.policy = StephanoI_ATWifiCountryCodePolicy_Keep, .country_code = "DE", .start_channel = StephanoI_ATWifiChannel_1, .total_channel_count = 13};
    ret = StephanoI_ATWifi_SetCountryCode(countrycode);
    StephanoI_Examples_Print("Set country code", ret);

    StephanoI_ATWiFi_Hostname_t name = "Stephano-I station";
    ret = StephanoI_ATWifi_SetHostname(name);
    StephanoI_Examples_Print("Set host name", ret);

    StephanoI_ATWiFi_State_t state;
    ret = StephanoI_ATWifi_GetState(&state);
    StephanoI_Examples_Print("Get Wifi state", ret);
    WE_Delay(500);
    while (StephanoI_Examples_Wifi_State.state != StephanoI_ATWifiState_ConnectednIP)
    {
        if ((StephanoI_Examples_Wifi_State.state == StephanoI_ATWifiState_Disconnected) || (StephanoI_Examples_Wifi_State.state == StephanoI_ATWifiState_NotConnected))
        {
            ret = StephanoI_ATWifi_Station_Connect(target_network_ssid, target_network_password);
            StephanoI_Examples_Print("Connect to AP", ret);
            WE_Delay(2500);
        }
        ret = StephanoI_ATWifi_GetState(&state);
        StephanoI_Examples_Print("Get Wifi state", ret);
        WE_Delay(500);
    }

    ret = StephanoI_ATMQTT_Userconfig(0, StephanoI_ATMQTT_Scheme_TCP, MQTT_ID, MQTT_username, MQTT_password, 0, 0, "");
    StephanoI_Examples_Print("MQTT User config", ret);

    ret = StephanoI_ATMQTT_Connect(0, MQTT_test_server_url, MQTT_test_server_port, false);
    StephanoI_Examples_Print("MQTT Connect", ret);
    WE_Delay(1000);

    ret = StephanoI_ATMQTT_Subscribe(0, MQTT_test_subscribe_topic, StephanoI_ATMQTT_QOS_0);
    StephanoI_Examples_Print("MQTT Subscribe", ret);

    uint32_t counter = 0;
    char string_data[32];
    while (1)
    {
        sprintf(string_data, "Test data #%" PRIu32, counter++);
        ret = StephanoI_ATMQTT_Publish(0, MQTT_test_publish_topic, string_data, StephanoI_ATMQTT_QOS_0, 0);
        StephanoI_Examples_Print("MQTT Publish", ret);
        WE_Delay(2500);

        if (counter > 10)
        {
            ret = StephanoI_ATMQTT_Unsubscribe(0, MQTT_test_subscribe_topic);
            StephanoI_Examples_Print("MQTT Unsubscribe", ret);

            ret = StephanoI_ATWifi_Station_Disconnect();
            StephanoI_Examples_Print("Disconnect from AP", ret);
            break;
        }
    }
}

/**
 * @brief This example connects to a configured AP, gets and posts some HTTP strings
 */
void StephanoI_Wifi_HTTP_Example()
{
    WE_APP_PRINT("*** Start of StephanoI HTTP example ***\r\n");
    char target_network_ssid[WIFI_SSID_STRINGLEN] = "FRITZ!Box LTE";
    char target_network_password[WIFI_PWD_STRINGLEN] = "2314-5467-3428";

    if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_Wifi_Examples_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    bool ret = StephanoI_PinReset();
    StephanoI_Examples_Print("PinReset", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);

    do
    {
        ret = StephanoI_ATDevice_EchoOn(false);
        StephanoI_Examples_Print("Echo off", ret);
        WE_Delay(1000);
    } while (ret == false);

#if 1
    ret = StephanoI_ATDevice_Restore();
    StephanoI_Examples_Print("Restore", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);
#endif

    bool mult;
    ret = StephanoI_ATSocket_GetMultiple(&mult);
    StephanoI_Examples_Print("Multiple sockets enabled", ret);

    ret = StephanoI_ATWifi_SetMode(StephanoI_ATWifiMode_Station, false);
    StephanoI_Examples_Print("Wifi Station on", ret);

    StephanoI_ATWifiCountryCode_t countrycode = {.policy = StephanoI_ATWifiCountryCodePolicy_Keep, .country_code = "DE", .start_channel = StephanoI_ATWifiChannel_1, .total_channel_count = 13};
    ret = StephanoI_ATWifi_SetCountryCode(countrycode);
    StephanoI_Examples_Print("Set country code", ret);

    StephanoI_ATWiFi_Hostname_t name = "Stephano-I station";
    ret = StephanoI_ATWifi_SetHostname(name);
    StephanoI_Examples_Print("Set host name", ret);

    StephanoI_ATWiFi_State_t state;
    ret = StephanoI_ATWifi_GetState(&state);
    StephanoI_Examples_Print("Get Wifi state", ret);
    WE_Delay(500);
    while (StephanoI_Examples_Wifi_State.state != StephanoI_ATWifiState_ConnectednIP)
    {
        if ((StephanoI_Examples_Wifi_State.state == StephanoI_ATWifiState_Disconnected) || (StephanoI_Examples_Wifi_State.state == StephanoI_ATWifiState_NotConnected))
        {
            ret = StephanoI_ATWifi_Station_Connect(target_network_ssid, target_network_password);
            StephanoI_Examples_Print("Connect to AP", ret);
            WE_Delay(2500);
        }
        ret = StephanoI_ATWifi_GetState(&state);
        StephanoI_Examples_Print("Get Wifi state", ret);
        WE_Delay(500);
    }

    ret = StephanoI_ATHTTP_Client(StephanoI_ATHTTP_Opt_Head, StephanoI_ATHTTP_Content_ApplicationURLEncoded, "http://httpbin.org/get", "", "", false, NULL, NULL);
    StephanoI_Examples_Print("HTTP get head", ret);

    StephanoI_ATHTTP_Size_t size;
    ret = StephanoI_ATHTTP_GetSize("http://httpbin.org/get", &size);
    StephanoI_Examples_Print("HTTP get size", ret);
    WE_APP_PRINT("Size %d\r\n", size);
    WE_Delay(500);

    StephanoI_ATHTTP_Get_t data = {.length = size, .data = NULL};
    ret = StephanoI_ATHTTP_Get("http://httpbin.org/get", &data);
    StephanoI_Examples_Print("HTTP get", ret);
    free(data.data);
    WE_Delay(2500);

    char post_data[] = "{\"headers\": {\"Accept\": \"application/json\",\"Accept-Encoding\": \"gzip, deflate\",\"Accept-Language\": \"en-US,en;q=0.9,zh-CN;q=0.8,zh;q=0.7\",\"Content-Length\": \"0\",\"Host\": \"httpbin.org\",\"Origin\": \"http://httpbin.org\",\"Referer\": \"http://httpbin.org/\",\"User-Agent\": \"Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) "
                       "Chrome/91.0.4472.114 Safari/537.36\",\"X-Amzn-Trace-Id\": \"Root=1-6150581e-1ad4bd5254b4bf5218070413\"}}";
    ret = StephanoI_ATHTTP_Post("http://httpbin.org/post", (uint8_t*)post_data, strlen(post_data), 0, NULL);
    StephanoI_Examples_Print("HTTP post", ret);

    ret = StephanoI_ATWifi_Station_Disconnect();
    StephanoI_Examples_Print("Disconnect from AP", ret);
}

/**
 * @brief This example starts a Wifi station, connects to a configured AP, connects to a TCP server and repeatedly transmits data via TCP
 */
void StephanoI_TCP_Client_Example()
{
    WE_APP_PRINT("*** Start of StephanoI TCP client example ***\r\n");
    char target_network_ssid[WIFI_SSID_STRINGLEN] = "FRITZ!Box LTE";
    char target_network_password[WIFI_PWD_STRINGLEN] = "2314-5467-3428";
    char tcp_remote_host[WIFI_SSID_STRINGLEN] = "192.168.178.20";
    uint16_t tcp_remote_port = 8080;
    StephanoI_ATSocket_Type_t transmission_type = StephanoI_ATSocket_Type_TCP;

    /* generate data for later data transmission */
    uint8_t data[8192];
    for (uint32_t i = 0; i < sizeof(data); i++)
    {
        data[i] = (0x30) + (i % 10);
    }

    if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_Wifi_Examples_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    bool ret = StephanoI_PinReset();
    StephanoI_Examples_Print("PinReset", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);

#if 1
    ret = StephanoI_ATDevice_Restore();
    StephanoI_Examples_Print("Restore", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);
#endif

    ret = StephanoI_ATSocket_SetMultiple(true);
    StephanoI_Examples_Print("Set multiple sockets enabled", ret);

    ret = StephanoI_ATSocket_GetMultiple(&multiple_sockets_enabled);
    StephanoI_Examples_Print("Get ultiple sockets enabled", ret);

    ret = StephanoI_ATWifi_SetMode(StephanoI_ATWifiMode_Station, false);
    StephanoI_Examples_Print("Wifi Station on", ret);

    StephanoI_ATWifiCountryCode_t countrycode = {.policy = StephanoI_ATWifiCountryCodePolicy_Keep, .country_code = "DE", .start_channel = StephanoI_ATWifiChannel_1, .total_channel_count = 13};
    ret = StephanoI_ATWifi_SetCountryCode(countrycode);
    StephanoI_Examples_Print("Set country code", ret);

    StephanoI_ATWiFi_State_t state;
    ret = StephanoI_ATWifi_GetState(&state);
    StephanoI_Examples_Print("Get Wifi state", ret);
    WE_Delay(500);
    while (StephanoI_Examples_Wifi_State.state != StephanoI_ATWifiState_ConnectednIP)
    {
        if ((StephanoI_Examples_Wifi_State.state == StephanoI_ATWifiState_Disconnected) || (StephanoI_Examples_Wifi_State.state == StephanoI_ATWifiState_NotConnected))
        {
            ret = StephanoI_ATWifi_Station_Connect(target_network_ssid, target_network_password);
            StephanoI_Examples_Print("Connect to AP", ret);
            WE_Delay(2500);
        }
        ret = StephanoI_ATWifi_GetState(&state);
        StephanoI_Examples_Print("Get Wifi state", ret);
        WE_Delay(500);
    }

    ret = StephanoI_ATSocket_OpenTCPSocket(multiple_sockets_enabled, 0, transmission_type, tcp_remote_host, tcp_remote_port);
    StephanoI_Examples_Print("Open socket", ret);

    uint16_t counter = 0;
    while (counter++ < 10)
    {
        ret = StephanoI_ATSocket_Send(multiple_sockets_enabled, 0, NULL, 0, data, sizeof(data));
        StephanoI_Examples_Print("Socket send", ret);
        WE_Delay(1000);
    }

    ret = StephanoI_ATSocket_CloseSocket(multiple_sockets_enabled, 0);
    StephanoI_Examples_Print("Socket closed", ret);

    ret = StephanoI_ATWifi_Station_Disconnect();
    StephanoI_Examples_Print("Disconnect from AP", ret);
}

/**
 * @brief This example connects to a configured AP, connects to a TCP server and repeatedly transmits data as fast as possible via TCP
 */
void StephanoI_TCP_ClientThroughput_Example()
{
    WE_APP_PRINT("*** Start of StephanoI TCP throughput transmission example ***\r\n");
    char target_network_ssid[WIFI_SSID_STRINGLEN] = "FRITZ!Box LTE";
    char target_network_password[WIFI_PWD_STRINGLEN] = "2314-5467-3428";
    char tcp_remote_host[WIFI_SSID_STRINGLEN] = "192.168.178.20";
    uint16_t tcp_remote_port = 8080;
    StephanoI_ATSocket_Type_t transmission_type = StephanoI_ATSocket_Type_TCP;

    /* generate data for later data transmission */
    uint8_t data[2 * 8192];
    for (uint32_t i = 0; i < sizeof(data); i++)
    {
        data[i] = (0x30) + (i % 10);
    }

    bool ret;
#if 1
    /* run the test */
#pragma message("Radio module must be configured to 3MBaud with flow control active, first")
    StephanoI_uart.baudrate = 3000000;
    StephanoI_uart.flowControl = WE_FlowControl_RTSAndCTS;
    if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_Wifi_Examples_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }
    ret = StephanoI_PinReset();
    StephanoI_Examples_Print("PinReset", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);
#elif 0
    /* restore the UART baudrate from 3 MBaud */
#pragma message("Radio module must be configured to 3MBaud with flow control active, first")
    StephanoI_uart.baudrate = 3000000;
    StephanoI_uart.flowControl = WE_FlowControl_RTSAndCTS;
    if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_Wifi_Examples_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }
    ret = StephanoI_ATDevice_Restore();
    StephanoI_Examples_Print("Restore", ret);
    ret = StephanoI_PinReset();
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);
    return;
#elif 0
    /* set the UART baudrate to 3 MBaud */
    if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_Wifi_Examples_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }
    ret = StephanoI_PinReset();
    StephanoI_Examples_Print("PinReset", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);

    StephanoI_ATDevice_UART_t uart_def = {.baudrate = 3000000, .flow_control = StephanoI_ATDevice_UartFlowControl_RTSnCTS, .parity = StephanoI_ATDevice_UartParity_None};
    ret = StephanoI_ATDevice_SetDefaultUART(uart_def);
    StephanoI_Examples_Print("Set UART baud rate", ret);
    return;
#endif

    ret = StephanoI_ATSocket_SetMultiple(true);
    StephanoI_Examples_Print("Set multiple sockets enabled", ret);

    ret = StephanoI_ATSocket_GetMultiple(&multiple_sockets_enabled);
    StephanoI_Examples_Print("Get multiple sockets enabled", ret);

    ret = StephanoI_ATWifi_SetMode(StephanoI_ATWifiMode_Station, false);
    StephanoI_Examples_Print("Wifi Station on", ret);

    StephanoI_ATWifiCountryCode_t countrycode = {.policy = StephanoI_ATWifiCountryCodePolicy_Keep, .country_code = "DE", .start_channel = StephanoI_ATWifiChannel_1, .total_channel_count = 13};
    ret = StephanoI_ATWifi_SetCountryCode(countrycode);
    StephanoI_Examples_Print("Set country code", ret);

    StephanoI_ATWiFi_State_t state;
    ret = StephanoI_ATWifi_GetState(&state);
    StephanoI_Examples_Print("Get Wifi state", ret);
    WE_Delay(500);
    while (StephanoI_Examples_Wifi_State.state != StephanoI_ATWifiState_ConnectednIP)
    {
        if ((StephanoI_Examples_Wifi_State.state == StephanoI_ATWifiState_Disconnected) || (StephanoI_Examples_Wifi_State.state == StephanoI_ATWifiState_NotConnected))
        {
            ret = StephanoI_ATWifi_Station_Connect(target_network_ssid, target_network_password);
            StephanoI_Examples_Print("Connect to AP", ret);
            WE_Delay(2500);
        }
        ret = StephanoI_ATWifi_GetState(&state);
        StephanoI_Examples_Print("Get Wifi state", ret);
        WE_Delay(500);
    }

    ret = StephanoI_ATSocket_OpenTCPSocket(multiple_sockets_enabled, 0, transmission_type, tcp_remote_host, tcp_remote_port);
    StephanoI_Examples_Print("Open socket", ret);

    uint16_t counter = 0;
    while (counter++ < 1000)
    {
        ret = StephanoI_ATSocket_Send(multiple_sockets_enabled, 0, NULL, 0, data, sizeof(data));
        StephanoI_Examples_Print("Socket send", ret);
    }

    ret = StephanoI_ATSocket_CloseSocket(multiple_sockets_enabled, 0);
    StephanoI_Examples_Print("Socket closed", ret);

    ret = StephanoI_ATWifi_Station_Disconnect();
    StephanoI_Examples_Print("Disconnect from AP", ret);
}

/**
 * @brief This example starts a Wifi AP and a web server such that a remote device can do the provisioning (choose a WiFi network, where the module should connect to)
 */
void StephanoI_Wifi_Provisioning_Example()
{
    WE_APP_PRINT("*** Start of StephanoI provisioning example ***\r\n");

    if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_Wifi_Examples_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    bool ret = StephanoI_PinReset();
    StephanoI_Examples_Print("PinReset", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);

#if 1
    ret = StephanoI_ATDevice_Restore();
    StephanoI_Examples_Print("Restore", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);
#endif

    ret = StephanoI_ATSocket_SetMultiple(true);
    StephanoI_Examples_Print("Set multiple sockets enabled", ret);
    ret = StephanoI_ATSocket_GetMultiple(&multiple_sockets_enabled);
    StephanoI_Examples_Print("Get multiple sockets enabled", ret);

    ret = StephanoI_ATWifi_SetMode(StephanoI_ATWifiMode_APplusStation, true);
    StephanoI_Examples_Print("Wifi AP and Station on", ret);

    StephanoI_ATWifiCountryCode_t countrycode = {.policy = StephanoI_ATWifiCountryCodePolicy_Keep, .country_code = "DE", .start_channel = StephanoI_ATWifiChannel_1, .total_channel_count = 13};
    ret = StephanoI_ATWifi_SetCountryCode(countrycode);
    StephanoI_Examples_Print("Set country code", ret);

    StephanoI_ATWiFi_Hostname_t name = "Stephano-I";
    ret = StephanoI_ATWifi_SetHostname(name);
    StephanoI_Examples_Print("Set host name", ret);

    StephanoI_ATWiFi_APConfiguration_t APconfig = {.ssid = "Provision me", .pwd = "", .channel = StephanoI_ATWifiChannel_11, .ecn = StephanoI_ATWifiEncryptionMethod_Open, .max_connections = 5, .ssid_hidden = false};
    ret = StephanoI_ATWifi_AP_SetConfiguration(APconfig);
    StephanoI_Examples_Print("Configure AP", ret);

    ret = StephanoI_ATWebserver_Enable(true, 80, 25);
    StephanoI_Examples_Print("Start Web server", ret);

    /* wait until provisioning has been done and device has connected to a AP */
    while (StephanoI_Examples_Wifi_State.state != StephanoI_ATWifiState_ConnectednIP)
    {
        StephanoI_ATWiFi_State_t state;
        ret = StephanoI_ATWifi_GetState(&state);
        StephanoI_Examples_Print("Get Wifi state", ret);
        WE_Delay(10000);
    }

    ret = StephanoI_ATWifi_Station_GetIP();
    StephanoI_Examples_Print("Get IP", ret);

    ret = StephanoI_ATWebserver_Enable(false, 0, 0);
    StephanoI_Examples_Print("Stop Web server", ret);

    StephanoI_ATSocket_DomainResolved_t domain;
    ret = StephanoI_ATSocket_ResolveDomain("www.we-online.de", &domain);
    StephanoI_Examples_Print("Resolve domain", ret);

    int counter = 0;
    while (counter++ < 5)
    {
        StephanoI_ATSocket_Ping_t ping;
        ret = StephanoI_ATSocket_Ping("www.google.de", &ping);
        StephanoI_Examples_Print("Ping", ret);
        WE_Delay(1000);
    }

    ret = StephanoI_ATWifi_Station_Disconnect();
    StephanoI_Examples_Print("Disconnect from AP", ret);

    /* restart and connect again to AP */
    ret = StephanoI_PinReset();
    StephanoI_Examples_Print("PinReset", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);

    ret = StephanoI_ATWifi_SetMode(StephanoI_ATWifiMode_Station, true);
    StephanoI_Examples_Print("Wifi Station on", ret);

    /* wait until station has connected to a AP */
    StephanoI_ATWiFi_State_t state;
    ret = StephanoI_ATWifi_GetState(&state);
    StephanoI_Examples_Print("Get Wifi state", ret);
    WE_Delay(2500);
    while (StephanoI_Examples_Wifi_State.state != StephanoI_ATWifiState_ConnectednIP)
    {
        ret = StephanoI_ATWifi_GetState(&state);
        StephanoI_Examples_Print("Get Wifi state", ret);
        WE_Delay(2500);
    }

    counter = 0;
    while (counter++ < 5)
    {
        StephanoI_ATSocket_Ping_t ping;
        ret = StephanoI_ATSocket_Ping("www.google.de", &ping);
        StephanoI_Examples_Print("Ping", ret);
        WE_Delay(1000);
    }

    ret = StephanoI_ATWifi_Station_Disconnect();
    StephanoI_Examples_Print("Disconnect from AP", ret);
}

/**
 * @brief This example starts a Wifi AP and a web server such that a remote device can do the OTA (over the air update)
 */
void StephanoI_Wifi_OTA_Example()
{
    WE_APP_PRINT("*** Start of StephanoI OTA example ***\r\n");

    if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_Wifi_Examples_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    bool ret = StephanoI_PinReset();
    StephanoI_Examples_Print("PinReset", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);

#if 1
    ret = StephanoI_ATDevice_Restore();
    StephanoI_Examples_Print("Restore", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);
#endif

    ret = StephanoI_ATSocket_SetMultiple(true);
    StephanoI_Examples_Print("Set multiple sockets enabled", ret);
    ret = StephanoI_ATSocket_GetMultiple(&multiple_sockets_enabled);
    StephanoI_Examples_Print("Get multiple sockets enabled", ret);

    ret = StephanoI_ATWifi_SetMode(StephanoI_ATWifiMode_APplusStation, true);
    StephanoI_Examples_Print("Wifi AP and Station on", ret);

    StephanoI_ATWifiCountryCode_t countrycode = {.policy = StephanoI_ATWifiCountryCodePolicy_Keep, .country_code = "DE", .start_channel = StephanoI_ATWifiChannel_1, .total_channel_count = 13};
    ret = StephanoI_ATWifi_SetCountryCode(countrycode);
    StephanoI_Examples_Print("Set country code", ret);

    StephanoI_ATWiFi_Hostname_t name = "Stephano-I";
    ret = StephanoI_ATWifi_SetHostname(name);
    StephanoI_Examples_Print("Set host name", ret);

    StephanoI_ATWiFi_APConfiguration_t APconfig = {.ssid = "Update me", .pwd = "", .channel = StephanoI_ATWifiChannel_11, .ecn = StephanoI_ATWifiEncryptionMethod_Open, .max_connections = 5, .ssid_hidden = false};
    ret = StephanoI_ATWifi_AP_SetConfiguration(APconfig);
    StephanoI_Examples_Print("Configure AP", ret);

    ret = StephanoI_ATWebserver_Enable(true, 80, 25);
    StephanoI_Examples_Print("Start Web server", ret);

    ret = StephanoI_ATDevice_GetVersion();
    StephanoI_Examples_Print("Get version", ret);

    /* wait until provisioning has been done and device has connected to a AP */
    while (StephanoI_Examples_Webserver_State != StephanoI_ATWebserver_Response_OTAReceiveDataSuccess)
    {
        WE_APP_PRINT("Waiting to finish OTA, state: %d\r\n", StephanoI_Examples_Webserver_State);
        WE_Delay(10000);
    }

    ret = StephanoI_ATDevice_Reset();
    StephanoI_Examples_Print("Reset", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);

    ret = StephanoI_ATDevice_GetVersion();
    StephanoI_Examples_Print("Get version", ret);
}

/**
 * @brief This example starts a Wifi station, connects to a configured AP, creates TCP server, wait for a client request and repeatedly transmits data via TCP
 */
void StephanoI_TCP_Server_Example()
{
    WE_APP_PRINT("*** Start of StephanoI TCP server example ***\r\n");
    char target_network_ssid[WIFI_SSID_STRINGLEN] = "FRITZ!Box LTE";
    char target_network_password[WIFI_PWD_STRINGLEN] = "2314-5467-3428";
    uint16_t tcp_host_port = 5678;
    StephanoI_ATSocket_Type_t transmission_type = StephanoI_ATSocket_Type_TCP;

    /* generate data for later data transmission */
    uint8_t data[8192];
    for (uint32_t i = 0; i < sizeof(data); i++)
    {
        data[i] = (0x30) + (i % 10);
    }

    if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_Wifi_Examples_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    bool ret = StephanoI_PinReset();
    StephanoI_Examples_Print("PinReset", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);

#if 1
    ret = StephanoI_ATDevice_Restore();
    StephanoI_Examples_Print("Restore", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);
#endif

    ret = StephanoI_ATSocket_SetMultiple(true);
    StephanoI_Examples_Print("Set multiple sockets enabled", ret);
    ret = StephanoI_ATSocket_GetMultiple(&multiple_sockets_enabled);
    StephanoI_Examples_Print("Get multiple sockets enabled", ret);

    ret = StephanoI_ATWifi_SetMode(StephanoI_ATWifiMode_Station, false);
    StephanoI_Examples_Print("Wifi Station on", ret);

    StephanoI_ATWifiCountryCode_t countrycode = {.policy = StephanoI_ATWifiCountryCodePolicy_Keep, .country_code = "DE", .start_channel = StephanoI_ATWifiChannel_1, .total_channel_count = 13};
    ret = StephanoI_ATWifi_SetCountryCode(countrycode);
    StephanoI_Examples_Print("Set country code", ret);

    StephanoI_ATWiFi_State_t state;
    ret = StephanoI_ATWifi_GetState(&state);
    StephanoI_Examples_Print("Get Wifi state", ret);
    WE_Delay(500);
    while (StephanoI_Examples_Wifi_State.state != StephanoI_ATWifiState_ConnectednIP)
    {
        if ((StephanoI_Examples_Wifi_State.state == StephanoI_ATWifiState_Disconnected) || (StephanoI_Examples_Wifi_State.state == StephanoI_ATWifiState_NotConnected))
        {
            ret = StephanoI_ATWifi_Station_Connect(target_network_ssid, target_network_password);
            StephanoI_Examples_Print("Connect to AP", ret);
            WE_Delay(2500);
        }
        ret = StephanoI_ATWifi_GetState(&state);
        StephanoI_Examples_Print("Get Wifi state", ret);
        ret = StephanoI_ATWifi_Station_GetIP();
        StephanoI_Examples_Print("Get station IP address", ret);
        WE_Delay(500);
    }

    ret = StephanoI_ATSocket_EnableTCPSSLServer(multiple_sockets_enabled, 1, tcp_host_port, transmission_type, 0);
    StephanoI_Examples_Print("TCP server created", ret);

    uint16_t timeout;
    ret = StephanoI_ATSocket_GetTCPSSLServerTimeout(&timeout);
    StephanoI_Examples_Print("Get server timeout", ret);
    WE_APP_PRINT("%d\r\n", timeout);

    ret = StephanoI_ATSocket_SetTCPSSLServerTimeout(2000);
    StephanoI_Examples_Print("Set server timeout", ret);

    while (1)
    {
        if (client_request == true)
            break;
        WE_Delay(1000);
    }

    uint16_t counter = 0;
    while (counter++ < 10)
    {
        ret = StephanoI_ATSocket_Send(multiple_sockets_enabled, 0, NULL, 0, data, sizeof(data));
        StephanoI_Examples_Print("Socket send", ret);
        WE_Delay(1000);
    }

    while (1)
    {
        WE_Delay(1000);
    }

    ret = StephanoI_ATSocket_CloseSocket(multiple_sockets_enabled, 0);
    StephanoI_Examples_Print("Socket closed", ret);

    ret = StephanoI_ATWifi_Station_Disconnect();
    StephanoI_Examples_Print("Disconnect from AP", ret);
}

/**
 * @brief This example starts a Wifi station, connects to a configured AP, connects to a fixed remote IP and port and repeatedly transmits data via UDP
 */
void StephanoI_UDP_Client_Example()
{
    WE_APP_PRINT("*** Start of StephanoI UDP client example ***\r\n");
    char target_network_ssid[WIFI_SSID_STRINGLEN] = "FRITZ!Box LTE";
    char target_network_password[WIFI_PWD_STRINGLEN] = "2314-5467-3428";
    char udp_remote_host[WIFI_SSID_STRINGLEN] = "192.168.178.20";
    uint16_t udp_remote_port = 8080;
    uint16_t udp_local_port = 8080;
    StephanoI_ATSocket_Type_t transmission_type = StephanoI_ATSocket_Type_UDP;

    /* generate data for later data transmission */
    uint8_t data[8192];
    for (uint32_t i = 0; i < sizeof(data); i++)
    {
        data[i] = (0x30) + (i % 10);
    }

    if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_Wifi_Examples_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    bool ret = StephanoI_PinReset();
    StephanoI_Examples_Print("PinReset", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);

#if 1
    ret = StephanoI_ATDevice_Restore();
    StephanoI_Examples_Print("Restore", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);
#endif

    ret = StephanoI_ATSocket_SetMultiple(true);
    StephanoI_Examples_Print("Set multiple sockets enabled", ret);
    ret = StephanoI_ATSocket_GetMultiple(&multiple_sockets_enabled);
    StephanoI_Examples_Print("Get multiple sockets enabled", ret);

    ret = StephanoI_ATWifi_SetMode(StephanoI_ATWifiMode_Station, false);
    StephanoI_Examples_Print("Wifi Station on", ret);

    StephanoI_ATWifiCountryCode_t countrycode = {.policy = StephanoI_ATWifiCountryCodePolicy_Keep, .country_code = "DE", .start_channel = StephanoI_ATWifiChannel_1, .total_channel_count = 13};
    ret = StephanoI_ATWifi_SetCountryCode(countrycode);
    StephanoI_Examples_Print("Set country code", ret);

    StephanoI_ATWiFi_State_t state;
    ret = StephanoI_ATWifi_GetState(&state);
    StephanoI_Examples_Print("Get Wifi state", ret);
    WE_Delay(500);
    while (StephanoI_Examples_Wifi_State.state != StephanoI_ATWifiState_ConnectednIP)
    {
        if ((StephanoI_Examples_Wifi_State.state == StephanoI_ATWifiState_Disconnected) || (StephanoI_Examples_Wifi_State.state == StephanoI_ATWifiState_NotConnected))
        {
            ret = StephanoI_ATWifi_Station_Connect(target_network_ssid, target_network_password);
            StephanoI_Examples_Print("Connect to AP", ret);
            WE_Delay(2500);
        }
        ret = StephanoI_ATWifi_GetState(&state);
        StephanoI_Examples_Print("Get Wifi state", ret);
        WE_Delay(500);
    }

    ret = StephanoI_ATWifi_Station_GetIP();
    StephanoI_Examples_Print("Get IP", ret);
    WE_Delay(2500);

    ret = StephanoI_ATSocket_OpenUDPSocket(multiple_sockets_enabled, 0, transmission_type, udp_remote_host, udp_remote_port, udp_local_port);
    StephanoI_Examples_Print("Open socket", ret);

    uint16_t counter = 0;
    while (counter++ < 10)
    {
        ret = StephanoI_ATSocket_Send(multiple_sockets_enabled, 0, udp_remote_host, udp_remote_port, data, sizeof(data));
        StephanoI_Examples_Print("Socket send", ret);
        WE_Delay(1000);
    }

    ret = StephanoI_ATSocket_CloseSocket(multiple_sockets_enabled, 0);
    StephanoI_Examples_Print("Socket closed", ret);

    ret = StephanoI_ATWifi_Station_Disconnect();
    StephanoI_Examples_Print("Disconnect from AP", ret);
}

/**
 * @brief This example connects to a configured AP, connects to a UDP server and repeatedly transmits data as fast as possible via UDP
 */
void StephanoI_UDP_ClientThroughput_Example()
{
    WE_APP_PRINT("*** Start of StephanoI UDP throughput example ***\r\n");
    char target_network_ssid[WIFI_SSID_STRINGLEN] = "FRITZ!Box LTE";
    char target_network_password[WIFI_PWD_STRINGLEN] = "2314-5467-3428";
    char udp_remote_host[WIFI_SSID_STRINGLEN] = "192.168.178.20";
    uint16_t udp_remote_port = 8080;
    uint16_t udp_local_port = 8080;
    StephanoI_ATSocket_Type_t transmission_type = StephanoI_ATSocket_Type_UDP;

    /* generate data for later data transmission */
    uint8_t data[2 * 8192];
    for (uint32_t i = 0; i < sizeof(data); i++)
    {
        data[i] = (0x30) + (i % 10);
    }

    bool ret;
#if 1
    /* run the test */
#pragma message("Radio module must be configured to 3MBaud with flow control active, first")
    StephanoI_uart.baudrate = 3000000;
    StephanoI_uart.flowControl = WE_FlowControl_RTSAndCTS;
    if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_Wifi_Examples_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }
    ret = StephanoI_PinReset();
    StephanoI_Examples_Print("PinReset", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);
#elif 0
    /* restore the UART baudrate from 3 MBaud */
#pragma message("Radio module must be configured to 3MBaud with flow control active, first")
    StephanoI_uart.baudrate = 3000000;
    StephanoI_uart.flowControl = WE_FlowControl_RTSAndCTS;
    if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_Wifi_Examples_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }
    ret = StephanoI_ATDevice_Restore();
    StephanoI_Examples_Print("Restore", ret);
    ret = StephanoI_PinReset();
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);
    return;
#elif 0
    /* set the UART baudrate to 3 MBaud */
    if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_Wifi_Examples_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }
    ret = StephanoI_PinReset();
    StephanoI_Examples_Print("PinReset", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);

    StephanoI_ATDevice_UART_t uart_def = {.baudrate = 3000000, .flow_control = StephanoI_ATDevice_UartFlowControl_RTSnCTS, .parity = StephanoI_ATDevice_UartParity_None};
    ret = StephanoI_ATDevice_SetDefaultUART(uart_def);
    StephanoI_Examples_Print("Set UART baud rate", ret);
    return;
#endif

    ret = StephanoI_ATSocket_SetMultiple(true);
    StephanoI_Examples_Print("Set multiple sockets enabled", ret);
    ret = StephanoI_ATSocket_GetMultiple(&multiple_sockets_enabled);
    StephanoI_Examples_Print("Get multiple sockets enabled", ret);

    ret = StephanoI_ATWifi_SetMode(StephanoI_ATWifiMode_Station, false);
    StephanoI_Examples_Print("Wifi Station on", ret);

    StephanoI_ATWifiCountryCode_t countrycode = {.policy = StephanoI_ATWifiCountryCodePolicy_Keep, .country_code = "DE", .start_channel = StephanoI_ATWifiChannel_1, .total_channel_count = 13};
    ret = StephanoI_ATWifi_SetCountryCode(countrycode);
    StephanoI_Examples_Print("Set country code", ret);

    StephanoI_ATWiFi_State_t state;
    ret = StephanoI_ATWifi_GetState(&state);
    StephanoI_Examples_Print("Get Wifi state", ret);
    WE_Delay(500);
    while (StephanoI_Examples_Wifi_State.state != StephanoI_ATWifiState_ConnectednIP)
    {
        if ((StephanoI_Examples_Wifi_State.state == StephanoI_ATWifiState_Disconnected) || (StephanoI_Examples_Wifi_State.state == StephanoI_ATWifiState_NotConnected))
        {
            ret = StephanoI_ATWifi_Station_Connect(target_network_ssid, target_network_password);
            StephanoI_Examples_Print("Connect to AP", ret);
            WE_Delay(2500);
        }
        ret = StephanoI_ATWifi_GetState(&state);
        StephanoI_Examples_Print("Get Wifi state", ret);
        WE_Delay(500);
    }

    ret = StephanoI_ATSocket_OpenUDPSocket(multiple_sockets_enabled, 0, transmission_type, udp_remote_host, udp_remote_port, udp_local_port);
    StephanoI_Examples_Print("Open socket", ret);

    uint16_t counter = 0;
    while (counter++ < 1000)
    {
        ret = StephanoI_ATSocket_Send(multiple_sockets_enabled, 0, udp_remote_host, udp_remote_port, data, sizeof(data));
        StephanoI_Examples_Print("Socket send", ret);
    }

    ret = StephanoI_ATSocket_CloseSocket(multiple_sockets_enabled, 0);
    StephanoI_Examples_Print("Socket closed", ret);

    ret = StephanoI_ATWifi_Station_Disconnect();
    StephanoI_Examples_Print("Disconnect from AP", ret);
}

/**
 * @brief This example starts a Wifi station, connects to a configured AP, creates SSL server, wait for a client request and repeatedly transmits data
 */
void StephanoI_SSL_Server_Example()
{
    WE_APP_PRINT("*** Start of StephanoI SSL server example ***\r\n");
    char target_network_ssid[WIFI_SSID_STRINGLEN] = "FRITZ!Box LTE";
    char target_network_password[WIFI_PWD_STRINGLEN] = "2314-5467-3428";
    uint16_t tcp_host_port = 5678;
    StephanoI_ATSocket_Type_t transmission_type = StephanoI_ATSocket_Type_SSL;
    char SNTP_server[] = "0.de.pool.ntp.org";
    bool SNTPEnable = true;
    bool CAEnable = true;
    uint8_t timezone = 2; // UTC + 2 for germany
    /* generate data for later data transmission */
    uint8_t data[8192];
    for (uint32_t i = 0; i < sizeof(data); i++)
    {
        data[i] = (0x30) + (i % 10);
    }

    if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_Wifi_Examples_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    bool ret = StephanoI_PinReset();
    StephanoI_Examples_Print("PinReset", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);

#if 1
    ret = StephanoI_ATDevice_Restore();
    StephanoI_Examples_Print("Restore", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
    ret = StephanoI_ATDevice_EchoOn(false);
    StephanoI_Examples_Print("Echo off", ret);
#endif

    ret = StephanoI_ATSocket_SetMultiple(true);
    StephanoI_Examples_Print("Set multiple sockets enabled", ret);
    ret = StephanoI_ATSocket_GetMultiple(&multiple_sockets_enabled);
    StephanoI_Examples_Print("Get multiple sockets enabled", ret);

    ret = StephanoI_ATWifi_SetMode(StephanoI_ATWifiMode_Station, false);
    StephanoI_Examples_Print("Wifi Station on", ret);

    StephanoI_ATWifiCountryCode_t countrycode;
    ret = StephanoI_ATWifi_GetCountryCode(&countrycode);
    StephanoI_Examples_Print("Get country code", ret);

    countrycode.policy = StephanoI_ATWifiCountryCodePolicy_Keep;
    strcpy(countrycode.country_code, "DE");
    countrycode.start_channel = 1;
    countrycode.total_channel_count = 13;
    ret = StephanoI_ATWifi_SetCountryCode(countrycode);
    StephanoI_Examples_Print("Set country code", ret);

    StephanoI_ATWiFi_State_t state;
    ret = StephanoI_ATWifi_GetState(&state);
    StephanoI_Examples_Print("Get Wifi state", ret);
    WE_Delay(500);
    while (StephanoI_Examples_Wifi_State.state != StephanoI_ATWifiState_ConnectednIP)
    {
        if ((StephanoI_Examples_Wifi_State.state == StephanoI_ATWifiState_Disconnected) || (StephanoI_Examples_Wifi_State.state == StephanoI_ATWifiState_NotConnected))
        {
            ret = StephanoI_ATWifi_Station_Connect(target_network_ssid, target_network_password);
            StephanoI_Examples_Print("Connect to AP", ret);
            WE_Delay(2500);
        }
        ret = StephanoI_ATWifi_GetState(&state);
        StephanoI_Examples_Print("Get Wifi state", ret);
        ret = StephanoI_ATWifi_Station_GetIP();
        StephanoI_Examples_Print("Get station IP address", ret);
        ret = StephanoI_ATSocket_EnableSNTPServer(SNTPEnable, timezone, SNTP_server);
        StephanoI_Examples_Print("SNTP Server enabled", ret);
        WE_Delay(500);
    }

    ret = StephanoI_ATSocket_EnableTCPSSLServer(multiple_sockets_enabled, true, tcp_host_port, transmission_type, CAEnable);
    StephanoI_Examples_Print("SSL server created", ret);

    ret = StephanoI_ATSocket_GetSSLClient();
    StephanoI_Examples_Print("Get SSL client", ret);
    StephanoI_ATSocket_SSLClientName_t name = {.link_ID = 0, .common_name = "Link0"};
    ret = StephanoI_ATSocket_SetSSLClientName(multiple_sockets_enabled, name);
    StephanoI_Examples_Print("Set SSL client name", ret);
    ret = StephanoI_ATSocket_GetSSLClientName(&name);
    StephanoI_Examples_Print("Get SSL client name", ret);
    StephanoI_ATSocket_SSLClientServerNameIndication_t sni;
    ret = StephanoI_ATSocket_GetSSLClientServerNameIndication(&sni);
    StephanoI_Examples_Print("Get SSL client SNI", ret);
    StephanoI_ATSocket_SSLClientALPN_t alpn;
    ret = StephanoI_ATSocket_GetSSLClientALPN(&alpn);
    StephanoI_Examples_Print("Get SSL ALPN", ret);
    StephanoI_ATSocket_SSLClientPSK_t psk;
    ret = StephanoI_ATSocket_GetSSLClientPSK(&psk);
    StephanoI_Examples_Print("Get SSL PSK", ret);

    while (1)
    {
        if (client_request == true)
            break;
        WE_Delay(1000);
    }

    uint16_t counter = 0;
    while (counter++ < 10)
    {
        ret = StephanoI_ATSocket_Send(multiple_sockets_enabled, 0, NULL, 0, data, sizeof(data));
        StephanoI_Examples_Print("Socket send", ret);
        WE_Delay(1000);
    }

    while (1)
    {
        WE_Delay(1000);
    }

    ret = StephanoI_ATSocket_CloseSocket(multiple_sockets_enabled, 0);
    StephanoI_Examples_Print("Socket closed", ret);

    ret = StephanoI_ATWifi_Station_Disconnect();
    StephanoI_Examples_Print("Disconnect from AP", ret);
}
