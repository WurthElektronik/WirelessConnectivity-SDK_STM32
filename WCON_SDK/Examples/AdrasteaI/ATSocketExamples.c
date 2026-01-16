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
#include <AdrasteaI/ATCommands/ATPacketDomain.h>
#include <AdrasteaI/ATCommands/ATSocket.h>
#include <AdrasteaI/ATSMSExamples.h>
#include <AdrasteaI/AdrasteaI.h>
#include <AdrasteaI/AdrasteaI_Examples.h>
#include <stdio.h>

void AdrasteaI_ATSocket_EventCallback(char* eventText);

static AdrasteaI_ATPacketDomain_Network_Registration_Status_t status = {.state = 0};
static AdrasteaI_ATSocket_ID_t dataReceivedsocketID = AdrasteaI_ATSocket_ID_Invalid;
static bool dataReceived = false;

void ATSocketExample()
{
    WE_APP_PRINT("*** Start of Adrastea-I ATSocket example ***\r\n");

    if (!AdrasteaI_Init(&AdrasteaI_uart, &AdrasteaI_pins, &AdrasteaI_ATSocket_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    bool ret = AdrasteaI_ATPacketDomain_SetNetworkRegistrationResultCode(AdrasteaI_ATPacketDomain_Network_Registration_Result_Code_Enable_with_Location_Info);
    AdrasteaI_ExamplesPrint("Set Network Registration Result Code", ret);
    while (status.state != AdrasteaI_ATPacketDomain_Network_Registration_State_Registered_Roaming)
    {
        WE_Delay(10);
    }

    ret = AdrasteaI_ATSocket_SetSocketUnsolicitedNotificationEvents(AdrasteaI_ATSocket_Event_All, AdrasteaI_ATCommon_Event_State_Enable);
    AdrasteaI_ExamplesPrint("Set Socket Unsolicited Notification Events", ret);

    AdrasteaI_ATSocket_ID_t socketID;
    ret = AdrasteaI_ATSocket_AllocateSocket(1, AdrasteaI_ATSocket_Type_TCP, AdrasteaI_ATSocket_Behaviour_Open_Connection, "52.43.121.77", 9001, 0, 0, 0, AdrasteaI_ATSocket_IP_Addr_Format_IPv4, &socketID);
    AdrasteaI_ExamplesPrint("Allocate Socket", ret);
    if (ret)
    {
        WE_APP_PRINT("Socket ID: %d\r\n", socketID);
    }

    ret = AdrasteaI_ATSocket_ActivateSocket(1, AdrasteaI_ATCommon_Session_ID_Invalid);
    AdrasteaI_ExamplesPrint("Activate Socket", ret);

    char Payload[256];
    AdrasteaI_ATSocket_Data_Read_t dataRead;
    dataRead.data = Payload;
    while (dataReceived == false)
    {
        WE_Delay(10);
    }

    ret = AdrasteaI_ATSocket_ReceiveFromSocket(dataReceivedsocketID, &dataRead, sizeof(Payload));
    AdrasteaI_ExamplesPrint("Receive From Socket", ret);
    if (ret)
    {
        WE_APP_PRINT("Socket ID: %d, Data length: %d, Payload: %s\r\n", dataRead.socketID, dataRead.dataLength, dataRead.data);
    }
}

void AdrasteaI_ATSocket_EventCallback(char* eventText)
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
        case AdrasteaI_ATEvent_Socket_Data_Received:
        {
            AdrasteaI_ATSocket_ParseDataReceivedEvent(eventText, &dataReceivedsocketID);
            dataReceived = true;
            break;
        }
        case AdrasteaI_ATEvent_Socket_Sockets_Read:
        {
            AdrasteaI_ATSocket_Read_Result_t readResult;
            AdrasteaI_ATSocket_ParseSocketsReadEvent(eventText, &readResult);
            break;
        }
        default:
            break;
    }
}
