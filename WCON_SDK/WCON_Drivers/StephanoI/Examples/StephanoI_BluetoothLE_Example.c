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
 * @brief StephanoI Bluetooth LE example.
 */

#include <StephanoI/Examples/StephanoI_Examples.h>
#include <StephanoI/Examples/StephanoI_BluetoothLE_Example.h>
#include <StephanoI/ATCommands/ATBluetoothLE.h>
#include <StephanoI/ATCommands/ATDevice.h>

#define PROTEUS_MAX_MTU 247

/**
 * @brief Contains information on last discover service event (if any)
 */
StephanoI_ATBluetoothLE_Peripheral_DiscoverService_t StephanoI_Examples_Peripheral_DiscoverService = {
		0 };

/**
 * @brief Contains information on last discover characteristics event (if any)
 */
StephanoI_ATBluetoothLE_Peripheral_DiscoverCharacteristics_t StephanoI_Examples_Peripheral_DiscoverCharacteristics = { };

/**
 * @brief Contains information on last connection event (if any)
 */
StephanoI_ATBluetoothLE_Connection_t StephanoI_Examples_Connection = {
		.conn_index = BLE_CONNECTION_INDEX_INVALID,
		.channelopen = false,
		.secure = false, };

/**
 * @brief Contains information on last MTU event (if any)
 */
StephanoI_ATBluetoothLE_MTU_t StephanoI_Examples_MTU = {
		.conn_index = BLE_CONNECTION_INDEX_INVALID,
		.MTU = BLE_DEFAULT_MTU };

/**
 * @brief Contains information on last write event (if any)
 */
StephanoI_ATBluetoothLE_Write_t StephanoI_Examples_Write = {
		.conn_index = BLE_CONNECTION_INDEX_INVALID };

/**
 * @brief Contains information on last read event (if any)
 */
StephanoI_ATBluetoothLE_Read_t StephanoI_Examples_Read = {
		.conn_index = BLE_CONNECTION_INDEX_INVALID };

/**
 * @brief Contains information on last scan event (if any)
 */
StephanoI_ATBluetoothLE_Central_Scan_t StephanoI_Examples_Central_Scan = { };

/**
 * @brief Contains information on last discover primary service event (if any)
 */
StephanoI_ATBluetoothLE_Central_DiscoverPrimaryService_t StephanoI_Examples_Central_DiscoverPrimaryService = { };

/**
 * @brief Contains information on last discover included services event (if any)
 */
StephanoI_ATBluetoothLE_Central_DiscoverIncludedServices_t StephanoI_Examples_Central_DiscoverIncludedServices = { };

/**
 * @brief Contains information on last discover characteristics event (if any)
 */
StephanoI_ATBluetoothLE_Central_DiscoverCharacteristics_t StephanoI_Examples_Central_DiscoverCharacteristics = { };

/**
 * @brief Variable indicating, when advertising shall be restarted
 */
bool restart_advertising = false;

/**
 * @brief Variable indicating, when connection setup has been already triggered
 */
bool central_connection_setup_ongoing = false;

/**
 * @brief Variable indicating, when a encryption request has been received
 */
bool received_encryption_request = false;
StephanoI_ATBluetoothLE_EncryptionRequest_t StephanoI_Examples_EncryptionRequest = {
		.conn_index = BLE_CONNECTION_INDEX_INVALID };

