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
 * COPYRIGHT (c) 2022 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Calypso device example.
 */

#include "Calypso_Device_Example.h"

#include <stdio.h>

#include <Calypso/ATCommands/ATDevice.h>

#include "Calypso_Examples.h"

/**
 * @brief Basic device functionality example.
 */
void Calypso_Device_Example()
{
    printf("*** Start of Calypso ATDevice example ***\r\n");

    if (!Calypso_Init(Calypso_Examples_baudRate, Calypso_Examples_flowControl, Calypso_Examples_parity, &Calypso_Examples_EventCallback, NULL))
    {
        return;
    }

    Calypso_PinReset();

    Calypso_Examples_WaitForStartup(5000);

    WE_Delay(1000);

    bool ret = false;

    /* Uncomment the following lines to activate factory reset (takes up to 90s) */
//        ret = ATDevice_FactoryReset();
//        Calypso_Examples_Print("Factory reset", ret);
//        /* Must wait for startup event before sending commands */
//        Calypso_Examples_WaitForStartup(10000);


    ret = ATDevice_Reboot();
    Calypso_Examples_Print("Reboot", ret);
    Calypso_Examples_WaitForStartup(5000);


    ret = ATDevice_Stop(250);
    Calypso_Examples_Print("Stop NWP", ret);

    WE_Delay(500);

    ret = ATDevice_Start();
    Calypso_Examples_Print("Start NWP", ret);

    WE_Delay(500);

    /* Get version info. This retrieves Calypso's firmware version (amongst other version info) and
     * stores the firmware version in Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
     * Calypso_firmwareVersionPatch for later use. */
    ATDevice_Value_t deviceValue;
    ret = ATDevice_Get(ATDevice_GetId_General, ATDevice_GetGeneral_Version, &deviceValue);
    Calypso_Examples_Print("Get device version", ret);
    if (ret)
    {
        printf("Chip ID: %s\r\n", deviceValue.general.version.chipId);
        printf("MAC Version: %s\r\n", deviceValue.general.version.MACVersion);
        printf("NWP Version: %s\r\n", deviceValue.general.version.NWPVersion);
        printf("PHY Version: %s\r\n", deviceValue.general.version.PHYVersion);
        printf("ROM Version: %s\r\n", deviceValue.general.version.ROMVersion);
        printf("Calypso firmware version: %s\r\n", deviceValue.general.version.calypsoFirmwareVersion);
    }


    ret = ATDevice_Get(ATDevice_GetId_UART, ATDevice_GetUart_Baudrate, &deviceValue);
    Calypso_Examples_Print("Get UART baud rate", ret);
    if (ret)
    {
        printf("baudrate: %lu\r\n", deviceValue.uart.baudrate);
    }

    ret = ATDevice_Get(ATDevice_GetId_UART, ATDevice_GetUart_Parity, &deviceValue);
    Calypso_Examples_Print("Get UART parity", ret);
    if (ret)
    {
        printf("parity: %u\r\n", deviceValue.uart.parity);
    }

    ret = ATDevice_Get(ATDevice_GetId_UART, ATDevice_GetUart_FlowControl, &deviceValue);
    Calypso_Examples_Print("Get UART flow control", ret);
    if (ret)
    {
        printf("flow control: %s\r\n", deviceValue.uart.flowControl ? "on" : "off");
    }

    ret = ATDevice_Get(ATDevice_GetId_General, ATDevice_GetGeneral_Time, &deviceValue);
    Calypso_Examples_Print("Get device time", ret);
    if (ret)
    {
        printf("date(dd:mm:yy): %u.%u.%u time(hh:mm:ss): %u:%u:%u \r\n",
               deviceValue.general.time.day,
               deviceValue.general.time.month,
               deviceValue.general.time.year,
               deviceValue.general.time.hour,
               deviceValue.general.time.minute,
               deviceValue.general.time.second);
    }

    WE_Delay(100);

    deviceValue.general.time.hour++;
    deviceValue.general.time.year = 2021;

    ret = ATDevice_Set(ATDevice_GetId_General, ATDevice_GetGeneral_Time, &deviceValue);
    Calypso_Examples_Print("Set device time", ret);

    WE_Delay(100);

    ret = ATDevice_Get(ATDevice_GetId_General, ATDevice_GetGeneral_Time, &deviceValue);
    Calypso_Examples_Print("Get device time", ret);

    ret = ATDevice_Sleep(2);
    Calypso_Examples_Print("Sleep", ret);

    WE_Delay(3000);

    Calypso_Deinit();
}
