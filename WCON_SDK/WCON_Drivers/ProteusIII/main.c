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
 * @brief Proteus-III example.
 */

#include <stdio.h>
#include <string.h>

#include "../../WCON_Drivers/ProteusIII/ProteusIII.h"
#include "../../WCON_Drivers/global/global.h"

/* Proteus-III examples. Pick the example to be executed in the main function. */
static void CommandModeExample();
static void PeripheralOnlyModeExample();
static void DTMExample();

/* Callback functions for various indications sent by the Proteus-III. */
static void RxCallback(uint8_t *payload, uint16_t payloadLength, uint8_t *btMac, int8_t rssi);
static void BeaconRxCallback(uint8_t *payload, uint16_t payloadLength, uint8_t *btMac, int8_t rssi);
static void ConnectCallback(bool success, uint8_t *btMac);
static void SecurityCallback(uint8_t *btMac, ProteusIII_SecurityState_t securityState);
static void PasskeyCallback(uint8_t *btMac);
static void DisplayPasskeyCallback(ProteusIII_DisplayPasskeyAction_t action, uint8_t *btMac, uint8_t *passkey);
static void DisconnectCallback(ProteusIII_DisconnectReason_t reason);
static void ChannelOpenCallback(uint8_t *btMac, uint16_t maxPayload);
static void PhyUpdateCallback(bool success, uint8_t *btMac, uint8_t phyRx, uint8_t phyTx);
static void SleepCallback();
static void RssiCallback(uint8_t *btMac, int8_t rssi, int8_t txPower);
static void GpioWriteCallback(bool remote, uint8_t gpioId, uint8_t value);
static void GpioRemoteConfigCallback(ProteusIII_GPIOConfigBlock_t *gpioConfig);
static void ErrorCallback(uint8_t errorCode);
static void OnPeripheralOnlyModeByteReceived(uint8_t receivedByte);

/* Is set to true if the channel has been opened */
bool cmd_channelOpen = false;

/* Is set to true when a passkey indication has been received. */
bool passkeyRequestReceived = false;

/* Is set to true when a display passkey indication has been received. */
bool displayPasskeyRequestReceived = false;

/**
 * @brief The application's main function.
 */
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

	CommandModeExample();
//    PeripheralOnlyModeExample();
//    DTMExample();
}

/**
 * @brief Prints the supplied string, prefixed with OK or NOK (depending on the success parameter).
 *
 * @param str String to print
 * @param success Variable indicating if action was ok
 */
void Examples_Print(char *str, bool success)
{
	printf("%s%s\r\n", success ? "OK    " : "NOK   ", str);
}

/**
 * @brief Proteus-III command mode example.
 *
 * Connects to Proteus-III in command mode registering callbacks for all
 * available indications, then prints info when receiving indications.
 */
