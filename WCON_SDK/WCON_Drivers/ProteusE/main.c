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
 * COPYRIGHT (c) 2022 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Proteus-e example.
 */

#include <stdio.h>
#include <string.h>

#include "../../WCON_Drivers/ProteusE/ProteusE.h"
#include "../../WCON_Drivers/global/global.h"

/* Proteus-e examples. Pick the example to be executed in the main function. */
static void CommandModeExample();
static void TransparentModeExample();

/* Callback functions for various indications sent by the Proteus-e. */
static void RxCallback(uint8_t* payload, uint16_t payloadLength, uint8_t* btMac, int8_t rssi);
static void ConnectCallback(bool success, uint8_t* btMac);
static void SecurityCallback(uint8_t* btMac, ProteusE_SecurityState_t securityState);
static void DisconnectCallback(ProteusE_DisconnectReason_t reason);
static void ChannelOpenCallback(uint8_t* btMac, uint16_t maxPayload);
static void PhyUpdateCallback(bool success, uint8_t* btMac, uint8_t phyRx, uint8_t phyTx);
static void SleepCallback();
static void GpioWriteCallback(bool remote, uint8_t gpioId, uint8_t value);
static void GpioRemoteConfigCallback(ProteusE_GPIOConfigBlock_t *gpioConfig);
static void ErrorCallback(uint8_t errorCode);
static void OnTransparentModeByteReceived(uint8_t receivedByte);

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
//    TransparentModeExample();
}

/**
 * @brief Proteus-e command mode example.
 *
 * Connects to Proteus-e in command mode registering callbacks for all
 * available indications, then prints info when receiving indications.
 */