bool received_securitykey_request = false;
StephanoI_ATBluetoothLE_SecurityKeyRequest_t StephanoI_Examples_SecurityKeyRequest = {
		.conn_index = BLE_CONNECTION_INDEX_INVALID };

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
void StephanoI_BluetoothLE_Examples_EventCallback(char *eventText)
{
	char *eventText_original = eventText;
	StephanoI_ATEvent_t event;
	if (false == StephanoI_ATEvent_ParseEventType(&eventText, &event))
	{
		return;
	}

	switch (event)
	{
	case StephanoI_ATEvent_BLE_BondingInformation:
	{
		StephanoI_ATBluetoothLE_Bonding_t bond;
		if (StephanoI_ATBluetoothLE_ParseBondingInformations(eventText, &bond))
		{
			printf("Bonding information event received.\r\n"
					"Bonding index: %d\r\n"
					"Mac: %s\r\n", bond.enc_dev_index, bond.mac);
		}
	}
		break;
	case StephanoI_ATEvent_BLE_Peripheral_DiscoverService:
	{
		if (StephanoI_ATBluetoothLE_ParsePeripheralDiscoverService(eventText, &StephanoI_Examples_Peripheral_DiscoverService))
		{
			printf("Peripheral Discover service event received.\r\n"
					"Service index: %d\r\n"
					"UUID: %s\r\n"
					"Service type: %d\r\n", StephanoI_Examples_Peripheral_DiscoverService.srv_index, StephanoI_Examples_Peripheral_DiscoverService.srv_uuid, StephanoI_Examples_Peripheral_DiscoverService.srv_type);
		}
	}
		break;
	case StephanoI_ATEvent_BLE_Peripheral_DiscoverCharacteristics:
	{
		if (StephanoI_ATBluetoothLE_ParsePeripheralDiscoverCharacteristics(eventText, &StephanoI_Examples_Peripheral_DiscoverCharacteristics))
		{
			printf("Characteristics event received.\r\n"
					"Type: %s\r\n"
					"Service index: %d\r\n"
					"Char. index: %d\r\n"
					"Desc. index: %d\r\n", StephanoI_Examples_Peripheral_DiscoverCharacteristics.characteristics_type, StephanoI_Examples_Peripheral_DiscoverCharacteristics.srv_index, StephanoI_Examples_Peripheral_DiscoverCharacteristics.char_index, StephanoI_Examples_Peripheral_DiscoverCharacteristics.desc_index);
		}
	}
		break;
	case StephanoI_ATEvent_BLE_Connection:
	{
		if (StephanoI_ATBluetoothLE_ParseConnection(eventText, &StephanoI_Examples_Connection))
		{
			printf("Connection event received.\r\n"
					"Connection index: %d\r\n"
					"Remote address: %s\r\n", StephanoI_Examples_Connection.conn_index, StephanoI_Examples_Connection.remote_address);

			if (0 == strcmp(StephanoI_Examples_Connection.remote_address, "-1"))
			{
				/* connection setup failed */
				StephanoI_Examples_Connection.conn_index = BLE_CONNECTION_INDEX_INVALID;
			}
			StephanoI_Examples_Connection.channelopen = false;
			StephanoI_Examples_Connection.secure = false;
			central_connection_setup_ongoing = false;

			/* reset values as new connection is open */
			StephanoI_Examples_MTU.conn_index = StephanoI_Examples_Connection.conn_index;
			StephanoI_Examples_MTU.MTU = BLE_DEFAULT_MTU;
		}
		else
		{
			/* interpretation error */
			StephanoI_Examples_Connection.conn_index = BLE_CONNECTION_INDEX_INVALID;
		}
	}
		break;
	case StephanoI_ATEvent_BLE_Disconnection:
	{
		if (StephanoI_ATBluetoothLE_ParseConnection(eventText, &StephanoI_Examples_Connection))
		{
			printf("Disconnection event received.\r\n"
					"Connection index: %d\r\n"
					"Remote address: %s\r\n", StephanoI_Examples_Connection.conn_index, StephanoI_Examples_Connection.remote_address);
			StephanoI_Examples_Connection.conn_index = BLE_CONNECTION_INDEX_INVALID;
			StephanoI_Examples_MTU.conn_index = StephanoI_Examples_Connection.conn_index;
			StephanoI_Examples_Connection.channelopen = false;
			StephanoI_Examples_Connection.secure = false;
		}
		restart_advertising = true;
	}
		break;
	case StephanoI_ATEvent_BLE_AuthenticationComplete:
	{
		StephanoI_ATBluetoothLE_AuthenticationComplete_t t;
		if (StephanoI_ATBluetoothLE_ParseAuthenticationComplete(eventText, &t))
		{
			printf("Authentication complete event received.\r\n"
					"Connection index: %d\r\n"
					"%s\r\n", t.conn_index, t.success ? "Success" : "Failed");

			StephanoI_Examples_Connection.secure = t.success;
		}
	}
		break;
	case StephanoI_ATEvent_BLE_MTU:
	{
		if (StephanoI_ATBluetoothLE_ParseMTU(eventText, &StephanoI_Examples_MTU))
		{
			printf("MTU event received.\r\n"
					"Connection index: %d\r\n"
					"MTU: %d\r\n", StephanoI_Examples_MTU.conn_index, StephanoI_Examples_MTU.MTU);
		}
	}
		break;
	case StephanoI_ATEvent_BLE_Write:
	{
		if (StephanoI_ATBluetoothLE_ParseWrite(eventText, &StephanoI_Examples_Write))
		{
			printf("Write event received.\r\n"
					"Connection index: %d\r\n"
					"Service index: %d\r\n"
					"Characteristic index: %d\r\n"
					"Descriptor: %d\r\n"
					"Length: %d\r\n", StephanoI_Examples_Write.conn_index, StephanoI_Examples_Write.srv_index, StephanoI_Examples_Write.char_index, StephanoI_Examples_Write.desc, StephanoI_Examples_Write.length);

			if ((StephanoI_Examples_Write.srv_index == 1) && (StephanoI_Examples_Write.char_index == 2) && (StephanoI_Examples_Write.desc == 1) && (StephanoI_Examples_Write.length == 2))
			{
				/* TXCHAR descriptor of the WE SPP-like profile has been written  */

				/* check for notification enable bit */
				StephanoI_Examples_Connection.channelopen = ((StephanoI_Examples_Write.data[0] & 0x01) == 0x01);
			}
			else if ((StephanoI_Examples_Write.srv_index == 1) && (StephanoI_Examples_Write.char_index == 1) && (StephanoI_Examples_Write.desc == 0) && (StephanoI_Examples_Write.length > 0))
			{
				/* RXCHAR of the WE SPP-like profile has been written */
				printf("Data received: 0x");
				for (uint16_t i = 0; i < StephanoI_Examples_Write.length; i++)
				{
					printf("%02X", StephanoI_Examples_Write.data[i]);
				}
				printf("\r\n");
			}
		}
	}
		break;
	case StephanoI_ATEvent_BLE_Read:
	{
		if (StephanoI_ATBluetoothLE_ParseRead(eventText, &StephanoI_Examples_Read))
		{
			printf("Write event received.\r\n"
					"Connection index: %d\r\n"
					"Length: %d\r\n", StephanoI_Examples_Read.conn_index, StephanoI_Examples_Read.length);

			printf("Data read: 0x");
			for (uint16_t i = 0; i < StephanoI_Examples_Read.length; i++)
			{
				printf("%02X", StephanoI_Examples_Read.data[i]);
			}
			printf("\r\n");
		}
	}
		break;
	case StephanoI_ATEvent_BLE_Central_Scan:
	{
		if (StephanoI_ATBluetoothLE_ParseScan(eventText, &StephanoI_Examples_Central_Scan))
		{
			printf("Scan event received.\r\n"
					"Remote address: %s\r\n"
					"RSSI: %d\r\n", StephanoI_Examples_Central_Scan.remote_address, StephanoI_Examples_Central_Scan.rssi);
		}
	}
		break;
	case StephanoI_ATEvent_BLE_Central_DiscoverPrimaryService:
	{
		if (StephanoI_ATBluetoothLE_ParseCentralDiscoverPrimaryService(eventText, &StephanoI_Examples_Central_DiscoverPrimaryService))
		{
			printf("Primary service event received.\r\n"
					"Connection index: %d\r\n"
					"Service index: %d\r\n"
					"Service UUID: %s\r\n"
					"Service type: %d\r\n", StephanoI_Examples_Central_DiscoverPrimaryService.conn_index, StephanoI_Examples_Central_DiscoverPrimaryService.srv_index, StephanoI_Examples_Central_DiscoverPrimaryService.srv_uuid, StephanoI_Examples_Central_DiscoverPrimaryService.srv_type);
		}
	}
		break;
	case StephanoI_ATEvent_BLE_Central_DiscoverIncludedServices:
	{
		if (StephanoI_ATBluetoothLE_ParseCentralDiscoverIncludedServices(eventText, &StephanoI_Examples_Central_DiscoverIncludedServices))
		{
			printf("Included service event received.\r\n"
					"Connection index: %d\r\n"
					"Service index: %d\r\n"
					"Service UUID: %s\r\n"
					"Service type: %d\r\n"
					"Included service UUID: %s\r\n"
					"Included service type: %d\r\n", StephanoI_Examples_Central_DiscoverIncludedServices.conn_index, StephanoI_Examples_Central_DiscoverIncludedServices.srv_index, StephanoI_Examples_Central_DiscoverIncludedServices.srv_uuid, StephanoI_Examples_Central_DiscoverIncludedServices.srv_type, StephanoI_Examples_Central_DiscoverIncludedServices.included_srv_uuid, StephanoI_Examples_Central_DiscoverIncludedServices.included_srv_type);
		}
	}
		break;
	case StephanoI_ATEvent_BLE_Central_DiscoverCharacteristics:
	{
		if (StephanoI_ATBluetoothLE_ParseCentralDiscoverCharacteristics(eventText, &StephanoI_Examples_Central_DiscoverCharacteristics))
		{
			printf("Characteristics event received.\r\n"
					"Type: %s\r\n"
					"Connection index: %d\r\n"
					"Service index: %d\r\n"
					"Char. index: %d\r\n"
					"Char. UUID: %s\r\n"
					"Desc. index: %d\r\n"
					"Desc. UUID: %s\r\n", StephanoI_Examples_Central_DiscoverCharacteristics.characteristics_type, StephanoI_Examples_Central_DiscoverCharacteristics.conn_index, StephanoI_Examples_Central_DiscoverCharacteristics.srv_index, StephanoI_Examples_Central_DiscoverCharacteristics.char_index, StephanoI_Examples_Central_DiscoverCharacteristics.char_uuid, StephanoI_Examples_Central_DiscoverCharacteristics.desc_index, StephanoI_Examples_Central_DiscoverCharacteristics.desc_uuid);
		}
	}
		break;
	case StephanoI_ATEvent_BLE_SetPhy:
	{
	}
		break;
	case StephanoI_ATEvent_BLE_ReceivedEncryptionRequest:
	{
		if (StephanoI_ATBluetoothLE_ParseEncryptionRequest(eventText, &StephanoI_Examples_EncryptionRequest))
		{
			received_encryption_request = true;
		}
	}
		break;
	case StephanoI_ATEvent_BLE_SecurityKeyRequest:
	{
		if (StephanoI_ATBluetoothLE_ParseSecurityKeyRequest(eventText, &StephanoI_Examples_SecurityKeyRequest))
		{
			received_securitykey_request = true;
		}
	}
		break;
	case StephanoI_ATEvent_BLE_NotifySecurityKey:
	{
		StephanoI_ATBluetoothLE_NotifyKey_t key;
		if (StephanoI_ATBluetoothLE_ParseNotifyKey(eventText, &key))
		{
			printf("Key event received.\r\n"
					"Connection index: %d\r\n"
					"Key: %s\r\n", key.conn_index, key.key);
		}
	}
		break;
	default:
	{
		StephanoI_Examples_EventCallback(eventText_original);
	}
		break;
	}
}
/**
 * @brief Bluetooth LE central data functionality example.
 */
