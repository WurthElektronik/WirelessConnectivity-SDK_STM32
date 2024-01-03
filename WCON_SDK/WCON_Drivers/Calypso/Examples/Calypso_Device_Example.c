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

/**
 * @file
 * @brief Calypso device example.
 */
#include <Calypso/Examples/Calypso_Device_Example.h>
#include <stdio.h>
#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/Examples/Calypso_Examples.h>

/**
 * @brief Basic device functionality example.
 */
void Calypso_Device_Example()
{
	printf("*** Start of Calypso ATDevice example ***\r\n");

	if (!Calypso_Init(&Calypso_uart, &Calypso_pins, &Calypso_Examples_EventCallback))
	{
		printf("Initialization error\r\n");
		return;
	}

	Calypso_PinReset();

	Calypso_Examples_WaitForStartup(5000);

	WE_Delay(1000);

	bool ret = false;

	/* Uncomment the following lines to activate factory reset (takes up to 90s) */
//        ret = Calypso_ATDevice_FactoryReset();
//        Calypso_Examples_Print("Factory reset", ret);
//        /* Must wait for startup event before sending commands */
//        Calypso_Examples_WaitForStartup(10000);
	ret = Calypso_ATDevice_Reboot();
	Calypso_Examples_Print("Reboot", ret);
	Calypso_Examples_WaitForStartup(5000);

	ret = Calypso_ATDevice_Stop(250);
	Calypso_Examples_Print("Stop NWP", ret);

	WE_Delay(500);

	ret = Calypso_ATDevice_Start();
	Calypso_Examples_Print("Start NWP", ret);

	WE_Delay(500);

	/* Get version info. This retrieves Calypso's firmware version (amongst other version info) and
	 * stores the firmware version in Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
	 * Calypso_firmwareVersionPatch for later use. */
	Calypso_ATDevice_Value_t deviceValue;
	ret = Calypso_ATDevice_Get(Calypso_ATDevice_GetId_General, Calypso_ATDevice_GetGeneral_Version, &deviceValue);
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

	ret = Calypso_ATDevice_Get(Calypso_ATDevice_GetId_UART, Calypso_ATDevice_GetUart_Baudrate, &deviceValue);
	Calypso_Examples_Print("Get UART baud rate", ret);
	if (ret)
	{
		printf("baudrate: %lu\r\n", deviceValue.uart.baudrate);
	}

	ret = Calypso_ATDevice_Get(Calypso_ATDevice_GetId_UART, Calypso_ATDevice_GetUart_Parity, &deviceValue);
	Calypso_Examples_Print("Get UART parity", ret);
	if (ret)
	{
		printf("parity: %u\r\n", deviceValue.uart.parity);
	}

	ret = Calypso_ATDevice_Get(Calypso_ATDevice_GetId_UART, Calypso_ATDevice_GetUart_FlowControl, &deviceValue);
	Calypso_Examples_Print("Get UART flow control", ret);
	if (ret)
	{
		printf("flow control: %s\r\n", deviceValue.uart.flowControl ? "on" : "off");
	}

	ret = Calypso_ATDevice_Get(Calypso_ATDevice_GetId_General, Calypso_ATDevice_GetGeneral_Time, &deviceValue);
	Calypso_Examples_Print("Get device time", ret);
	if (ret)
	{
		printf("date(dd:mm:yy): %u.%u.%u time(hh:mm:ss): %u:%u:%u \r\n", deviceValue.general.time.day, deviceValue.general.time.month, deviceValue.general.time.year, deviceValue.general.time.hour, deviceValue.general.time.minute, deviceValue.general.time.second);
	}

	WE_Delay(100);

	deviceValue.general.time.hour++;
	deviceValue.general.time.year = 2021;

	ret = Calypso_ATDevice_Set(Calypso_ATDevice_GetId_General, Calypso_ATDevice_GetGeneral_Time, &deviceValue);
	Calypso_Examples_Print("Set device time", ret);

	WE_Delay(100);

	ret = Calypso_ATDevice_Get(Calypso_ATDevice_GetId_General, Calypso_ATDevice_GetGeneral_Time, &deviceValue);
	Calypso_Examples_Print("Get device time", ret);

	ret = Calypso_ATDevice_Get(Calypso_ATDevice_GetId_IOT, Calypso_ATDevice_GetIot_UDID, &deviceValue);
	Calypso_Examples_Print("Get IoT UDID", ret);
	if (ret)
	{
		printf("IoT UDID: %s\r\n", deviceValue.iot.udid);
	}

	ret = Calypso_ATDevice_Sleep(2);
	Calypso_Examples_Print("Sleep", ret);

	WE_Delay(3000);

	Calypso_Deinit();
}
