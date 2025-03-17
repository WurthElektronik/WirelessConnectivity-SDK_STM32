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
 * @brief Calypso transparent mode example.
 */
#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/ATCommands/ATNetCfg.h>
#include <Calypso/ATCommands/ATWLAN.h>
#include <Calypso/Calypso_Examples.h>
#include <Calypso/Calypso_TransparentMode_Example.h>
#include <stdio.h>

/**
 * @brief Max. size of payload used in this example (max. length of transmitted lines)
 */
#define CALYPSO_TRANSPARENT_MODE_EXAMPLE_MAX_PAYLOAD_SIZE 64

/**
 * @brief IPv4 remote address (address of peer to exchange data with)
 */
static char* transparentModeExampleRemoteAddress = "192.168.178.86";

/**
 * @brief Remote port
 */
static uint16_t transparentModeExampleRemotePort = 8888;

/**
 * @brief Local port
 */
static uint16_t transparentModeExampleLocalPort = 8888;

/**
 * @brief Type of socket to use
 */
static Calypso_ATDevice_TransparentModeSocketType_t transparentModeExampleSocketType = Calypso_ATDevice_TransparentModeSocketType_TCPClient;

/**
 * @brief Trigger configuration for transparent mode (trigger for transmitting data)
 */
static uint8_t transparentModeExampleTrigger = Calypso_ATDevice_TransparentModeUartTrigger_OneETX | Calypso_ATDevice_TransparentModeUartTrigger_TransmitETX;

/**
 * @brief Trigger timeout in milliseconds
 * (only applicable if using Calypso_ATDevice_TransparentModeUartTrigger_Timer)
 */
static uint16_t transparentModeExampleTriggerTimeoutMs = 6;

/**
 * @brief First end of transmission character used for transparent mode
 */
static char transparentModeExampleEtx1 = '\r';

/**
 * @brief Second end of transmission character used for transparent mode
 * (only applicable if using Calypso_ATDevice_TransparentModeUartTrigger_TwoETX)
 */
static char transparentModeExampleEtx2 = '\n';

/**
 * @brief Enables power save mode
 */
static bool transparentModeExamplePowerSave = false;

/**
 * @brief Timeout when waiting for incoming data
 */
static uint16_t transparentModeExampleRxTimeoutMs = 500;

/**
 * @brief Is set to true when a complete line has been received (terminated
 * with transparentModeExampleEtx1/2)
 *
 * The received text is stored in transparentModeExampleRxBuffer and
 * has length transparentModeExampleBytesReceived.
 */
static bool transparentModeExampleLineReceived = false;

/**
 * @brief Buffer storing received bytes (last received line)
 */
static uint8_t transparentModeExampleRxBuffer[CALYPSO_TRANSPARENT_MODE_EXAMPLE_MAX_PAYLOAD_SIZE + 1];

/**
 * @brief Number of bytes received (length of text in transparentModeExampleRxBuffer)
 */
static uint16_t transparentModeExampleBytesReceived = 0;

/**
 * @brief Timestamp (tick) of last received byte.
 * Is used to check timeout when waiting for incoming data.
 * Is initialized to time of last request sent.
 */
static uint32_t transparentModeExampleLastByteReceivedTimestamp = 0;

static void Calypso_TransparentModeExampleByteHandler(uint8_t* dataP, size_t size);

/**
 * @brief Transparent mode example.
 *
 * Calypso is first started in AT-Command mode in order to set transparent mode
 * parameters (such as WLAN, socket and trigger parameters).
 * Calypso is then restarted in transparent mode (using the application mode pins) and
 * transmission of data is started.
 *
 * Note that for this example to work, it is required to connect the Calypso's
 * application mode pins APP_MODE_0 and APP_MODE_1 as well as the status pins
 * STATUS_IND_0 and STATUS_IND_1
 */
