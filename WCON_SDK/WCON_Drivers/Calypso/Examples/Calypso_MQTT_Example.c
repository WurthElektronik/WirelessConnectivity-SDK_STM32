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
 * COPYRIGHT (c) 2022 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Calypso MQTT example.
 */

#include "Calypso_MQTT_Example.h"

#include <stdio.h>

#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/ATCommands/ATMQTT.h>
#include <Calypso/ATCommands/ATNetCfg.h>
#include <Calypso/ATCommands/ATSocket.h>
#include <Calypso/ATCommands/ATWLAN.h>

#include "Calypso_Examples.h"

/**
 * @brief MQTT server address (used in MQTT example)
 */
static const char *mqttServerAddress = "192.168.178.57";

/**
 * @brief MQTT server port (used in MQTT example)
 */
static const uint16_t mqttServerPort = 1883;

/**
 * @brief MQTT example.
 */
void Calypso_MQTT_Example(void)
{
    printf("*** Start of Calypso ATMQTT example ***\r\n");

    bool ret = false;

    if (!Calypso_Init(Calypso_Examples_baudRate, Calypso_Examples_flowControl, Calypso_Examples_parity, &Calypso_Examples_EventCallback, NULL))
    {
        return;
    }

    Calypso_PinReset();

    Calypso_Examples_WaitForStartup(5000);

    WE_Delay(1000);

    /* Get version info. This retrieves Calypso's firmware version (amongst other version info) and
     * stores the firmware version in Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
     * Calypso_firmwareVersionPatch for later use. */
    ATDevice_Value_t deviceValue;
    ret = ATDevice_Get(ATDevice_GetId_General, ATDevice_GetGeneral_Version, &deviceValue);
    Calypso_Examples_Print("Get device version", ret);

    ATWLAN_Disconnect();

    /* Set IPv4 address method DHCP */
    ATNetCfg_IPv4Config_t newIpV4Config = {0};
    newIpV4Config.method = ATNetCfg_IPv4Method_Dhcp;
    ret = ATNetCfg_SetIPv4AddressStation(&newIpV4Config);
    Calypso_Examples_Print("Set IPv4 DHCP", ret);

    /* No automatic connection */
    ret = ATWLAN_SetConnectionPolicy(ATWLAN_PolicyConnection_None);
    Calypso_Examples_Print("Set no WLAN connection policy", ret);

    /* WLAN station mode */
    ret = ATWLAN_SetMode(ATWLAN_SetMode_Station);
    Calypso_Examples_Print("Set WLAN station mode", ret);

    ret = ATDevice_Restart(0);
    Calypso_Examples_Print("Restart network processor", ret);

    WE_Delay(1000);

    /* Connect to WLAN */
    ATWLAN_ConnectionArguments_t connectArgs;
    memset(&connectArgs, 0, sizeof(connectArgs));
    strcpy(connectArgs.SSID, Calypso_Examples_wlanSSID);
    connectArgs.securityParams.securityType = ATWLAN_SecurityType_WPA_WPA2;
    strcpy(connectArgs.securityParams.securityKey, Calypso_Examples_wlanKey);

    ret = ATWLAN_Connect(connectArgs);
    Calypso_Examples_Print("Connect to WLAN", ret);

    WE_Delay(2000);


    /* Create MQTT client */
    ATMQTT_ServerInfo_t serverInfo;
    strcpy(serverInfo.address, mqttServerAddress);
    serverInfo.port = mqttServerPort;

    ATMQTT_SecurityParams_t securityParams;
    securityParams.securityMethod = ATMQTT_SecurityMethod_SSLV3;
    strcpy(securityParams.CAFile, CALYPSO_STRING_EMPTY);
    strcpy(securityParams.certificateFile, CALYPSO_STRING_EMPTY);
    strcpy(securityParams.DHKey, CALYPSO_STRING_EMPTY);
    strcpy(securityParams.privateKeyFile, CALYPSO_STRING_EMPTY);
    securityParams.cipher = ATSocket_Cipher_SSL_RSA_WITH_RC4_128_MD5;

    ATMQTT_ConnectionParams_t connectionParams;
    connectionParams.format = Calypso_DataFormat_Binary;
    connectionParams.protocolVersion = ATMQTT_ProtocolVersion_v3_1;
    connectionParams.blockingSend = 0;

    uint8_t mqttIndex = 0;
    ret = ATMQTT_Create("testClient",
                        ATMQTT_CreateFlags_IPv4,
                        serverInfo,
                        securityParams,
                        connectionParams,
                        &mqttIndex);
    Calypso_Examples_Print("Create MQTT client", ret);


    /* Connect to MQTT broker */
    ret = ATMQTT_Connect(mqttIndex);
    Calypso_Examples_Print("Connect to MQTT broker", ret);

    WE_Delay(2000);

    /* Publish some MQTT topics */
    char *message = "10.4 deg";
    ret = ATMQTT_Publish(mqttIndex, "kitchen/temp", ATMQTT_QoS_QoS0, 1, strlen(message), message);
    Calypso_Examples_Print("Publish MQTT topic", ret);

    ATMQTT_SubscribeTopic_t topics[4];
    strcpy(topics[0].topic, "kitchen/temp");
    topics[0].QoS = ATMQTT_QoS_QoS0;

    strcpy(topics[1].topic, "kitchen/humidity");
    topics[1].QoS = ATMQTT_QoS_QoS0;


    /* Subscribe to the above topics, then unsubscribe from "kitchen/temp" */

    ret = ATMQTT_Subscribe(mqttIndex, 2, topics);
    Calypso_Examples_Print("MQTT subscribe", ret);

    WE_Delay(1000);

    ret = ATMQTT_Unsubscribe(mqttIndex, "kitchen/temp", "", "", "");
    Calypso_Examples_Print("MQTT unsubscribe", ret);

    WE_Delay(1000);


    /* Disconnect and delete MQTT client */

    ret = ATMQTT_Disconnect(mqttIndex);
    Calypso_Examples_Print("MQTT disconnect", ret);

    WE_Delay(1000);

    ret = ATMQTT_Delete(mqttIndex);
    Calypso_Examples_Print("MQTT delete", ret);

    Calypso_Deinit();
}