void StephanoI_BluetoothLE_Central_ThroughputData_Example()
{
	printf("*** Start of StephanoI Bluetooth LE data throughput example ***\r\n");
	char target_remote_address[BLE_MAC_STRINGLEN] = "84:f7:03:a2:23:f6";

	/* generate data for later data transmission */
	uint8_t data[1024];
	for (uint16_t i = 0; i < sizeof(data); i++)
	{
		data[i] = (0x30) + (i % 10);
	}
	/* First byte must be 0x01 to transmit data to a Proteus device,
	 * For any other device there is no restriction */
	data[0] = 0x01;

	bool ret;
#if 1
	/* run the test */
#pragma message("Radio module must be configured to 3MBaud with flow control active, first")
	StephanoI_uart.baudrate = 3000000;
	StephanoI_uart.flowControl = WE_FlowControl_RTSAndCTS;
	if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_BluetoothLE_Examples_EventCallback))
	{
		printf("Initialization error\r\n");
		return;
	}
	ret = StephanoI_PinReset();
	StephanoI_Examples_Print("PinReset", ret);
	ret = StephanoI_Examples_WaitForStartup(5000);
	StephanoI_Examples_Print("Startup event", ret);
	ret = StephanoI_ATDevice_EchoOn(false);
	StephanoI_Examples_Print("Echo off", ret);
