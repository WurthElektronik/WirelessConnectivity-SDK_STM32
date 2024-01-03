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
 * @brief Proteus-II example.
 */

#include <ProteusII/ProteusII.h>
#include <global/global.h>
#include <stdio.h>
#include <string.h>

/* Proteus-II examples. Pick the example to be executed in the main function. */
static void CommandModeExample();
static void PeripheralOnlyModeExample();
static void DTMExample();

/* Callback functions for various indications sent by the Proteus-II. */
static void RxCallback(uint8_t *payload, uint16_t payloadLength, uint8_t *btMac, int8_t rssi);
static void BeaconRxCallback(uint8_t *payload, uint16_t payloadLength, uint8_t *btMac, int8_t rssi);
static void ConnectCallback(bool success, uint8_t *btMac);
static void SecurityCallback(uint8_t *btMac, ProteusII_SecurityState_t securityState);
static void PasskeyCallback(uint8_t *btMac);
static void DisplayPasskeyCallback(ProteusII_DisplayPasskeyAction_t action, uint8_t *btMac, uint8_t *passkey);
static void DisconnectCallback(ProteusII_DisconnectReason_t reason);
static void ChannelOpenCallback(uint8_t *btMac, uint16_t maxPayload);
static void PhyUpdateCallback(bool success, uint8_t *btMac, uint8_t phyRx, uint8_t phyTx);
static void SleepCallback();
static void RssiCallback(uint8_t *btMac, int8_t rssi, int8_t txPower);
static void ErrorCallback(uint8_t errorCode);
static void OnPeripheralOnlyModeByteReceived(uint8_t *dataP, size_t size);

/* Is set to true when a passkey indication has been received. */
static bool passkeyRequestReceived = false;

/* Is set to true when a display passkey indication has been received. */
static bool displayPasskeyRequestReceived = false;

/**
 * @brief Definition of the pins
 */
static ProteusII_Pins_t ProteusII_pins;

/**
 * @brief Definition of the uart
 */
static WE_UART_t ProteusII_uart;

/**
 * @brief The application's main function.
 */
void ProteusII_Examples(void)
{
	ProteusII_pins.ProteusII_Pin_Reset.port = (void*) GPIOA;
	ProteusII_pins.ProteusII_Pin_Reset.pin = GPIO_PIN_10;
	ProteusII_pins.ProteusII_Pin_SleepWakeUp.port = (void*) GPIOA;
	ProteusII_pins.ProteusII_Pin_SleepWakeUp.pin = GPIO_PIN_9;
	ProteusII_pins.ProteusII_Pin_Boot.port = (void*) GPIOA;
	ProteusII_pins.ProteusII_Pin_Boot.pin = GPIO_PIN_7;
	ProteusII_pins.ProteusII_Pin_Mode.port = (void*) GPIOA;
	ProteusII_pins.ProteusII_Pin_Mode.pin = GPIO_PIN_8;
	ProteusII_pins.ProteusII_Pin_Busy.port = (void*) GPIOB;
	ProteusII_pins.ProteusII_Pin_Busy.pin = GPIO_PIN_8;
	ProteusII_pins.ProteusII_Pin_StatusLed2.port = (void*) GPIOB;
	ProteusII_pins.ProteusII_Pin_StatusLed2.pin = GPIO_PIN_9;

	ProteusII_uart.baudrate = PROTEUSII_DEFAULT_BAUDRATE;
	ProteusII_uart.flowControl = WE_FlowControl_NoFlowControl;
	ProteusII_uart.parity = WE_Parity_None;
	ProteusII_uart.uartInit = WE_UART1_Init;
	ProteusII_uart.uartDeinit = WE_UART1_DeInit;
	ProteusII_uart.uartTransmit = WE_UART1_Transmit;

	CommandModeExample();
//    PeripheralOnlyModeExample();
//    DTMExample();

	return;
}

/**
 * @brief Prints the supplied string, prefixed with OK or NOK (depending on the success parameter).
 *
 * @param str String to print
 * @param success Variable indicating if action was ok
 */
static void Examples_Print(char *str, bool success)
{
	printf("%s%s\r\n", success ? "OK    " : "NOK   ", str);
}

