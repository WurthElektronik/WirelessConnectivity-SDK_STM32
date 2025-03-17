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
 * @brief Proteus-e example.
 */

#include <ProteusE/ProteusE.h>
#include <global/global.h>
#include <global_platform_types.h>
#include <stdio.h>
#include <string.h>

/* Proteus-e examples. Pick the example to be executed in the main function. */
static void CommandModeExample();
static void TransparentModeExample();
static void DTMExample();

/* Callback functions for various indications sent by the Proteus-e. */
static void RxCallback(uint8_t* payload, uint16_t payloadLength, uint8_t* btMac, int8_t rssi);
static void ConnectCallback(bool success, uint8_t* btMac);
static void SecurityCallback(uint8_t* btMac, ProteusE_SecurityState_t securityState);
static void DisconnectCallback(ProteusE_DisconnectReason_t reason);
static void ChannelOpenCallback(uint8_t* btMac, uint16_t maxPayload);
static void PhyUpdateCallback(bool success, uint8_t* btMac, uint8_t phyRx, uint8_t phyTx);
static void SleepCallback();
static void GpioWriteCallback(bool remote, uint8_t gpioId, uint8_t value);
static void GpioRemoteConfigCallback(ProteusE_GPIOConfigBlock_t* gpioConfig);
static void ErrorCallback(uint8_t errorCode);
static void OnTransparentModeByteReceived(uint8_t* dataP, size_t size);

/**
 * @brief Definition of the pins
 */
static ProteusE_Pins_t ProteusE_pins = {
    .ProteusE_Pin_Reset = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_10)),
    .ProteusE_Pin_BusyUartEnable = WE_PIN((void*)&WE_STM32_PIN(GPIOB, GPIO_PIN_8)),
    .ProteusE_Pin_Mode = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_8)),
    .ProteusE_Pin_StatusLed1 = WE_PIN((void*)&WE_STM32_PIN(GPIOB, GPIO_PIN_9)),
};

/**
 * @brief Definition of the uart
 */
static WE_UART_t ProteusE_uart;

/**
 * @brief The application's main function.
 */
