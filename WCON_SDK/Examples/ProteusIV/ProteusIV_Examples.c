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
 * @brief Proteus-IV example.
 */

#include <ProteusIV/ProteusIV.h>
#include <global/global.h>
#include <global_platform.h>
#include <print.h>
#include <stdio.h>
#include <string.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))

/* Proteus-IV examples. Pick the example to be executed in the main function. */
static void Peripheral_CommandMode_Example();
static void Central_CommandMode_Example();
static void Peripheral_TransparentModeExample();

static void Test_UsersettingsExample();
static void Test_OtherfunctionsExample();

static bool ConfigureModule();

/* Callback functions for various indications sent by the Proteus-IV. */
static void RxCallback(uint8_t conn_ID, uint8_t* payload, uint16_t payloadLength, int8_t rssi);
static void ConnectCallback(uint8_t conn_ID, bool success, ProteusIV_Bluetooth_Address_Type_t type, uint8_t* btMac);
static void MaxPayloadCallback(uint8_t conn_ID, uint16_t max_payload);
static void SecurityCallback(uint8_t conn_ID, ProteusIV_SecurityState_t securityState);
static void PasskeyCallback(uint8_t conn_ID);
static void DisplayPasskeyCallback(uint8_t conn_ID, ProteusIV_DisplayPasskeyAction_t action, uint8_t* passkey);
static void DisconnectCallback(uint8_t conn_ID, ProteusIV_DisconnectReason_t reason);
static void LinkOpenCallback(uint8_t conn_ID, bool link_open);
static void PhyUpdateCallback(uint8_t conn_ID, uint8_t phyTx, uint8_t phyRx);
static void ScanCallback(ProteusIV_Bluetooth_Address_Type_t type, uint8_t* btMac, int8_t rssi, char* devicename);
static void OnTransparentModeByteReceived(uint8_t* dataP, size_t size);

/* Is set to true when a pass key indication has been received. */
static bool passkeyRequestReceived = false;

/* Is set to true when a display pass key indication has been received. */
static bool displayPasskeyRequestReceived = false;

/* Conn_ID that requested authentication */
static uint8_t authentication_conn_ID = 0;

/**
 * @brief Definition of the pins
 */
static ProteusIV_Pins_t ProteusIV_pins = {
    .ProteusIV_Pin_Reset = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_10)),
    .ProteusIV_Pin_Mode0 = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_7)),
    .ProteusIV_Pin_Mode1 = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_8)),
    .ProteusIV_Pin_Led1 = WE_PIN((void*)&WE_STM32_PIN(GPIOB, GPIO_PIN_9)),
    .ProteusIV_UART_Enable = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_0)),
};

/**
 * @brief Definition of the uart
 */
static WE_UART_t ProteusIV_uart;

/**
 * @brief The application's main function.
 */
void ProteusIV_Examples(void)
{
    ProteusIV_uart.baudrate = PROTEUSIV_DEFAULT_BAUDRATE;
    ProteusIV_uart.flowControl = WE_FlowControl_RTSAndCTS;
    ProteusIV_uart.parity = WE_Parity_None;
    ProteusIV_uart.uartInit = WE_UART1_Init;
    ProteusIV_uart.uartDeinit = WE_UART1_DeInit;
    ProteusIV_uart.uartTransmit = WE_UART1_Transmit;

    //Peripheral_CommandMode_Example();
    //Central_CommandMode_Example();
    Peripheral_TransparentModeExample();

    //Test_OtherfunctionsExample();
    //Test_UsersettingsExample();

    return;
}

/**
 * @brief Prints the supplied string, prefixed with OK or NOK (depending on the success parameter).
 *
 * @param[in] str: String to print
 * @param[in] success: Variable indicating if action was ok
 */
static void Examples_Print(char* str, bool success) { WE_APP_PRINT("%s%s\r\n", success ? "OK    " : "NOK   ", str); }

/**
 * @brief Proteus-IV peripheral command mode example.
 *
 * @details: The radio module acts as peripheral and waits for an incoming connection. Once the Bluetooth LE link is open, it sends data.
 * Since the module is in command mode, it can react to all events (i.e. for secure connection setup).
 * As peer device use another Proteus-IV radio module (Central_CommandMode_Example), or the WE Bluetooth LE terminal app.
 */
static void Peripheral_CommandMode_Example()
{
    WE_APP_PRINT("Peripheral_CommandMode_Example started\r\n");
    bool ret = false;

    ProteusIV_CallbackConfig_t callbackConfig = {0};
    callbackConfig.rxCb = RxCallback;
    callbackConfig.connectCb = ConnectCallback;
    callbackConfig.maxPayloadCb = MaxPayloadCallback;
    callbackConfig.disconnectCb = DisconnectCallback;
    callbackConfig.linkOpenCb = LinkOpenCallback;
    callbackConfig.securityCb = SecurityCallback;
    callbackConfig.passkeyCb = PasskeyCallback;
    callbackConfig.displayPasskeyCb = DisplayPasskeyCallback;
    callbackConfig.phyUpdateCb = PhyUpdateCallback;
    callbackConfig.scanCb = ScanCallback;

    if (false == ProteusIV_Init(&ProteusIV_uart, &ProteusIV_pins, ProteusIV_OperationMode_CommandMode, callbackConfig))
    {
        WE_APP_PRINT("Initialisation error\r\n");
        return;
    }

    ret = ProteusIV_SetSecFlags(ProteusIV_SecFlags_NO_IO_MINPERIPHERAL_LEVEL_L2);
    Examples_Print("Set SecFlags", ret);
    WE_Delay(500);

    uint8_t data[] = "Peripheral Example";

    while (1)
    {
        if (passkeyRequestReceived)
        {
            /* Respond to passkey request */

            passkeyRequestReceived = false;

            /* default pass key */
            uint8_t passkey[6] = {'1', '2', '3', '1', '2', '3'};
#pragma message("Please use the passkey of the connection here displayed by the peer device")
            ret = ProteusIV_Passkey(authentication_conn_ID, passkey);
            Examples_Print("Enter passkey", ret);
        }

        if (displayPasskeyRequestReceived)
        {
            /* Respond to display passkey request */

            displayPasskeyRequestReceived = false;

            /* Confirm key */
            ret = ProteusIV_NumericCompareConfirm(authentication_conn_ID, true);
            Examples_Print("Numeric comparison confirmation", ret);
        }

        bool link_open = false;

        /* transmit data to all open connections */
        for (int conn_ID = 0; conn_ID < PROTEUSIV_MAX_NUMBER_OF_CONNECTEDDEVICES; conn_ID++)
        {
            if (ProteusIV_connection_list[conn_ID].is_linkopen)
            {
                link_open = true;
                ret = ProteusIV_Transmit(conn_ID, data, MIN(sizeof(data), ProteusIV_connection_list[conn_ID].max_payload));
                Examples_Print("ProteusIV_Transmit", ret);
                WE_APP_PRINT("-> to Connection ID %d", conn_ID);
                WE_Delay(500);
            }
        }

        if (!link_open)
        {
            WE_APP_PRINT("Waiting for link to open");
            WE_Delay(2000);
        }
    }
}