#elif 0
    /* restore the UART baudrate from 3 MBaud */
#pragma message("Radio module must be configured to 3MBaud with flow control active, first")
	StephanoI_uart.baudrate = 3000000;
	StephanoI_uart.flowControl = WE_FlowControl_RTSAndCTS;
	if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_BluetoothLE_Examples_EventCallback))
	{
		printf("Initialization error\r\n");
		return;
	}
	ret = StephanoI_ATDevice_Restore();
	StephanoI_Examples_Print("Restore", ret);
	ret = StephanoI_PinReset();
	StephanoI_Examples_Print("Startup event", ret);
	ret = StephanoI_ATDevice_EchoOn(false);
	StephanoI_Examples_Print("Echo off", ret);
    return;
#elif 0
    /* set the UART baudrate to 3 MBaud */
	if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins,  &StephanoI_BluetoothLE_Examples_EventCallback))
	{
		printf("Initialization error\r\n");
		return;
	}
	ret = StephanoI_PinReset();
	StephanoI_Examples_Print("PinReset", ret);
	ret = StephanoI_Examples_WaitForStartup(5000);
	StephanoI_Examples_Print("Startup event", ret);
	ret = StephanoI_ATDevice_EchoOn(false);
	StephanoI_Examples_Print("Echo off", ret);

	StephanoI_ATDevice_UART_t uart_def = {.baudrate = 3000000, .flow_control = StephanoI_ATDevice_UartFlowControl_RTSnCTS, .parity = StephanoI_ATDevice_UartParity_None};
    ret = StephanoI_ATDevice_SetDefaultUART(uart_def);
    StephanoI_Examples_Print("Set UART baud rate", ret);
    return;
#endif

	ret = StephanoI_ATWifi_SetMode(StephanoI_ATWifiMode_Disabled, false);
	StephanoI_Examples_Print("Wifi off", ret);

	ret = StephanoI_ATWifi_Init(false);
	StephanoI_Examples_Print("Wifi deinit", ret);

	ret = StephanoI_ATBluetoothLE_Init(StephanoI_ATBluetoothLE_InitType_Client);
	StephanoI_Examples_Print("Bluetooth LE Init", ret);

	ret = StephanoI_ATBluetoothLE_CentralScan(true, 1, StephanoI_ATBluetoothLE_ScanFilter_MAC, target_remote_address);
	StephanoI_Examples_Print("Scan start", ret);
	WE_Delay(2500);

	while (1)
	{
		if (StephanoI_Examples_Connection.conn_index == BLE_CONNECTION_INDEX_INVALID)
		{
			/* no connection open */
			StephanoI_Examples_Print("Waiting for connection setup", true);
			ret = StephanoI_ATBluetoothLE_CentralConnect(0, target_remote_address, true, 3);
			StephanoI_Examples_Print("Connection setup", ret);
			WE_Delay(3000);
		}
		else if (StephanoI_Examples_Connection.channelopen == false)
		{
			/* no channel open -> thus configure connection and open channel */
			StephanoI_Examples_Print("Waiting for channel open", true);

			StephanoI_ATBluetoothLE_Central_ConnectionParameters_t conn_params = {
					.conn_index = StephanoI_Examples_Connection.conn_index,
					.min_interval = 6,
					.max_interval = 6,
					.latency = 0,
					.timeout = 400 };
			ret = StephanoI_ATBluetoothLE_CentralSetConnectionParameters(conn_params);
			StephanoI_Examples_Print("Set connection parameters", ret);
			WE_Delay(250);

			StephanoI_ATBluetoothLE_MTU_t mtu = {
					.conn_index = StephanoI_Examples_Connection.conn_index,
					.MTU = 512 };
			ret = StephanoI_ATBluetoothLE_CentralSetMTU(mtu);
			StephanoI_Examples_Print("Set MTU", ret);
			WE_Delay(1000);
			ret = StephanoI_ATBluetoothLE_SetConnectionPhy(StephanoI_Examples_Connection.conn_index, StephanoI_ATBluetoothLE_Phy_2Mbit);
			StephanoI_Examples_Print("Set Phy", ret);
			WE_Delay(1000);

			ret = StephanoI_ATBluetoothLE_CentralDiscoverPrimaryService(StephanoI_Examples_Connection.conn_index);
			StephanoI_Examples_Print("Discover primary service", ret);
			WE_Delay(500);
			ret = StephanoI_ATBluetoothLE_CentralDiscoverIncludedServices(StephanoI_Examples_Connection.conn_index, StephanoI_Examples_Central_DiscoverPrimaryService.srv_index);
			StephanoI_Examples_Print("Discover included services", ret);
			ret = StephanoI_ATBluetoothLE_CentralDiscoverCharacteristics(StephanoI_Examples_Connection.conn_index, StephanoI_Examples_Central_DiscoverPrimaryService.srv_index);
			StephanoI_Examples_Print("Discover characteristics", ret);
			WE_Delay(250);

			/* enable notification -> send 0x0001 to the descriptor of the char_index 2, which is RX_CHARACTERISTICS of WE SPP-like profile */
			uint8_t enable[2] = {
					0x01,
					0x00 };
			ret = StephanoI_ATBluetoothLE_CentralWrite(StephanoI_Examples_Connection.conn_index, StephanoI_Examples_Central_DiscoverPrimaryService.srv_index, 2, 1, enable, 2);
			StephanoI_Examples_Print("Enable notifications", ret);

			StephanoI_Examples_Connection.channelopen = true;
		}
		else
		{
			/* channel open -> transmit data to char_index 1, which is RX_CHARACTERISTICS of WE SPP-like profile */
			ret = StephanoI_ATBluetoothLE_CentralWrite(StephanoI_Examples_Connection.conn_index, StephanoI_Examples_Central_DiscoverPrimaryService.srv_index, 1, -1, data, StephanoI_Examples_MTU.MTU - 3);
			StephanoI_Examples_Print("Transmit data", ret);
		}
	}
}

