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
#include "ATDeviceExamples.h"
#include "../ATCommands/ATDevice.h"
#include <AdrasteaI/Adrastea.h>
#include "AdrasteaExamples.h"

void ATDeviceExample()
{

	if (!Adrastea_Init(115200, WE_FlowControl_NoFlowControl, WE_Parity_None, NULL, NULL))
	{
		return;
	}

	printf("*** Start of Adrastea ATDevice example ***\r\n");

	WE_Delay(1000);

	bool ret = false;

	ATDevice_Manufacturer_Identity_t manufacturerIdentity;

	ret = ATDevice_RequestManufacturerIdentity(&manufacturerIdentity);

	AdrasteaExamplesPrint("Request Manufacturer Identity", ret);

	printf("Manufacturer Identity: %s\r\n", manufacturerIdentity);

	ATDevice_Model_Identity_t modelIdentity;

	ret = ATDevice_RequestModelIdentity(&modelIdentity);

	AdrasteaExamplesPrint("Request Model Identity", ret);

	printf("Model Identity: %s\r\n", modelIdentity);

	ATDevice_Revision_Identity_t revisionIdentity;

	ret = ATDevice_RequestRevisionIdentity(&revisionIdentity);

	AdrasteaExamplesPrint("Request Revision Identity", ret);

	printf("Revision Identity Major: %d Minor: %d \r\n", revisionIdentity.major, revisionIdentity.minor);

	ATDevice_IMEI_t imei;

	ret = ATDevice_RequestIMEI(&imei);

	AdrasteaExamplesPrint("Request IMEI", ret);

	printf("IMEI: %s\r\n", imei);

	ATDevice_IMEISV_t imeisv;

	ret = ATDevice_RequestIMEISV(&imeisv);

	AdrasteaExamplesPrint("Request IMEISV", ret);

	printf("IMEISV: %s\r\n", imeisv);

	ATDevice_SVN_t svn;

	ret = ATDevice_RequestSVN(&svn);

	AdrasteaExamplesPrint("Request SVN", ret);

	printf("SVN: %s\r\n", svn);

	ATDevice_Serial_Number_t serialNumber;

	ret = ATDevice_RequestSerialNumber(&serialNumber);

	AdrasteaExamplesPrint("Request Serial Number", ret);

	printf("Serial Number: %s\r\n", serialNumber);

	ATDevice_Character_Set_t charset;

	ret = ATDevice_GetTECharacterSet(&charset);

	AdrasteaExamplesPrint("Request Charset", ret);

	printf("Charset: %d\r\n", charset);

	char capList[64];

	ret = ATDevice_GetCapabilitiesList(capList, 64);

	AdrasteaExamplesPrint("Request Manufacturer Identity", ret);

	printf("Manufacturer Identity: %s\r\n", capList);

	ATDevice_Phone_Functionality_t phoneFun;

	ret = ATDevice_GetPhoneFunctionality(&phoneFun);

	AdrasteaExamplesPrint("Request Phone Functionality", ret);

	printf("Phone Functionality: %d\r\n", phoneFun);
}
