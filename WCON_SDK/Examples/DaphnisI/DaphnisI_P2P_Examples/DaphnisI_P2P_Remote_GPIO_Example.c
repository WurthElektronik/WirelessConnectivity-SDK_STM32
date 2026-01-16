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
 * @brief Daphnis-I P2P Remote GPIO example.
 *
 */
#include <DaphnisI/ATCommands/ATDevice.h>
#include <DaphnisI/ATCommands/ATGPIO.h>
#include <DaphnisI/ATCommands/P2P.h>
#include <DaphnisI/DaphnisI.h>
#include <DaphnisI/DaphnisI_Examples.h>
#include <DaphnisI/DaphnisI_P2P_Examples/DaphnisI_P2P_Examples_Helper.h>
#include <DaphnisI/DaphnisI_P2P_Examples/DaphnisI_P2P_Remote_GPIO_Example.h>
#include <global/global.h>
#include <stdio.h>

#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)

/* Set the role of the module whether it will send data or receive data */
#define MODULE_ROLE MODULE_ROLE_TX

#define RF_PROFILE 7

static DaphnisI_GPIOConfigBlock_t configBlocks[DaphnisI_GPIO_Count];

static void DaphnisI_Remote_GPIO_EventCallback(DaphnisI_ATEvent_t event, char* eventText);

/*
 * State machine for Daphnis-I sdk example.
 * */
typedef enum
{
    DaphnisI_Remote_GPIO_SM_Idle,
#if MODULE_ROLE == MODULE_ROLE_TX
    DaphnisI_Remote_GPIO_SM_Cfg_Get,
    DaphnisI_Remote_GPIO_SM_Cfg_Get_Response_Received,
    DaphnisI_Remote_GPIO_SM_Cfg_Set,
    DaphnisI_Remote_GPIO_SM_Cfg_Set_Response_Received,
    DaphnisI_Remote_GPIO_SM_Value_Set,
    DaphnisI_Remote_GPIO_SM_Value_Set_Response_Received,
#elif MODULE_ROLE == MODULE_ROLE_RX
    DaphnisI_Remote_GPIO_SM_Cfg_Changed,
    DaphnisI_Remote_GPIO_SM_Value_Changed,
#endif /* MODULE_ROLE == MODULE_ROLE_TX / MODULE_ROLE == MODULE_ROLE_RX */
} DaphnisI_Remote_GPIO_SM_t;

static volatile DaphnisI_Remote_GPIO_SM_t currentState = DaphnisI_Remote_GPIO_SM_Idle;

#if MODULE_ROLE == MODULE_ROLE_TX

#define DESTINATION_ADDR (DaphnisI_Device_Address_t){0x05, 0x2e, 0x7d, 0xb1}

static DaphnisI_GPIOStatusBlock_t statusBlocks[DaphnisI_GPIO_Count];

static DaphnisI_P2P_GPIO_RemoteCfgSetResponseData_t remoteCfgSetResponseData;
static DaphnisI_P2P_GPIO_RemoteCfgGetResponseData_t remoteCfgGetResponseData;
static DaphnisI_P2P_GPIO_RemoteValueSetResponseData_t remoteValueSetResponseData;

#elif MODULE_ROLE == MODULE_ROLE_RX

static DaphnisI_GPIOValueGetBlock_t valueGetBlocks[DaphnisI_GPIO_Count];

static DaphnisI_P2P_GPIO_RemoteCfgChangedData_t remoteCfgChangedData;
static DaphnisI_P2P_GPIO_RemoteValueChangedData_t remoteValueChangedData;

#endif /* MODULE_ROLE == MODULE_ROLE_TX / MODULE_ROLE == MODULE_ROLE_RX */

