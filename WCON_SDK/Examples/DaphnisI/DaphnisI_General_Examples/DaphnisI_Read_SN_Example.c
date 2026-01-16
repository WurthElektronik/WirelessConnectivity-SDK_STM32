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
 * @brief DaphnisI Read Serial Number example.
 *
 */
#include <DaphnisI/ATCommands/ATDevice.h>
#include <DaphnisI/DaphnisI.h>
#include <DaphnisI/DaphnisI_Examples.h>
#include <DaphnisI/DaphnisI_General_Examples/DaphnisI_Read_SN_Example.h>
#include <global/global.h>
#include <stdio.h>

#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)

/**
 * @brief In this example the serial number of the module is read back.
 */
void DaphnisI_Read_Serial_Number_Example()
{
    if (!DaphnisI_Init(&DaphnisI_uart, &DaphnisI_pins, NULL))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    DaphnisI_SerialNumber_t serial_number;

    if (!DaphnisI_GetSerialNumber(&serial_number))
    {
        WE_APP_PRINT("Failed to read back serial number\r\n");
        return;
    }

    WE_APP_PRINT("The read back serial number is ");
    DaphnisI_Print_Key_Addr(serial_number, DAPHNISI_ADDRESS_LENGTH);
    WE_APP_PRINT("\r\n");
}

#endif