/**
 * @brief In this example the Stephano-I is configured as Bluetooth LE central and connects to a device that supports WE SPP-like profile (like Proteus or Stephano-I).
 * It waits for the complete connection setup and starts transmission of data.
 */
void StephanoI_BluetoothLE_Central_Data_Example()
{
	printf("*** Start of StephanoI Bluetooth LE data transmission (Stephano-I Central -> Bluetooth LE) example ***\r\n");
	char target_remote_address[BLE_MAC_STRINGLEN] = "00:18:DA:00:10:88";
	char static_passkey[] = "123123"; //default static passkey of Proteus-III

	/* generate data for later data transmission */
	uint8_t data[1024];
	for (uint16_t i = 0; i < sizeof(data); i++)
	{
		data[i] = (0x30) + (i % 10);
	}
	/* First byte must be 0x01 to transmit data to a Proteus device,
	 * For any other device there is no restriction */
	data[0] = 0x01;

	if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_BluetoothLE_Examples_EventCallback))
	{
		printf("Initialization error\r\n");
		return;
	}

	bool ret = StephanoI_PinReset();
	StephanoI_Examples_Print("PinReset", ret);
	ret = StephanoI_Examples_WaitForStartup(5000);
	StephanoI_Examples_Print("Startup event", ret);
	ret = StephanoI_ATDevice_EchoOn(false);
	StephanoI_Examples_Print("Echo off", ret);

#if 1
	ret = StephanoI_ATDevice_Restore();
	StephanoI_Examples_Print("Restore", ret);
	ret = StephanoI_Examples_WaitForStartup(5000);
	StephanoI_Examples_Print("Startup event", ret);
	ret = StephanoI_ATDevice_EchoOn(false);
	StephanoI_Examples_Print("Echo off", ret);
#endif

	ret = StephanoI_ATWifi_SetMode(StephanoI_ATWifiMode_Disabled, false);
	StephanoI_Examples_Print("Wifi off", ret);

	ret = StephanoI_ATWifi_Init(false);
	StephanoI_Examples_Print("Wifi deinit", ret);

	ret = StephanoI_ATBluetoothLE_Init(StephanoI_ATBluetoothLE_InitType_Client);
	StephanoI_Examples_Print("Bluetooth LE Init", ret);

#if false
	StephanoI_ATBluetoothLE_ScanParameters_t params;
	ret = StephanoI_ATBluetoothLE_GetScanParameters(&params);
	StephanoI_Examples_Print("Bluetooth LE get scan params", ret);
	ret = StephanoI_ATBluetoothLE_SetScanParameters(params);
	StephanoI_Examples_Print("Bluetooth LE set scan params", ret);
	WE_Delay(500);
#endif

