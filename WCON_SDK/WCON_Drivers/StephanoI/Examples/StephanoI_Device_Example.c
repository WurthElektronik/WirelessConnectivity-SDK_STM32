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
 * @brief StephanoI device example.
 */

#include <StephanoI/Examples/StephanoI_Examples.h>
#include <StephanoI/Examples/StephanoI_Device_Example.h>
#include <StephanoI/ATCommands/ATDevice.h>

/**
 * @brief Is called when an event notification has been received.
 *
 * Note that this function is called from an interrupt - code in this function
 * should thus be kept simple.
 *
 * Note in particular that it is not possible to send AT commands to StephanoI
 * from within this event handler.
 *
 * Also note that not all calls of this handler necessarily correspond to valid
 * events (i.e. events from StephanoI_ATEvent_t). Some events might in fact be responses
 * to AT commands that are not included in StephanoI_ATEvent_t.
 */
void StephanoI_Device_Examples_EventCallback(char *eventText)
{
	char *eventText_original = eventText;
	StephanoI_ATEvent_t event;
	if (false == StephanoI_ATEvent_ParseEventType(&eventText, &event))
	{
		return;
	}

	switch (event)
	{
	default:
		StephanoI_Examples_EventCallback(eventText_original);
		break;
	}
}

void StephanoI_ManufacturingUserPartitions_SubExample()
{
	bool ret = StephanoI_ATDevice_GetManufacturingUserPartitions();
	StephanoI_Examples_Print("Get manufacturing user partitions", ret);

#if 0
	char new_server_cert[] = "-----BEGIN CERTIFICATE-----\r\n\
MIIDLTCCAhWgAwIBAgIJAPgqhiqX1DNNMA0GCSqGSIb3DQEBCwUAMDcxCzAJBgNV\r\n\
BAYTAlMxMQ8wDQYDVQQKDAZFU1AgUzExFzAVBgNVBAMMDkVTUCBSb290IENBIFMx\r\n\
MB4XDTE5MDYyMTA4MDAyMloXDTI5MDYxODA4MDAyMlowNzELMAkGA1UEBhMCUzIx\r\n\
DzANBgNVBAoMBkVTUCBTMjEXMBUGA1UEAwwORVNQIFJvb3QgQ0EgUzIwggEiMA0G\r\n\
CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDJQ79YocJlOmZRPAYLSmzDRjl0wypl\r\n\
IlTUEP7zo1XhOx1DPC5vbTLMmMNEsUF8J3VG8WroYkKfwe26HBz4EfGMnDip+YOZ\r\n\
UqpOSJWDw39dbYtkB3qsSa6ZLCpA1up7GbZpmgriWwQqMo5gpRL0uZjT5l5GUMNb\r\n\
5ljiLkmiq230cTUsBfozOxScUBsNIQk0NsZiXRTAQZp8+dMaarX1ZmN55V3NGiiV\r\n\
U6RYkN6kBdrI0pZYUhOd3rPnx7C7B8n3oYUbppG2CNEP219jwY47SUnLIFPIE+ot\r\n\
2/itnIhQUcdVBBktaEkgER+WnveemEGSagOYwRyvHqogBUT/hzQvcI+/AgMBAAGj\r\n\
PDA6MAwGA1UdEwQFMAMBAf8wCwYDVR0PBAQDAgGmMB0GA1UdDgQWBBRFiQ1STxIw\r\n\
6gxMzTLdYdlDQW3uMDANBgkqhkiG9w0BAQsFAAOCAQEAG5BFCC7D3IrLvPCulVX3\r\n\
jkWq5OZIjxSWaYyOritPY/1oC4LY5WM4QornFt7GEt1d9wADYsmh39y/UPLuK0Cm\r\n\
+msEhxSJ0D7Wx05rbuIfpmpsZD/GBJEZlrykjZz77i1Y/jEVc7fGnA49KCWFx7Ix\r\n\
SSKpdoF5MT0PZeZUijVF28xR2GBRUlbQ6RtCMawsAO/Spq0WfUJHCxbi1C3khNKc\r\n\
H0ltyOHkox8HGQkJv6G610ig3cxKdbxD0JwR5uh7fOG41DwC4sN2y2FTvi+xurZB\r\n\
6ozGuZlMeFS7mFLqU5uQfi730FBJMA05DCpDhAvbFk8mzJdwBMWbqKHsGW0DlNXN\r\n\
zA==\r\n\
-----END CERTIFICATE-----\r\n";
	ret = StephanoI_ATDevice_EraseManufacturingUserPartitions("server_cert", "server_cert");
	StephanoI_Examples_Print("Erase server certificate", ret);
	ret = StephanoI_ATDevice_WriteManufacturingUserPartitions("server_cert", "server_cert", StephanoI_ATDevice_SYSMFG_data_binary, strlen(new_server_cert), (uint8_t*)new_server_cert);
	StephanoI_Examples_Print("Write server certificate", ret);
#endif

	StephanoI_ATDevice_SYSMFG_t manu;
	ret = StephanoI_ATDevice_ReadManufacturingUserPartitions("server_cert", "server_cert", 0, 0, &manu);
	StephanoI_Examples_Print("Read server cert", ret);
	if (manu.value != NULL)
	{
		free(manu.value);
	}
}