void CommandModeExample()
{
	bool ret = false;

	ProteusIII_CallbackConfig_t callbackConfig = {
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
	callbackConfig.gpioWriteCb = GpioWriteCallback;
	callbackConfig.gpioRemoteConfigCb = GpioRemoteConfigCallback;
	callbackConfig.errorCb = ErrorCallback;

	ProteusIII_Init(PROTEUSIII_DEFAULT_BAUDRATE, WE_FlowControl_NoFlowControl, ProteusIII_OperationMode_CommandMode, callbackConfig);

//    printf("Performing factory reset\n");
//    bool ret = ProteusIII_FactoryReset();
//    printf("Factory reset %s\n", ret ? "OK" : "NOK");

	ProteusIII_DeviceInfo_t deviceInfo;
	if (ProteusIII_GetDeviceInfo(&deviceInfo))
	{
		printf("Device info OS version = 0x%04x, "
				"build code = 0x%08lx, "
				"package variant = 0x%04x, "
				"chip ID = 0x%08lx\r\n", deviceInfo.osVersion, deviceInfo.buildCode, deviceInfo.packageVariant, deviceInfo.chipId);
	}

	uint8_t fwVersion[3];
	ret = ProteusIII_GetFWVersion(fwVersion);
	Examples_Print("Get firmware version", ret);
	printf("Firmware version is %u.%u.%u\r\n", fwVersion[2], fwVersion[1], fwVersion[0]);
	WE_Delay(500);

	uint8_t mac[8];
	ret = ProteusIII_GetMAC(mac);
	Examples_Print("Get MAC", ret);
	printf("MAC is 0x%02x%02x%02x%02x%02x%02x%02x%02x\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], mac[6], mac[7]);
	WE_Delay(500);

	uint8_t btMac[6];
	ret = ProteusIII_GetBTMAC(btMac);
	Examples_Print("Get BT MAC", ret);
	printf("BTMAC is 0x%02x%02x%02x%02x%02x%02x\r\n", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
	WE_Delay(500);

	uint8_t serialNr[3];
	ret = ProteusIII_GetSerialNumber(serialNr);
	Examples_Print("Get serial number", ret);
	printf("Serial number is 0x%02x%02x%02x\r\n", serialNr[2], serialNr[1], serialNr[0]);
	WE_Delay(500);

	ProteusIII_TXPower_t txPower;
	ret = ProteusIII_GetTXPower(&txPower);
	Examples_Print("Get TX power", ret);
	printf("TX power index is %d\r\n", txPower);
	WE_Delay(500);

	ret = ProteusIII_ScanStart();
	Examples_Print("Scan start", ret);
	WE_Delay(2500);

	ret = ProteusIII_ScanStop();
	Examples_Print("Scan stop", ret);

	ProteusIII_GetDevices_t devices;
	ret = ProteusIII_GetDevices(&devices);
	Examples_Print("Get devices", ret);
	printf("%d device(s) found\r\n", devices.numberOfDevices);
	for (uint8_t i = 0; i < devices.numberOfDevices; i++)
	{

		printf("Device number %d with BTMAC 0x%02x%02x%02x%02x%02x%02x\r\n", i, devices.devices[i].btmac[0], devices.devices[i].btmac[1], devices.devices[i].btmac[2], devices.devices[i].btmac[3], devices.devices[i].btmac[4], devices.devices[i].btmac[5]);
	}

	while (1)
	{
		WE_Delay(500);

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
			ProteusIII_Passkey(passkey);
		}

		if (displayPasskeyRequestReceived)
		{
			/* Respond to display passkey request */

			displayPasskeyRequestReceived = false;

			/* Confirm key */
			ProteusIII_NumericCompareConfirm(true);
		}

		if (cmd_channelOpen == true)
		{
			uint8_t echo[] = "Hello, I'm connected ";
			ProteusIII_Transmit(echo, sizeof(echo));
		}
		else
		{
			printf("Wait for channel open\r\n");
		}
	}
}

/**
 * @brief Proteus-III peripheral only mode example.
 *
 * The example consists of two steps:
 * 1) Connect to Proteus-III in command mode to check/set parameters (if required).
 * 2) Disconnect and reconnect in peripheral only mode. In peripheral only mode, the
 *    status and busy pins are monitored and any received data is echoed back.
 *
 * Note that this example requires pins MODE_1, BUSY and LED_2 to be connected.
 */
static void PeripheralOnlyModeExample()
{
	/* No callbacks required */
	ProteusIII_CallbackConfig_t callbackConfig = {
			0 };

	if (!ProteusIII_Init(PROTEUSIII_DEFAULT_BAUDRATE, WE_FlowControl_NoFlowControl, ProteusIII_OperationMode_CommandMode, callbackConfig))
	{
		/* Error */
		while (1)
		{
		}
	}

	/* Parameters may be set here if necessary */

	uint8_t deviceName[32];
	uint16_t deviceNameLength;
	if (ProteusIII_GetDeviceName(deviceName, &deviceNameLength))
	{
		deviceName[deviceNameLength] = '\0';
		printf("Device name: %s\r\n", deviceName);
	}

	ProteusIII_Deinit();

	uint32_t lastStatusPinLowTick = WE_GetTick();
	bool per_channelOpen = false;
	bool busy = false;

	if (!ProteusIII_Init(PROTEUSIII_DEFAULT_BAUDRATE, WE_FlowControl_NoFlowControl, ProteusIII_OperationMode_PeripheralOnlyMode, callbackConfig))
	{
		/* Error */
		while (1)
		{
		}
	}

	/* In peripheral only mode, all bytes received should be diverted to custom callback OnPeripheralOnlyModeByteReceived() */
	ProteusIII_SetByteRxCallback(OnPeripheralOnlyModeByteReceived);

	printf("Peripheral only mode started.\r\n");

	while (1)
	{
		/* Check connection status and print message on state change (using status i.e. LED_2 pin) */
		bool statusPinState = ProteusIII_GetStatusLed2PinLevel();
		if (per_channelOpen)
		{
			if (!statusPinState)
			{
				/* Status pin changed to low - channel is now closed */
				printf("Channel closed.\r\n");
				per_channelOpen = false;
			}
			else
			{
				uint8_t echo[] = "Hello, I'm connected ";
				printf("Transmit data\r\n");
				WE_UART_Transmit(echo, sizeof(echo));
				WE_Delay(500);
			}
		}
		else if (statusPinState)
		{
			if (WE_GetTick() - lastStatusPinLowTick > PROTEUSIII_STATUS_LED_CONNECTED_TIMEOUT)
			{
				/* Status pin has been high for at least
				 * PROTEUSIII_STATUS_LED_CONNECTED_TIMEOUT ms - channel is now open */
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

		/* Check state of busy pin and print message on state change */
		bool b = ProteusIII_IsPeripheralOnlyModeBusy();
		if (b != busy)
		{
			busy = b;
			printf("Busy state changed to %s\r\n", busy ? "true" : "false");
		}

//        WE_Delay(1);
	}
}


/**
 * @brief Proteus-III DTM example.
 *
 * Starts the direct test mode (DTM) and switches between several transmission modes
 */
void DTMExample()
{
	bool ret = false;

	ProteusIII_CallbackConfig_t callbackConfig = {
			0 };
	callbackConfig.sleepCb = SleepCallback;
	callbackConfig.errorCb = ErrorCallback;

	ProteusIII_Init(PROTEUSIII_DEFAULT_BAUDRATE, WE_FlowControl_NoFlowControl, ProteusIII_OperationMode_CommandMode, callbackConfig);

//    printf("Performing factory reset\n");
//    bool ret = ProteusIII_FactoryReset();
//    printf("Factory reset %s\n", ret ? "OK" : "NOK");

	ProteusIII_DeviceInfo_t deviceInfo;
	if (ProteusIII_GetDeviceInfo(&deviceInfo))
	{
		printf("Device info OS version = 0x%04x, "
				"build code = 0x%08lx, "
				"package variant = 0x%04x, "
				"chip ID = 0x%08lx\r\n", deviceInfo.osVersion, deviceInfo.buildCode, deviceInfo.packageVariant, deviceInfo.chipId);
	}

	uint8_t fwVersion[3];
	ret = ProteusIII_GetFWVersion(fwVersion);
	Examples_Print("Get firmware version", ret);
	printf("Firmware version is %u.%u.%u\r\n", fwVersion[2], fwVersion[1], fwVersion[0]);
	WE_Delay(500);

	ret = ProteusIII_DTMEnable();
	Examples_Print("DTM enable", ret);

	ProteusIII_Phy_t phy = ProteusIII_Phy_1MBit;
	uint8_t channel = 19;
	ProteusIII_TXPower_t power = ProteusIII_TXPower_8;

	ret = ProteusIII_DTMSetTXPower(power);
	Examples_Print("Set TX power\r\n", ret);

	while(1)
	{
		phy = (phy == ProteusIII_Phy_1MBit)?ProteusIII_Phy_2MBit:ProteusIII_Phy_1MBit;
		ret = ProteusIII_DTMSetPhy(phy);
		Examples_Print("Set phy", ret);

		ret = ProteusIII_DTMStartTX(channel, 16, ProteusIII_DTMTXPattern_PRBS9);
		Examples_Print("Start TX", ret);
		WE_Delay(2000);

		ret = ProteusIII_DTMStop();
		Examples_Print("Stop TX\r\n", ret);

		ret = ProteusIII_DTMStartTXCarrier(channel);
		Examples_Print("Start TX carrier", ret);
		WE_Delay(2000);

		ret = ProteusIII_DTMStop();
		Examples_Print("Stop TX carrier\r\n", ret);
	}
}

static void RxCallback(uint8_t *payload, uint16_t payloadLength, uint8_t *btMac, int8_t rssi)
{
	int i = 0;
	printf("Received data from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) with RSSI = %d dBm:\r\n-> ", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], rssi);
	printf("0x ");
	for (i = 0; i < payloadLength; i++)
	{
		printf("%02x ", *(payload + i));
	}
	printf("\n-> ");
	for (i = 0; i < payloadLength; i++)
	{
		printf("%c", *(payload + i));
	}
	printf("\r\n");
	fflush(stdout);
}

static void BeaconRxCallback(uint8_t *payload, uint16_t payloadLength, uint8_t *btMac, int8_t rssi)
{
	int i = 0;
	printf("Received beacon data from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) with RSSI = %d dBm:\r\n-> ", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], rssi);
	printf("0x ");
	for (i = 0; i < payloadLength; i++)
	{
		printf("%02x ", *(payload + i));
	}
	printf("\n-> ");
	for (i = 0; i < payloadLength; i++)
	{
		printf("%c", *(payload + i));
	}
	printf("\r\n");
	fflush(stdout);
}

static void ConnectCallback(bool success, uint8_t *btMac)
{
	cmd_channelOpen = false;
	printf("%s to device with BTMAC (0x%02x%02x%02x%02x%02x%02x) ", success ? "Connected" : "Failed to connect", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
	printf("\r\n");
	fflush(stdout);
}

static void SecurityCallback(uint8_t *btMac, ProteusIII_SecurityState_t securityState)
{
	static const char *stateStrings[] = {
			"rebonded",
			"bonded",
			"paired" };

	printf("Encrypted link to device with BTMAC (0x%02x%02x%02x%02x%02x%02x) established (%s)", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], stateStrings[securityState]);
	printf("\r\n");
	fflush(stdout);
}

static void PasskeyCallback(uint8_t *btMac)
{
	printf("Passkey request from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) ", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
	printf("\r\n");
	fflush(stdout);

	/* Handle passkey request asynchronously in main (must not send response directly from callback) */
	passkeyRequestReceived = true;
}

static void DisplayPasskeyCallback(ProteusIII_DisplayPasskeyAction_t action, uint8_t *btMac, uint8_t *passkey)
{
	printf("Passkey request from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) ", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
	printf("and pass key (%c%c%c%c%c%c) ", passkey[0], passkey[1], passkey[2], passkey[3], passkey[4], passkey[5]);
	printf("\r\n");
	fflush(stdout);

	if (ProteusIII_DisplayPasskeyAction_PleaseConfirm == action)
	{
		/* Handle display passkey request asynchronously in main (must not send response directly from callback) */
		displayPasskeyRequestReceived = true;
	}
}

static void DisconnectCallback(ProteusIII_DisconnectReason_t reason)
{
	cmd_channelOpen = false;

	static const char *reasonStrings[] = {
			"unknown",
			"connection timeout",
			"user terminated connection",
			"host terminated connection",
			"connection interval unacceptable",
			"MIC failure",
			"connection setup failed" };

	printf("Disconnected (reason: %s)\n", reasonStrings[reason]);
	fflush(stdout);
}

static void ChannelOpenCallback(uint8_t *btMac, uint16_t maxPayload)
{
	cmd_channelOpen = true;

	printf("Channel opened to BTMAC (0x%02x%02x%02x%02x%02x%02x) with maximum payload = %d", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], maxPayload);
	printf("\r\n");
	fflush(stdout);
}

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

static void SleepCallback()
{
	printf("Advertising timeout detected, will go to sleep now\n");
}

static void RssiCallback(uint8_t *btMac, int8_t rssi, int8_t txPower)
{
	printf("Received RSSI indication from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) with RSSI = %d dBm and TX power = %d dBm.\n", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], rssi, txPower);
	fflush(stdout);
}

static void GpioWriteCallback(bool remote, uint8_t gpioId, uint8_t value)
{
	printf("GPIO write indication received (remote: %s, GPIO: %u, value: %u)\n", remote ? "true" : "false", gpioId, value);
	fflush(stdout);
}

static void GpioRemoteConfigCallback(ProteusIII_GPIOConfigBlock_t *gpioConfig)
{
	static const char *functionStrings[] = {
			"disconnected",
			"input",
			"output",
			"PWM" };

	static const char *pullStrings[] = {
			"no pull",
			"pull down",
			"pull up" };

	printf("GPIO remote config indication received (GPIO: %u, function: %s", gpioConfig->gpioId, functionStrings[gpioConfig->function]);

	switch (gpioConfig->function)
	{
	case ProteusIII_GPIO_IO_Disconnected:
		break;

	case ProteusIII_GPIO_IO_Input:
		printf(", input type: %s", pullStrings[gpioConfig->value.input]);
		break;

	case ProteusIII_GPIO_IO_Output:
		printf(", output level: %s", gpioConfig->value.output == ProteusIII_GPIO_Output_High ? "HIGH" : "LOW");
		break;

	case ProteusIII_GPIO_IO_PWM:
		printf(", PWM period: %u ms, ratio: %u%%", gpioConfig->value.pwm.period, gpioConfig->value.pwm.ratio);
		break;
	}
	printf(")\n");
	fflush(stdout);
}

static void ErrorCallback(uint8_t errorCode)
{
	printf("Error %u\n", errorCode);
}

/**
 * @brief Handles bytes received in peripheral only mode (is set as custom callback
 * for bytes received from Proteus-III).
 */
static void OnPeripheralOnlyModeByteReceived(uint8_t receivedByte)
{
	printf("Rx 0x%02x ('%c')\n", receivedByte, receivedByte);
}
