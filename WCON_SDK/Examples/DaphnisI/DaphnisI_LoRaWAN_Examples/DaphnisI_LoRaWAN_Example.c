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
 * @brief DaphnisI LoRaWAN example.
 *
 */
#include <DaphnisI/ATCommands/ATDevice.h>
#include <DaphnisI/ATCommands/ATUserSettings.h>
#include <DaphnisI/ATCommands/LoRaWAN.h>
#include <DaphnisI/DaphnisI.h>
#include <DaphnisI/DaphnisI_Examples.h>
#include <DaphnisI/DaphnisI_LoRaWAN_Examples/DaphnisI_LoRaWAN_Example.h>
#include <global/global.h>
#include <stdio.h>
#include <string.h>

static void DaphnisI_OTAA_EventCallback(DaphnisI_ATEvent_t event, char* eventText);

static uint8_t rxDataBuffer[DAPHNISI_LORAWAN_MAX_PAYLOAD_SIZE];
static DaphnisI_RxData_t rxData = {.data = rxDataBuffer};

#define TX_INTERVAL (uint32_t)60000

/*
 * State machine for Daphnis-I sdk example.
 * */
typedef enum
{
    DaphnisI_LoRaWAN_SM_Join_Network,
    DaphnisI_LoRaWAN_SM_Send_Data,
    DaphnisI_LoRaWAN_SM_Receive_Data,
    DaphnisI_LoRaWAN_SM_Idle,
} DaphnisI_LoRaWAN_SM_t;

static volatile DaphnisI_LoRaWAN_SM_t currentState = DaphnisI_LoRaWAN_SM_Join_Network;

/**
 * @brief In this example the module, that is already registered at TTN network, is joining the network
 * with the provided application EUI and application key. After joining, the module transmits a test string and is able
 * to receive data from the TTN server.
 */
void DaphnisI_OTAA_JoinExample()
{
    if (!DaphnisI_Init(&DaphnisI_uart, &DaphnisI_pins, DaphnisI_OTAA_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)

    DaphnisI_Setting_Status_t setting_status = DaphnisI_Setting_Status_Unmodified;

    setting_status |= DaphnisI_Mode_Check_and_Set(DaphnisI_Mode_Lorawan);

    if (!DaphnisI_Apply_Settings(setting_status))
    {
        return;
    }

#endif /* DAPHNISI_MIN_FW_VER >= FW(1, 4, 0) */

    // Setup keys
    DaphnisI_EUI_t appEUI = {0};
    DaphnisI_Key_t appKey = {0xA8, 0x98, 0x5A, 0x16, 0xC0, 0xE3, 0x79, 0x7E, 0xAD, 80, 0x43, 0x2B, 0x01, 0xA2, 0x42, 0x71};
    DaphnisI_EUI_t device_eui;

    // Read device EUI to enter on network server
    if (!DaphnisI_LoRaWAN_GetDeviceEUI(&device_eui))
    {
        WE_APP_PRINT("Failed to get Device EUI.\r\n");
        return;
    }
    WE_APP_PRINT("Device EUI is ");
    DaphnisI_Print_Key_Addr(device_eui, DAPHNISI_LORAWAN_EUI_LENGTH);
    WE_APP_PRINT("\r\n");

    if (!DaphnisI_LoRaWAN_SetAPPEUI(appEUI))
    {
        WE_APP_PRINT("Failed to set APP EUI.\r\n");
        return;
    }

    WE_APP_PRINT("APP EUI is ");
    DaphnisI_Print_Key_Addr(appEUI, DAPHNISI_LORAWAN_EUI_LENGTH);
    WE_APP_PRINT("\r\n");

    if (!DaphnisI_LoRaWAN_SetKey(DaphnisI_KeyType_APP, appKey))
    {
        WE_APP_PRINT("Failed to set APP KEY.\r\n");
        return;
    }

    WE_APP_PRINT("APP KEY is ");
    DaphnisI_Print_Key_Addr(appKey, DAPHNISI_LORAWAN_KEY_LENGTH);
    WE_APP_PRINT("\r\n");

    uint32_t current_time, last_send_time = WE_GetTick() - TX_INTERVAL;

    while (1)
    {
        switch (currentState)
        {
            case DaphnisI_LoRaWAN_SM_Join_Network:
            {
                // Join the network
                currentState = DaphnisI_LoRaWAN_Join(DaphnisI_JoinMode_OTAA) ? DaphnisI_LoRaWAN_SM_Idle : DaphnisI_LoRaWAN_SM_Join_Network;
                break;
            }
            case DaphnisI_LoRaWAN_SM_Send_Data:
            {
                current_time = WE_GetTick();
                // Transmit one packet each TX_INTERVAL
                if (current_time - last_send_time >= TX_INTERVAL)
                {
                    const char* payload = "Hello. I'm a test string";
                    if (!DaphnisI_LoRaWAN_Send(1, (const uint8_t*)payload, strlen(payload), false))
                    {
                        WE_APP_PRINT("Failed to send data\r\n");
                    }
                    last_send_time = current_time;
                }
                break;
            }
            case DaphnisI_LoRaWAN_SM_Receive_Data:
            {
                WE_APP_PRINT("Data Received on port %u\r\n", rxData.port);
                DaphnisI_Print_Payload(rxData.data, rxData.dataLength);
                currentState = DaphnisI_LoRaWAN_SM_Send_Data;
                break;
            }
            default:
                break;
        }
    }
}

/**
 * @brief Is called when an event notification has been received.
 *
 * Note that this function is called from an interrupt - code in this function
 * should thus be kept simple.
 *
 * Note in particular that it is not possible to send AT commands to Daphnis
 * from within this event handler.
 *
 * Also note that not all calls of this handler necessarily correspond to valid
 * events (i.e. events from DaphnisI_ATEvent_t). Some events might in fact be responses
 * to AT commands that are not included in DaphnisI_ATEvent_t.
 */
static void DaphnisI_OTAA_EventCallback(DaphnisI_ATEvent_t event, char* eventText)
{
    switch (event)
    {
        case DaphnisI_ATEvent_LoRaWAN_JoinState_Success:
        {
            WE_APP_PRINT("Network Joined Successfully\r\n");
            currentState = DaphnisI_LoRaWAN_SM_Send_Data;
            break;
        }
        case DaphnisI_ATEvent_LoRaWAN_JoinState_Fail:
        {
            WE_APP_PRINT("Failed to join network\r\n");
            currentState = DaphnisI_LoRaWAN_SM_Join_Network;
            break;
        }
        case DaphnisI_ATEvent_LoRaWAN_RxData:
        {
            currentState = DaphnisI_LoRaWAN_ParseRxDataEvent(&eventText, &rxData) ? DaphnisI_LoRaWAN_SM_Receive_Data : currentState;
            break;
        }
        default:
            break;
    }
}