void StephanoI_Filesystem_SubExample()
{
	char *filename = "test3.txt";
	bool ret = StephanoI_ATDevice_FileSystemGetFiles();
	StephanoI_Examples_Print("Get FS files", ret);

	char *testdata = "Hello world";
	ret = StephanoI_ATDevice_FileSystemWrite(filename, 0, strlen(testdata), (uint8_t*) testdata);
	StephanoI_Examples_Print("Write FS file", ret);

	uint16_t size = 0;
	ret = StephanoI_ATDevice_FileSystemGetFileSize(filename, &size);
	StephanoI_Examples_Print("Get FS file size", ret);

	StephanoI_ATDevice_FileSystemRead_t read;
	ret = StephanoI_ATDevice_FileSystemRead(filename, 0, size, &read);
	StephanoI_Examples_Print("Read FS file", ret);
	if (read.value != NULL)
	{
		free(read.value);
	}

	ret = StephanoI_ATDevice_FileSystemDelete(filename);
	StephanoI_Examples_Print("Delete FS file", ret);
}

/**
 * @brief Basic device functionality example.
 */
void StephanoI_Device_Example()
{
	printf("*** Start of StephanoI ATDevice example ***\r\n");

	if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_Device_Examples_EventCallback))
	{
		printf("Initialization error\r\n");
		return;
	}

	bool ret = StephanoI_PinReset();
	StephanoI_Examples_Print("PinReset", ret);
	ret = StephanoI_Examples_WaitForStartup(5000);
	StephanoI_Examples_Print("Startup event", ret);

#if 0
    ret = StephanoI_ATDevice_Restore();
    StephanoI_Examples_Print("Restore", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
#endif

	ret = StephanoI_ATDevice_EchoOn(false);
	StephanoI_Examples_Print("Echo off", ret);

	ret = StephanoI_ATDevice_GetVersion();
	StephanoI_Examples_Print("Get version", ret);

	bool enable;
	ret = StephanoI_ATWifi_GetInit(&enable);
	StephanoI_Examples_Print("Read wifi driver state", ret);
	if (enable)
	{
		ret = StephanoI_ATWifi_Init(false);
		StephanoI_Examples_Print("Disable wifi", ret);
	}

	uint32_t time = 0;
	ret = StephanoI_ATDevice_SetSystemTimestamp(100);
	StephanoI_Examples_Print("Set time stamp", ret);
	ret = StephanoI_ATDevice_GetSystemTimestamp(&time);
	StephanoI_Examples_Print("Get time stamp", ret);
	printf("Time stamp = %d\r\n", (int) time);

	ret = StephanoI_ATDevice_SetTXPower(StephanoI_ATDevice_WifiPower_Plus20, StephanoI_ATDevice_BluetoothLEPower_None);
	StephanoI_Examples_Print("Set WiFi TX power", ret);

	StephanoI_ATDevice_WifiPower_t wifi_power;
	StephanoI_ATDevice_BluetoothLEPower_t ble_power;
	ret = StephanoI_ATDevice_GetTXPower(&wifi_power, &ble_power);
	StephanoI_Examples_Print("Get WiFi TX power", ret);

	StephanoI_ATDevice_UART_t uart;
	ret = StephanoI_ATDevice_GetDefaultUART(&uart);
	StephanoI_Examples_Print("Get default uart TX power", ret);
	printf("%d Baud\r\n", (int) uart.baudrate);

	float temp;
	ret = StephanoI_ATDevice_GetSystemTemp(&temp);
	StephanoI_Examples_Print("Get system temperature", ret);
	printf("%.2f C\r\n", temp);

	bool storemode;
	ret = StephanoI_ATDevice_GetSystemStoremode(&storemode);
	StephanoI_Examples_Print("Get system store mode", ret);
	printf("Storemode %s\r\n", storemode ? "enabled" : "disabled");

	//StephanoI_ManufacturingUserPartitions_SubExample();
	StephanoI_Filesystem_SubExample();

	ret = StephanoI_ATDevice_SetDeepSleep(ATDEVICE_DEEPSLEEP_TIME_MAX);
	StephanoI_Examples_Print("Deep sleep", ret);
	WE_Delay(500);

	StephanoI_Deinit();
}