/**
 * @brief Proteus-IV central command mode example.
 *
 * @details: The radio module acts as central and connects to the first Proteus-IV module found during scan. Once the Bluetooth LE link is open, it sends data.
 * Since the module is in command mode, it can react to all events (i.e. for secure connection setup).
 * As peer device use another Proteus-IV radio module (Peripheral_CommandMode_Example).
 */
static void Central_CommandMode_Example()
{
    WE_APP_PRINT("Central_CommandMode_Example started\r\n");
    bool ret = false;

    uint8_t data[] = "Central Example";

    ProteusIV_CallbackConfig_t callbackConfig = {0};
    callbackConfig.rxCb = RxCallback;
    callbackConfig.connectCb = ConnectCallback;
    callbackConfig.maxPayloadCb = MaxPayloadCallback;
    callbackConfig.disconnectCb = DisconnectCallback;
    callbackConfig.linkOpenCb = LinkOpenCallback;
    callbackConfig.securityCb = SecurityCallback;
    callbackConfig.passkeyCb = PasskeyCallback;
    callbackConfig.displayPasskeyCb = DisplayPasskeyCallback;
    callbackConfig.phyUpdateCb = PhyUpdateCallback;
    callbackConfig.scanCb = ScanCallback;

    if (false == ProteusIV_Init(&ProteusIV_uart, &ProteusIV_pins, ProteusIV_OperationMode_CommandMode, callbackConfig))
    {
        WE_APP_PRINT("Initialisation error\r\n");
        return;
    }

    ret = ProteusIV_SetSecFlags(ProteusIV_SecFlags_NO_IO_MINPERIPHERAL_LEVEL_L2);
    Examples_Print("Set SecFlags", ret);
    WE_Delay(500);

    ret = ProteusIV_AdvertisingStop();
    Examples_Print("Turn off advertising", ret);
    WE_Delay(500);

    while (1)
    {
        bool is_linkopen = false;
        bool connect = false;
        for (int conn_ID = 0; conn_ID < PROTEUSIV_MAX_NUMBER_OF_CONNECTEDDEVICES; conn_ID++)
        {
            if (ProteusIV_connection_list[conn_ID].is_linkopen)
            {
                is_linkopen = true;
                connect = false;
                ret = ProteusIV_Transmit(conn_ID, data, MIN(sizeof(data), ProteusIV_connection_list[conn_ID].max_payload));
                Examples_Print("ProteusIV_Transmit", ret);
                WE_APP_PRINT("-> to Connection ID %d", conn_ID);
                WE_Delay(500);
            }
        }

        ProteusIV_ScanResult_t scanresult = {.numberOfDevices = 0};
        while ((scanresult.numberOfDevices == 0) && (is_linkopen == false))
        {
            ret = ProteusIV_ScanStart(&scanresult);
            Examples_Print("Scan start", ret);
            WE_Delay(2500);

            ret = ProteusIV_ScanStop();
            Examples_Print("Scan stop", ret);

            WE_APP_PRINT("%d device(s) found\r\n", scanresult.numberOfDevices);
            for (uint8_t i = 0; i < PROTEUSIV_MAX_NUMBER_OF_SCANDEVICES; i++)
            {
                if (scanresult.devices[i].inUse)
                {
                    WE_APP_PRINT("Device number %d with BTMAC 0x%02x%02x%02x%02x%02x%02x\r\n", i, scanresult.devices[i].btMac[0], scanresult.devices[i].btMac[1], scanresult.devices[i].btMac[2], scanresult.devices[i].btMac[3], scanresult.devices[i].btMac[4], scanresult.devices[i].btMac[5]);
                }
            }
        }

        /* found an advertising Proteus-IV to connect to */
        if ((scanresult.numberOfDevices > 0) && (is_linkopen == false) && (connect == false))
        {
            connect = false;
            for (uint8_t i = 0; i < PROTEUSIV_MAX_NUMBER_OF_SCANDEVICES; i++)
            {
                if (scanresult.devices[i].inUse)
                {
                    WE_APP_PRINT("Connect to BTMAC 0x%02x%02x%02x%02x%02x%02x\r\n", scanresult.devices[i].btMac[0], scanresult.devices[i].btMac[1], scanresult.devices[i].btMac[2], scanresult.devices[i].btMac[3], scanresult.devices[i].btMac[4], scanresult.devices[i].btMac[5]);

                    if (ProteusIV_Connect(scanresult.devices[i].addr_type, scanresult.devices[i].btMac))
                    {
                        connect = true;
                        WE_Delay(2500);
                        break;
                    }
                }
            }
        }

        if (passkeyRequestReceived)
        {
            /* Respond to passkey request */

            passkeyRequestReceived = false;

            /* default pass key */
            uint8_t passkey[6] = {'1', '2', '3', '1', '2', '3'};
#pragma message("Please use the passkey of the connection here displayed by the peer device")
            ret = ProteusIV_Passkey(authentication_conn_ID, passkey);
            Examples_Print("Enter passkey", ret);
        }

        if (displayPasskeyRequestReceived)
        {
            /* Respond to display passkey request */

            displayPasskeyRequestReceived = false;

            /* Confirm key */
            ret = ProteusIV_NumericCompareConfirm(authentication_conn_ID, true);
            Examples_Print("Numeric comparison confirmation", ret);
        }
    }
}

