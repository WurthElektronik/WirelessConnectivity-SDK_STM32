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
 * COPYRIGHT (c) 2025 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Proteus-II example.
 */

#include <ProteusII/ProteusII.h>
#include <global/global.h>
#include <global_platform_types.h>
#include <stdio.h>
#include <string.h>

/* Proteus-II examples. Pick the example to be executed in the main function. */
static void CommandModeExample();
static void PeripheralOnlyModeExample();
static void DTMExample();

/* Callback functions for various indications sent by the Proteus-II. */
static void RxCallback(uint8_t* payload, uint16_t payloadLength, uint8_t* btMac, int8_t rssi);
static void BeaconRxCallback(uint8_t* payload, uint16_t payloadLength, uint8_t* btMac, int8_t rssi);
static void ConnectCallback(bool success, uint8_t* btMac);
static void SecurityCallback(uint8_t* btMac, ProteusII_SecurityState_t securityState);
static void PasskeyCallback(uint8_t* btMac);
static void DisplayPasskeyCallback(ProteusII_DisplayPasskeyAction_t action, uint8_t* btMac, uint8_t* passkey);
static void DisconnectCallback(ProteusII_DisconnectReason_t reason);
static void ChannelOpenCallback(uint8_t* btMac, uint16_t maxPayload);
static void PhyUpdateCallback(bool success, uint8_t* btMac, uint8_t phyRx, uint8_t phyTx);
static void SleepCallback();
static void RssiCallback(uint8_t* btMac, int8_t rssi, int8_t txPower);
static void ErrorCallback(uint8_t errorCode);
static void OnPeripheralOnlyModeByteReceived(uint8_t* dataP, size_t size);

/* Is set to true when a passkey indication has been received. */
static bool passkeyRequestReceived = false;

/* Is set to true when a display passkey indication has been received. */
static bool displayPasskeyRequestReceived = false;

/**
 * @brief Definition of the pins
 */
static ProteusII_Pins_t ProteusII_pins = {
    .ProteusII_Pin_Reset = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_10)),
    .ProteusII_Pin_SleepWakeUp = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_9)),
    .ProteusII_Pin_Boot = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_7)),
    .ProteusII_Pin_Mode = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_8)),
    .ProteusII_Pin_Busy = WE_PIN((void*)&WE_STM32_PIN(GPIOB, GPIO_PIN_8)),
    .ProteusII_Pin_StatusLed2 = WE_PIN((void*)&WE_STM32_PIN(GPIOB, GPIO_PIN_9)),
};

/**
 * @brief Definition of the uart
 */
static WE_UART_t ProteusII_uart;

/**
 * @brief The application's main function.
 */
