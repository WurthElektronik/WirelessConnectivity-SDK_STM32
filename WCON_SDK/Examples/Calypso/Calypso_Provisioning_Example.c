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
 * @brief Calypso provisioning example.
 */
#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/Calypso_Device_Example.h>
#include <Calypso/Calypso_Examples.h>
#include <stdio.h>

/**
 * @brief Calypso provisioning example.
 */
void Calypso_Provisioning_Example()
{
    WE_APP_PRINT("*** Start of Calypso provisioning example ***\r\n");

    if (!Calypso_Init(&Calypso_uart, &Calypso_pins, &Calypso_Examples_EventCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    Calypso_PinReset();

    Calypso_Examples_WaitForStartup(10000);

    WE_Delay(1000);

    bool ret = false;

    /* Uncomment the following lines to activate factory reset (takes up to 90s) */
    //    ret = Calypso_ATDevice_FactoryReset();
    //    Calypso_Examples_Print("Factory reset", ret);
    //    /* Must wait for startup event before sending commands */
    //    ret = Calypso_Examples_WaitForStartup(10000);
    //    Calypso_Examples_Print("Wait for startup message", ret);
    /* Get version info. This retrieves Calypso's firmware version (amongst other version info) and
     * stores the firmware version in Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
     * Calypso_firmwareVersionPatch for later use. */
    Calypso_ATDevice_Value_t deviceValue;
    ret = Calypso_ATDevice_Get(Calypso_ATDevice_GetId_General, Calypso_ATDevice_GetGeneral_Version, &deviceValue);
    Calypso_Examples_Print("Get device version", ret);

    ret = Calypso_ATDevice_StartProvisioning();
    Calypso_Examples_Print("Start provisioning", ret);
}