/**
 * @brief Proteus-IV user settings example.
 *
 * @details: This example demonstrates how to use the functions to read and modify the user settings.
 * At the end a factory reset is applied, resetting all the settings.
 */
static void Test_UsersettingsExample()
{
    WE_APP_PRINT("Test_UsersettingsExample started\r\n");

    bool ret = false;
    ProteusIV_CallbackConfig_t callbackConfig = {0};

    if (false == ProteusIV_Init(&ProteusIV_uart, &ProteusIV_pins, ProteusIV_OperationMode_CommandMode, callbackConfig))
    {
        WE_APP_PRINT("Initialisation error\r\n");
        return;
    }

    ProteusIV_DeviceInfo_t deviceInfo;
    if (ProteusIV_GetDeviceInfo(&deviceInfo))
    {
        WE_APP_PRINT("Device info OS version = 0x%06lx, "
                     "build code = 0x%08lx, "
                     "package variant = 0x%04x, "
                     "chip ID = 0x%08lx\r\n",
                     deviceInfo.osVersion, deviceInfo.buildCode, deviceInfo.packageVariant, deviceInfo.chipId);
    }

    uint8_t fwVersion[3];
    ret = ProteusIV_GetFWVersion(fwVersion);
    Examples_Print("Get firmware version", ret);
    WE_APP_PRINT("Firmware version is %u.%u.%u\r\n", fwVersion[2], fwVersion[1], fwVersion[0]);
    WE_Delay(500);

    uint16_t min_adv, max_adv;
    ret = ProteusIV_GetAdvertisingInterval(&min_adv, &max_adv);
    Examples_Print("Get advertising interval", ret);
    WE_APP_PRINT("%d - %d\r\n", min_adv, max_adv);
    ret = ProteusIV_SetAdvertisingInterval(MSEC_TO_625UNITS(30), MSEC_TO_625UNITS(100));
    Examples_Print("Set advertising interval", ret);
    ret = ProteusIV_GetAdvertisingInterval(&min_adv, &max_adv);
    Examples_Print("Get advertising interval", ret);
    WE_APP_PRINT("%d - %d\r\n", min_adv, max_adv);
    WE_Delay(500);

    ret = ProteusIV_GetConnectionInterval(&min_adv, &max_adv);
    Examples_Print("Get connection interval", ret);
    WE_APP_PRINT("%d - %d\r\n", min_adv, max_adv);
    ret = ProteusIV_SetConnectionInterval(MSEC_TO_1250UNITS(15), MSEC_TO_1250UNITS(30));
    Examples_Print("Set connection interval", ret);
    ret = ProteusIV_GetConnectionInterval(&min_adv, &max_adv);
    Examples_Print("Get connection interval", ret);
    WE_APP_PRINT("%d - %d\r\n", min_adv, max_adv);
    WE_Delay(500);

    uint8_t mac[8];
    ret = ProteusIV_GetMAC(mac);
    Examples_Print("Get MAC", ret);
    WE_APP_PRINT("MAC is 0x%02x%02x%02x%02x%02x%02x%02x%02x\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], mac[6], mac[7]);
    WE_Delay(500);

    uint8_t serialNr[3];
    ret = ProteusIV_GetSerialNumber(serialNr);
    Examples_Print("Get serial number", ret);
    WE_APP_PRINT("Serial number is 0x%02x%02x%02x\r\n", serialNr[2], serialNr[1], serialNr[0]);
    WE_Delay(500);

    ProteusIV_BaudRate_t baudrate;
    ProteusIV_UartParity_t parity;
    bool flowControlEnable;
    ret = ProteusIV_GetBaudrateIndex(&baudrate, &parity, &flowControlEnable);
    Examples_Print("Read baudrate settings", ret);
    WE_APP_PRINT("Baudrate index %d, Parity %s, flow control %s\r\n", baudrate, (parity == ProteusIV_UartParity_Even) ? "even" : "none", flowControlEnable ? "enabled" : "disabled");
    WE_Delay(500);

    uint8_t deviceName[32];
    uint16_t nameLength = sizeof(deviceName);
    ret = ProteusIV_GetDeviceName(deviceName, &nameLength);
    deviceName[nameLength] = '\0';
    Examples_Print("Get device name", ret);
    WE_APP_PRINT("Device name is %s\r\n", deviceName);
    sprintf((char*)deviceName, "%s", "Newname");
    ret = ProteusIV_SetDeviceName(deviceName, strlen((char*)deviceName));
    Examples_Print("Set device name", ret);
    nameLength = sizeof(deviceName);
    ret = ProteusIV_GetDeviceName(deviceName, &nameLength);
    deviceName[nameLength] = '\0';
    Examples_Print("Get device name", ret);
    WE_APP_PRINT("Device name is %s\r\n", deviceName);
    WE_Delay(500);

    uint8_t btMac[PROTEUSIV_BTMAC_LENGTH];
    ret = ProteusIV_GetBTMAC(btMac);
    Examples_Print("Get BT MAC", ret);
    WE_APP_PRINT("BTMAC is 0x%02x%02x%02x%02x%02x%02x\r\n", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
    btMac[0] = 0x00;
    btMac[1] = 0x11;
    btMac[2] = 0x22;
    ret = ProteusIV_SetBTMAC(btMac);
    Examples_Print("Set BT MAC", ret);
    ret = ProteusIV_GetBTMAC(btMac);
    Examples_Print("Get BT MAC", ret);
    WE_APP_PRINT("BTMAC is 0x%02x%02x%02x%02x%02x%02x\r\n", btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
    WE_Delay(500);

    ProteusIV_TXPower_t txPower;
    ret = ProteusIV_GetTXPower(&txPower);
    Examples_Print("Get TX power", ret);
    WE_APP_PRINT("TX power index is %d\r\n", txPower);
    ret = ProteusIV_SetTXPower(0);
    Examples_Print("Set TX power", ret);
    ret = ProteusIV_GetTXPower(&txPower);
    Examples_Print("Get TX power", ret);
    WE_APP_PRINT("TX power index is %d\r\n", txPower);
    WE_Delay(500);

    ProteusIV_SecFlags_t secFlags;
    ret = ProteusIV_GetSecFlags(&secFlags);
    Examples_Print("Get SecFlags", ret);
    WE_APP_PRINT("SecFlags is %d\r\n", secFlags);
    ret = ProteusIV_SetSecFlags(ProteusIV_SecFlags_NO_IO_MINPERIPHERAL_LEVEL_L2);
    Examples_Print("Set SecFlags", ret);
    ret = ProteusIV_GetSecFlags(&secFlags);
    Examples_Print("Get SecFlags", ret);
    WE_APP_PRINT("Sec flags is %d\r\n", secFlags);
    WE_Delay(500);

    uint8_t staticPasskey[PROTEUSIV_PASSKEY_LENGTH];
    ret = ProteusIV_GetStaticPasskey(staticPasskey);
    Examples_Print("Get static passkey", ret);
    WE_APP_PRINT("Static passkey is %c%c%c%c%c%c\r\n", staticPasskey[0], staticPasskey[1], staticPasskey[2], staticPasskey[3], staticPasskey[4], staticPasskey[5]);
    staticPasskey[0] = '9';
    ret = ProteusIV_SetStaticPasskey(staticPasskey);
    Examples_Print("Set static passkey", ret);
    ret = ProteusIV_GetStaticPasskey(staticPasskey);
    Examples_Print("Get static passkey", ret);
    WE_APP_PRINT("Static passkey is %c%c%c%c%c%c\r\n", staticPasskey[0], staticPasskey[1], staticPasskey[2], staticPasskey[3], staticPasskey[4], staticPasskey[5]);
    WE_Delay(500);

    uint16_t appearance;
    ret = ProteusIV_GetAppearance(&appearance);
    Examples_Print("Get appearance", ret);
    WE_APP_PRINT("Appearance is %d\r\n", appearance);
    ret = ProteusIV_SetAppearance(128); //BLE_APPEARANCE_GENERIC_COMPUTER
    Examples_Print("Set appearance", ret);
    ret = ProteusIV_GetAppearance(&appearance);
    Examples_Print("Get appearance", ret);
    WE_APP_PRINT("Appearance is %d\r\n", appearance);
    WE_Delay(500);

    uint8_t max;
    ret = ProteusIV_GetMaxPeripheralConnections(&max);
    Examples_Print("Get max peripheral connections", ret);
    WE_APP_PRINT("Max peripheral connections is %d\r\n", max);
    ret = ProteusIV_SetMaxPeripheralConnections(2);
    Examples_Print("Set max peripheral connections", ret);
    ret = ProteusIV_GetMaxPeripheralConnections(&max);
    Examples_Print("Get max peripheral connections", ret);
    WE_APP_PRINT("Max peripheral connections is %d\r\n", max);
    WE_Delay(500);

    uint8_t uuid[PROTEUSIV_UUID_LENGTH];
    ret = ProteusIV_GetSppServiceUuid(uuid);
    Examples_Print("Get service UUID", ret);
    WE_APP_PRINT("Service UUID is 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\r\n", uuid[0], uuid[1], uuid[2], uuid[3], uuid[4], uuid[5], uuid[6], uuid[7], uuid[8], uuid[9], uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
    uuid[0] = 0x67;
    ret = ProteusIV_SetSppServiceUuid(uuid);
    Examples_Print("Set service UUID", ret);
    ret = ProteusIV_GetSppServiceUuid(uuid);
    Examples_Print("Get service UUID", ret);
    WE_APP_PRINT("Service UUID is 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\r\n", uuid[0], uuid[1], uuid[2], uuid[3], uuid[4], uuid[5], uuid[6], uuid[7], uuid[8], uuid[9], uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
    WE_Delay(500);

    ret = ProteusIV_GetSppRxUuid(uuid);
    Examples_Print("Get RX UUID", ret);
    WE_APP_PRINT("RX UUID is 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\r\n", uuid[0], uuid[1], uuid[2], uuid[3], uuid[4], uuid[5], uuid[6], uuid[7], uuid[8], uuid[9], uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
    uuid[0] = 0x67;
    ret = ProteusIV_SetSppRxUuid(uuid);
    Examples_Print("Set RX UUID", ret);
    ret = ProteusIV_GetSppRxUuid(uuid);
    Examples_Print("Get RX UUID", ret);
    WE_APP_PRINT("RX UUID is 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\r\n", uuid[0], uuid[1], uuid[2], uuid[3], uuid[4], uuid[5], uuid[6], uuid[7], uuid[8], uuid[9], uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
    WE_Delay(500);

    ret = ProteusIV_GetSppTxUuid(uuid);
    Examples_Print("Get TX UUID", ret);
    WE_APP_PRINT("TX UUID is 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\r\n", uuid[0], uuid[1], uuid[2], uuid[3], uuid[4], uuid[5], uuid[6], uuid[7], uuid[8], uuid[9], uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
    uuid[0] = 0x67;
    ret = ProteusIV_SetSppTxUuid(uuid);
    Examples_Print("Set TX UUID", ret);
    ret = ProteusIV_GetSppTxUuid(uuid);
    Examples_Print("Get TX UUID", ret);
    WE_APP_PRINT("TX UUID is 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\r\n", uuid[0], uuid[1], uuid[2], uuid[3], uuid[4], uuid[5], uuid[6], uuid[7], uuid[8], uuid[9], uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
    WE_Delay(500);

    uint8_t adv_data[PROTEUSIV_ADVDATA_LENGTH];
    uint16_t adv_length = sizeof(adv_data);
    ret = ProteusIV_GetRAMAdvertisingData(adv_data, &adv_length);
    Examples_Print("GetRAM advertising data", ret);
    WE_APP_PRINT("Data: 0x");
    for (uint8_t i = 0; i < adv_length; i++)
    {
        WE_APP_PRINT("%02x", adv_data[i]);
    }
    WE_APP_PRINT("\r\n");
    uint8_t new_adv_data[] = {0x02, 0x01, 0x06, 0x05, 0x08, 0x4E, 0x61, 0x6D, 0x65, 0x02, 0x0A, 0x04}; //02010605084E616D65020A04
    ret = ProteusIV_SetRAMAdvertisingData(new_adv_data, sizeof(new_adv_data));
    Examples_Print("SetRAM advertising data", ret);
    adv_length = sizeof(adv_data);
    ret = ProteusIV_GetRAMAdvertisingData(adv_data, &adv_length);
    Examples_Print("GetRAM advertising data", ret);
    WE_APP_PRINT("Data: 0x");
    for (uint8_t i = 0; i < adv_length; i++)
    {
        WE_APP_PRINT("%02x", adv_data[i]);
    }
    WE_APP_PRINT("\r\n");
    WE_Delay(1000);

    uint8_t scanrsp_data[PROTEUSIV_ADVDATA_LENGTH];
    uint16_t scanrsp_length = sizeof(scanrsp_data);
    ret = ProteusIV_GetRAMScanResponseData(scanrsp_data, &scanrsp_length);
    Examples_Print("GetRAM scan response data", ret);
    WE_APP_PRINT("Data: 0x");
    for (uint8_t i = 0; i < scanrsp_length; i++)
    {
        WE_APP_PRINT("%02x", scanrsp_data[i]);
    }
    WE_APP_PRINT("\r\n");
    uint8_t new_scanrsp_data[] = {0x08, 0xFF, 0x1A, 0x03, 0x01, 0x74, 0x65, 0x73, 0x74}; //08FF1A030174657374
    ret = ProteusIV_SetRAMScanResponseData(new_scanrsp_data, sizeof(new_scanrsp_data));
    Examples_Print("SetRAM scan response data", ret);
    scanrsp_length = sizeof(scanrsp_data);
    ret = ProteusIV_GetRAMScanResponseData(scanrsp_data, &scanrsp_length);
    Examples_Print("GetRAM scan response data", ret);
    WE_APP_PRINT("Data: 0x");
    for (uint8_t i = 0; i < scanrsp_length; i++)
    {
        WE_APP_PRINT("%02x", scanrsp_data[i]);
    }
    WE_APP_PRINT("\r\n");
    WE_Delay(1000);

    ret = ProteusIV_Reset();
    Examples_Print("Reset", ret);
    WE_Delay(500);

    adv_length = sizeof(adv_data);
    ret = ProteusIV_GetAdvertisingData(adv_data, &adv_length);
    Examples_Print("Get advertising data", ret);
    WE_APP_PRINT("Data: 0x");
    for (uint8_t i = 0; i < adv_length; i++)
    {
        WE_APP_PRINT("%02x", adv_data[i]);
    }
    WE_APP_PRINT("\r\n");

    ret = ProteusIV_SetAdvertisingData(new_adv_data, sizeof(new_adv_data));
    Examples_Print("Set advertising data", ret);
    adv_length = sizeof(adv_data);
    ret = ProteusIV_GetAdvertisingData(adv_data, &adv_length);
    Examples_Print("Get advertising data", ret);
    WE_APP_PRINT("Data: 0x");
    for (uint8_t i = 0; i < adv_length; i++)
    {
        WE_APP_PRINT("%02x", adv_data[i]);
    }
    WE_APP_PRINT("\r\n");
    WE_Delay(1000);

    scanrsp_length = sizeof(scanrsp_data);
    ret = ProteusIV_GetScanResponseData(scanrsp_data, &scanrsp_length);
    Examples_Print("Get scan response data", ret);
    WE_APP_PRINT("Data: 0x");
    for (uint8_t i = 0; i < scanrsp_length; i++)
    {
        WE_APP_PRINT("%02x", scanrsp_data[i]);
    }
    WE_APP_PRINT("\r\n");

    ret = ProteusIV_SetScanResponseData(new_scanrsp_data, sizeof(new_scanrsp_data));
    Examples_Print("Set scan response data", ret);
    scanrsp_length = sizeof(scanrsp_data);
    ret = ProteusIV_GetScanResponseData(scanrsp_data, &scanrsp_length);
    Examples_Print("Get scan response data", ret);
    WE_APP_PRINT("Data: 0x");
    for (uint8_t i = 0; i < scanrsp_length; i++)
    {
        WE_APP_PRINT("%02x", scanrsp_data[i]);
    }
    WE_APP_PRINT("\r\n");

    uint16_t cfgFlags;
    ret = ProteusIV_GetCFGFlags(&cfgFlags);
    Examples_Print("Get cfg flags", ret);
    WE_APP_PRINT("Cfg flags is %d\r\n", cfgFlags);
    ret = ProteusIV_SetCFGFlags(0);
    Examples_Print("Set cfg flags", ret);
    ret = ProteusIV_GetCFGFlags(&cfgFlags);
    Examples_Print("Get cfg flags", ret);
    WE_APP_PRINT("Cfg flags is %d\r\n", cfgFlags);
    WE_Delay(500);

    uint8_t etxconfig;
    ret = ProteusIV_GetETXConfig(&etxconfig);
    Examples_Print("Get etx config", ret);
    WE_APP_PRINT("Etx config is %d\r\n", etxconfig);
    ret = ProteusIV_SetETXConfig(12);
    Examples_Print("Set etx config", ret);
    ret = ProteusIV_GetETXConfig(&etxconfig);
    Examples_Print("Get etx config", ret);
    WE_APP_PRINT("Etx config is %d\r\n", etxconfig);
    WE_Delay(500);

    uint16_t etx;
    ret = ProteusIV_GetReceiveETX(&etx);
    Examples_Print("Get receive etx", ret);
    WE_APP_PRINT("Etx is 0x%x\r\n", etx);
    ret = ProteusIV_SetReceiveETX(0x1122);
    Examples_Print("Set receive etx", ret);
    ret = ProteusIV_GetReceiveETX(&etx);
    Examples_Print("Get receive etx", ret);
    WE_APP_PRINT("Etx is 0x%x\r\n", etx);
    WE_Delay(500);

    ret = ProteusIV_GetTransmitETX(&etx);
    Examples_Print("Get transmit etx", ret);
    WE_APP_PRINT("Etx is 0x%x\r\n", etx);
    ret = ProteusIV_SetTransmitETX(0x1122);
    Examples_Print("Set transmit etx", ret);
    ret = ProteusIV_GetTransmitETX(&etx);
    Examples_Print("Get transmit etx", ret);
    WE_APP_PRINT("Etx is 0x%x\r\n", etx);
    WE_Delay(500);

    WE_Delay(1000);

    ret = ProteusIV_FactoryReset();
    Examples_Print("Factory reset", ret);

    ProteusIV_Deinit();
    return;
}

/**
 * @brief Proteus-IV user settings example.
 *
 * @details: This example demonstrates how to use the functions to read and modify the user settings.
 * At the end a factory reset is applied, resetting all the settings.
 */
static void Test_OtherfunctionsExample()
{
    WE_APP_PRINT("Test_OtherfunctionsExample started\r\n");

    bool ret = false;
    ProteusIV_CallbackConfig_t callbackConfig = {0};

    if (false == ProteusIV_Init(&ProteusIV_uart, &ProteusIV_pins, ProteusIV_OperationMode_CommandMode, callbackConfig))
    {
        WE_APP_PRINT("Initialisation error\r\n");
        return;
    }

    ProteusIV_ModuleState_t moduleState;
    ret = ProteusIV_GetState(&moduleState);
    Examples_Print("Get module state", ret);
    WE_Delay(500);

    ret = ProteusIV_Sleep();
    Examples_Print("Sleep", ret);
    WE_Delay(500);

    ret = ProteusIV_PinReset();
    Examples_Print("Pin reset", ret);
    WE_Delay(500);

    ret = ProteusIV_Reset();
    Examples_Print("Reset", ret);
    WE_Delay(500);

    ret = ProteusIV_SetSecFlags(ProteusIV_SecFlags_NO_IO_MINPERIPHERAL_LEVEL_L2);
    Examples_Print("Set SecFlags", ret);
    WE_Delay(500);

    ret = ProteusIV_AdvertisingStop();
    Examples_Print("Advertising stop", ret);
    WE_Delay(500);

    ret = ProteusIV_AdvertisingStart();
    Examples_Print("Advertising start", ret);
    WE_Delay(500);

    ProteusIV_BondDatabase_t bondDatabase;
    ret = ProteusIV_GetBonds(&bondDatabase);
    Examples_Print("Get bonds", ret);
    WE_APP_PRINT("%d device(s) found\r\n", bondDatabase.numberOfDevices);
    for (uint8_t i = 0; i < bondDatabase.numberOfDevices; i++)
    {
        WE_APP_PRINT("Bonded to device (type %d, BTMAC 0x%02x%02x%02x%02x%02x%02x)\r\n", bondDatabase.devices[i].addr_type, bondDatabase.devices[i].btMac[0], bondDatabase.devices[i].btMac[1], bondDatabase.devices[i].btMac[2], bondDatabase.devices[i].btMac[3], bondDatabase.devices[i].btMac[4], bondDatabase.devices[i].btMac[5]);
    }
    WE_Delay(500);

    while (1)
    {
        for (int conn_ID = 0; conn_ID < PROTEUSIV_MAX_NUMBER_OF_CONNECTEDDEVICES; conn_ID++)
        {
            if (ProteusIV_connection_list[conn_ID].is_linkopen)
            {
                ProteusIV_ConnectedDevices_t devices;
                ret = ProteusIV_GetConnectionInfo(conn_ID, &devices);
                Examples_Print("Get connected devices", ret);
                WE_APP_PRINT("%d device(s) found\r\n", devices.numberOfDevices);
                for (uint8_t i = 0; i < devices.numberOfDevices; i++)
                {
                    WE_APP_PRINT("Conn_ID %d: Link %s, payload %d, phy %d/%d\r\n", devices.devices[i].conn_ID, devices.devices[i].is_linkopen ? "open" : "closed", devices.devices[i].max_payload, devices.devices[i].phy_rx, devices.devices[i].phy_tx);
                }

                ret = ProteusIV_PhyUpdate(conn_ID, ProteusIV_Phy_2MBit);
                Examples_Print("Update phy", ret);
                WE_Delay(2500);

                ret = ProteusIV_GetConnectionInfo(conn_ID, &devices);
                Examples_Print("Get connected devices", ret);
                WE_APP_PRINT("%d device(s) found\r\n", devices.numberOfDevices);
                for (uint8_t i = 0; i < devices.numberOfDevices; i++)
                {
                    WE_APP_PRINT("Conn_ID %d: Link %s, payload %d, phy %d/%d\r\n", devices.devices[i].conn_ID, devices.devices[i].is_linkopen ? "open" : "closed", devices.devices[i].max_payload, devices.devices[i].phy_rx, devices.devices[i].phy_tx);
                }

                ret = ProteusIV_Disconnect(conn_ID);
                Examples_Print("Disconnect", ret);
                WE_Delay(500);
            }
        }

        WE_Delay(5000);
    }

    ProteusIV_Deinit();
    return;
}

/**
 * @brief Proteus-IV peripheral transparent mode example.
 *
 * @details: The radio module acts as peripheral and waits for an incoming connection. Once the Bluetooth LE link is open, it sends data.
 * As peer device use another Proteus-IV radio module in command mode, or the WE Bluetooth LE terminal app.
 *
 * The example consists of the following steps:
 * 1) Start the Proteus-IV in command mode to check/set parameters (if required).
 * 2) Restart the Proteus-IV in transparent mode.
 * 3) Monitor the LED_1 pin and send packet based data using ETX characters "\r\n".
 *
 * Note that this example requires pins /RESET, MODE_0, MODE_1 and LED_1 to be connected.
 *
 */
static void Peripheral_TransparentModeExample()
{
    WE_APP_PRINT("Peripheral_TransparentModeExample started\r\n");

    /* run radio module configuration first */
    if (!ConfigureModule())
    {
        WE_APP_PRINT("Module configuration failed\r\n");
        return;
    }

    /* No callbacks required */
    ProteusIV_CallbackConfig_t callbackConfig = {0};
    callbackConfig.transparentRxCb = OnTransparentModeByteReceived;
    if (!ProteusIV_Init(&ProteusIV_uart, &ProteusIV_pins, ProteusIV_OperationMode_TransparentMode, callbackConfig))
    {
        WE_APP_PRINT("Initialisation error\r\n");
        return;
    }

    WE_APP_PRINT("Transparent mode started.\r\n");

    uint32_t lastStatusPinLowTick = WE_GetTick();
    bool per_linkOpen = false;

    WE_Pin_Level_t statusPinState;
    if (!ProteusIV_GetLed1PinLevel(&statusPinState))
    {
        WE_APP_PRINT("Failed to get pin state.\r\n");
        return;
    }

    char data[] = "I'm transparent";
    while (1)
    {
        /* Check connection status and print message on state change (using status i.e. LED_1 pin) */
        if (!ProteusIV_GetLed1PinLevel(&statusPinState))
        {
            WE_APP_PRINT("Failed to get pin state.\r\n");
            return;
        }

        if (per_linkOpen)
        {
            if (statusPinState == WE_Pin_Level_Low)
            {
                /* Status pin changed to low - link is now closed */
                WE_APP_PRINT("Link closed.\r\n");
                per_linkOpen = false;
            }
            else
            {
                WE_APP_PRINT("Transmit data\r\n");

                if (!ProteusIV_Transparent_Transmit((uint8_t*)data, (uint16_t)strlen(data), 0x0D0A))
                {
                    WE_APP_PRINT("Failed\r\n");
                }
                WE_Delay(1000);
            }
        }
        else if (statusPinState == WE_Pin_Level_High)
        {
            if (WE_GetTick() - lastStatusPinLowTick > PROTEUSIV_STATUS_LED_CONNECTED_TIMEOUT_MS)
            {
                /* Status pin has been high for at least
                 * ProteusIV_STATUS_LED_CONNECTED_TIMEOUT_MS ms - link is now open */
                WE_APP_PRINT("Link opened.\r\n");
                per_linkOpen = true;
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
 * @brief Enter the config mode to configure the radio module
 *
 * @details: The config mode is entered, a few configurations are done, then the Proteus driver is de-initialized again.
 *
 */
bool ConfigureModule()
{
    WE_UART_t ProteusIV_uart_configMode;
    ProteusIV_uart_configMode.baudrate = PROTEUSIV_CONFIGMODE_BAUDRATE;
    ProteusIV_uart_configMode.flowControl = WE_FlowControl_NoFlowControl;
    ProteusIV_uart_configMode.parity = WE_Parity_None;
    ProteusIV_uart_configMode.uartInit = ProteusIV_uart.uartInit;
    ProteusIV_uart_configMode.uartDeinit = ProteusIV_uart.uartDeinit;
    ProteusIV_uart_configMode.uartTransmit = ProteusIV_uart.uartTransmit;

    /* No callbacks required */
    ProteusIV_CallbackConfig_t callbackConfig = {0};

    if (!ProteusIV_Init(&ProteusIV_uart_configMode, &ProteusIV_pins, ProteusIV_OperationMode_ConfigMode, callbackConfig))
    {
        WE_APP_PRINT("Initialisation error\r\n");
        return false;
    }

    uint8_t fwVersion[3];
    if (!ProteusIV_GetFWVersion(fwVersion))
    {
        WE_APP_PRINT("Failed to read firmware version\r\n");
        return false;
    }
    WE_APP_PRINT("Firmware version is %u.%u.%u\r\n", fwVersion[2], fwVersion[1], fwVersion[0]);

    uint16_t etx;
    if (!ProteusIV_GetReceiveETX(&etx))
    {
        WE_APP_PRINT("Failed to read ETX\r\n");
        return false;
    }
    WE_APP_PRINT("The ETX is %04x", etx);

    /* add/run your configuration commands here */

    if (!ProteusIV_Deinit())
    {
        WE_APP_PRINT("Failed to deinit\r\n");
        return false;
    }

    return true;
}

/**
 * @brief Callback called when data has been received via radio
 */
static void RxCallback(uint8_t conn_ID, uint8_t* payload, uint16_t length, int8_t rssi)
{
    uint16_t i = 0;
    WE_APP_PRINT("Conn_ID %d: Data received (RSSI %d dBm):\r\n-> ", conn_ID, rssi);
    WE_APP_PRINT("0x");
    for (i = 0; i < length; i++)
    {
        WE_APP_PRINT("%02x", *(payload + i));
    }
    WE_APP_PRINT(" (");
    for (i = 0; i < length; i++)
    {
        WE_APP_PRINT("%c", *(payload + i));
    }
    WE_APP_PRINT(")\r\n");
}

/**
 * @brief Callback called when a Bluetooth connection request is received
 */
static void ConnectCallback(uint8_t conn_ID, bool success, ProteusIV_Bluetooth_Address_Type_t type, uint8_t* btMac)
{
    if (success)
    {
        WE_APP_PRINT("Conn_ID %d: Connected to device (type %d, BTMAC 0x%02x%02x%02x%02x%02x%02x)\r\n", conn_ID, type, btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
    }
    else
    {
        WE_APP_PRINT("Connection failed to device (type %d, BTMAC 0x%02x%02x%02x%02x%02x%02x)\r\n", type, btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5]);
    }
}

/**
 * @brief Callback called when the maximum payload of a Bluetooth link is updated
 */
static void MaxPayloadCallback(uint8_t conn_ID, uint16_t max_payload) { WE_APP_PRINT("Conn_ID %d: Max payload updated to %d bytes\r\n", conn_ID, max_payload); }

/**
 * @brief Callback called when security level has changed
 */
static void SecurityCallback(uint8_t conn_ID, ProteusIV_SecurityState_t securityState) { WE_APP_PRINT("Conn_ID %d: Encrypted link to peer device established (success: %d, level: %d)\r\n", conn_ID, securityState.success, securityState.level); }

/**
 * @brief Callback called when remote device has requested for pass key
 */
static void PasskeyCallback(uint8_t conn_ID)
{
    WE_APP_PRINT("Conn_ID %d: Pass key requested\r\n", conn_ID);

    /* Handle pass key request asynchronously in main (must not send response directly from callback) */
    authentication_conn_ID = conn_ID;
    passkeyRequestReceived = true;
}

/**
 * @brief Callback called when a pass key is displayed to enter it on the remote device
 */
static void DisplayPasskeyCallback(uint8_t conn_ID, ProteusIV_DisplayPasskeyAction_t action, uint8_t* passkey)
{
    WE_APP_PRINT("Conn_ID %d: Pass key displayed (%c%c%c%c%c%c)\r\n", conn_ID, passkey[0], passkey[1], passkey[2], passkey[3], passkey[4], passkey[5]);

    if (ProteusIV_DisplayPasskeyAction_PleaseConfirm == action)
    {
        /* Handle display pass key request asynchronously in main (must not send response directly from callback) */
        authentication_conn_ID = conn_ID;
        displayPasskeyRequestReceived = true;
        WE_APP_PRINT("Please confirm");
    }
}

/**
 * @brief Callback called when connection has been dropped
 */
static void DisconnectCallback(uint8_t conn_ID, ProteusIV_DisconnectReason_t reason)
{
    static const char* reasonStrings[] = {"unknown", "connection timeout", "user terminated connection", "host terminated connection", "connection interval unacceptable", "MIC failure", "connection setup failed"};
    WE_APP_PRINT("Conn_ID %d: Disconnected from device (reason: %s)\r\n", conn_ID, reasonStrings[reason]);

    displayPasskeyRequestReceived = false;
    passkeyRequestReceived = false;
}

/**
 * @brief Callback called when a Bluetooth link has been opened. Now data transmission can be done
 */
static void LinkOpenCallback(uint8_t conn_ID, bool link_open) { WE_APP_PRINT("Conn_ID %d: Link %s\r\n", conn_ID, link_open ? "opened" : "closed"); }

/**
 * @brief Callback called when a device has been found while scanning
 */
static void ScanCallback(ProteusIV_Bluetooth_Address_Type_t type, uint8_t* btMac, int8_t rssi, char* devicename) { WE_APP_PRINT("Found device (type %d, BTMAC 0x%02x%02x%02x%02x%02x%02x, RSSI %d dBm, name %s) during scan\r\n", type, btMac[0], btMac[1], btMac[2], btMac[3], btMac[4], btMac[5], rssi, devicename); }
/**
 * @brief Callback called when the PHY of the connection has changed
 */
static void PhyUpdateCallback(uint8_t conn_ID, uint8_t phyTx, uint8_t phyRx) { WE_APP_PRINT("Conn_ID %d: Phy of connection updated (RX: %d, TX: %d)\r\n", conn_ID, phyRx, phyTx); }

/**
 * @brief This functions handles bytes received in transparent mode
 */
static void OnTransparentModeByteReceived(uint8_t* dataP, size_t size)
{
    uint8_t* p = dataP;
    size_t s = size;
    WE_APP_PRINT("Rx 0x");
    for (; s > 0; s--, p++)
    {
        WE_APP_PRINT("%02x", *p);
    }
    p = dataP;
    s = size;
    WE_APP_PRINT("(");
    for (; s > 0; s--, p++)
    {
        WE_APP_PRINT("%c", *p);
    }
    WE_APP_PRINT(")\r\n");
}
