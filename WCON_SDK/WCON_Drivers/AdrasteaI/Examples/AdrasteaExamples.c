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
#include "AdrasteaExamples.h"
#include "ATDeviceExamples.h"
#include "ATSocketExamples.h"
#include "ATNetServiceExamples.h"
#include "ATSIMExamples.h"
#include "ATProprietaryExamples.h"
#include "ATPacketDomainExamples.h"
#include "ATGNSSExamples.h"
#include "ATMQTTExamples.h"
#include "ATHTTPExamples.h"
#include "ATSMSExamples.h"
#include "ATPowerExamples.h"
#include <global/global.h>

/**
 * @brief Runs Adrastea examples.
 *
 * Comment/uncomment lines in this function to start the desired example.
 */
void AdrasteaExamples()
{

	/* Initialize platform (peripherals, flash interface, Systick, system clock) */
	WE_Platform_Init();

#ifdef WE_DEBUG
	WE_Debug_Init();
#endif

	uint8_t version[3];
	WE_GetDriverVersion(version);
	printf("Wuerth Elektronik eiSos Wireless Connectivity SDK version %d.%d.%d\r\n", version[0], version[1], version[2]);

//    ATDeviceExample();
//    ATGNSSExample();
//    ATHTTPExample();
//    ATMQTTExample();
//    ATNetServiceExample();
//    ATPacketDomainExample();
//    ATPowerExample();
//    ATProprietaryExample();
//    ATSIMExample();
//    ATSMSExample();
	ATSocketExample();

	printf("*** End of example ***\r\n");

	while (1)
	{
		WE_Delay(500);
	}

}

/**
 * @brief Prints the supplied string, prefixed with OK or NOK (depending on the success parameter).
 *
 * @param str String to print
 * @param success Variable indicating if action was ok
 */
void AdrasteaExamplesPrint(char *str, bool success)
{
	printf("%s%s\r\n", success ? "OK    " : "NOK   ", str);
}