void CommandModeExample()
{
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

    ProteusE_Init(PROTEUSE_DEFAULT_BAUDRATE,
                  WE_FlowControl_NoFlowControl,
                  ProteusE_OperationMode_CommandMode,
                  callbackConfig);

//    printf("Performing factory reset\n");
//    bool ret = ProteusE_FactoryReset();
//    printf("Factory reset %s\n", ret ? "OK" : "NOK");

    ProteusE_DeviceInfo_t deviceInfo;
    if (ProteusE_GetDeviceInfo(&deviceInfo))
    {
        printf("Device info OS version = 0x%04x, "
                "build code = 0x%08lx, "
                "package variant = 0x%04x, "
                "chip ID = 0x%08lx\n",
                deviceInfo.osVersion,
                deviceInfo.buildCode,
                deviceInfo.packageVariant,
                deviceInfo.chipId);
    }

    uint8_t fwVersion[3];
    memset(fwVersion, 0, sizeof(fwVersion));
    ProteusE_GetFWVersion(fwVersion);
    printf("Firmware version is %u.%u.%u\n", fwVersion[2], fwVersion[1], fwVersion[0]);
    WE_Delay(500);

    uint8_t mac[8];
    memset(mac, 0, sizeof(mac));
    ProteusE_GetMAC(mac);
    printf("MAC is 0x%02x%02x%02x%02x%02x%02x%02x%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], mac[6], mac[7]);
    WE_Delay(500);

    uint8_t btMac[6];
    memset(btMac, 0, sizeof(btMac));
    ProteusE_GetBTMAC(btMac);
    printf("BTMAC is 0x%02x%02x%02x%02x%02x%02x\n", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
    WE_Delay(500);

    uint8_t serialNr[3];
    memset(serialNr, 0, sizeof(serialNr));
    ProteusE_GetSerialNumber(serialNr);
    printf("Serial number is 0x%02x%02x%02x\n", serialNr[2], serialNr[1], serialNr[0]);
    WE_Delay(500);

    while (1)
    {
        uint8_t version[3];
        memset(version, 0, sizeof(version));
        ProteusE_GetFWVersion(version);
        WE_Delay(500);

//        ProteusE_PinReset();
//        WE_Delay(500);
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

    if (!ProteusE_Init(PROTEUSE_DEFAULT_BAUDRATE,
                       WE_FlowControl_NoFlowControl,
                       ProteusE_OperationMode_CommandMode,
                       callbackConfig))
    {
        /* Error */
        while (1)
        {
        }
    }

    /* Parameters may be set here if necessary */

    uint8_t deviceName[32];
    uint16_t deviceNameLength;
    if (ProteusE_GetDeviceName(deviceName, &deviceNameLength))
    {
        deviceName[deviceNameLength] = '\0';
        printf("Device name: %s\n", deviceName);
    }

    ProteusE_Deinit();

    uint32_t lastStatusPinLowTick = WE_GetTick();
    bool channelOpen = false;
    bool busy = false;

    if (!ProteusE_Init(PROTEUSE_DEFAULT_BAUDRATE,
                       WE_FlowControl_NoFlowControl,
                       ProteusE_OperationMode_TransparentMode,
                       callbackConfig))
    {
        /* Error */
        while (1)
        {
        }
    }

    /* In transparent mode, all bytes received should be diverted to custom callback OnTransparentModeByteReceived() */
    ProteusE_SetByteRxCallback(OnTransparentModeByteReceived);

    printf("Transparent mode started.\n");

    while (1)
    {
        /* Check connection status and print message on state change (using status i.e. LED_1 pin) */
        bool statusPinState = ProteusE_GetStatusPinLed1Level();
        if (channelOpen)
        {
            if (!statusPinState)
            {
                /* Status pin changed to low - channel is now closed */
                printf("Channel closed.\n");
                channelOpen = false;
            }
        }
        else if (statusPinState)
        {
            if (WE_GetTick() - lastStatusPinLowTick > PROTEUSE_STATUS_LED_CONNECTED_TIMEOUT)
            {
                /* Status pin has been high for at least
                 * PROTEUSE_STATUS_LED_CONNECTED_TIMEOUT ms - channel is now open */
                printf("Channel opened.\n");
                channelOpen = true;
            }
        }
        if (!statusPinState)
        {
            /* Status pin is low - store current tick value (required for checking
             * the duration that the status pin is high) */
            lastStatusPinLowTick = WE_GetTick();
        }

        /* Check state of busy pin and print message on state change */
        bool b = ProteusE_IsTransparentModeBusy();
        if (b != busy)
        {
            busy = b;
            printf("Busy state changed to %s\n", busy ? "true" : "false");
        }

//        WE_Delay(1);
    }
}


static void RxCallback(uint8_t* payload, uint16_t payloadLength, uint8_t* btMac, int8_t rssi)
{
    int i = 0;
    printf("Received data from device with BTMAC (0x%02x%02x%02x%02x%02x%02x) with RSSI = %d dBm:\n-> ",
           btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], rssi);
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
    printf("\n");
    fflush(stdout);
}

static void ConnectCallback(bool success, uint8_t* btMac)
{
    printf("%s to device with BTMAC (0x%02x%02x%02x%02x%02x%02x) ",
           success ? "Connected" : "Failed to connect",
           btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
    printf("\n");
    fflush(stdout);
}

static void SecurityCallback(uint8_t* btMac, ProteusE_SecurityState_t securityState)
{
    static const char *stateStrings[] =
    {
        "rebonded",
        "bonded",
        "paired"
    };

    printf("Encrypted link to device with BTMAC (0x%02x%02x%02x%02x%02x%02x) established (%s)",
           btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5],
           stateStrings[securityState]);
    printf("\n");
    fflush(stdout);
}

static void DisconnectCallback(ProteusE_DisconnectReason_t reason)
{
    static const char *reasonStrings[] =
    {
        "unknown",
        "connection timeout",
        "user terminated connection",
        "host terminated connection",
        "connection interval unacceptable",
        "MIC failure",
        "connection setup failed"
    };

    printf("Disconnected (reason: %s)\n", reasonStrings[reason]);
    fflush(stdout);
}

static void ChannelOpenCallback(uint8_t* btMac, uint16_t maxPayload)
{
    printf("Channel opened to BTMAC (0x%02x%02x%02x%02x%02x%02x) with maximum payload = %d",
           btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], maxPayload);
    printf("\n");
    fflush(stdout);
}

static void PhyUpdateCallback(bool success, uint8_t* btMac, uint8_t phyRx, uint8_t phyTx)
{
    if (success)
    {
        printf("Phy of connection to BTMAC (0x%02x%02x%02x%02x%02x%02x) updated (RX: %dMBit, TX: %dMBit)",
               btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], phyRx, phyTx);
    }
    else
    {
        printf("Failed to update Phy connection");
    }
    printf("\n");
    fflush(stdout);
}

static void SleepCallback()
{
    printf("Advertising timeout detected, will go to sleep now\n");
}

static void GpioWriteCallback(bool remote, uint8_t gpioId, uint8_t value)
{
    printf("GPIO write indication received (remote: %s, GPIO: %u, value: %u)\n",
           remote ? "true" : "false",
           gpioId,
           value);
    fflush(stdout);
}

static void GpioRemoteConfigCallback(ProteusE_GPIOConfigBlock_t *gpioConfig)
{
    static const char *functionStrings[] =
    {
        "disconnected",
        "input",
        "output"
    };

    static const char *pullStrings[] =
    {
        "no pull",
        "pull down",
        "pull up"
    };

    printf("GPIO remote config indication received (GPIO: %u, function: %s",
           gpioConfig->gpioId,
           functionStrings[gpioConfig->function]);

    switch (gpioConfig->function)
    {
    case ProteusE_GPIO_IO_Disconnected:
        break;

    case ProteusE_GPIO_IO_Input:
        printf(", input type: %s", pullStrings[gpioConfig->value.input]);
        break;

    case ProteusE_GPIO_IO_Output:
        printf(", output level: %s", gpioConfig->value.output == ProteusE_GPIO_Output_High ? "HIGH" : "LOW");
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
 * @brief Handles bytes received in transparent mode (is set as custom callback
 * for bytes received from Proteus-e).
 */
static void OnTransparentModeByteReceived(uint8_t receivedByte)
{
    printf("Rx 0x%02x ('%c')\n", receivedByte, receivedByte);

    /* Echo */
    WE_UART_Transmit(&receivedByte, 1);
}