void ProteusE_Examples(void)
{

    ProteusE_uart.baudrate = PROTEUSE_DEFAULT_BAUDRATE;
    ProteusE_uart.flowControl = WE_FlowControl_NoFlowControl;
    ProteusE_uart.parity = WE_Parity_None;
    ProteusE_uart.uartInit = WE_UART1_Init;
    ProteusE_uart.uartDeinit = WE_UART1_DeInit;
    ProteusE_uart.uartTransmit = WE_UART1_Transmit;

    CommandModeExample();
    //  TransparentModeExample();
    //  DTMExample();

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
 * @brief Proteus-e command mode example.
 *
 * Connects to Proteus-e in command mode registering callbacks for all
 * available indications, then prints info when receiving indications.
 */
static void CommandModeExample()
{
    bool ret = false;

    ProteusE_CallbackConfig_t callbackConfig = {0};
    callbackConfig.rxCb = RxCallback;
    callbackConfig.connectCb = ConnectCallback;
    callbackConfig.disconnectCb = DisconnectCallback;
    callbackConfig.channelOpenCb = ChannelOpenCallback;
    callbackConfig.securityCb = SecurityCallback;
    callbackConfig.phyUpdateCb = PhyUpdateCallback;
    callbackConfig.sleepCb = SleepCallback;
    callbackConfig.gpioWriteCb = GpioWriteCallback;
    callbackConfig.gpioRemoteConfigCb = GpioRemoteConfigCallback;
    callbackConfig.errorCb = ErrorCallback;

    if (false == ProteusE_Init(&ProteusE_uart, &ProteusE_pins, ProteusE_OperationMode_CommandMode, callbackConfig))
    {
        WE_DEBUG_PRINT("Initialization error\r\n");
        return;
    }

    ProteusE_DeviceInfo_t deviceInfo;
    if (ProteusE_GetDeviceInfo(&deviceInfo))
    {
        WE_DEBUG_PRINT("Device info OS version = 0x%04x, "
                       "build code = 0x%08lx, "
                       "package variant = 0x%04x, "
                       "chip ID = 0x%08lx\r\n",
                       deviceInfo.osVersion, deviceInfo.buildCode, deviceInfo.packageVariant, deviceInfo.chipId);
    }

    uint8_t fwVersion[3];
    ret = ProteusE_GetFWVersion(fwVersion);
    Examples_Print("Get firmware version", ret);
    WE_DEBUG_PRINT("Firmware version is %u.%u.%u\r\n", fwVersion[2], fwVersion[1], fwVersion[0]);
    WE_Delay(500);

    uint8_t mac[8];
    ret = ProteusE_GetMAC(mac);
    Examples_Print("Get MAC", ret);
    WE_DEBUG_PRINT("MAC is 0x%02x%02x%02x%02x%02x%02x%02x%02x\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], mac[6], mac[7]);
    WE_Delay(500);

    uint8_t btMac[6];
    ret = ProteusE_GetBTMAC(btMac);
    Examples_Print("Get BT MAC", ret);
    WE_DEBUG_PRINT("BTMAC is 0x%02x%02x%02x%02x%02x%02x\r\n", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
    WE_Delay(500);

    uint8_t serialNr[3];
    ret = ProteusE_GetSerialNumber(serialNr);
    Examples_Print("Get serial number", ret);
    WE_DEBUG_PRINT("Serial number is 0x%02x%02x%02x\r\n", serialNr[2], serialNr[1], serialNr[0]);
    WE_Delay(500);

    ProteusE_TXPower_t txPower;
    ret = ProteusE_GetTXPower(&txPower);
    Examples_Print("Get TX power", ret);
    WE_DEBUG_PRINT("TX power index is %d\r\n", txPower);
    WE_Delay(500);

    uint8_t echo[] = "Hello, I'm connected ";
    while (1)
    {
        if (ProteusE_DriverState_BLE_ChannelOpen == ProteusE_GetDriverState())
        {
            if (!ProteusE_Transmit(echo, sizeof(echo)))
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
 * @brief Proteus-e transparent mode example.
 *
 * The example consists of two steps:
 * 1) Connect to Proteus-e in command mode to check/set parameters (if required).
 * 2) Disconnect and reconnect in transparent mode. In transparent mode, the
 *    status and busy pins are monitored and any received data is echoed back.
 *
 * Note that this example requires pins MODE_1, BUSY/UART_ENABLE and LED_1 to be connected.
 */
static void TransparentModeExample()
{
    /* No callbacks required */
    ProteusE_CallbackConfig_t callbackConfig = {0};

    if (false == ProteusE_Init(&ProteusE_uart, &ProteusE_pins, ProteusE_OperationMode_CommandMode, callbackConfig))
    {
        WE_DEBUG_PRINT("Initialization error\r\n");
        return;
    }

    /* Parameters may be set here if necessary */

    uint8_t deviceName[32];
    uint16_t deviceNameLength;
    if (ProteusE_GetDeviceName(deviceName, &deviceNameLength))
    {
        deviceName[deviceNameLength] = '\0';
        WE_DEBUG_PRINT("Device name: %s\r\n", deviceName);
    }

    ProteusE_Deinit();

    if (false == ProteusE_Init(&ProteusE_uart, &ProteusE_pins, ProteusE_OperationMode_TransparentMode, callbackConfig))
    {
        WE_DEBUG_PRINT("Initialization error\r\n");
        return;
    }

    /* In transparent mode, all bytes received should be diverted to custom callback OnTransparentModeByteReceived() */
    ProteusE_SetByteRxCallback(OnTransparentModeByteReceived);

    WE_DEBUG_PRINT("Transparent mode started.\r\n");

    uint32_t lastStatusPinLowTick = WE_GetTick();
    bool per_channelOpen = false;

    WE_Pin_Level_t statusPinState;
    bool busyState;

    if (!ProteusE_GetStatusPinLed1Level(&statusPinState))
    {
        WE_DEBUG_PRINT("Failed to get pin state.\r\n");
        return;
    }

    uint8_t echo[] = "Hello, I'm connected ";
    while (1)
    {
        /* Check connection status and print message on state change (using status i.e. LED_2 pin) */
        if (!ProteusE_GetStatusPinLed1Level(&statusPinState))
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
                if (!ProteusE_IsTransparentModeBusy(&busyState))
                {
                    WE_DEBUG_PRINT("Failed to get pin state.\r\n");
                    return;
                }

                if (!busyState)
                {
                    /* module not busy */
                    WE_DEBUG_PRINT("Transmit data\r\n");

                    if (ProteusE_Transparent_Transmit(echo, sizeof(echo)))
                    {
                        uint8_t count = 0;
                        /* transmission success */
                        while (count < 20)
                        {
                            /* wait for high */
                            WE_Delay(2);
                            count++;
                            if (!ProteusE_IsTransparentModeBusy(&busyState))
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
                            if (!ProteusE_IsTransparentModeBusy(&busyState))
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
            if (WE_GetTick() - lastStatusPinLowTick > PROTEUSE_STATUS_LED_CONNECTED_TIMEOUT_MS)
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

    ProteusE_CallbackConfig_t callbackConfig = {0};
    callbackConfig.sleepCb = SleepCallback;
    callbackConfig.errorCb = ErrorCallback;

    if (false == ProteusE_Init(&ProteusE_uart, &ProteusE_pins, ProteusE_OperationMode_CommandMode, callbackConfig))
    {
        WE_DEBUG_PRINT("Initialization error\r\n");
        return;
    }

    ProteusE_DeviceInfo_t deviceInfo;
    if (ProteusE_GetDeviceInfo(&deviceInfo))
    {
        WE_DEBUG_PRINT("Device info OS version = 0x%04x, "
                       "build code = 0x%08lx, "
                       "package variant = 0x%04x, "
                       "chip ID = 0x%08lx\r\n",
                       deviceInfo.osVersion, deviceInfo.buildCode, deviceInfo.packageVariant, deviceInfo.chipId);
    }

    uint8_t fwVersion[3];
    ret = ProteusE_GetFWVersion(fwVersion);
    Examples_Print("Get firmware version", ret);
    WE_DEBUG_PRINT("Firmware version is %u.%u.%u\r\n", fwVersion[2], fwVersion[1], fwVersion[0]);
    WE_Delay(500);

    ret = ProteusE_DTMEnable();
    Examples_Print("DTM enable", ret);

    ProteusE_Phy_t phy = ProteusE_Phy_1MBit;
    uint8_t channel = 19;
    ProteusE_TXPower_t power = ProteusE_TXPower_4;

    ret = ProteusE_DTMSetTXPower(power);
    Examples_Print("Set TX power\r\n", ret);

    while (1)
    {
        phy = (phy == ProteusE_Phy_1MBit) ? ProteusE_Phy_2MBit : ProteusE_Phy_1MBit;
        ret = ProteusE_DTMSetPhy(phy);
        Examples_Print("Set phy", ret);

        ret = ProteusE_DTMStartTX(channel, 16, ProteusE_DTMTXPattern_PRBS9);
        Examples_Print("Start TX", ret);
        WE_Delay(2000);

        ret = ProteusE_DTMStop();
        Examples_Print("Stop TX\r\n", ret);

        ret = ProteusE_DTMStartTXCarrier(channel);
        Examples_Print("Start TX carrier", ret);
        WE_Delay(2000);

        ret = ProteusE_DTMStop();
        Examples_Print("Stop TX carrier\r\n", ret);
    }
}

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
static void SecurityCallback(uint8_t* btMac, ProteusE_SecurityState_t securityState)
{
    static const char* stateStrings[] = {"rebonded", "bonded", "paired"};

    WE_DEBUG_PRINT("Encrypted link to device with BTMAC (0x%02x%02x%02x%02x%02x%02x) established (%s)", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], stateStrings[securityState]);
    WE_DEBUG_PRINT("\r\n");
}

/**
 * @brief Callback called when connection has been dropped
 */
static void DisconnectCallback(ProteusE_DisconnectReason_t reason)
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
 * @brief Callback called when remote device has changed the state of the module's GPIOs
 */
static void GpioWriteCallback(bool remote, uint8_t gpioId, uint8_t value) { WE_DEBUG_PRINT("GPIO write indication received (remote: %s, GPIO: %u, value: %u)\r\n", remote ? "true" : "false", gpioId, value); }

/**
 * @brief Callback called when remote device has configured the module's GPIOs
 */
static void GpioRemoteConfigCallback(ProteusE_GPIOConfigBlock_t* gpioConfig)
{
    static const char* functionStrings[] = {"disconnected", "input", "output"};

    static const char* pullStrings[] = {"no pull", "pull down", "pull up"};

    WE_DEBUG_PRINT("GPIO remote config indication received (GPIO: %u, function: %s", gpioConfig->gpioId, functionStrings[gpioConfig->function]);

    switch (gpioConfig->function)
    {
        case ProteusE_GPIO_IO_Disconnected:
            break;

        case ProteusE_GPIO_IO_Input:
            WE_DEBUG_PRINT(", input type: %s", pullStrings[gpioConfig->value.input]);
            break;

        case ProteusE_GPIO_IO_Output:
            WE_DEBUG_PRINT(", output level: %s", gpioConfig->value.output == ProteusE_GPIO_Output_High ? "HIGH" : "LOW");
            break;
    }
    WE_DEBUG_PRINT(")\r\n");
}

/**
 * @brief Callback called when module reports an error state
 */
static void ErrorCallback(uint8_t errorCode) { WE_DEBUG_PRINT("Error %u\r\n", errorCode); }

/**
 * @brief Handles bytes received in transparent mode (is set as custom callback
 * for bytes received from Proteus-e).
 */
static void OnTransparentModeByteReceived(uint8_t* dataP, size_t size)
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