/**
 * @brief Sleep example: this examples defines a wake-up pin, sets the module to sleep and temporarily wakes it up to run an action
 */
void StephanoI_DeviceSleep_Example()
{
	printf("*** Start of StephanoI ATDeviceSleep example ***\r\n");

	if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_Device_Examples_EventCallback))
	{
		printf("Initialization error\r\n");
		return;
	}

	bool ret = StephanoI_PinReset();
	StephanoI_Examples_Print("PinReset", ret);
	ret = StephanoI_Examples_WaitForStartup(5000);
	StephanoI_Examples_Print("Startup event", ret);

#if 0
    ret = StephanoI_ATDevice_Restore();
    StephanoI_Examples_Print("Restore", ret);
    ret = StephanoI_Examples_WaitForStartup(5000);
    StephanoI_Examples_Print("Startup event", ret);
#endif

	ret = StephanoI_ATDevice_EchoOn(false);
	StephanoI_Examples_Print("Echo off", ret);

	ret = StephanoI_ATDevice_GetVersion();
	StephanoI_Examples_Print("Get version", ret);

	uint32_t time = 0;
	ret = StephanoI_ATDevice_GetSystemTimestamp(&time);
	StephanoI_Examples_Print("Get time stamp", ret);
	printf("Time stamp = %d\r\n", (int) time);

	/* set wake-up pin to pin 10, active level to high and enable sleep mode */
	bool active_level = true;
	ret = StephanoI_ATDevice_SetWakeUpSource(StephanoI_ATDevice_WakeupPin_GPIO10, active_level);
	StephanoI_Examples_Print("Set wake-up source", ret);
	ret = StephanoI_SetWakeUpPin(active_level);
	StephanoI_Examples_Print("Set wake-up pin", ret);
	ret = StephanoI_ATDevice_SetSleep(StephanoI_ATDevice_SleepMode_Light);
	StephanoI_Examples_Print("Enable sleep mode", ret);

	for (uint8_t i = 0; i < 10; i++)
	{
		/* wake-up */
		ret = StephanoI_SetWakeUpPin(active_level);
		StephanoI_Examples_Print("Wake-up", ret);
		/* wait to wake-up */
		WE_Delay(1);

		/* do your stuff */
		ret = StephanoI_ATDevice_GetSystemTimestamp(&time);
		StephanoI_Examples_Print("Get time stamp", ret);
		printf("Time stamp = %d\r\n", (int) time);

		/* go to sleep */
		ret = StephanoI_SetWakeUpPin(!active_level);
		StephanoI_Examples_Print("Go to sleep", ret);

		WE_Delay(100);
	}

	/* wake-up and disable sleep */
	ret = StephanoI_SetWakeUpPin(active_level);
	StephanoI_Examples_Print("Wake-up", ret);
	ret = StephanoI_ATDevice_SetSleep(StephanoI_ATDevice_SleepMode_Disable);
	StephanoI_Examples_Print("Disable sleep mode", ret);

	ret = StephanoI_ATDevice_SetDeepSleep(ATDEVICE_DEEPSLEEP_TIME_MAX);
	StephanoI_Examples_Print("Deep sleep", ret);
	WE_Delay(500);

	StephanoI_Deinit();
}
