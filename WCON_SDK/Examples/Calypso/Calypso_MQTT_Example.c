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
 * @brief Calypso MQTT example.
 */
#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/ATCommands/ATMQTT.h>
#include <Calypso/ATCommands/ATNetCfg.h>
#include <Calypso/ATCommands/ATSocket.h>
#include <Calypso/ATCommands/ATWLAN.h>
#include <Calypso/Calypso_Examples.h>
#include <Calypso/Calypso_MQTT_Example.h>
#include <stdio.h>

/**
 * @brief MQTT server address (used in MQTT example)
 */
static const char* mqttServerAddress = "test.mosquitto.org";

/**
 * @brief MQTT server port (used in MQTT example)
 */
static const uint16_t mqttServerPort = 1883;

/**
 * @brief MQTT example.
 */
void Calypso_MQTT_Example(void)
{
    WE_DEBUG_PRINT("*** Start of Calypso ATMQTT example ***\r\n");

    bool ret = false;

    if (!Calypso_Init(&Calypso_uart, &Calypso_pins, &Calypso_Examples_EventCallback))
    {
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

    /* Create MQTT client */
    Calypso_ATMQTT_ServerInfo_t serverInfo;
    strcpy(serverInfo.address, mqttServerAddress);
    serverInfo.port = mqttServerPort;

    Calypso_ATMQTT_SecurityParams_t securityParams;
    securityParams.securityMethod = Calypso_ATMQTT_SecurityMethod_SSLV3;
    strcpy(securityParams.CAFile, ATCOMMAND_STRING_EMPTY);
    strcpy(securityParams.certificateFile, ATCOMMAND_STRING_EMPTY);
    strcpy(securityParams.DHKey, ATCOMMAND_STRING_EMPTY);
    strcpy(securityParams.privateKeyFile, ATCOMMAND_STRING_EMPTY);
    securityParams.cipher = Calypso_ATSocket_Cipher_SSL_RSA_WITH_RC4_128_MD5;

    Calypso_ATMQTT_ConnectionParams_t connectionParams;
    connectionParams.format = Calypso_DataFormat_Binary;
    connectionParams.protocolVersion = Calypso_ATMQTT_ProtocolVersion_v3_1;
    connectionParams.blockingSend = 0;

    uint8_t mqttIndex = 0;
    ret = Calypso_ATMQTT_Create("testClient", Calypso_ATMQTT_CreateFlags_URL, serverInfo, securityParams, connectionParams, &mqttIndex);
    Calypso_Examples_Print("Create MQTT client", ret);

    /* Connect to MQTT broker */
    ret = Calypso_ATMQTT_Connect(mqttIndex);
    Calypso_Examples_Print("Connect to MQTT broker", ret);

    WE_Delay(2000);

    /* Publish some MQTT topics */
    char* message = "10.4 deg";
    ret = Calypso_ATMQTT_Publish(mqttIndex, "kitchen/temp", Calypso_ATMQTT_QoS_QoS0, 1, strlen(message), message);
    Calypso_Examples_Print("Publish MQTT topic", ret);

    Calypso_ATMQTT_SubscribeTopic_t topics[4];
    strcpy(topics[0].topic, "kitchen/temp");
    topics[0].QoS = Calypso_ATMQTT_QoS_QoS0;

    strcpy(topics[1].topic, "kitchen/humidity");
    topics[1].QoS = Calypso_ATMQTT_QoS_QoS0;

    /* Subscribe to the above topics, then unsubscribe from "kitchen/temp" */

    ret = Calypso_ATMQTT_Subscribe(mqttIndex, 2, topics);
    Calypso_Examples_Print("MQTT subscribe", ret);

    WE_Delay(1000);

    ret = Calypso_ATMQTT_Unsubscribe(mqttIndex, "kitchen/temp", "", "", "");
    Calypso_Examples_Print("MQTT unsubscribe", ret);

    WE_Delay(1000);

    /* Disconnect and delete MQTT client */

    ret = Calypso_ATMQTT_Disconnect(mqttIndex);
    Calypso_Examples_Print("MQTT disconnect", ret);

    WE_Delay(1000);

    ret = Calypso_ATMQTT_Delete(mqttIndex);
    Calypso_Examples_Print("MQTT delete", ret);

    Calypso_Deinit();
}