/**
 * @brief Proteus-II command mode example.
 *
 * Connects to Proteus-II in command mode registering callbacks for all
 * available indications, then prints info when receiving indications.
 */
static void CommandModeExample()
{
	bool ret = false;

	ProteusII_CallbackConfig_t callbackConfig = {
			0 };
	callbackConfig.rxCb = RxCallback;
	callbackConfig.beaconRxCb = BeaconRxCallback;
	callbackConfig.connectCb = ConnectCallback;
	callbackConfig.disconnectCb = DisconnectCallback;
	callbackConfig.channelOpenCb = ChannelOpenCallback;
	callbackConfig.securityCb = SecurityCallback;
	callbackConfig.passkeyCb = PasskeyCallback;
	callbackConfig.displayPasskeyCb = DisplayPasskeyCallback;
	callbackConfig.phyUpdateCb = PhyUpdateCallback;
	callbackConfig.sleepCb = SleepCallback;
	callbackConfig.rssiCb = RssiCallback;
	callbackConfig.errorCb = ErrorCallback;

	if (false == ProteusII_Init(&ProteusII_uart, &ProteusII_pins, ProteusII_OperationMode_CommandMode, callbackConfig))
	{
		printf("Initialization error\r\n");
		return;
	}

	ProteusII_DeviceInfo_t deviceInfo;
	if (ProteusII_GetDeviceInfo(&deviceInfo))
	{
		printf("Device info OS version = 0x%04x, "
				"build code = 0x%08lx, "
				"package variant = 0x%04x, "
				"chip ID = 0x%08lx\r\n", deviceInfo.osVersion, deviceInfo.buildCode, deviceInfo.packageVariant, deviceInfo.chipId);
	}

	uint8_t fwVersion[3];
	ret = ProteusII_GetFWVersion(fwVersion);
	Examples_Print("Get firmware version", ret);
	printf("Firmware version is %u.%u.%u\r\n", fwVersion[2], fwVersion[1], fwVersion[0]);
	WE_Delay(500);

	uint8_t mac[8];
	ret = ProteusII_GetMAC(mac);
	Examples_Print("Get MAC", ret);
	printf("MAC is 0x%02x%02x%02x%02x%02x%02x%02x%02x\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], mac[6], mac[7]);
	WE_Delay(500);

	uint8_t btMac[6];
	ret = ProteusII_GetBTMAC(btMac);
	Examples_Print("Get BT MAC", ret);
	printf("BTMAC is 0x%02x%02x%02x%02x%02x%02x\r\n", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
	WE_Delay(500);

	uint8_t serialNr[3];
	ret = ProteusII_GetSerialNumber(serialNr);
	Examples_Print("Get serial number", ret);
	printf("Serial number is 0x%02x%02x%02x\r\n", serialNr[2], serialNr[1], serialNr[0]);
	WE_Delay(500);

	ProteusII_TXPower_t txPower;
	ret = ProteusII_GetTXPower(&txPower);
	Examples_Print("Get TX power", ret);
	printf("TX power index is %d\r\n", txPower);
	WE_Delay(500);

	ret = ProteusII_ScanStart();
	Examples_Print("Scan start", ret);
	WE_Delay(2500);

	ret = ProteusII_ScanStop();
	Examples_Print("Scan stop", ret);

	ProteusII_GetDevices_t devices;
	ret = ProteusII_GetDevices(&devices);
	Examples_Print("Get devices", ret);
	printf("%d device(s) found\r\n", devices.numberOfDevices);
	for (uint8_t i = 0; i < devices.numberOfDevices; i++)
	{
		printf("Device number %d with BTMAC 0x%02x%02x%02x%02x%02x%02x\r\n", i, devices.devices[i].btmac[0], devices.devices[i].btmac[1], devices.devices[i].btmac[2], devices.devices[i].btmac[3], devices.devices[i].btmac[4], devices.devices[i].btmac[5]);
	}

	uint8_t echo[] = "Hello, I'm connected ";
	while (1)
	{
		if (passkeyRequestReceived)
		{
			/* Respond to passkey request */

			passkeyRequestReceived = false;

			/* default passkey */
			uint8_t passkey[6] = {
					'1',
					'2',
					'3',
					'1',
					'2',
					'3' };
			ProteusII_Passkey(passkey);
		}

		if (displayPasskeyRequestReceived)
		{
			/* Respond to display passkey request */

			displayPasskeyRequestReceived = false;

			/* Confirm key */
			ProteusII_NumericCompareConfirm(true);
		}

		if (ProteusII_DriverState_BLE_ChannelOpen == ProteusII_GetDriverState())
		{
			if (!ProteusII_Transmit(echo, sizeof(echo)))
			{
				printf("Transmission failed\r\n");
			};
			WE_Delay(500);
		}
		else
		{
			printf("Wait for channel open\r\n");
			WE_Delay(2000);
		}
	}
}

/**
 * @brief Proteus-II peripheral only mode example.
 *
 * The example consists of two steps:
 * 1) Connect to Proteus-II in command mode to check/set parameters (if required).
 * 2) Disconnect and reconnect in peripheral only mode. In peripheral only mode, the
 *    status and busy pins are monitored and any received data is echoed back.
 *
 * Note that this example requires pins MODE_1, BUSY and LED_2 to be connected.
 */
static void PeripheralOnlyModeExample()
{
	/* No callbacks required */
	ProteusII_CallbackConfig_t callbackConfig = {
			0 };

	if (!ProteusII_Init(&ProteusII_uart, &ProteusII_pins, ProteusII_OperationMode_CommandMode, callbackConfig))
	{
		printf("Initialization error\r\n");
		return;
	}
	/* Parameters may be set here if necessary */

	uint8_t deviceName[32];
	uint16_t deviceNameLength;
	if (ProteusII_GetDeviceName(deviceName, &deviceNameLength))
	{
		deviceName[deviceNameLength] = '\0';
		printf("Device name: %s\r\n", deviceName);
	}

	ProteusII_Deinit();

	if (!ProteusII_Init(&ProteusII_uart, &ProteusII_pins, ProteusII_OperationMode_PeripheralOnlyMode, callbackConfig))
	{
		printf("Initialization error\r\n");
		return;
	}

	/* In peripheral only mode, all bytes received should be diverted to custom callback OnPeripheralOnlyModeByteReceived() */
	ProteusII_SetByteRxCallback(OnPeripheralOnlyModeByteReceived);

	printf("Peripheral only mode started.\r\n");

	uint32_t lastStatusPinLowTick = WE_GetTick();
	bool per_channelOpen = false;
	bool statusPinState = ProteusII_GetStatusLed2PinLevel();
	uint8_t echo[] = "Hello, I'm connected ";
	while (1)
	{
		/* Check connection status and print message on state change (using status i.e. LED_2 pin) */
		statusPinState = ProteusII_GetStatusLed2PinLevel();
		if (per_channelOpen)
		{
			if (!statusPinState)
			{
				/* Status pin changed to low - channel is now closed */
				printf("Channel closed.\r\n");
				per_channelOpen = false;
			}
			else if (false == ProteusII_IsPeripheralOnlyModeBusy())
			{
				/* module not busy */
				printf("Transmit data\r\n");

				if (ProteusII_Transparent_Transmit(echo, sizeof(echo)))
				{
					uint8_t count = 0;
					/* transmission success */
					while (count < 20)
					{
						/* wait for high */
						WE_Delay(2);
						count++;
						if (true == ProteusII_IsPeripheralOnlyModeBusy())
						{
							/* high detected */
							break;
						}
					}
					count = 0;
					while (count < 128)
					{
						/* wait for low */
						WE_Delay(1);
						count++;
						if (false == ProteusII_IsPeripheralOnlyModeBusy())
						{
							/* low detected */
							break;
						}
					}
				}
				WE_Delay(500);
			}
			else
			{
				/* module busy */
				printf("Module busy\r\n");
				WE_Delay(20);
			}
		}
		else if (statusPinState)
		{
			if (WE_GetTick() - lastStatusPinLowTick > PROTEUSII_STATUS_LED_CONNECTED_TIMEOUT_MS)
			{
				/* Status pin has been high for at least
				 * PROTEUSII_STATUS_LED_CONNECTED_TIMEOUT_MS ms - channel is now open */
				printf("Channel opened.\r\n");
				per_channelOpen = true;
			}
		}

		if (!statusPinState)
		{
			/* Status pin is low - store current tick value (required for checking
			 * the duration that the status pin is high) */
			lastStatusPinLowTick = WE_GetTick();
		}
	}
}

/**
 * @brief Proteus-III DTM example.
 *
 * Starts the direct test mode (DTM) and switches between several transmission modes
 */
static void DTMExample()
{
	bool ret = false;

	ProteusII_CallbackConfig_t callbackConfig = {
			0 };
	callbackConfig.sleepCb = SleepCallback;
	callbackConfig.errorCb = ErrorCallback;

	if (false == ProteusII_Init(&ProteusII_uart, &ProteusII_pins, ProteusII_OperationMode_CommandMode, callbackConfig))
	{
		printf("Initialization error\r\n");
		return;
	}

	ProteusII_DeviceInfo_t deviceInfo;
	if (ProteusII_GetDeviceInfo(&deviceInfo))
	{
		printf("Device info OS version = 0x%04x, "
				"build code = 0x%08lx, "
				"package variant = 0x%04x, "
				"chip ID = 0x%08lx\r\n", deviceInfo.osVersion, deviceInfo.buildCode, deviceInfo.packageVariant, deviceInfo.chipId);
	}

	uint8_t fwVersion[3];
	ret = ProteusII_GetFWVersion(fwVersion);
	Examples_Print("Get firmware version", ret);
	printf("Firmware version is %u.%u.%u\r\n", fwVersion[2], fwVersion[1], fwVersion[0]);
	WE_Delay(500);

	ret = ProteusII_DTMEnable();
	Examples_Print("DTM enable", ret);

	ProteusII_Phy_t phy = ProteusII_Phy_1MBit;
	uint8_t channel = 19;
	ProteusII_TXPower_t power = ProteusII_TXPower_4;

	ret = ProteusII_DTMSetTXPower(power);
	Examples_Print("Set TX power\r\n", ret);

	while (1)
	{
		phy = (phy == ProteusII_Phy_1MBit) ? ProteusII_Phy_2MBit : ProteusII_Phy_1MBit;
		ret = ProteusII_DTMSetPhy(phy);
		Examples_Print("Set phy", ret);

		ret = ProteusII_DTMStartTX(channel, 16, ProteusII_DTMTXPattern_PRBS9);
		Examples_Print("Start TX", ret);
		WE_Delay(2000);

		ret = ProteusII_DTMStop();
		Examples_Print("Stop TX\r\n", ret);

		ret = ProteusII_DTMStartTXCarrier(channel);
		Examples_Print("Start TX carrier", ret);
		WE_Delay(2000);

		ret = ProteusII_DTMStop();
		Examples_Print("Stop TX carrier\r\n", ret);
	}
}

/**
 * @brief Callback called when data has been received via radio
 */
static void RxCallback(uint8_t *payload, uint16_t payloadLength, uint8_t *btMac, int8_t rssi)
{
	uint16_t i = 0;
	printf("Received data from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) with RSSI = %d dBm:\r\n-> ", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], rssi);
	printf("0x");
	for (i = 0; i < payloadLength; i++)
	{
		printf("%02x", *(payload + i));
	}
	printf(" (");
	for (i = 0; i < payloadLength; i++)
	{
		printf("%c", *(payload + i));
	}
	printf(")\r\n");
	fflush(stdout);
}

/**
 * @brief Callback called when beacon has been received via radio
 */
static void BeaconRxCallback(uint8_t *payload, uint16_t payloadLength, uint8_t *btMac, int8_t rssi)
{
	uint16_t i = 0;
	printf("Received beacon data from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) with RSSI = %d dBm:\r\n-> ", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], rssi);
	printf("0x");
	for (i = 0; i < payloadLength; i++)
	{
		printf("%02x", *(payload + i));
	}
	printf(" (");
	for (i = 0; i < payloadLength; i++)
	{
		printf("%c", *(payload + i));
	}
	printf(")\r\n");
	fflush(stdout);
}

