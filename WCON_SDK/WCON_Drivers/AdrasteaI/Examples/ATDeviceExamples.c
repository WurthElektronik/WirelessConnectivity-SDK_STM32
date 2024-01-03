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
#include <AdrasteaI/Examples/ATDeviceExamples.h>
#include <AdrasteaI/ATCommands/ATDevice.h>
#include <AdrasteaI/AdrasteaI.h>
#include <AdrasteaI/Examples/AdrasteaI_Examples.h>

/**
 * @brief Running device specific commands (read out configuration values)
 *
 */
void ATDeviceExample()
{
	printf("*** Start of Adrastea-I ATDevice example ***\r\n");

	if (!AdrasteaI_Init(&AdrasteaI_uart, &AdrasteaI_pins, NULL))
	{
		printf("Initialization error\r\n");
		return;
	}

	WE_Delay(1000);

	AdrasteaI_ATDevice_Manufacturer_Identity_t manufacturerIdentity;

	bool ret = AdrasteaI_ATDevice_RequestManufacturerIdentity(&manufacturerIdentity);
	AdrasteaI_ExamplesPrint("Request Manufacturer Identity", ret);
	printf("Manufacturer Identity: %s\r\n", manufacturerIdentity);

	AdrasteaI_ATDevice_Model_Identity_t modelIdentity;
	ret = AdrasteaI_ATDevice_RequestModelIdentity(&modelIdentity);
	AdrasteaI_ExamplesPrint("Request Model Identity", ret);
	printf("Model Identity: %s\r\n", modelIdentity);

	AdrasteaI_ATDevice_Revision_Identity_t revisionIdentity;
	ret = AdrasteaI_ATDevice_RequestRevisionIdentity(&revisionIdentity);
	AdrasteaI_ExamplesPrint("Request Revision Identity", ret);
	printf("Revision Identity Major: %d Minor: %d \r\n", revisionIdentity.major, revisionIdentity.minor);

	AdrasteaI_ATDevice_IMEI_t imei;
	ret = AdrasteaI_ATDevice_RequestIMEI(&imei);
	AdrasteaI_ExamplesPrint("Request IMEI", ret);
	printf("IMEI: %s\r\n", imei);

	AdrasteaI_ATDevice_IMEISV_t imeisv;
	ret = AdrasteaI_ATDevice_RequestIMEISV(&imeisv);
	AdrasteaI_ExamplesPrint("Request IMEISV", ret);
	printf("IMEISV: %s\r\n", imeisv);

	AdrasteaI_ATDevice_SVN_t svn;
	ret = AdrasteaI_ATDevice_RequestSVN(&svn);
	AdrasteaI_ExamplesPrint("Request SVN", ret);
	printf("SVN: %s\r\n", svn);

	AdrasteaI_ATDevice_Serial_Number_t serialNumber;
	ret = AdrasteaI_ATDevice_RequestSerialNumber(&serialNumber);
	AdrasteaI_ExamplesPrint("Request Serial Number", ret);
	printf("Serial Number: %s\r\n", serialNumber);

	AdrasteaI_ATDevice_Character_Set_t charset;
	ret = AdrasteaI_ATDevice_GetTECharacterSet(&charset);
	AdrasteaI_ExamplesPrint("Request Charset", ret);
	printf("Charset: %d\r\n", charset);

	char capList[64];
	ret = AdrasteaI_ATDevice_GetCapabilitiesList(capList, sizeof(capList));
	AdrasteaI_ExamplesPrint("Request Manufacturer Identity", ret);
	printf("Manufacturer Identity: %s\r\n", capList);

	AdrasteaI_ATDevice_Phone_Functionality_t phoneFun;
	ret = AdrasteaI_ATDevice_GetPhoneFunctionality(&phoneFun);
	AdrasteaI_ExamplesPrint("Request Phone Functionality", ret);
	printf("Phone Functionality: %d\r\n", phoneFun);
}