#define nENABLE_CENTRAL_SECURITY
#ifdef ENABLE_CENTRAL_SECURITY
	/* enable security */
	ret = StephanoI_ATBluetoothLE_BondsDelete(-1);
	StephanoI_Examples_Print("Bluetooth LE Delete all bonds", ret);
	WE_Delay(1000);

	StephanoI_ATBluetoothLE_SecurityParameters_t sec_params;
	sec_params.io = StephanoI_ATBluetoothLE_IOCapabilities_KeyboardOnly;
	sec_params.auth = StephanoI_ATBluetoothLE_AuthenticationOptions_SC_MITM_BOND;
	sec_params.auth_option = false;
	sec_params.key_size = 16;
	sec_params.init_key = 3;
	sec_params.rsp_key = 3;
	ret = StephanoI_ATBluetoothLE_SetSecurityParameters(sec_params);
	StephanoI_Examples_Print("Bluetooth LE Set security parameters", ret);
#endif

	ret = StephanoI_ATBluetoothLE_CentralScan(true, 1, StephanoI_ATBluetoothLE_ScanFilter_MAC, target_remote_address);
	StephanoI_Examples_Print("Scan start", ret);
	WE_Delay(2500);

	while (1)
	{
		if (received_encryption_request == true)
		{
			ret = StephanoI_ATBluetoothLE_RespondPairingRequest(StephanoI_Examples_EncryptionRequest.conn_index, true);
			StephanoI_Examples_Print("Accept pairing request", ret);
			received_encryption_request = false;
		}
		else if (received_securitykey_request == true)
		{
			ret = StephanoI_ATBluetoothLE_ReplyKey(StephanoI_Examples_SecurityKeyRequest.conn_index, static_passkey);
			StephanoI_Examples_Print("Sending key", ret);
			received_securitykey_request = false;
			WE_Delay(10);
		}

		if (central_connection_setup_ongoing == true)
		{
			WE_Delay(10);
		}
		else if (StephanoI_Examples_Connection.conn_index == BLE_CONNECTION_INDEX_INVALID)
		{
			/* no connection open -> open connection */
			StephanoI_Examples_Print("Waiting for connection setup", true);

			central_connection_setup_ongoing = true;
			ret = StephanoI_ATBluetoothLE_CentralConnect(0, target_remote_address, true, 3);
			StephanoI_Examples_Print("Connection setup", ret);
		}
#ifdef ENABLE_CENTRAL_SECURITY
		else if(StephanoI_Examples_Connection.secure == false)
		{
			WE_Delay(10);
		}
#endif
		else if (StephanoI_Examples_Connection.channelopen == false)
		{
			/* no channel open -> thus configure connection and open channel */
			StephanoI_Examples_Print("Waiting for channel open", true);

			StephanoI_ATBluetoothLE_Central_ConnectionParameters_t conn_params;
			uint16_t curr_interval;
			ret = StephanoI_ATBluetoothLE_CentralGetConnectionParameters(&conn_params, &curr_interval);
			StephanoI_Examples_Print("Get connection parameters", ret);

			StephanoI_ATBluetoothLE_MTU_t mtu = {
					.conn_index = StephanoI_Examples_Connection.conn_index,
					.MTU = PROTEUS_MAX_MTU };
			ret = StephanoI_ATBluetoothLE_CentralSetMTU(mtu);
			StephanoI_Examples_Print("Set MTU", ret);
			ret = StephanoI_ATBluetoothLE_SetConnectionPhy(StephanoI_Examples_Connection.conn_index, StephanoI_ATBluetoothLE_Phy_2Mbit);
			StephanoI_Examples_Print("Set Phy", ret);

			ret = StephanoI_ATBluetoothLE_CentralDiscoverPrimaryService(StephanoI_Examples_Connection.conn_index);
			StephanoI_Examples_Print("Discover primary service", ret);
			WE_Delay(500);
			ret = StephanoI_ATBluetoothLE_CentralDiscoverIncludedServices(StephanoI_Examples_Connection.conn_index, StephanoI_Examples_Central_DiscoverPrimaryService.srv_index);
			StephanoI_Examples_Print("Discover included services", ret);
			ret = StephanoI_ATBluetoothLE_CentralDiscoverCharacteristics(StephanoI_Examples_Connection.conn_index, StephanoI_Examples_Central_DiscoverPrimaryService.srv_index);
			StephanoI_Examples_Print("Discover characteristics", ret);
			WE_Delay(250);

			/* enable notification -> send 0x0001 to the descriptor of the char_index 2, which is RX_CHARACTERISTICS of WE SPP-like profile */
			uint8_t enable[2] = {
					0x01,
					0x00 };
			ret = StephanoI_ATBluetoothLE_CentralWrite(StephanoI_Examples_Connection.conn_index, StephanoI_Examples_Central_DiscoverPrimaryService.srv_index, 2, 1, enable, sizeof(enable));
			StephanoI_Examples_Print("Enable notifications", ret);

			StephanoI_Examples_Connection.channelopen = true;
		}
		else
		{
			data[1]++;
			if (data[1] > '9')
			{
				data[1] = '0';
			}

			/* channel open -> transmit data to char_index 1, which is RX_CHARACTERISTICS of WE SPP-like profile */
			ret = StephanoI_ATBluetoothLE_CentralWrite(StephanoI_Examples_Connection.conn_index, StephanoI_Examples_Central_DiscoverPrimaryService.srv_index, 1, -1, data, StephanoI_Examples_MTU.MTU - 3);
			StephanoI_Examples_Print("Transmit data", ret);
			WE_Delay(250);
		}
	}
}