/**
 * @brief Callback called when connection request is received
 */
static void ConnectCallback(bool success, uint8_t *btMac)
{
	printf("%s to device with BTMAC (0x%02x%02x%02x%02x%02x%02x) ", success ? "Connected" : "Failed to connect", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
	printf("\r\n");
	fflush(stdout);
}

/**
 * @brief Callback called when security level has changed
 */
static void SecurityCallback(uint8_t *btMac, ProteusII_SecurityState_t securityState)
{
	static const char *stateStrings[] = {
			"rebonded",
			"bonded",
			"paired" };

	printf("Encrypted link to device with BTMAC (0x%02x%02x%02x%02x%02x%02x) established (%s)", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], stateStrings[securityState]);
	printf("\r\n");
	fflush(stdout);
}

/**
 * @brief Callback called when remote device has requested for pass key
 */
static void PasskeyCallback(uint8_t *btMac)
{
	printf("Pass key request from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) ", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
	printf("\r\n");
	fflush(stdout);

	/* Handle pass key request asynchronously in main (must not send response directly from callback) */
	passkeyRequestReceived = true;
}

/**
 * @brief Callback called when pass key is displayed to enter on remote device
 */
static void DisplayPasskeyCallback(ProteusII_DisplayPasskeyAction_t action, uint8_t *btMac, uint8_t *passkey)
{
	printf("Pass key request from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) ", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
	printf("and pass key (%c%c%c%c%c%c) ", passkey[0], passkey[1], passkey[2], passkey[3], passkey[4], passkey[5]);
	printf("\r\n");
	fflush(stdout);

	if (ProteusII_DisplayPasskeyAction_PleaseConfirm == action)
	{
		/* Handle display pass key request asynchronously in main (must not send response directly from callback) */
		displayPasskeyRequestReceived = true;
	}
}

/**
 * @brief Callback called when connection has been dropped
 */
static void DisconnectCallback(ProteusII_DisconnectReason_t reason)
{
	static const char *reasonStrings[] = {
			"unknown",
			"connection timeout",
			"user terminated connection",
			"host terminated connection",
			"connection interval unacceptable",
			"MIC failure",
			"connection setup failed" };

	printf("Disconnected (reason: %s)\r\n", reasonStrings[reason]);
	fflush(stdout);
}

/**
 * @brief Callback called when Bluetooth channel has been opened. Now data transmission can be done
 */
static void ChannelOpenCallback(uint8_t *btMac, uint16_t maxPayload)
{
	printf("Channel opened to BTMAC (0x%02x%02x%02x%02x%02x%02x) with maximum payload = %d", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], maxPayload);
	printf("\r\n");
	fflush(stdout);
}

/**
 * @brief Callback called when the PHY of the connection has changed
 */
static void PhyUpdateCallback(bool success, uint8_t *btMac, uint8_t phyRx, uint8_t phyTx)
{
	if (success)
	{
		printf("Phy of connection to BTMAC (0x%02x%02x%02x%02x%02x%02x) updated (RX: %dMBit, TX: %dMBit)", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], phyRx, phyTx);
	}
	else
	{
		printf("Failed to update Phy connection");
	}
	printf("\r\n");
	fflush(stdout);
}

/**
 * @brief Callback called when module goes to sleep mode
 */
static void SleepCallback()
{
	printf("Will go to sleep now\r\n");
}

/**
 * @brief Callback called when RSSI indication message has been received
 */
static void RssiCallback(uint8_t *btMac, int8_t rssi, int8_t txPower)
{
	printf("Received RSSI indication from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) with RSSI = %d dBm and TX power = %d dBm.\r\n", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], rssi, txPower);
	fflush(stdout);
}

/**
 * @brief Callback called when module reports an error state
 */
static void ErrorCallback(uint8_t errorCode)
{
	printf("Error %u\r\n", errorCode);
}

/**
 * @brief Handles bytes received in peripheral only mode (is set as custom callback
 * for bytes received from Proteus-II).
 */
static void OnPeripheralOnlyModeByteReceived(uint8_t *dataP, size_t size)
{
	uint8_t *p = dataP;
	size_t s = size;
	printf("Rx 0x");
	for (; s > 0; s--, p++)
	{
		printf("%02x", *p);
	}
	p = dataP;
	s = size;
	printf("(");
	for (; s > 0; s--, p++)
	{
		printf("%c", *p);
	}
	printf(")\r\n");
}
