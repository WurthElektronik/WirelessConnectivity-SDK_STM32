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
 * COPYRIGHT (c) 2023 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

#include "stdio.h"
#include "ATSIMExamples.h"
#include "../ATCommands/ATPower.h"
#include "../ATCommands/ATDevice.h"
#include <AdrasteaI/Adrastea.h>
#include <stdio.h>
#include "AdrasteaExamples.h"

void ATPowerExample()
{

	if (!Adrastea_Init(115200, WE_FlowControl_NoFlowControl, WE_Parity_None, NULL, NULL))
	{
		return;
	}

	printf("*** Start of Adrastea ATPower example ***\r\n");

	WE_Delay(1000);

	bool ret = false;

	ret = ATDevice_SetPhoneFunctionality(ATDevice_Phone_Functionality_Min, ATDevice_Phone_Functionality_Reset_Invalid);

	AdrasteaExamplesPrint("Set Phone Functionality", ret);

	ret = ATPower_SetPowerMode(ATPower_Mode_Stop, ATPower_Mode_Duration_Invalid);

	AdrasteaExamplesPrint("Set Power Mode", ret);

	ret = ATPower_EnableSleep();

	AdrasteaExamplesPrint("Enable Sleep", ret);

	WE_Delay(5000);

	ret = Adrastea_PinWakeUp();

	AdrasteaExamplesPrint("Pin Wake Up", ret);

	WE_Delay(5000);

}

