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
 * @brief Main file for DaphnisI driver examples.
 *
 */
#include <DaphnisI/ATCommands/ATDevice.h>
#include <DaphnisI/ATCommands/ATGPIO.h>
#include <DaphnisI/DaphnisI.h>
#include <DaphnisI/DaphnisI_Examples.h>
#include <DaphnisI/DaphnisI_Remote_GPIO_Example.h>
#include <global/global.h>
#include <stdio.h>

#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)

static void DaphnisI_Remote_GPIO_EventCallback(DaphnisI_ATEvent_t event, char* eventText);

/*
 * State machine for Daphnis-I sdk example.
 * */
typedef enum
{
    DaphnisI_Remote_GPIO_SM_Idle,
    DaphnisI_Remote_GPIO_SM_Cfg_Changed,
    DaphnisI_Remote_GPIO_SM_Value_Changed,
    DaphnisI_Remote_GPIO_SM_Error,
} DaphnisI_Remote_GPIO_SM_t;

static volatile DaphnisI_Remote_GPIO_SM_t currentState = DaphnisI_Remote_GPIO_SM_Idle;

static DaphnisI_P2P_GPIO_RemoteCfgChangedData_t remoteCfgChangedData;
static DaphnisI_P2P_GPIO_RemoteValueChangedData_t remoteValueChangedData;
static DaphnisI_P2P_GPIO_RemoteCfgSetResponseData_t remoteCfgSetResponseData;
static DaphnisI_P2P_GPIO_RemoteCfgGetResponseData_t remoteCfgGetResponseData;
static DaphnisI_P2P_GPIO_RemoteValueSetResponseData_t remoteValueSetResponseData;
static DaphnisI_P2P_GPIO_RemoteValueGetResponseData_t remoteValueGetResponseData;

void DaphnisI_Remote_GPIO_Example()
{
    if (!DaphnisI_Init(&DaphnisI_uart, &DaphnisI_pins, DaphnisI_Remote_GPIO_EventCallback))
    {
        WE_DEBUG_PRINT("Initialization error\r\n");
        return;
    }

    DaphnisI_SerialNumber_t sn;

    DaphnisI_GetSerialNumber(&sn);

    DaphnisI_GPIOConfigBlock_t configBlocks[DaphnisI_GPIO_Count];

    remoteCfgChangedData.configBlocksP = configBlocks;

    remoteCfgGetResponseData.configBlocksP = configBlocks;

    DaphnisI_GPIOValueGetBlock_t valueGetBlocks[DaphnisI_GPIO_Count];

    remoteValueChangedData.valueGetBlocksP = valueGetBlocks;

    remoteValueGetResponseData.valueGetBlocksP = valueGetBlocks;

    DaphnisI_GPIOStatusBlock_t statusBlocks[DaphnisI_GPIO_Count];

    remoteCfgSetResponseData.statusBlocksP = statusBlocks;

    remoteValueSetResponseData.statusBlocksP = statusBlocks;

    DaphnisI_Device_Address_t dest_address = {0x05, 0x2e, 0x7d, 0xb1};

    DaphnisI_GPIOConfigBlock_t gpio0_configBlock = {.GPIO_ID = DaphnisI_GPIO_0, .function = DaphnisI_GPIO_IO_Output, .value = {.input = DaphnisI_GPIO_Output_High}};

    if (!DaphnisI_P2P_GPIO_Remote_Configuration_Set(dest_address, &gpio0_configBlock, 1))
    {
        WE_DEBUG_PRINT("Failed to send GPIO Remote command\r\n");
    }

    while (1)
    {
        switch (currentState)
        {
            default:
            case DaphnisI_Remote_GPIO_SM_Idle:
            {

                break;
            }
            case DaphnisI_Remote_GPIO_SM_Cfg_Changed:
            {
                for (uint8_t i = 0; i < remoteCfgChangedData.configBlocksCount; i++)
                {
                    WE_DEBUG_PRINT("GPIO ID: %d, Function: ", remoteCfgChangedData.configBlocksP[i].GPIO_ID);
                    switch (remoteCfgChangedData.configBlocksP[i].function)
                    {

                        case DaphnisI_GPIO_IO_Disconnected:
                        {
                            WE_DEBUG_PRINT("Disconnected\r\n");
                            break;
                        }
                        case DaphnisI_GPIO_IO_Input:
                        {
                            WE_DEBUG_PRINT("Input, Value: %d\r\n", remoteCfgChangedData.configBlocksP[i].value.input);
                            break;
                        }
                        case DaphnisI_GPIO_IO_Output:
                        {
                            WE_DEBUG_PRINT("Output, Value: %d\r\n", remoteCfgChangedData.configBlocksP[i].value.output);
                            break;
                        }
                        default:
                            break;
                    }
                }
                currentState = DaphnisI_Remote_GPIO_SM_Idle;
                break;
            }
            case DaphnisI_Remote_GPIO_SM_Value_Changed:
            {
                for (uint8_t i = 0; i < remoteValueChangedData.valueGetBlocksCount; i++)
                {
                    WE_DEBUG_PRINT("GPIO ID: %d, Value: %d\r\n", remoteValueChangedData.valueGetBlocksP[i].GPIO_ID, remoteValueChangedData.valueGetBlocksP[i].value);
                }
                currentState = DaphnisI_Remote_GPIO_SM_Idle;
                break;
            }
            case DaphnisI_Remote_GPIO_SM_Error:
            {
                WE_DEBUG_PRINT("Error occurred\r\n");
                return;
            }
        }
    };
}