void DaphnisI_P2P_Remote_GPIO_Example()
{
    if (!DaphnisI_Init(&DaphnisI_uart, &DaphnisI_pins, DaphnisI_Remote_GPIO_EventCallback))
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

#if MODULE_ROLE == MODULE_ROLE_RX

    setting_status |= DaphnisI_P2P_GPIO_Remote_Config_Check_and_Set(true);

#endif /* MODULE_ROLE == MODULE_ROLE_RX */

    if (!DaphnisI_Apply_Settings(setting_status))
    {
        return;
    }

#if MODULE_ROLE == MODULE_ROLE_TX

    remoteCfgGetResponseData.configBlocksP = configBlocks;
    remoteCfgSetResponseData.statusBlocksP = statusBlocks;
    remoteValueSetResponseData.statusBlocksP = statusBlocks;

    DaphnisI_GPIOConfigBlock_t gpio0_config_block = {.GPIO_ID = DaphnisI_GPIO_0, .function = DaphnisI_GPIO_IO_Output, .value = {.input = DaphnisI_GPIO_Output_High}};
    DaphnisI_GPIOValueSetBlock_t gpio0_value_set_block = {.GPIO_ID = DaphnisI_GPIO_0, .value = DaphnisI_GPIO_Output_High};

    currentState = DaphnisI_Remote_GPIO_SM_Cfg_Get;

#elif MODULE_ROLE == MODULE_ROLE_RX

    remoteCfgChangedData.configBlocksP = configBlocks;
    remoteValueChangedData.valueGetBlocksP = valueGetBlocks;

    if (!DaphnisI_P2P_SetRXEnabled(true))
    {
        WE_APP_PRINT("Failed to turn on radio receiving.\r\n");
        return;
    }

#endif /* MODULE_ROLE == MODULE_ROLE_TX / MODULE_ROLE == MODULE_ROLE_RX */

    while (1)
    {
        switch (currentState)
        {
#if MODULE_ROLE == MODULE_ROLE_TX
            case DaphnisI_Remote_GPIO_SM_Cfg_Get:
            {
                currentState = DaphnisI_Remote_GPIO_SM_Idle;
                if (!DaphnisI_P2P_GPIO_Remote_Configuration_Get(DESTINATION_ADDR))
                {
                    WE_APP_PRINT("Failed to get GPIO Remote Configuration\r\n");
                    return;
                }
                break;
            }
            case DaphnisI_Remote_GPIO_SM_Cfg_Get_Response_Received:
            {
                if (remoteCfgGetResponseData.command_status != DaphnisI_GPIO_Command_Status_Success)
                {
                    WE_APP_PRINT("Failed to get GPIO Remote Configuration\r\n");
                    return;
                }

                if (!DaphnisI_Are_GPIO_Configurations_Valid(remoteCfgGetResponseData.configBlocksP, remoteCfgGetResponseData.configBlocksCount, &gpio0_config_block, 1))
                {
                    currentState = DaphnisI_Remote_GPIO_SM_Cfg_Set;
                    break;
                }

                currentState = DaphnisI_Remote_GPIO_SM_Value_Set;
                break;
            }
            case DaphnisI_Remote_GPIO_SM_Cfg_Set:
            {
                currentState = DaphnisI_Remote_GPIO_SM_Idle;
                if (!DaphnisI_P2P_GPIO_Remote_Configuration_Set(DESTINATION_ADDR, &gpio0_config_block, 1))
                {
                    WE_APP_PRINT("Failed to set GPIO Remote configuration\r\n");
                    return;
                }
                break;
            }
            case DaphnisI_Remote_GPIO_SM_Cfg_Set_Response_Received:
            {
                if (remoteCfgSetResponseData.command_status != DaphnisI_GPIO_Command_Status_Success)
                {
                    WE_APP_PRINT("Failed to set GPIO Remote Configuration\r\n");
                    return;
                }

                for (uint8_t i = 0; i < remoteCfgSetResponseData.statusBlocksCount; i++)
                {
                    if (remoteCfgSetResponseData.statusBlocksP[i].status != DaphnisI_GPIO_Block_Status_Success)
                    {
                        WE_APP_PRINT("Failed to set GPIO Remote Configuration\r\n");
                        return;
                    }
                }

                currentState = DaphnisI_Remote_GPIO_SM_Value_Set;
                break;
            }
            case DaphnisI_Remote_GPIO_SM_Value_Set:
            {
                currentState = DaphnisI_Remote_GPIO_SM_Idle;
                if (!DaphnisI_P2P_GPIO_Remote_Value_Set(DESTINATION_ADDR, &gpio0_value_set_block, 1))
                {
                    WE_APP_PRINT("Failed to set GPIO Remote configuration\r\n");
                    return;
                }
                break;
            }
            case DaphnisI_Remote_GPIO_SM_Value_Set_Response_Received:
            {
                if (remoteValueSetResponseData.command_status != DaphnisI_GPIO_Command_Status_Success)
                {
                    WE_APP_PRINT("Failed to set GPIO Remote Configuration\r\n");
                    return;
                }

                for (uint8_t i = 0; i < remoteValueSetResponseData.statusBlocksCount; i++)
                {
                    if (remoteValueSetResponseData.statusBlocksP[i].status != DaphnisI_GPIO_Block_Status_Success)
                    {
                        WE_APP_PRINT("Failed to set GPIO Remote Configuration\r\n");
                        return;
                    }
                }

                WE_APP_PRINT("GPIO ID: %d, Value set: %d\r\n", gpio0_value_set_block.GPIO_ID, gpio0_value_set_block.value);

                gpio0_value_set_block.value = (gpio0_value_set_block.value == DaphnisI_GPIO_Output_High) ? DaphnisI_GPIO_Output_Low : DaphnisI_GPIO_Output_High;

                WE_Delay(1000);

                currentState = DaphnisI_Remote_GPIO_SM_Value_Set;
                break;
            }
#elif MODULE_ROLE == MODULE_ROLE_RX
            case DaphnisI_Remote_GPIO_SM_Cfg_Changed:
            {
                for (uint8_t i = 0; i < remoteCfgChangedData.configBlocksCount; i++)
                {
                    WE_APP_PRINT("GPIO ID: %d, Function: ", remoteCfgChangedData.configBlocksP[i].GPIO_ID);
                    switch (remoteCfgChangedData.configBlocksP[i].function)
                    {

                        case DaphnisI_GPIO_IO_Disconnected:
                        {
                            WE_APP_PRINT("Disconnected\r\n");
                            break;
                        }
                        case DaphnisI_GPIO_IO_Input:
                        {
                            WE_APP_PRINT("Input, Value: %d\r\n", remoteCfgChangedData.configBlocksP[i].value.input);
                            break;
                        }
                        case DaphnisI_GPIO_IO_Output:
                        {
                            WE_APP_PRINT("Output, Value: %d\r\n", remoteCfgChangedData.configBlocksP[i].value.output);
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
                    WE_APP_PRINT("GPIO ID: %d, Value: %d\r\n", remoteValueChangedData.valueGetBlocksP[i].GPIO_ID, remoteValueChangedData.valueGetBlocksP[i].value);
                }
                currentState = DaphnisI_Remote_GPIO_SM_Idle;
                break;
            }
#endif /* MODULE_ROLE == MODULE_ROLE_TX / MODULE_ROLE == MODULE_ROLE_RX */
            default:
            case DaphnisI_Remote_GPIO_SM_Idle:
            {
                break;
            }
        }
    };
}

static void DaphnisI_Remote_GPIO_EventCallback(DaphnisI_ATEvent_t event, char* eventText)
{
    switch (event)
    {
#if MODULE_ROLE == MODULE_ROLE_TX
        case DaphnisI_ATEvent_P2P_GPIO_Remote_Cfg_Set_Response:
        {
            if (!DaphnisI_P2P_GPIO_Remote_ParseCfgSetResponseEvent(&eventText, &remoteCfgSetResponseData))
            {
                break;
            }

            currentState = DaphnisI_Remote_GPIO_SM_Cfg_Set_Response_Received;
            break;
        }
        case DaphnisI_ATEvent_P2P_GPIO_Remote_Cfg_Get_Response:
        {
            if (!DaphnisI_P2P_GPIO_Remote_ParseCfgGetResponseEvent(&eventText, &remoteCfgGetResponseData))
            {
                break;
            }

            currentState = DaphnisI_Remote_GPIO_SM_Cfg_Get_Response_Received;
            break;
        }
        case DaphnisI_ATEvent_P2P_GPIO_Remote_Value_Set_Response:
        {
            if (!DaphnisI_P2P_GPIO_Remote_ParseValueSetResponseEvent(&eventText, &remoteValueSetResponseData))
            {
                break;
            }

            currentState = DaphnisI_Remote_GPIO_SM_Value_Set_Response_Received;
            break;
        }
#elif MODULE_ROLE == MODULE_ROLE_RX
        case DaphnisI_ATEvent_P2P_GPIO_Remote_Cfg_Changed:
        {
            if (!DaphnisI_P2P_GPIO_Remote_ParseCfgChangedEvent(&eventText, &remoteCfgChangedData))
            {
                break;
            }

            currentState = DaphnisI_Remote_GPIO_SM_Cfg_Changed;
            break;
        }
        case DaphnisI_ATEvent_P2P_GPIO_Remote_Value_Changed:
        {
            if (!DaphnisI_P2P_GPIO_Remote_ParseValueChangedEvent(&eventText, &remoteValueChangedData))
            {
                break;
            }

            currentState = DaphnisI_Remote_GPIO_SM_Value_Changed;
            break;
        }
#endif /* MODULE_ROLE == MODULE_ROLE_TX*/
        default:
            break;
    }
}

#endif
