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

#include <stdio.h>
#include <global/global.h>
#include <AdrasteaI/Examples/AdrasteaI_Examples.h>
#include <Calypso/Examples/Calypso_Examples.h>
#include <DaphnisI/DaphnisI_Examples.h>
#include <Metis/Metis_Examples.h>
#include <MetisE/MetisE_Examples.h>
#include <ProteusE/ProteusE_Examples.h>
#include <ProteusII/ProteusII_Examples.h>
#include <ProteusIII/ProteusIII_Examples.h>
#include <StephanoI/Examples/StephanoI_Examples.h>
#include <TarvosIII/TarvosIII_Examples.h>
#include <ThyoneE/ThyoneE_Examples.h>
#include <ThyoneI/ThyoneI_Examples.h>
#include <ThebeII/ThebeII_Examples.h>
#include <ThemistoI/ThemistoI_Examples.h>
#include <TelestoIII/TelestoIII_Examples.h>
#include <MoreExamples/MultiModule_ProteusIII_TarvosIII/MultiModule_ProteusIII_TarvosIII_Examples.h>

int main(void)
{
	/* Initialize platform (peripherals, flash interface, Systick, system clock) */
	WE_Platform_Init();

#ifdef WE_DEBUG
	WE_Debug_Init();
#endif

	uint8_t driverVersion[3];
	WE_GetDriverVersion(driverVersion);
	printf("Wuerth Elektronik eiSos Wireless Connectivity SDK version %d.%d.%d\r\n", driverVersion[0], driverVersion[1], driverVersion[2]);
	/* select the example to run */
	//AdrasteaI_Examples();
	//Calypso_Examples();
	//DaphnisI_Examples();
	//Metis_Examples();
	//MetisE_Examples();
	//ProteusE_Examples();
	//ProteusII_Examples();
	ProteusIII_Examples();
	//StephanoI_Examples();
	//ThyoneE_Examples();
	//ThyoneI_Examples();
	//TarvosIII_Examples();
	//TelestoIII_Examples();
	//ThebeII_Examples();
	//ThemistoI_Examples();
	//MultiModule_ProteusIII_TarvosIII_Examples();

	printf("*** End of execution ***\r\n");
	while (1)
	{
		WE_Delay(5000);
	}
	return 0;
}

