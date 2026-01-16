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
#include <AdrasteaI/ATCommands/ATProprietary.h>
#include <AdrasteaI/ATCommands/ATSMS.h>
#include <AdrasteaI/ATSMSExamples.h>
#include <AdrasteaI/AdrasteaI.h>
#include <AdrasteaI/AdrasteaI_Examples.h>
#include <stdio.h>

void AdrasteaI_ATSMS_EventCallback(char* eventText);

static AdrasteaI_ATPacketDomain_Network_Registration_Status_t status = {.state = 0};

void ATSMSExample()
{
    WE_APP_PRINT("*** Start of Adrastea-I ATSMS example ***\r\n");

    if (!AdrasteaI_Init(&AdrasteaI_uart, &AdrasteaI_pins, &AdrasteaI_ATSMS_EventCallback))
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

    ret = AdrasteaI_ATSMS_SetSMSUnsolicitedNotificationEvents(AdrasteaI_ATCommon_Event_State_Enable);
    AdrasteaI_ExamplesPrint("Set SMS Unsolicited Notification Events", ret);

    AdrasteaI_ATSMS_Message_Index_t msgIdx;
    ret = AdrasteaI_ATSMS_WriteMessageToStorage("+491638975759", AdrasteaI_ATSMS_Address_Type_International_Number, "Test Message", &msgIdx);
    AdrasteaI_ExamplesPrint("Write Message To Storage", ret);

    ret = AdrasteaI_ATSMS_ListMessages(AdrasteaI_ATSMS_Message_State_All);
    AdrasteaI_ExamplesPrint("List Messages", ret);

    AdrasteaI_ATSMS_Message_Storage_Usage_t storageUsage;
    ret = AdrasteaI_ATSMS_ReadMessageStorageUsage(&storageUsage);
    AdrasteaI_ExamplesPrint("Read Message Storage Usage", ret);

    if (ret)
    {
        WE_APP_PRINT("Read & Delete Storage: \r\n");
        WE_APP_PRINT("Location: %d, Messages Used Count: %d, Messages Max Count: %d\r\n", storageUsage.readDeleteStorageUsage.storageLocation, storageUsage.readDeleteStorageUsage.usedMessages, storageUsage.readDeleteStorageUsage.maxMessages);
        WE_APP_PRINT("Write & Send Storage: \r\n");
        WE_APP_PRINT("Location: %d, Messages Used Count: %d, Messages Max Count: %d\r\n", storageUsage.writeSendStorageUsage.storageLocation, storageUsage.writeSendStorageUsage.usedMessages, storageUsage.writeSendStorageUsage.maxMessages);
        WE_APP_PRINT("Receive Storage: \r\n");
        WE_APP_PRINT("Location: %d, Messages Used Count: %d, Messages Max Count: %d\r\n", storageUsage.receiveStorageUsage.storageLocation, storageUsage.receiveStorageUsage.usedMessages, storageUsage.receiveStorageUsage.maxMessages);
    }

    AdrasteaI_ATSMS_Message_Reference_t msg1ID, msg2ID;
    ret = AdrasteaI_ATSMS_SendMessage("+491638975759", AdrasteaI_ATSMS_Address_Type_International_Number, "Test Message Direct", &msg1ID);
    AdrasteaI_ExamplesPrint("Send Message", ret);
    if (ret)
    {
        WE_APP_PRINT("Message Reference: %d\r\n", msg1ID);
    }

    ret = AdrasteaI_ATSMS_SendMessageFromStorage(msgIdx, &msg2ID);
    AdrasteaI_ExamplesPrint("Send Message From Storage", ret);
    if (ret)
    {
        WE_APP_PRINT("Message Reference: %d\r\n", msg2ID);
    }

    ret = AdrasteaI_ATSMS_DeleteAllMessages();
    AdrasteaI_ExamplesPrint("Delete All Messages", ret);
}

void AdrasteaI_ATSMS_EventCallback(char* eventText)
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
        case AdrasteaI_ATEvent_SMS_Read_Message:
        {
            AdrasteaI_ATSMS_Message_t message;
            AdrasteaI_ATSMS_ParseReadMessageEvent(eventText, &message);
            break;
        }
        case AdrasteaI_ATEvent_SMS_List_Messages:
        {
            AdrasteaI_ATSMS_Message_t message;
            if (!AdrasteaI_ATSMS_ParseListMessagesEvent(eventText, &message))
            {
                return;
            }
            WE_APP_PRINT("Message Index: %d, Message State: %d, Address: %s, Payload: %s\r\n", message.messageIndex, message.messageState, message.address, message.payload);
            break;
        }
        case AdrasteaI_ATEvent_SMS_Message_Received:
        {
            AdrasteaI_ATSMS_Message_Received_Result_t result;
            AdrasteaI_ATSMS_ParseMessageReceivedEvent(eventText, &result);
            break;
        }
        case AdrasteaI_ATEvent_SMS_Error:
        {
            AdrasteaI_ATSMS_Error_t errorCode;
            AdrasteaI_ATSMS_ParseSMSErrorEvent(eventText, &errorCode);
            break;
        }
        default:
            break;
    }
}