/**
 * @brief In this example the Stephano-I is configured as Bluetooth LE peripheral and starts advertising. It waits for the complete
 * connection setup and starts transmission of data.
 */
void StephanoI_BluetoothLE_Peripheral_Data_Example()
{
	printf("*** Start of StephanoI Bluetooth LE data transmission (Bluetooth LE -> Stephano-I peripheral) example ***\r\n");

	/* generate data for later data transmission */
	uint8_t data[1024];
	for (uint16_t i = 0; i < sizeof(data); i++)
	{
		data[i] = (0x30) + (i % 10);
	}
	/* First byte must be 0x01 to transmit data to a Proteus device,
	 * For any other device there is no restriction */
	data[0] = 0x01;

	if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_BluetoothLE_Examples_EventCallback))
	{
		printf("Initialization error\r\n");
		return;
	}

	bool ret = StephanoI_PinReset();
	StephanoI_Examples_Print("PinReset", ret);
	ret = StephanoI_Examples_WaitForStartup(5000);
	StephanoI_Examples_Print("Startup event", ret);
	ret = StephanoI_ATDevice_EchoOn(false);
	StephanoI_Examples_Print("Echo off", ret);

#if 1
	ret = StephanoI_ATDevice_Restore();
	StephanoI_Examples_Print("Restore", ret);
	ret = StephanoI_Examples_WaitForStartup(5000);
	StephanoI_Examples_Print("Startup event", ret);
	ret = StephanoI_ATDevice_EchoOn(false);
	StephanoI_Examples_Print("Echo off", ret);
#endif

	ret = StephanoI_ATWifi_SetMode(StephanoI_ATWifiMode_Disabled, false);
	StephanoI_Examples_Print("Wifi off", ret);

	ret = StephanoI_ATWifi_Init(false);
	StephanoI_Examples_Print("Wifi deinit", ret);

	ret = StephanoI_ATBluetoothLE_Init(StephanoI_ATBluetoothLE_InitType_Server);
	StephanoI_Examples_Print("Bluetooth LE Init", ret);

	StephanoI_ATBluetoothLE_SecurityParameters_t sec_params;
	ret = StephanoI_ATBluetoothLE_GetSecurityParameters(&sec_params);
	StephanoI_Examples_Print("Bluetooth LE Get security parameters", ret);
	ret = StephanoI_ATBluetoothLE_BondsGet();
	StephanoI_Examples_Print("Bluetooth LE GetBonds", ret);

#define nENABLE_PERIPHERAL_SECURITY
#if ENABLE_PERIPHERAL_SECURITY
	ret = StephanoI_ATBluetoothLE_BondsDelete(-1);
	StephanoI_Examples_Print("Bluetooth LE Delete all bonds", ret);
	WE_Delay(1000);

	sec_params.io = StephanoI_ATBluetoothLE_IOCapabilities_DisplayOnly;
	sec_params.auth = StephanoI_ATBluetoothLE_AuthenticationOptions_SC_MITM_BOND;
	sec_params.auth_option = true;
	sec_params.key_size = 16;
	sec_params.init_key = 3;
	sec_params.rsp_key = 3;
	ret = StephanoI_ATBluetoothLE_SetSecurityParameters(sec_params);
	StephanoI_Examples_Print("Bluetooth LE Set security parameters", ret);

#if 0
	StephanoI_ATBluetoothLE_Key_t key = "123123";
	ret = StephanoI_ATBluetoothLE_SetStaticKey(key);
	StephanoI_Examples_Print("Bluetooth LE set static key", ret);
	ret = StephanoI_ATBluetoothLE_GetStaticKey(&key);
	StephanoI_Examples_Print("Bluetooth LE get static key", ret);
#endif
#endif

	ret = StephanoI_ATBluetoothLE_Peripheral_ServiceCreate();
	StephanoI_Examples_Print("Bluetooth LE Create Service", ret);

	ret = StephanoI_ATBluetoothLE_Peripheral_ServiceStart(-1);
	StephanoI_Examples_Print("Bluetooth LE Start Service", ret);

	ret = StephanoI_ATBluetoothLE_Peripheral_DiscoverServices();
	StephanoI_Examples_Print("Bluetooth LE Discover own services", ret);

	ret = StephanoI_ATBluetoothLE_StartAdvertising();
	StephanoI_Examples_Print("Bluetooth LE StartAdvertising", ret);

	/* Advertise connectable with device name "Stephano-I" */
	StephanoI_ATBluetoothLE_AdvertisingRawData_t adv = "0201060B095374657068616E6F2D49";
	ret = StephanoI_ATBluetoothLE_SetAdvertisingRawData(adv);
	StephanoI_Examples_Print("Bluetooth LE SetAdvertisingRawData", ret);

	/* Transmit data as long as connection is open */
	while (1)
	{
		if (restart_advertising == true)
		{
			ret = StephanoI_ATBluetoothLE_StartAdvertising();
			StephanoI_Examples_Print("Bluetooth LE StartAdvertising", ret);
			restart_advertising = !ret;
		}
		if (received_encryption_request == true)
		{
			ret = StephanoI_ATBluetoothLE_RespondPairingRequest(StephanoI_Examples_EncryptionRequest.conn_index, true);
			StephanoI_Examples_Print("Accept pairing request", ret);
			received_encryption_request = false;
		}

		if (StephanoI_Examples_Connection.conn_index == BLE_CONNECTION_INDEX_INVALID)
		{
			/* no connection open */
			StephanoI_Examples_Print("Waiting for connection request", true);
			WE_Delay(1000);
		}
		else if (StephanoI_Examples_Connection.channelopen == false)
		{
			/* no channel open */
			StephanoI_Examples_Print("Waiting for channel open", true);
#if 0
			StephanoI_ATBluetoothLE_ReadPhy_t phy;
			ret = StephanoI_ATBluetoothLE_GetConnectionPhy(StephanoI_Examples_Connection.conn_index, &phy);
			StephanoI_Examples_Print("Get Phy", ret);
#endif

			WE_Delay(1000);
		}
		else
		{
			data[1]++;
			if (data[1] > '9')
			{
				data[1] = '0';
			}

			/* channel open -> transmit data to srv_index 1 and char_index 2, which is TX_CHARACTERISTICS of WE SPP-like profile */
			ret = StephanoI_ATBluetoothLE_PeripheralNotifyClient(StephanoI_Examples_Connection.conn_index, 1, 2, data, StephanoI_Examples_MTU.MTU - 3);
			StephanoI_Examples_Print("Bluetooth LE Notify Client", ret);
			WE_Delay(250);
		}
	}
}

