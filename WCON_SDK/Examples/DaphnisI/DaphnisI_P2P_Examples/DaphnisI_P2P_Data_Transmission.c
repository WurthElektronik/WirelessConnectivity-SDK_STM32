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
 * @brief Daphnis-I P2P Data transmission example.
 *
 */
#include <DaphnisI/ATCommands/ATUserSettings.h>
#include <DaphnisI/ATCommands/P2P.h>
#include <DaphnisI/DaphnisI.h>
#include <DaphnisI/DaphnisI_Examples.h>
#include <DaphnisI/DaphnisI_P2P_Examples/DaphnisI_P2P_Examples_Helper.h>
#include <global/global.h>
#include <stdio.h>

#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)

/* Set the role of the module whether it will send data or receive data */
#define MODULE_ROLE MODULE_ROLE_TX

#define RF_PROFILE 7

#if MODULE_ROLE == MODULE_ROLE_TX

#define EVENT_CALLBACK NULL

#elif MODULE_ROLE == MODULE_ROLE_RX

static void DaphnisI_P2P_EventCallback(DaphnisI_ATEvent_t event, char* eventText);
#define EVENT_CALLBACK DaphnisI_P2P_EventCallback

static uint8_t rxDataBuffer[DAPHNISI_P2P_MAX_PAYLOAD_SIZE];
static DaphnisI_P2P_RxData_t rxData = {.data = rxDataBuffer};

/*
 * State machine for Daphnis-I P2P RX example.
 * */
typedef enum
{
    DaphnisI_P2P_SM_WaitingForData,
    DaphnisI_P2P_SM_DataReceived
} DaphnisI_P2P_SM_t;

static volatile DaphnisI_P2P_SM_t current_state = DaphnisI_P2P_SM_WaitingForData;

#endif /* MODULE_ROLE == MODULE_ROLE_TX / MODULE_ROLE == MODULE_ROLE_RX */

/**
 * @brief This example showcases the data exchange procedure between two modules
 * where one is sending and the other is receiving.
 *
 * To run this example two Daphnis-I modules/EV-boards and two host controllers are needed.
 * The project should be built twice one time with MODULE_ROLE set to TX and the other set to RX.
 */
void DaphnisI_P2P_Data_Transmission()
{

    if (!DaphnisI_Init(&DaphnisI_uart, &DaphnisI_pins, EVENT_CALLBACK))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    DaphnisI_Setting_Status_t setting_status = DaphnisI_Setting_Status_Unmodified;

    setting_status |= DaphnisI_Mode_Check_and_Set(DaphnisI_Mode_P2P);

    if (!DaphnisI_Apply_Settings(setting_status))
    {
        return;
    }

    setting_status = DaphnisI_Setting_Status_Unmodified;

    setting_status |= DaphnisI_P2P_Role_Check_and_Set(DaphnisI_P2P_Role_Transceiver);
    setting_status |= DaphnisI_P2P_RF_Profile_Check_and_Set(RF_PROFILE);

    if (!DaphnisI_Apply_Settings(setting_status))
    {
        return;
    }

#if MODULE_ROLE == MODULE_ROLE_TX

    uint8_t payload[] = {0x12, 0x34};

#elif MODULE_ROLE == MODULE_ROLE_RX

    if (!DaphnisI_P2P_SetRXEnabled(true))
    {
        WE_APP_PRINT("Failed to turn on radio receiving.\r\n");
        return;
    }

#endif /* MODULE_ROLE == MODULE_ROLE_TX / MODULE_ROLE == MODULE_ROLE_RX */

    while (1)
    {

#if MODULE_ROLE == MODULE_ROLE_TX
        if (!DaphnisI_P2P_TransmitBroadcast(payload, sizeof(payload)))
        {
            WE_APP_PRINT("Data transmission failed\r\n");
            return;
        }

        WE_Delay(500);
#elif MODULE_ROLE == MODULE_ROLE_RX

        switch (current_state)
        {
            case DaphnisI_P2P_SM_DataReceived:
            {
                WE_APP_PRINT("Received data from address ");
                DaphnisI_Print_Key_Addr(rxData.source_address, DAPHNISI_ADDRESS_LENGTH);
                WE_APP_PRINT(" with %d dBm: ", rxData.RSSI);
                DaphnisI_Print_Payload(rxData.data, rxData.data_length);
                current_state = DaphnisI_P2P_SM_WaitingForData;
                break;
            }
            default:
                break;
        }

#endif /* MODULE_ROLE == MODULE_ROLE_TX / MODULE_ROLE == MODULE_ROLE_RX */
    }
}

#if MODULE_ROLE == MODULE_ROLE_RX

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
static void DaphnisI_P2P_EventCallback(DaphnisI_ATEvent_t event, char* eventText)
{
    switch (event)
    {
        case DaphnisI_ATEvent_P2P_RxData:
        {
            if (!DaphnisI_P2P_ParseRXDataEvent(&eventText, &rxData))
            {
                return;
            }
            current_state = DaphnisI_P2P_SM_DataReceived;
            break;
        }
        default:
            break;
    }
}
#endif /* MODULE_ROLE == MODULE_ROLE_RX */

#endif /* DAPHNISI_MIN_FW_VER >= FW(1, 4, 0) */