static void DaphnisI_Remote_GPIO_EventCallback(DaphnisI_ATEvent_t event, char* eventText)
{
    switch (event)
    {
        case DaphnisI_ATEvent_P2P_GPIO_Remote_Cfg_Changed:
        {
            if (!DaphnisI_P2P_GPIO_Remote_ParseCfgChangedEvent(&eventText, &remoteCfgChangedData))
            {
                currentState = DaphnisI_Remote_GPIO_SM_Error;
                break;
            }

            currentState = DaphnisI_Remote_GPIO_SM_Cfg_Changed;
            break;
        }
        case DaphnisI_ATEvent_P2P_GPIO_Remote_Value_Changed:
        {
            if (!DaphnisI_P2P_GPIO_Remote_ParseValueChangedEvent(&eventText, &remoteValueChangedData))
            {
                currentState = DaphnisI_Remote_GPIO_SM_Error;
                break;
            }

            currentState = DaphnisI_Remote_GPIO_SM_Idle;
            break;
        }
        case DaphnisI_ATEvent_P2P_GPIO_Remote_Cfg_Set_Response:
        {
            if (!DaphnisI_P2P_GPIO_Remote_ParseCfgSetResponseEvent(&eventText, &remoteCfgSetResponseData))
            {
                currentState = DaphnisI_Remote_GPIO_SM_Error;
                break;
            }

            currentState = DaphnisI_Remote_GPIO_SM_Idle;
            break;
        }
        case DaphnisI_ATEvent_P2P_GPIO_Remote_Cfg_Get_Response:
        {
            if (!DaphnisI_P2P_GPIO_Remote_ParseCfgGetResponseEvent(&eventText, &remoteCfgGetResponseData))
            {
                currentState = DaphnisI_Remote_GPIO_SM_Error;
                break;
            }

            currentState = DaphnisI_Remote_GPIO_SM_Idle;
            break;
        }
        case DaphnisI_ATEvent_P2P_GPIO_Remote_Value_Set_Response:
        {
            if (!DaphnisI_P2P_GPIO_Remote_ParseValueSetResponseEvent(&eventText, &remoteValueSetResponseData))
            {
                currentState = DaphnisI_Remote_GPIO_SM_Error;
                break;
            }

            currentState = DaphnisI_Remote_GPIO_SM_Idle;
            break;
        }
        case DaphnisI_ATEvent_P2P_GPIO_Remote_Value_Get_Response:
        {
            if (!DaphnisI_P2P_GPIO_Remote_ParseValueGetResponseEvent(&eventText, &remoteValueGetResponseData))
            {
                currentState = DaphnisI_Remote_GPIO_SM_Error;
                break;
            }

            currentState = DaphnisI_Remote_GPIO_SM_Idle;
            break;
        }
        default:
            break;
    }
}

#endif
