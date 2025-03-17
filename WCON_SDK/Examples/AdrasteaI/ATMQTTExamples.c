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
#include <AdrasteaI/ATCommands/ATEvent.h>
#include <AdrasteaI/ATCommands/ATMQTT.h>
#include <AdrasteaI/ATCommands/ATPacketDomain.h>
#include <AdrasteaI/ATMQTTExamples.h>
#include <AdrasteaI/AdrasteaI.h>
#include <AdrasteaI/AdrasteaI_Examples.h>
#include <stdio.h>

void AdrasteaI_ATMQTT_EventCallback(char* eventText);

static AdrasteaI_ATPacketDomain_Network_Registration_Status_t status = {.state = 0};
static AdrasteaI_ATMQTT_Connection_Result_t conResult = {.resultCode = -1};
static AdrasteaI_ATMQTT_Subscription_Result_t subResult = {.resultCode = -1};

/**
 * @brief This example connects to the cellular network and accesses mosquitto.org via MQTT
 *
 */
void ATMQTTExample()
{
    WE_DEBUG_PRINT("*** Start of Adrastea-I ATMQTT example ***\r\n");

    if (!AdrasteaI_Init(&AdrasteaI_uart, &AdrasteaI_pins, &AdrasteaI_ATMQTT_EventCallback))
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

    ret = AdrasteaI_ATMQTT_SetMQTTUnsolicitedNotificationEvents(AdrasteaI_ATMQTT_Event_All, 1);
    AdrasteaI_ExamplesPrint("MQTT Unsolicited Notification Events", ret);

    ret = AdrasteaI_ATMQTT_ConfigureNodes(1, "adrastea_module_23", "test.mosquitto.org", "", "");
    AdrasteaI_ExamplesPrint("Configure Nodes", ret);

    ret = AdrasteaI_ATMQTT_ConfigureProtocol(1, 1200, 1);
    AdrasteaI_ExamplesPrint("Configure Protocol", ret);

    ret = AdrasteaI_ATMQTT_Connect(1);
    AdrasteaI_ExamplesPrint("Connect", ret);

    while (conResult.resultCode != AdrasteaI_ATMQTT_Event_Result_Code_Success)
    {
        WE_Delay(10);
    }

    ret = AdrasteaI_ATMQTT_Publish(1, 0, 0, "adrtopic", "241", 3);
    AdrasteaI_ExamplesPrint("Publish", ret);

    ret = AdrasteaI_ATMQTT_Subscribe(1, AdrasteaI_ATMQTT_QoS_Exactly_Once, "adrtopic");
    AdrasteaI_ExamplesPrint("Subscribe", ret);

    while (subResult.resultCode != AdrasteaI_ATMQTT_Event_Result_Code_Success)
    {
        WE_Delay(10);
    }
    WE_Delay(5000);
}

void AdrasteaI_ATMQTT_EventCallback(char* eventText)
{
    AdrasteaI_ATEvent_t event;
    if (false == AdrasteaI_ATEvent_ParseEventType(&eventText, &event))
    {
        return;
    }

    switch (event)
    {
        case AdrasteaI_ATEvent_MQTT_Connection_Confirmation:
        {
            AdrasteaI_ATMQTT_ParseConnectionConfirmationEvent(eventText, &conResult);
            break;
        }
        case AdrasteaI_ATEvent_MQTT_Subscription_Confirmation:
        {
            AdrasteaI_ATMQTT_ParseSubscriptionConfirmationEvent(eventText, &subResult);
            break;
        }
        case AdrasteaI_ATEvent_MQTT_Publication_Received:
        {
            AdrasteaI_ATMQTT_Publication_Received_Result_t result;
            char payload[128];
            result.payload = payload;
            result.payloadMaxBufferSize = sizeof(payload);
            if (!AdrasteaI_ATMQTT_ParsePublicationReceivedEvent(eventText, &result))
            {
                return;
            }
            WE_DEBUG_PRINT("Connection ID: %d, Message ID: %d, Topic Name: %s, Payload Size: %d, Payload: %s\r\n", result.connID, result.msgID, result.topicName, result.payloadSize, result.payload);
            break;
        }
        case AdrasteaI_ATEvent_PacketDomain_Network_Registration_Status:
        {
            AdrasteaI_ATPacketDomain_ParseNetworkRegistrationStatusEvent(eventText, &status);
            break;
        }
        default:
            break;
    }
}