/**
 * @brief In this example the Stephano-I is configured as Bluetooth LE peripheral and starts advertising
 */
void StephanoI_BluetoothLE_Peripheral_Advertising_Example()
{
	printf("*** Start of StephanoI Bluetooth LE advertising example ***\r\n");

	if (!StephanoI_Init(&StephanoI_uart, &StephanoI_pins, &StephanoI_BluetoothLE_Examples_EventCallback))
	{
		printf("Initialization error\r\n");
		return;
	}

	bool ret = StephanoI_PinReset();
	StephanoI_Examples_Print("PinReset", ret);
	ret = StephanoI_Examples_WaitForStartup(5000);
	StephanoI_Examples_Print("Startup event", ret);
	ret = StephanoI_ATDevice_EchoOn(false);
	StephanoI_Examples_Print("Echo off", ret);

#if 1
	ret = StephanoI_ATDevice_Restore();
	StephanoI_Examples_Print("Restore", ret);
	ret = StephanoI_Examples_WaitForStartup(5000);
	StephanoI_Examples_Print("Startup event", ret);
	ret = StephanoI_ATDevice_EchoOn(false);
	StephanoI_Examples_Print("Echo off", ret);
#endif

	ret = StephanoI_ATWifi_SetMode(StephanoI_ATWifiMode_Disabled, false);
	StephanoI_Examples_Print("Wifi off", ret);

	ret = StephanoI_ATWifi_Init(false);
	StephanoI_Examples_Print("Wifi deinit", ret);

	ret = StephanoI_ATBluetoothLE_Init(StephanoI_ATBluetoothLE_InitType_Server);
	StephanoI_Examples_Print("Bluetooth LE Init", ret);

	StephanoI_ATBluetoothLE_DeviceName_t name;
	ret = StephanoI_ATBluetoothLE_GetDeviceName(&name);
	StephanoI_Examples_Print("Bluetooth LE get device name", ret);

	StephanoI_ATBluetoothLE_DeviceID_t ID;
	ret = StephanoI_ATBluetoothLE_GetDeviceID(&ID);
	StephanoI_Examples_Print("Bluetooth LE get device ID", ret);

	StephanoI_ATBluetoothLE_AdvertisingParameters_t adv_params = {
			.min_interval = 50,
			.max_interval = 50,
			.type = StephanoI_ATBluetoothLE_AdvertisingType_DIRECT_IND_LOW,
			.addr_type = StephanoI_ATBluetoothLE_Address_Public,
			.channel = StephanoI_ATBluetoothLE_ChannelMap_ALL,
			.filter = StephanoI_ATBluetoothLE_AdvertisingFilterPolicy_SCAN_ANY_CON_WLST };
	ret = StephanoI_ATBluetoothLE_SetAdvertisingParameters(adv_params);
	StephanoI_Examples_Print("Bluetooth LE SetAdvertisingParameters", ret);
	ret = StephanoI_ATBluetoothLE_GetAdvertisingParameters(&adv_params);
	StephanoI_Examples_Print("Bluetooth LE GetAdvertisingParameters", ret);

	StephanoI_ATBluetoothLE_AdvertisingData_t adv_data = {
			.devname = "Stephano",
			.uuid = BLE_WE_SPP_PRIMARYSERVICE_UUID,
			.manufacturerdata = "00",
			.include_power = false };
	ret = StephanoI_ATBluetoothLE_SetAdvertisingData(adv_data);
	StephanoI_Examples_Print("Bluetooth LE SetAdvertisingData", ret);

	ret = StephanoI_ATBluetoothLE_StartAdvertising();
	StephanoI_Examples_Print("Bluetooth LE StartAdvertising", ret);

	ret = StephanoI_ATBluetoothLE_GetAdvertisingData(&adv_data);
	StephanoI_Examples_Print("Bluetooth LE GetAdvertisingData", ret);

	while (1)
	{
		WE_Delay(5000);
	}
}
