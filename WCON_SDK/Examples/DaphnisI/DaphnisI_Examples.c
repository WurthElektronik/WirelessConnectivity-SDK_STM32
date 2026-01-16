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
#include <DaphnisI/DaphnisI_Examples.h>
#include <DaphnisI/DaphnisI_General_Examples/DaphnisI_Local_GPIO_Example.h>
#include <DaphnisI/DaphnisI_General_Examples/DaphnisI_Read_SN_Example.h>
#include <DaphnisI/DaphnisI_LoRaWAN_Examples/DaphnisI_LoRaWAN_Example.h>
#include <DaphnisI/DaphnisI_P2P_Examples/DaphnisI_P2P_Data_Transmission.h>
#include <DaphnisI/DaphnisI_P2P_Examples/DaphnisI_P2P_Remote_GPIO_Example.h>
#include <DaphnisI/DaphnisI_P2P_Examples/DaphnisI_P2P_Throughput_Test.h>
#include <global/global.h>
#include <global_platform.h>
#include <stdio.h>
/**
 * @brief Definition of the control pins
 */
DaphnisI_Pins_t DaphnisI_pins = {
    .DaphnisI_Pin_Reset = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_10)),
    .DaphnisI_Pin_WakeUp = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_5)),
    .DaphnisI_Pin_Boot = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_7)),
};

/**
 * @brief Definition of the uart
 */
WE_UART_t DaphnisI_uart;

/**
 * @brief The application's main function.
 */
void DaphnisI_Examples(void)
{

    DaphnisI_uart.baudrate = DAPHNISI_DEFAULT_BAUDRATE;
    DaphnisI_uart.flowControl = WE_FlowControl_NoFlowControl;
    DaphnisI_uart.parity = WE_Parity_None;
    DaphnisI_uart.uartInit = WE_UART1_Init;
    DaphnisI_uart.uartDeinit = WE_UART1_DeInit;
    DaphnisI_uart.uartTransmit = WE_UART1_Transmit;

    //    DaphnisI_Read_Serial_Number_Example();
    //    DaphnisI_Local_GPIO_Example();
    DaphnisI_OTAA_JoinExample();
    //    DaphnisI_P2P_Data_Transmission();
    //    DaphnisI_P2P_Remote_GPIO_Example();
    //    DaphnisI_P2P_Throughput_Test();
}

void DaphnisI_Print_Key_Addr(uint8_t* key_addr, uint8_t key_addr_length)
{
    if (key_addr == NULL)
    {
        return;
    }

    for (uint8_t i = key_addr_length - 1; i > 0; i--)
    {
        WE_APP_PRINT("%02X:", key_addr[i]);
    }
    WE_APP_PRINT("%02X", key_addr[0]);
}

void DaphnisI_Print_Payload(uint8_t* payload, uint8_t payload_length)
{
    if (payload == NULL)
    {
        return;
    }

    WE_APP_PRINT("0x");
    for (uint8_t i = 0; i < payload_length; i++)
    {
        WE_APP_PRINT("%02x", *(payload + i));
    }
    WE_APP_PRINT(" (");
    for (uint8_t i = 0; i < payload_length; i++)
    {
        WE_APP_PRINT("%c", *(payload + i));
    }
    WE_APP_PRINT(")\r\n");
}

bool DaphnisI_Apply_Settings(DaphnisI_Setting_Status_t settings_statuses)
{
    if ((settings_statuses & DaphnisI_Setting_Status_Failure) == DaphnisI_Setting_Status_Failure)
    {
        WE_APP_PRINT("One of the User/Runtime Settings couldn't be read or set.\r\n");
        return false;
    }

    if ((settings_statuses & DaphnisI_Setting_Status_Modified) == DaphnisI_Setting_Status_Modified)
    {
        if (!DaphnisI_PinReset())
        {
            WE_APP_PRINT("Failed to reset module.\r\n");
            return false;
        }
    }

    return true;
}

#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)

DaphnisI_Setting_Status_t DaphnisI_Mode_Check_and_Set(DaphnisI_Mode_t new_mode)
{
    DaphnisI_Mode_t current_mode;

    if (!DaphnisI_GetModeRS(&current_mode))
    {
        return DaphnisI_Setting_Status_Failure;
    }

    if (current_mode == new_mode)
    {
        return DaphnisI_Setting_Status_Unmodified;
    }

    if (!DaphnisI_SetModeUS(new_mode))
    {
        return DaphnisI_Setting_Status_Failure;
    }

    return DaphnisI_Setting_Status_Modified;
}

bool DaphnisI_Are_GPIO_Configurations_Valid(DaphnisI_GPIOConfigBlock_t* current_configurations, uint8_t current_configurations_count, DaphnisI_GPIOConfigBlock_t* new_configurations, uint8_t new_configurations_count)
{
    for (uint8_t i = 0; i < current_configurations_count; i++)
    {
        for (uint8_t j = 0; j < new_configurations_count; j++)
        {
            if (current_configurations[i].GPIO_ID != new_configurations[j].GPIO_ID)
            {
                continue;
            }

            if (current_configurations[i].function != new_configurations[j].function)
            {
                return false;
            }

            switch (current_configurations[i].function)
            {
                case DaphnisI_GPIO_IO_Disconnected:
                {
                    continue;
                    break;
                }
                case DaphnisI_GPIO_IO_Input:
                {
                    if (current_configurations[i].value.input != new_configurations[j].value.input)
                    {
                        return false;
                    }
                    continue;
                    break;
                }
                case DaphnisI_GPIO_IO_Output:
                {
                    if (current_configurations[i].value.output != new_configurations[j].value.output)
                    {
                        return false;
                    }
                    continue;
                    break;
                }
                default:
                    break;
            }

            return false;
        }
    }

    return true;
}

#endif /* DAPHNISI_MIN_FW_VER >= FW(1, 4, 0) */
