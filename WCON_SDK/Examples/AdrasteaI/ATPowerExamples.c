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
#include <AdrasteaI/ATCommands/ATDevice.h>
#include <AdrasteaI/ATCommands/ATPower.h>
#include <AdrasteaI/ATSIMExamples.h>
#include <AdrasteaI/AdrasteaI.h>
#include <AdrasteaI/AdrasteaI_Examples.h>
#include <stdio.h>

/**
 * @brief This example demonstrates the power mode options
 *
 */
void ATPowerExample()
{
    WE_DEBUG_PRINT("*** Start of Adrastea-I ATPower example ***\r\n");

    if (!AdrasteaI_Init(&AdrasteaI_uart, &AdrasteaI_pins, NULL))
    {
        WE_DEBUG_PRINT("Initialization error\r\n");
        return;
    }

    bool ret = AdrasteaI_ATDevice_SetPhoneFunctionality(AdrasteaI_ATDevice_Phone_Functionality_Min, AdrasteaI_ATDevice_Phone_Functionality_Reset_Invalid);
    AdrasteaI_ExamplesPrint("Set Phone Functionality", ret);

    ret = AdrasteaI_ATPower_SetPowerMode(AdrasteaI_ATPower_Mode_Stop, AdrasteaI_ATPower_Mode_Duration_Invalid);
    AdrasteaI_ExamplesPrint("Set Power Mode", ret);

    ret = AdrasteaI_ATPower_EnableSleep();
    AdrasteaI_ExamplesPrint("Enable Sleep", ret);
    WE_Delay(5000);

    ret = AdrasteaI_PinWakeUp();
    AdrasteaI_ExamplesPrint("Pin Wake Up", ret);

    WE_Delay(5000);
}