void Calypso_TransparentMode_Example(void)
{
    WE_DEBUG_PRINT("*** Start of Calypso transparent mode example ***\r\n");

    bool ret = false;

    if (!Calypso_Init(&Calypso_uart, &Calypso_pins, &Calypso_Examples_EventCallback))
    {
        WE_DEBUG_PRINT("Initialization error\r\n");
        return;
    }

    Calypso_PinReset();

    Calypso_Examples_WaitForStartup(5000);

    WE_Delay(1000);

    /* Get version info. This retrieves Calypso's firmware version (amongst other version info) and
	 * stores the firmware version in Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
	 * Calypso_firmwareVersionPatch for later use. */
    Calypso_ATDevice_Value_t deviceValue;
    ret = Calypso_ATDevice_Get(Calypso_ATDevice_GetId_General, Calypso_ATDevice_GetGeneral_Version, &deviceValue);
    Calypso_Examples_Print("Get device version", ret);

    /* Set IPv4 address method DHCP */
    Calypso_ATNetCfg_IPv4Config_t newIpV4Config = {0};
    newIpV4Config.method = Calypso_ATNetCfg_IPv4Method_Dhcp;
    ret = Calypso_ATNetCfg_SetIPv4AddressStation(&newIpV4Config);
    Calypso_Examples_Print("Set DHCP mode", ret);

    /* WLAN station mode */
    ret = Calypso_ATWLAN_SetMode(Calypso_ATWLAN_SetMode_Station);
    Calypso_Examples_Print("Set WLAN mode to station", ret);

    ret = Calypso_ATDevice_Restart(0);
    Calypso_Examples_Print("Restart network processor", ret);

    WE_Delay(1000);

    /* Set connection policy to AUTO|FAST */
    ret = Calypso_ATWLAN_SetConnectionPolicy(Calypso_ATWLAN_PolicyConnection_Auto | Calypso_ATWLAN_PolicyConnection_Fast);
    Calypso_Examples_Print("Set WLAN connection policy to AUTO|FAST", ret);

    /* Connect to WLAN.
	 * When starting in transparent mode later on, Calypso will connect to the last used WLAN.
	 * Alternatively, one or more WLAN profiles containing connection credentials (of
	 * networks to be used in transparent mode) can be added. */
    Calypso_ATWLAN_ConnectionArguments_t connectArgs;
    memset(&connectArgs, 0, sizeof(connectArgs));
    strcpy(connectArgs.SSID, Calypso_Examples_wlanSSID);
    connectArgs.securityParams.securityType = Calypso_ATWLAN_SecurityType_WPA_WPA2;
    strcpy(connectArgs.securityParams.securityKey, Calypso_Examples_wlanKey);

    ret = Calypso_ATWLAN_Connect(connectArgs);
    Calypso_Examples_Print("Connect to WLAN", ret);

    /* Wait for WLAN connection to be established */
    Calypso_Examples_WaitForIPv4Acquired(5000);

    /* Get IPv4 configuration (station) */
    Calypso_ATNetCfg_IPv4Config_t ipV4Config;
    ret = Calypso_ATNetCfg_GetIPv4AddressStation(&ipV4Config);
    Calypso_Examples_Print("AT+netCfgGet=IPV4_STA_ADDR", ret);
    if (ret)
    {
        WE_DEBUG_PRINT("*** Station IPv4 configuration ***\r\n");
        WE_DEBUG_PRINT("IPv4 address: %s\r\n", ipV4Config.ipAddress);
        WE_DEBUG_PRINT("Subnet mask: %s\r\n", ipV4Config.subnetMask);
        WE_DEBUG_PRINT("Gateway address: %s\r\n", ipV4Config.gatewayAddress);
        WE_DEBUG_PRINT("DNS address: %s\r\n", ipV4Config.dnsAddress);
    }

    /* Set transparent mode parameters */
    memset(&deviceValue, 0, sizeof(deviceValue));
    strcpy(deviceValue.transparentMode.remoteAddress, transparentModeExampleRemoteAddress);
    ret = Calypso_ATDevice_Set(Calypso_ATDevice_GetId_TransparentMode, Calypso_ATDevice_GetTransparentMode_RemoteAddress, &deviceValue);
    Calypso_Examples_Print("Set remote address", ret);
    memset(&deviceValue, 0, sizeof(deviceValue));
    deviceValue.transparentMode.remotePort = transparentModeExampleRemotePort;
    ret = Calypso_ATDevice_Set(Calypso_ATDevice_GetId_TransparentMode, Calypso_ATDevice_GetTransparentMode_RemotePort, &deviceValue);
    Calypso_Examples_Print("Set remote port", ret);
    memset(&deviceValue, 0, sizeof(deviceValue));
    deviceValue.transparentMode.localPort = transparentModeExampleLocalPort;
    ret = Calypso_ATDevice_Set(Calypso_ATDevice_GetId_TransparentMode, Calypso_ATDevice_GetTransparentMode_LocalPort, &deviceValue);
    Calypso_Examples_Print("Set local port", ret);
    memset(&deviceValue, 0, sizeof(deviceValue));
    deviceValue.transparentMode.socketType = transparentModeExampleSocketType;
    ret = Calypso_ATDevice_Set(Calypso_ATDevice_GetId_TransparentMode, Calypso_ATDevice_GetTransparentMode_SocketType, &deviceValue);
    Calypso_Examples_Print("Set socket type", ret);
    memset(&deviceValue, 0, sizeof(deviceValue));
    deviceValue.transparentMode.secureMethod = Calypso_ATDevice_TransparentModeSecureMethod_None;
    ret = Calypso_ATDevice_Set(Calypso_ATDevice_GetId_TransparentMode, Calypso_ATDevice_GetTransparentMode_SecureMethod, &deviceValue);
    Calypso_Examples_Print("Set secure method", ret);
    memset(&deviceValue, 0, sizeof(deviceValue));
    deviceValue.transparentMode.powerSave = transparentModeExamplePowerSave;
    ret = Calypso_ATDevice_Set(Calypso_ATDevice_GetId_TransparentMode, Calypso_ATDevice_GetTransparentMode_PowerSave, &deviceValue);
    Calypso_Examples_Print("Set power save parameter", ret);
    memset(&deviceValue, 0, sizeof(deviceValue));
    deviceValue.transparentMode.skipDateVerify = false;
    ret = Calypso_ATDevice_Set(Calypso_ATDevice_GetId_TransparentMode, Calypso_ATDevice_GetTransparentMode_SkipDateVerify, &deviceValue);
    Calypso_Examples_Print("Set skip date verify", ret);
    memset(&deviceValue, 0, sizeof(deviceValue));
    deviceValue.transparentMode.disableCertificateStore = false;
    ret = Calypso_ATDevice_Set(Calypso_ATDevice_GetId_TransparentMode, Calypso_ATDevice_GetTransparentMode_DisableCertificateStore, &deviceValue);
    Calypso_Examples_Print("Set disable certificate store", ret);

    memset(&deviceValue, 0, sizeof(deviceValue));
    deviceValue.uart.transparentTrigger = transparentModeExampleTrigger;
    ret = Calypso_ATDevice_Set(Calypso_ATDevice_GetId_UART, Calypso_ATDevice_GetUart_TransparentTrigger, &deviceValue);
    Calypso_Examples_Print("Set transparent trigger", ret);
    memset(&deviceValue, 0, sizeof(deviceValue));
    deviceValue.uart.transparentTimeoutMs = transparentModeExampleTriggerTimeoutMs;
    ret = Calypso_ATDevice_Set(Calypso_ATDevice_GetId_UART, Calypso_ATDevice_GetUart_TransparentTimeout, &deviceValue);
    Calypso_Examples_Print("Set transparent timeout", ret);
    memset(&deviceValue, 0, sizeof(deviceValue));
    deviceValue.uart.transparentETX[0] = transparentModeExampleEtx1;
    deviceValue.uart.transparentETX[1] = transparentModeExampleEtx2;
    ret = Calypso_ATDevice_Set(Calypso_ATDevice_GetId_UART, Calypso_ATDevice_GetUart_TransparentETX, &deviceValue);
    Calypso_Examples_Print("Set transparent ETX", ret);

    /* Set app mode pins to enable transparent mode after reset */
    ret = Calypso_SetApplicationModePins(Calypso_ApplicationMode_TransparentMode);
    Calypso_Examples_Print("Set app mode pins to transparent mode", ret);

    /* Reset Calypso (will enter transparent mode after restart) */
    ret = Calypso_PinReset();
    Calypso_Examples_Print("Reset Calypso", ret);

    /* Must wait for startup */
    Calypso_Examples_WaitForStartup(2000);

    /* Wait for connection of WLAN and socket (wait for status pins to turn high) */
    WE_DEBUG_PRINT("Will now wait for STATUS_IND_0 pin to turn high (WLAN connected)...\r\n");
    WE_Pin_Level_t status_ind_0_pin_level;
    while (Calypso_GetPinLevel(Calypso_pins.Calypso_Pin_StatusInd0, &status_ind_0_pin_level) && (status_ind_0_pin_level == WE_Pin_Level_Low))
    {
    }
    Calypso_Examples_Print("Wait for STATUS_IND_0 pin to turn high", true);

    WE_DEBUG_PRINT("Will now wait for STATUS_IND_1 pin to turn high (socket connected)...\r\n");
    WE_Pin_Level_t status_ind_1_pin_level;
    while (Calypso_GetPinLevel(Calypso_pins.Calypso_Pin_StatusInd0, &status_ind_1_pin_level) && (status_ind_1_pin_level == WE_Pin_Level_Low))
    {
    }
    Calypso_Examples_Print("Wait for STATUS_IND_1 pin to turn high", true);

    char payload[CALYPSO_TRANSPARENT_MODE_EXAMPLE_MAX_PAYLOAD_SIZE];
    transparentModeExampleBytesReceived = 0;
    transparentModeExampleLineReceived = false;

    /* Set byte received callback. This circumvents the SDK's regular processing of
	 * incoming data (which the SDK would interpret as AT commands / responses) and
	 * instead calls the custom callback for every byte received. */
    Calypso_SetByteRxCallback(Calypso_TransparentModeExampleByteHandler);

    uint16_t counter = 0;

    /* Start data transmission. In this example, it is assumed that the peer will
	 * send a response for each line sent by this device. Additionally, it is assumed
	 * that the response is terminated the same way as the request (i.e. one or two
	 * ETX characters). If the response is not terminated using the ETX character(s),
	 * it is assumed that all data has been received if a timeout is reached. */
    while (true)
    {
        sprintf(payload, "Hello Calypso (%u)!", counter++);

        /* Reset tick value to check for rx timeout */
        transparentModeExampleLastByteReceivedTimestamp = WE_GetTick();

        if (transparentModeExamplePowerSave)
        {
            /* Wake up Calypso so that it can forward the data that we are about to send */
            Calypso_PinWakeUp();

            /* Guard interval is required before sending data (wake-up signal is HIGH for 5ms + 5ms delay = 10ms in total)  */
            WE_Delay(5);
        }

        /* Transmit payload (plain data, directly written to UART) */
        Calypso_Transparent_Transmit(payload, strlen(payload));

        /* Transmit ETX character(s), if enabled */
        if (0 != (transparentModeExampleTrigger & Calypso_ATDevice_TransparentModeUartTrigger_OneETX) || 0 != (transparentModeExampleTrigger & Calypso_ATDevice_TransparentModeUartTrigger_TwoETX))
        {
            Calypso_Transparent_Transmit(&transparentModeExampleEtx1, 1);
        }
        if (0 != (transparentModeExampleTrigger & Calypso_ATDevice_TransparentModeUartTrigger_TwoETX))
        {
            Calypso_Transparent_Transmit(&transparentModeExampleEtx2, 1);
        }

        /* Wait until either a complete line has been received (ETX character(s) have been
		 * detected) or no bytes have been received for transparentModeExampleRxTimeoutMs
		 * milliseconds. */
        while (!transparentModeExampleLineReceived && (WE_GetTick() - transparentModeExampleLastByteReceivedTimestamp) < transparentModeExampleRxTimeoutMs)
        {
        }

        /* Print received text (if any) */
        if (transparentModeExampleBytesReceived > 0)
        {
            transparentModeExampleRxBuffer[transparentModeExampleBytesReceived] = '\0';
            WE_DEBUG_PRINT("Received \"%s\"\r\n", transparentModeExampleRxBuffer);
        }

        transparentModeExampleBytesReceived = 0;
        transparentModeExampleLineReceived = false;

        /* 1s delay */
        WE_Delay(1000);

        /* Check if still connected */
        if ((Calypso_GetPinLevel(Calypso_pins.Calypso_Pin_StatusInd0, &status_ind_0_pin_level) && (status_ind_0_pin_level == WE_Pin_Level_Low)) || (Calypso_GetPinLevel(Calypso_pins.Calypso_Pin_StatusInd0, &status_ind_1_pin_level) && (status_ind_1_pin_level == WE_Pin_Level_Low)))
        {
            WE_DEBUG_PRINT("ERROR: Connection to peer lost (WLAN or socket disconnected). Will now wait for reconnect.\r\n");
            while ((Calypso_GetPinLevel(Calypso_pins.Calypso_Pin_StatusInd0, &status_ind_0_pin_level) && (status_ind_0_pin_level == WE_Pin_Level_Low)) || (Calypso_GetPinLevel(Calypso_pins.Calypso_Pin_StatusInd0, &status_ind_1_pin_level) && (status_ind_1_pin_level == WE_Pin_Level_Low)))
            {
            }
            WE_DEBUG_PRINT("Reconnected!\r\n");

            /* Guard interval (required especially when power save mode is active) */
            WE_Delay(5);
        }
    }

    Calypso_Deinit();
}