void ProteusII_Examples(void)
{

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
static void Examples_Print(char* str, bool success) { WE_DEBUG_PRINT("%s%s\r\n", success ? "OK    " : "NOK   ", str); }

/**
 * @brief Proteus-II command mode example.
 *
 * Connects to Proteus-II in command mode registering callbacks for all
 * available indications, then prints info when receiving indications.
 */
static void CommandModeExample()
{
    bool ret = false;

    ProteusII_CallbackConfig_t callbackConfig = {0};
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
        WE_DEBUG_PRINT("Initialization error\r\n");
        return;
    }

    ProteusII_DeviceInfo_t deviceInfo;
    if (ProteusII_GetDeviceInfo(&deviceInfo))
    {
        WE_DEBUG_PRINT("Device info OS version = 0x%04x, "
                       "build code = 0x%08lx, "
                       "package variant = 0x%04x, "
                       "chip ID = 0x%08lx\r\n",
                       deviceInfo.osVersion, deviceInfo.buildCode, deviceInfo.packageVariant, deviceInfo.chipId);
    }

    uint8_t fwVersion[3];
    ret = ProteusII_GetFWVersion(fwVersion);
    Examples_Print("Get firmware version", ret);
    WE_DEBUG_PRINT("Firmware version is %u.%u.%u\r\n", fwVersion[2], fwVersion[1], fwVersion[0]);
    WE_Delay(500);

    uint8_t mac[8];
    ret = ProteusII_GetMAC(mac);
    Examples_Print("Get MAC", ret);
    WE_DEBUG_PRINT("MAC is 0x%02x%02x%02x%02x%02x%02x%02x%02x\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], mac[6], mac[7]);
    WE_Delay(500);

    uint8_t btMac[6];
    ret = ProteusII_GetBTMAC(btMac);
    Examples_Print("Get BT MAC", ret);
    WE_DEBUG_PRINT("BTMAC is 0x%02x%02x%02x%02x%02x%02x\r\n", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
    WE_Delay(500);

    uint8_t serialNr[3];
    ret = ProteusII_GetSerialNumber(serialNr);
    Examples_Print("Get serial number", ret);
    WE_DEBUG_PRINT("Serial number is 0x%02x%02x%02x\r\n", serialNr[2], serialNr[1], serialNr[0]);
    WE_Delay(500);

    ProteusII_TXPower_t txPower;
    ret = ProteusII_GetTXPower(&txPower);
    Examples_Print("Get TX power", ret);
    WE_DEBUG_PRINT("TX power index is %d\r\n", txPower);
    WE_Delay(500);

    ret = ProteusII_ScanStart();
    Examples_Print("Scan start", ret);
    WE_Delay(2500);

    ret = ProteusII_ScanStop();
    Examples_Print("Scan stop", ret);

    ProteusII_GetDevices_t devices;
    ret = ProteusII_GetDevices(&devices);
    Examples_Print("Get devices", ret);
    WE_DEBUG_PRINT("%d device(s) found\r\n", devices.numberOfDevices);
    for (uint8_t i = 0; i < devices.numberOfDevices; i++)
    {
        WE_DEBUG_PRINT("Device number %d with BTMAC 0x%02x%02x%02x%02x%02x%02x\r\n", i, devices.devices[i].btmac[0], devices.devices[i].btmac[1], devices.devices[i].btmac[2], devices.devices[i].btmac[3], devices.devices[i].btmac[4], devices.devices[i].btmac[5]);
    }

    uint8_t echo[] = "Hello, I'm connected ";
    while (1)
    {
        if (passkeyRequestReceived)
        {
            /* Respond to passkey request */

            passkeyRequestReceived = false;

            /* default passkey */
            uint8_t passkey[6] = {'1', '2', '3', '1', '2', '3'};
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
                WE_DEBUG_PRINT("Transmission failed\r\n");
            };
            WE_Delay(500);
        }
        else
        {
            WE_DEBUG_PRINT("Wait for channel open\r\n");
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
    ProteusII_CallbackConfig_t callbackConfig = {0};

    if (!ProteusII_Init(&ProteusII_uart, &ProteusII_pins, ProteusII_OperationMode_CommandMode, callbackConfig))
    {
        WE_DEBUG_PRINT("Initialization error\r\n");
        return;
    }
    /* Parameters may be set here if necessary */

    uint8_t deviceName[32];
    uint16_t deviceNameLength;
    if (ProteusII_GetDeviceName(deviceName, &deviceNameLength))
    {
        deviceName[deviceNameLength] = '\0';
        WE_DEBUG_PRINT("Device name: %s\r\n", deviceName);
    }

    ProteusII_Deinit();

    if (!ProteusII_Init(&ProteusII_uart, &ProteusII_pins, ProteusII_OperationMode_PeripheralOnlyMode, callbackConfig))
    {
        WE_DEBUG_PRINT("Initialization error\r\n");
        return;
    }

    /* In peripheral only mode, all bytes received should be diverted to custom callback OnPeripheralOnlyModeByteReceived() */
    ProteusII_SetByteRxCallback(OnPeripheralOnlyModeByteReceived);

    WE_DEBUG_PRINT("Peripheral only mode started.\r\n");

    uint32_t lastStatusPinLowTick = WE_GetTick();
    bool per_channelOpen = false;

    WE_Pin_Level_t statusPinState;
    bool busyState;

    if (!ProteusII_GetStatusLed2PinLevel(&statusPinState))
    {
        WE_DEBUG_PRINT("Failed to get pin state.\r\n");
        return;
    }

    uint8_t echo[] = "Hello, I'm connected ";
    while (1)
    {
        /* Check connection status and print message on state change (using status i.e. LED_2 pin) */
        if (!ProteusII_GetStatusLed2PinLevel(&statusPinState))
        {
            WE_DEBUG_PRINT("Failed to get pin state.\r\n");
            return;
        }

        if (per_channelOpen)
        {
            if (statusPinState == WE_Pin_Level_Low)
            {
                /* Status pin changed to low - channel is now closed */
                WE_DEBUG_PRINT("Channel closed.\r\n");
                per_channelOpen = false;
            }
            else
            {
                if (!ProteusII_IsPeripheralOnlyModeBusy(&busyState))
                {
                    WE_DEBUG_PRINT("Failed to get pin state.\r\n");
                    return;
                }

                if (!busyState)
                {
                    /* module not busy */
                    WE_DEBUG_PRINT("Transmit data\r\n");

                    if (ProteusII_Transparent_Transmit(echo, sizeof(echo)))
                    {
                        uint8_t count = 0;
                        /* transmission success */
                        while (count < 20)
                        {
                            /* wait for high */
                            WE_Delay(2);
                            count++;
                            if (!ProteusII_IsPeripheralOnlyModeBusy(&busyState))
                            {
                                WE_DEBUG_PRINT("Failed to get pin state.\r\n");
                                return;
                            }
                            if (busyState)
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
                            if (!ProteusII_IsPeripheralOnlyModeBusy(&busyState))
                            {
                                WE_DEBUG_PRINT("Failed to get pin state.\r\n");
                                return;
                            }
                            if (!busyState)
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
                    WE_DEBUG_PRINT("Module busy\r\n");
                    WE_Delay(20);
                }
            }
        }
        else if (statusPinState == WE_Pin_Level_High)
        {
            if (WE_GetTick() - lastStatusPinLowTick > PROTEUSII_STATUS_LED_CONNECTED_TIMEOUT_MS)
            {
                /* Status pin has been high for at least
				 * PROTEUSII_STATUS_LED_CONNECTED_TIMEOUT_MS ms - channel is now open */
                WE_DEBUG_PRINT("Channel opened.\r\n");
                per_channelOpen = true;
            }
        }

        if (statusPinState == WE_Pin_Level_Low)
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

    ProteusII_CallbackConfig_t callbackConfig = {0};
    callbackConfig.sleepCb = SleepCallback;
    callbackConfig.errorCb = ErrorCallback;

    if (false == ProteusII_Init(&ProteusII_uart, &ProteusII_pins, ProteusII_OperationMode_CommandMode, callbackConfig))
    {
        WE_DEBUG_PRINT("Initialization error\r\n");
        return;
    }

    ProteusII_DeviceInfo_t deviceInfo;
    if (ProteusII_GetDeviceInfo(&deviceInfo))
    {
        WE_DEBUG_PRINT("Device info OS version = 0x%04x, "
                       "build code = 0x%08lx, "
                       "package variant = 0x%04x, "
                       "chip ID = 0x%08lx\r\n",
                       deviceInfo.osVersion, deviceInfo.buildCode, deviceInfo.packageVariant, deviceInfo.chipId);
    }

    uint8_t fwVersion[3];
    ret = ProteusII_GetFWVersion(fwVersion);
    Examples_Print("Get firmware version", ret);
    WE_DEBUG_PRINT("Firmware version is %u.%u.%u\r\n", fwVersion[2], fwVersion[1], fwVersion[0]);
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
static void RxCallback(uint8_t* payload, uint16_t payloadLength, uint8_t* btMac, int8_t rssi)
{
    uint16_t i = 0;
    WE_DEBUG_PRINT("Received data from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) with RSSI = %d dBm:\r\n-> ", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], rssi);
    WE_DEBUG_PRINT("0x");
    for (i = 0; i < payloadLength; i++)
    {
        WE_DEBUG_PRINT("%02x", *(payload + i));
    }
    WE_DEBUG_PRINT(" (");
    for (i = 0; i < payloadLength; i++)
    {
        WE_DEBUG_PRINT("%c", *(payload + i));
    }
    WE_DEBUG_PRINT(")\r\n");
}

/**
 * @brief Callback called when beacon has been received via radio
 */
static void BeaconRxCallback(uint8_t* payload, uint16_t payloadLength, uint8_t* btMac, int8_t rssi)
{
    uint16_t i = 0;
    WE_DEBUG_PRINT("Received beacon data from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) with RSSI = %d dBm:\r\n-> ", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], rssi);
    WE_DEBUG_PRINT("0x");
    for (i = 0; i < payloadLength; i++)
    {
        WE_DEBUG_PRINT("%02x", *(payload + i));
    }
    WE_DEBUG_PRINT(" (");
    for (i = 0; i < payloadLength; i++)
    {
        WE_DEBUG_PRINT("%c", *(payload + i));
    }
    WE_DEBUG_PRINT(")\r\n");
}

/**
 * @brief Callback called when connection request is received
 */
static void ConnectCallback(bool success, uint8_t* btMac)
{
    WE_DEBUG_PRINT("%s to device with BTMAC (0x%02x%02x%02x%02x%02x%02x) ", success ? "Connected" : "Failed to connect", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
    WE_DEBUG_PRINT("\r\n");
}

/**
 * @brief Callback called when security level has changed
 */
static void SecurityCallback(uint8_t* btMac, ProteusII_SecurityState_t securityState)
{
    static const char* stateStrings[] = {"rebonded", "bonded", "paired"};

    WE_DEBUG_PRINT("Encrypted link to device with BTMAC (0x%02x%02x%02x%02x%02x%02x) established (%s)", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], stateStrings[securityState]);
    WE_DEBUG_PRINT("\r\n");
}

/**
 * @brief Callback called when remote device has requested for pass key
 */
static void PasskeyCallback(uint8_t* btMac)
{
    WE_DEBUG_PRINT("Pass key request from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) ", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
    WE_DEBUG_PRINT("\r\n");

    /* Handle pass key request asynchronously in main (must not send response directly from callback) */
    passkeyRequestReceived = true;
}

/**
 * @brief Callback called when pass key is displayed to enter on remote device
 */
static void DisplayPasskeyCallback(ProteusII_DisplayPasskeyAction_t action, uint8_t* btMac, uint8_t* passkey)
{
    WE_DEBUG_PRINT("Pass key request from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) ", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
    WE_DEBUG_PRINT("and pass key (%c%c%c%c%c%c) ", passkey[0], passkey[1], passkey[2], passkey[3], passkey[4], passkey[5]);
    WE_DEBUG_PRINT("\r\n");

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
    static const char* reasonStrings[] = {"unknown", "connection timeout", "user terminated connection", "host terminated connection", "connection interval unacceptable", "MIC failure", "connection setup failed"};

    WE_DEBUG_PRINT("Disconnected (reason: %s)\r\n", reasonStrings[reason]);
}

/**
 * @brief Callback called when Bluetooth channel has been opened. Now data transmission can be done
 */
static void ChannelOpenCallback(uint8_t* btMac, uint16_t maxPayload)
{
    WE_DEBUG_PRINT("Channel opened to BTMAC (0x%02x%02x%02x%02x%02x%02x) with maximum payload = %d", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], maxPayload);
    WE_DEBUG_PRINT("\r\n");
}

/**
 * @brief Callback called when the PHY of the connection has changed
 */
static void PhyUpdateCallback(bool success, uint8_t* btMac, uint8_t phyRx, uint8_t phyTx)
{
    if (success)
    {
        WE_DEBUG_PRINT("Phy of connection to BTMAC (0x%02x%02x%02x%02x%02x%02x) updated (RX: %dMBit, TX: %dMBit)", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], phyRx, phyTx);
    }
    else
    {
        WE_DEBUG_PRINT("Failed to update Phy connection");
    }
    WE_DEBUG_PRINT("\r\n");
}

/**
 * @brief Callback called when module goes to sleep mode
 */
static void SleepCallback() { WE_DEBUG_PRINT("Will go to sleep now\r\n"); }

/**
 * @brief Callback called when RSSI indication message has been received
 */
static void RssiCallback(uint8_t* btMac, int8_t rssi, int8_t txPower) { WE_DEBUG_PRINT("Received RSSI indication from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) with RSSI = %d dBm and TX power = %d dBm.\r\n", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], rssi, txPower); }

/**
 * @brief Callback called when module reports an error state
 */
static void ErrorCallback(uint8_t errorCode) { WE_DEBUG_PRINT("Error %u\r\n", errorCode); }

/**
 * @brief Handles bytes received in peripheral only mode (is set as custom callback
 * for bytes received from Proteus-II).
 */
static void OnPeripheralOnlyModeByteReceived(uint8_t* dataP, size_t size)
{
    uint8_t* p = dataP;
    size_t s = size;
    WE_DEBUG_PRINT("Rx 0x");
    for (; s > 0; s--, p++)
    {
        WE_DEBUG_PRINT("%02x", *p);
    }
    p = dataP;
    s = size;
    WE_DEBUG_PRINT("(");
    for (; s > 0; s--, p++)
    {
        WE_DEBUG_PRINT("%c", *p);
    }
    WE_DEBUG_PRINT(")\r\n");
}