/**
 * @brief Is called when a byte has been received in transparent mode.
 */
void Calypso_TransparentModeExampleByteHandler(uint8_t* dataP, size_t size)
{
    uint8_t byte;
    for (; size > 0; size--, dataP++)
    {
        byte = *dataP;
        if (transparentModeExampleLineReceived)
        {
            /* Ignore all received data until the previous line has been read */
            return;
        }

        /* Store current tick value for checking timeout (see Calypso_TransparentMode_Example()) */
        transparentModeExampleLastByteReceivedTimestamp = WE_GetTick();

        if (transparentModeExampleBytesReceived < CALYPSO_TRANSPARENT_MODE_EXAMPLE_MAX_PAYLOAD_SIZE)
        {
            /* Store received byte in rx buffer */
            transparentModeExampleRxBuffer[transparentModeExampleBytesReceived] = byte;
            transparentModeExampleBytesReceived++;

            /* Check if line is complete and set flag if so */
            if (0 != (transparentModeExampleTrigger & Calypso_ATDevice_TransparentModeUartTrigger_OneETX))
            {
                if (byte == transparentModeExampleEtx1)
                {
                    transparentModeExampleLineReceived = true;
                }
            }
            else if (0 != (transparentModeExampleTrigger & Calypso_ATDevice_TransparentModeUartTrigger_TwoETX) && transparentModeExampleBytesReceived > 1)
            {
                if (byte == transparentModeExampleEtx2 && transparentModeExampleRxBuffer[transparentModeExampleBytesReceived - 2] == transparentModeExampleEtx1)
                {
                    transparentModeExampleLineReceived = true;
                }
            }
        }
    }
}
