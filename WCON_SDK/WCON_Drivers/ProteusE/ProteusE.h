/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK:
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
 * @brief Proteus-e driver header file.
 */

#include <global/global_types.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef PROTEUSE_H_INCLUDED
#define PROTEUSE_H_INCLUDED

/* Max. payload length for transmission. */
#define PROTEUSE_MAX_RADIO_PAYLOAD_LENGTH (uint16_t)243
#define PROTEUSE_MAX_CMD_PAYLOAD_LENGTH (uint16_t)(PROTEUSE_MAX_RADIO_PAYLOAD_LENGTH + 7)

/* Time (ms) required for booting */
#define PROTEUSE_BOOT_DURATION (uint16_t)35

/* Channel is considered to be connected if status pin (LED_1) is on for this duration */
#define PROTEUSE_STATUS_LED_CONNECTED_TIMEOUT_MS (uint16_t)260

/* Default UART baudrate of Proteus-e module */
#define PROTEUSE_DEFAULT_BAUDRATE (uint32_t)115200

/* Max number of bonded devices supported by the Proteus-e module. */
#define PROTEUSE_MAX_BOND_DEVICES (uint8_t)12

typedef struct ProteusE_Pins_t
{
    WE_Pin_t ProteusE_Pin_Reset;
    WE_Pin_t ProteusE_Pin_BusyUartEnable;
    WE_Pin_t ProteusE_Pin_Mode;
    WE_Pin_t ProteusE_Pin_StatusLed1;
} ProteusE_Pins_t;

typedef enum ProteusE_OperationMode_t
{
    ProteusE_OperationMode_CommandMode,
    ProteusE_OperationMode_TransparentMode
} ProteusE_OperationMode_t;

/* Number of free GPIOs available on the module */
#define PROTEUSE_AMOUNT_GPIO_PINS 2

typedef enum ProteusE_GPIO_t
{
    ProteusE_GPIO_1 = (uint8_t)0x01,
    ProteusE_GPIO_2 = (uint8_t)0x02
} ProteusE_GPIO_t;

typedef enum ProteusE_GPIO_IO_t
{
    ProteusE_GPIO_IO_Disconnected = (uint8_t)0x00,
    ProteusE_GPIO_IO_Input = (uint8_t)0x01,
    ProteusE_GPIO_IO_Output = (uint8_t)0x02
} ProteusE_GPIO_IO_t;

typedef enum ProteusE_GPIO_Output_t
{
    ProteusE_GPIO_Output_Low = (uint8_t)0x00,
    ProteusE_GPIO_Output_High = (uint8_t)0x01
} ProteusE_GPIO_Output_t;

typedef enum ProteusE_GPIO_Input_t
{
    ProteusE_GPIO_Input_NoPull = (uint8_t)0x00,
    ProteusE_GPIO_Input_PullDown = (uint8_t)0x01,
    ProteusE_GPIO_Input_PullUp = (uint8_t)0x02
} ProteusE_GPIO_Input_t;

typedef struct ProteusE_GPIOConfigBlock_t
{
    ProteusE_GPIO_t gpioId;
    ProteusE_GPIO_IO_t function;
    union
    {
        ProteusE_GPIO_Input_t input;
        ProteusE_GPIO_Output_t output;
    } value;
} ProteusE_GPIOConfigBlock_t;

typedef struct ProteusE_GPIOControlBlock_t
{
    ProteusE_GPIO_t gpioId;
    union
    {
        ProteusE_GPIO_Output_t output;
    } value;
} ProteusE_GPIOControlBlock_t;

typedef enum ProteusE_DriverState_t
{
    ProteusE_DriverState_BLE_Idle = (uint8_t)0x00,
    ProteusE_DriverState_BLE_Connected = (uint8_t)0x01,
    ProteusE_DriverState_BLE_ChannelOpen = (uint8_t)0x02
} ProteusE_DriverState_t;

typedef enum ProteusE_SecurityState_t
{
    ProteusE_SecurityState_BLE_ReBonded = (uint8_t)0x00,
    ProteusE_SecurityState_BLE_Bonded = (uint8_t)0x01,
    ProteusE_SecurityState_BLE_Paired = (uint8_t)0x02
} ProteusE_SecurityState_t;

typedef enum ProteusE_DisconnectReason_t
{
    ProteusE_DisconnectReason_Unknown,
    ProteusE_DisconnectReason_ConnectionTimeout,
    ProteusE_DisconnectReason_UserTerminatedConnection,
    ProteusE_DisconnectReason_HostTerminatedConnection,
    ProteusE_DisconnectReason_ConnectionIntervalUnacceptable,
    ProteusE_DisconnectReason_MicFailure,
    ProteusE_DisconnectReason_ConnectionSetupFailed,
} ProteusE_DisconnectReason_t;

typedef enum ProteusE_BLE_Role_t
{
    ProteusE_BLE_Role_None = (uint8_t)0x00,
    ProteusE_BLE_Role_Peripheral = (uint8_t)0x01,
    ProteusE_BLE_Role_DTM = (uint8_t)0x10
} ProteusE_BLE_Role_t;

typedef enum ProteusE_BLE_Action_t
{
    ProteusE_BLE_Action_None = (uint8_t)0x00,
    ProteusE_BLE_Action_Idle = (uint8_t)0x01,
    ProteusE_BLE_Action_Connected = (uint8_t)0x03,
    ProteusE_BLE_Action_Sleep = (uint8_t)0x04,
    ProteusE_BLE_Action_DTM = (uint8_t)0x05
} ProteusE_BLE_Action_t;

/**
 * @brief User settings
 */
typedef enum ProteusE_UserSettings_t
{
    ProteusE_USERSETTING_FS_FWVersion = (uint8_t)0x01,
    ProteusE_USERSETTING_RF_DEVICE_NAME = (uint8_t)0x02,
    ProteusE_USERSETTING_FS_MAC = (uint8_t)0x03,
    ProteusE_USERSETTING_FS_BTMAC = (uint8_t)0x04,
    ProteusE_USERSETTING_UART_TRANSP_ETX = (uint8_t)0x05,
    ProteusE_USERSETTING_UART_TRANSP_ETX_CFG = (uint8_t)0x06,
    ProteusE_USERSETTING_RF_ADVERTISING_TIMEOUT = (uint8_t)0x07,
    ProteusE_USERSETTING_RF_CONNECTION_INTERVAL = (uint8_t)0x08,
    ProteusE_USERSETTING_RF_ADVERTISING_INTERVAL = (uint8_t)0x09,
    ProteusE_USERSETTING_UART_CONFIG_INDEX = (uint8_t)0x0B,
    ProteusE_USERSETTING_RF_SEC_FLAGS = (uint8_t)0x0C,
    ProteusE_USERSETTING_RF_ADVERTISING_DATA = (uint8_t)0x0D,
    ProteusE_USERSETTING_RF_SCAN_RESPONSE_DATA = (uint8_t)0x0E,
    ProteusE_USERSETTING_FS_DEVICE_INFO = (uint8_t)0x0F,
    ProteusE_USERSETTING_FS_SERIAL_NUMBER = (uint8_t)0x10,
    ProteusE_USERSETTING_RF_TX_POWER = (uint8_t)0x11,
    ProteusE_USERSETTING_RF_STATIC_PASSKEY = (uint8_t)0x12,
    ProteusE_USERSETTING_RF_APPEARANCE = (uint8_t)0x19,
    ProteusE_USERSETTING_RF_SPPBASEUUID = (uint8_t)0x1A,
    ProteusE_USERSETTING_RF_CFGFLAGS = (uint8_t)0x1C,
    ProteusE_USERSETTING_RF_SPPServiceUUID = (uint8_t)0x20,
    ProteusE_USERSETTING_RF_SPPRXUUID = (uint8_t)0x21,
    ProteusE_USERSETTING_RF_SPPTXUUID = (uint8_t)0x22,
    ProteusE_USERSETTING_UART_TRANSPARENT_TIMEOUT = (uint8_t)0x24,
    ProteusE_USERSETTING_UART_TRANSPARENT_MAXPAYLOAD = (uint8_t)0x25,
} ProteusE_UserSettings_t;

/**
 * @brief Configuration flags to be used with ProteusE_SetCFGFlags() and ProteusE_GetCFGFlags()
 */
typedef enum ProteusE_CfgFlags_t
{
    ProteusE_CfgFlags_GPIORemoteConfig = (1 << 2),
    ProteusE_CfgFlags_DCDCEnable = (1 << 3),
    ProteusE_CfgFlags_DisconnectEnable = (1 << 4)
} ProteusE_CfgFlags_t;

#define PROTEUSE_SEC_MODE_BONDING_ENABLE_MASK (uint8_t)0x08
#define PROTEUSE_SEC_MODE_BONDEDCONNECTIONSONLY_ENABLE (uint8_t)0x10

typedef enum ProteusE_SecFlags_t
{
    ProteusE_SecFlags_NONE = (uint8_t)0x00,
    ProteusE_SecFlags_JustWorks = (uint8_t)0x02,
    ProteusE_SecFlags_StaticPassKey = (uint8_t)0x03,
    ProteusE_SecFlags_JustWorks_Bonding = (uint8_t)(ProteusE_SecFlags_JustWorks | PROTEUSE_SEC_MODE_BONDING_ENABLE_MASK),
    ProteusE_SecFlags_StaticPassKey_Bonding = (uint8_t)(ProteusE_SecFlags_StaticPassKey | PROTEUSE_SEC_MODE_BONDING_ENABLE_MASK),
    ProteusE_SecFlags_JustWorks_BondingOnly = (uint8_t)(ProteusE_SecFlags_JustWorks | PROTEUSE_SEC_MODE_BONDING_ENABLE_MASK | PROTEUSE_SEC_MODE_BONDEDCONNECTIONSONLY_ENABLE),
    ProteusE_SecFlags_StaticPassKey_BondingOnly = (uint8_t)(ProteusE_SecFlags_StaticPassKey | PROTEUSE_SEC_MODE_BONDING_ENABLE_MASK | PROTEUSE_SEC_MODE_BONDEDCONNECTIONSONLY_ENABLE)
} ProteusE_SecFlags_t;

typedef enum ProteusE_TXPower_t
{
    ProteusE_TXPower_4 = (int8_t)4,
    ProteusE_TXPower_3 = (int8_t)3,
    ProteusE_TXPower_0 = (int8_t)0,
    ProteusE_TXPower_minus4 = (int8_t)-4,
    ProteusE_TXPower_minus8 = (int8_t)-8,
    ProteusE_TXPower_minus12 = (int8_t)-12,
    ProteusE_TXPower_minus16 = (int8_t)-16,
    ProteusE_TXPower_minus20 = (int8_t)-20,
    ProteusE_TXPower_minus40 = (int8_t)-40
} ProteusE_TXPower_t;

typedef enum ProteusE_BaudRate_t
{
    ProteusE_BaudRateIndex_1200 = (uint8_t)0,
    ProteusE_BaudRateIndex_2400 = (uint8_t)2,
    ProteusE_BaudRateIndex_4800 = (uint8_t)4,
    ProteusE_BaudRateIndex_9600 = (uint8_t)6,
    ProteusE_BaudRateIndex_14400 = (uint8_t)8,
    ProteusE_BaudRateIndex_19200 = (uint8_t)10,
    ProteusE_BaudRateIndex_28800 = (uint8_t)12,
    ProteusE_BaudRateIndex_38400 = (uint8_t)14,
    ProteusE_BaudRateIndex_56000 = (uint8_t)16,
    ProteusE_BaudRateIndex_57600 = (uint8_t)18,
    ProteusE_BaudRateIndex_76800 = (uint8_t)20,
    ProteusE_BaudRateIndex_115200 = (uint8_t)22,
    ProteusE_BaudRateIndex_230400 = (uint8_t)24,
    ProteusE_BaudRateIndex_250000 = (uint8_t)26,
    ProteusE_BaudRateIndex_460800 = (uint8_t)28,
    ProteusE_BaudRateIndex_921600 = (uint8_t)30,
    ProteusE_BaudRateIndex_1000000 = (uint8_t)32
} ProteusE_BaudRate_t;

typedef enum ProteusE_UartParity_t
{
    ProteusE_UartParity_Even,
    ProteusE_UartParity_None
} ProteusE_UartParity_t;

typedef enum ProteusE_Phy_t
{
    ProteusE_Phy_1MBit = 0x01,
    ProteusE_Phy_2MBit = 0x02
} ProteusE_Phy_t;

/**
 * @brief Device info structure.
 * @see ProteusE_GetDeviceInfo()
 */
typedef struct ProteusE_DeviceInfo_t
{
    uint16_t osVersion;
    uint32_t buildCode;
    uint16_t packageVariant;
    uint32_t chipId;
} ProteusE_DeviceInfo_t;

/**
 * @brief Module state structure.
 * @see ProteusE_GetState()
 */
typedef struct ProteusE_ModuleState_t
{
    ProteusE_BLE_Role_t role;
    ProteusE_BLE_Action_t action;
    uint8_t connectedDeviceBtMac[6];
    uint16_t connectedDeviceMaxPayloadSize;
} ProteusE_ModuleState_t;

/**
 * @brief Entry in ProteusE_BondDatabase_t.
 * @see ProteusE_GetBonds()
 */
typedef struct ProteusE_BondDatabaseEntry_t
{
    uint16_t id;
    uint8_t btMac[6];
} ProteusE_BondDatabaseEntry_t;

/**
 * @brief List of bonded devices.
 * @see ProteusE_GetBonds()
 */
typedef struct ProteusE_BondDatabase_t
{
    uint8_t nrOfDevices;
    ProteusE_BondDatabaseEntry_t devices[PROTEUSE_MAX_BOND_DEVICES];
} ProteusE_BondDatabase_t;

/**
 * @brief DTM command type
 */
typedef enum ProteusE_DTMCommand_t
{
    ProteusE_DTMCommand_Setup = 0x00,
    ProteusE_DTMCommand_StartRX = 0x01,
    ProteusE_DTMCommand_StartTX = 0x02,
    ProteusE_DTMCommand_Stop = 0x03,
} ProteusE_DTMCommand_t;

/**
 * @brief DTM TX pattern
 */
typedef enum ProteusE_DTMTXPattern_t
{
    ProteusE_DTMTXPattern_PRBS9 = 0x00,
    ProteusE_DTMTXPattern_0x0F = 0x01,
    ProteusE_DTMTXPattern_0x55 = 0x02,
} ProteusE_DTMTXPattern_t;

/* Callback definition */

typedef void (*ProteusE_RxCallback_t)(uint8_t* payload, uint16_t payloadLength, uint8_t* btMac, int8_t rssi);
/* Note that btMac is not provided if success == false (is set to all zeros) */
typedef void (*ProteusE_ConnectCallback_t)(bool success, uint8_t* btMac);
typedef void (*ProteusE_SecurityCallback_t)(uint8_t* btMac, ProteusE_SecurityState_t securityState);
typedef void (*ProteusE_DisconnectCallback_t)(ProteusE_DisconnectReason_t reason);
typedef void (*ProteusE_ChannelOpenCallback_t)(uint8_t* btMac, uint16_t maxPayload);
/* Note that btMac is not provided if success == false (is set to all zeros) */
typedef void (*ProteusE_PhyUpdateCallback_t)(bool success, uint8_t* btMac, uint8_t phyRx, uint8_t phyTx);
typedef void (*ProteusE_SleepCallback_t)();
/* Note that the gpioId parameter is of type uint8_t instead of ProteusE_GPIO_t, as the
 * remote device may support other GPIOs than this device. */
typedef void (*ProteusE_GpioWriteCallback_t)(bool remote, uint8_t gpioId, uint8_t value);
typedef void (*ProteusE_GpioRemoteConfigCallback_t)(ProteusE_GPIOConfigBlock_t* gpioConfig);
typedef void (*ProteusE_ErrorCallback_t)(uint8_t errorCode);

/**
 * @brief Callback configuration structure. Used as argument for ProteusE_Init().
 *
 * Please note that code in callbacks should be kept simple, as the callback
 * functions are called from ISRs. For this reason, it is also not possible
 * to send requests to the Proteus-e directly from inside a callback.
 */
typedef struct ProteusE_CallbackConfig_t
{
    ProteusE_RxCallback_t rxCb;                             /**< Callback for CMD_DATA_IND */
    ProteusE_ConnectCallback_t connectCb;                   /**< Callback for CMD_CONNECT_IND */
    ProteusE_SecurityCallback_t securityCb;                 /**< Callback for CMD_SECURITY_IND */
    ProteusE_DisconnectCallback_t disconnectCb;             /**< Callback for CMD_DISCONNECT_IND */
    ProteusE_ChannelOpenCallback_t channelOpenCb;           /**< Callback for CMD_CHANNELOPEN_RSP */
    ProteusE_PhyUpdateCallback_t phyUpdateCb;               /**< Callback for CMD_PHYUPDATE_IND */
    ProteusE_SleepCallback_t sleepCb;                       /**< Callback for CMD_SLEEP_IND */
    ProteusE_GpioWriteCallback_t gpioWriteCb;               /**< Callback for CMD_GPIO_LOCAL_WRITE_IND and CMD_GPIO_REMOTE_WRITE_IND */
    ProteusE_GpioRemoteConfigCallback_t gpioRemoteConfigCb; /**< Callback for CMD_GPIO_REMOTE_WRITECONFIG_IND */
    ProteusE_ErrorCallback_t errorCb;                       /**< Callback for CMD_ERROR_IND */
} ProteusE_CallbackConfig_t;

/**
* @brief Initialize the Proteus-e for serial interface.
 *
 * Caution: The parameter baudrate must match the configured UserSettings of the Proteus-e.
 *          The baudrate parameter must match to perform a successful FTDI communication.
 *          Updating this parameter during runtime may lead to communication errors.
 *
 * @param[in] uartP: Definition of the uart connected to the module
 * @param[in] pinoutP: Definition of the gpios connected to the module
 * @param[in] opMode: operation mode
 * @param[in] callbackConfig: callback configuration
 *
 * @return True if initialization succeeded,
 *         false otherwise
*/
extern bool ProteusE_Init(WE_UART_t* uartP, ProteusE_Pins_t* pinoutP, ProteusE_OperationMode_t opMode, ProteusE_CallbackConfig_t callbackConfig);

/**
* @brief Deinitialize the Proteus-e interface.
 *
 * @return True if deinitialization succeeded,
 *         false otherwise
*/
extern bool ProteusE_Deinit(void);

/**
* @brief Request the module state
 *
 * @param[out] moduleStateP: Pointer to module state
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetState(ProteusE_ModuleState_t* moduleStateP);

/**
* @brief Reset the Proteus-e by pin.
 *
 * @return True if reset succeeded,
 *         false otherwise
*/
extern bool ProteusE_PinReset(void);

/**
* @brief Reset the Proteus-e by command.
 *
 * @return True if reset succeeded,
 *         false otherwise
*/
extern bool ProteusE_Reset(void);

/**
* @brief Disables the UART of the Proteus-e.
 *
 * It will be re-enabled when the module has to send data to the host (e.g. data was received
 * via radio or a state is indicated) or it can be manually re-enabled using ProteusE_PinUartEnable().
 *
 * @return True if disable succeeded,
 *         false otherwise
*/
extern bool ProteusE_UartDisable(void);

/**
* @brief Re-enables the module's UART using the UART_ENABLE pin after having disabled
 * the UART using ProteusE_UartDisable().
 *
 * @return True if enabling UART succeeded,
 *         false otherwise
*/
extern bool ProteusE_PinUartEnable(void);

/**
* @brief Put the Proteus-e into sleep mode.
 *
 * @return True if succeeded,
 *         false otherwise
*/
extern bool ProteusE_Sleep();

/**
* @brief Disconnect the Proteus-e connection if open.
 *
 * @return True if disconnect succeeded,
 *         false otherwise
*/
extern bool ProteusE_Disconnect();

/**
* @brief Transmit data if a connection is open
 *
 * @param[in] payloadP: Pointer to the data to transmit
 * @param[in] length: Length of the data to transmit
 *
 * @return True if succeeded,
 *         false otherwise
*/
extern bool ProteusE_Transmit(uint8_t* payloadP, uint16_t length);

/**
************************************
 *         Global functions           *
 **************************************/

/**
 * @brief Transmitting the data via UART.
 *
 * @param[in] data: Pointer to the data.
 * @param[in] dataLength: Length of the data.
 *
 * @return True if transmission succeeded,
 *         false otherwise
*/
extern bool ProteusE_Transparent_Transmit(const uint8_t* data, uint16_t dataLength);

/**
* @brief Update the phy during an open connection
 *
 * @param[in] phy: new phy
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_PhyUpdate(ProteusE_Phy_t phy);

/**
* @brief Request the current state of the driver
 *
 * @return driver state
*/
extern ProteusE_DriverState_t ProteusE_GetDriverState();

/**
* @brief Returns the current level of the status pin.
 *
 * @param[out] statusPinLed1LevelP: status pin level
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetStatusPinLed1Level(WE_Pin_Level_t* statusPinLed1LevelP);

/**
* @brief Returns the current level of the BUSY/UART_ENABLE pin.
 *
 * @param[out] busyStateP: busy pin state
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_IsTransparentModeBusy(bool* busyStateP);

/**
* @brief Sets the callback function which is executed if a byte has been received from Proteus-e.
 *
 * The default callback is ProteusE_HandleRxByte().
 *
 * @param[in] callback: Pointer to byte received callback function (default callback is used if NULL)
*/
extern void ProteusE_SetByteRxCallback(WE_UART_HandleRxByte_t callback);

/* functions to control the GPIO feature */

/**
* @brief Configure the local GPIO of the module
 *
 * @param[in] configP: Pointer to one or more pin configurations
 * @param[in] numberOfConfigs: number of entries in configP array
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GPIOLocalWriteConfig(ProteusE_GPIOConfigBlock_t* configP, uint16_t numberOfConfigs);

/**
* @brief Read the local GPIO configuration of the module
 *
 * @param[out] configP: Pointer to one or more pin configurations
 * @param[out] numberOfConfigsP: Pointer to number of entries in configP array
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GPIOLocalReadConfig(ProteusE_GPIOConfigBlock_t* configP, uint16_t* numberOfConfigsP);

/**
* @brief Set the output value of the local pin. Pin has to be configured first.
 * See ProteusE_GPIOLocalWriteConfig
 *
 * @param[in] controlP: Pointer to one or more pin controls
 * @param[in] numberOfControls: number of entries in controlP array
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GPIOLocalWrite(ProteusE_GPIOControlBlock_t* controlP, uint16_t numberOfControls);

/**
* @brief Read the input of the pin. Pin has to be configured first.
 * See ProteusE_GPIOLocalWriteConfig
 *
 * @param[in] gpioToReadP: One or more pins to read.
 * @param[in] amountGPIOToRead: amount of pins to read and therefore length of GPIOToRead
 * @param[out] controlP: Pointer to controlBlock
 * @param[out] numberOfControlsP: Pointer to number of entries in controlP array
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GPIOLocalRead(uint8_t* gpioToReadP, uint8_t amountGPIOToRead, ProteusE_GPIOControlBlock_t* controlP, uint16_t* numberOfControlsP);

/**
* @brief Configure the remote GPIO of the module
 *
 * @param[in] configP: Pointer to one or more pin configurations
 * @param[in] numberOfConfigs: number of entries in configP array
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GPIORemoteWriteConfig(ProteusE_GPIOConfigBlock_t* configP, uint16_t numberOfConfigs);

/**
* @brief Read the remote GPIO configuration of the module
 *
 * @param[out] configP: Pointer to one or more pin configurations
 * @param[out] numberOfConfigsP: Pointer to number of entries in configP array
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GPIORemoteReadConfig(ProteusE_GPIOConfigBlock_t* configP, uint16_t* numberOfConfigsP);

/**
* @brief Set the output value of the remote pin. Pin has to be configured first.
 * See ProteusE_GPIORemoteWriteConfig
 *
 * @param[in] controlP: Pointer to one or more pin controls
 * @param[in] numberOfControls: number of entries in controlP array
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GPIORemoteWrite(ProteusE_GPIOControlBlock_t* controlP, uint16_t numberOfControls);

/**
* @brief Read the input of the pins. Pin has to be configured first.
 * See ProteusE_GPIORemoteWriteConfig
 *
 * @param[in] gpioToReadP: One or more pins to read.
 * @param[in] amountGPIOToRead: amount of pins to read and therefore length of gpioToReadP
 * @param[out] controlP: Pointer to controlBlock
 * @param[out] numberOfControlsP: Pointer to number of entries in controlP array
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GPIORemoteRead(uint8_t* gpioToReadP, uint8_t amountGPIOToRead, ProteusE_GPIOControlBlock_t* controlP, uint16_t* numberOfControlsP);

/**
* @brief Requests the BTMAC addresses of all bonded devices.
 *
 * @param[out] bondDatabaseP: Pointer to bond database
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetBonds(ProteusE_BondDatabase_t* bondDatabaseP);

/**
* @brief Removes all bonding data.
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_DeleteBonds();

/**
* @brief Removes the bonding information for a single device.
 *
 * @param[in] bondId: bond ID of the device to be removed
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_DeleteBond(uint8_t bondId);

/**
* @brief Temporarily allow unbonded connections, in case only bonded connections have been configured
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_AllowUnbondedConnections();

/* Functions writing/reading volatile settings */

/**
* @brief Sets the supplied user setting in RAM (not persistent).
 *
 * @param[in] userSetting: user setting to be updated
 * @param[in] valueP: Pointer to the new settings value (not persistent)
 * @param[in] length: Length of the value
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_SetRAM(ProteusE_UserSettings_t userSetting, uint8_t* valueP, uint8_t length);

/**
* @brief Request the current user settings in RAM (not persistent)
 *
 * @param[in] userSetting: user setting to be requested (not persistent)
 * @param[out] responseP: Pointer of the memory to put the requested content
 * @param[out] responseLengthP: Length of the requested content
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetRAM(ProteusE_UserSettings_t userSetting, uint8_t* responseP, uint16_t* responseLengthP);

/**
* @brief Sets the content of the advertising packet in RAM (not persistent)
 *
 * @param[in] dataP: Content to put in advertising packet
 * @param[in] length: Length of content
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_SetAdvertisingDataRAM(uint8_t* dataP, uint8_t length);

/**
* @brief Sets the content of the scan response packet in RAM (not persistent)
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] dataP: Content to put in scan response packet
 * @param[in] length: Length of content
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_SetScanResponseDataRAM(uint8_t* dataP, uint8_t length);

/**
* @brief Request the (custom) content of the advertising packet from RAM (not persistent)
 *
 * @param[in] dataP: Pointer to (custom) content of advertising packet
 * @param[in] lengthP: Pointer to length of content
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetAdvertisingDataRAM(uint8_t* dataP, uint16_t* lengthP);

/**
* @brief Request the (custom) content of the scan response packet from RAM (not persistent)
 *
 * @param[in] dataP: Pointer to (custom) content of scan response packet
 * @param[in] lengthP: Pointer to length of content
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetScanResponseDataRAM(uint8_t* dataP, uint16_t* lengthP);

/* Functions that write the non-volatile settings in the flash: After modification of any non-volatile setting,
 * the module must be reset such that the update takes effect.
 * IMPORTANT: Use only in rare cases, since flash can be written to only a limited number of times.
 */
extern bool ProteusE_FactoryReset();

/**
* @brief Sets the supplied user setting.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] userSetting: user setting to be updated
 * @param[in] valueP: Pointer to the new settings value
 * @param[in] length: Length of the value
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_Set(ProteusE_UserSettings_t userSetting, uint8_t* valueP, uint8_t length);

/**
* @brief Set a special user setting, but checks first if the value is already ok
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] userSetting: user setting to be updated
 * @param[in] valueP: Pointer to the new settings value
 * @param[in] length: Length of the value
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_CheckNSet(ProteusE_UserSettings_t userSetting, uint8_t* valueP, uint8_t length);

/**
* @brief Set the BLE device name.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] deviceNameP: Pointer to the device name (allowed characters are 0x20 - 0x7E)
 * @param[in] nameLength: Length of the device name (max 31 characters)
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_SetDeviceName(uint8_t* deviceNameP, uint8_t nameLength);

/**
* @brief Set the BLE advertising timeout.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] advTimeout: advertising timeout in seconds (allowed values: 0-650, where 0 = infinite)
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_SetAdvertisingTimeout(uint16_t advTimeout);

/**
* @brief Sets the minimum and maximum connection interval, which is used to negotiate
 * the connection interval during connection setup.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] minIntervalMs: Minimum connection interval in ms (must be between 8 ms and 4000 ms)
 * @param[in] maxIntervalMs: Maximum connection interval in ms (must be between 8 ms and 4000 ms, must be >= minIntervalMs)
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_SetConnectionInterval(uint16_t minIntervalMs, uint16_t maxIntervalMs);

/**
* @brief Sets the advertising interval, which defines how often advertising packets are transmitted.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] intervalMs: Advertising interval in ms (must be between 20 ms and 10240 ms)
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_SetAdvertisingInterval(uint16_t intervalMs);

/**
* @brief Set the CFG flags (see ProteusE_CfgFlags_t)
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] cfgFlags: CFG flags (see ProteusE_CfgFlags_t)
 *
 * @return True if request succeeded
 *         false otherwise
*/
extern bool ProteusE_SetCFGFlags(uint16_t cfgFlags);

/**
* @brief Sets the Bluetooth MAC address
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] btMacP: Pointer to the MAC (of 6 bytes)
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_SetBTMAC(uint8_t* btMacP);

/**
* @brief Set the BLE TX power.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] txPower: TX power
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_SetTXPower(ProteusE_TXPower_t txPower);

/**
* @brief Set the BLE security flags.
 *
 * Note: When updating this user setting (like enabling bonding or changing the security
 * mode) please remove all existing bonding data using ProteusE_DeleteBonds().
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] secFlags: security flags
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_SetSecFlags(ProteusE_SecFlags_t secFlags);

/**
* @brief Sets the content of the advertising packet (persistent)
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] dataP: Content to put in advertising packet
 * @param[in] length: Length of content
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_SetAdvertisingData(uint8_t* dataP, uint16_t length);

/**
* @brief Sets the content of the scan response packet (persistent)
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] dataP: Content to put in scan response packet
 * @param[in] length: Length of content
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_SetScanResponseData(uint8_t* dataP, uint16_t length);

/**
* @brief Set the UART baudrate index
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] baudrate: UART baudrate
 * @param[in] parity: Parity bit
 * @param[in] flowControlEnable: enable/disable flow control
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_SetBaudrateIndex(ProteusE_BaudRate_t baudrate, ProteusE_UartParity_t parity, bool flowControlEnable);

/**
* @brief Set the BLE static passkey
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] staticPasskeyP: Pointer to the static passkey (6 digits)
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_SetStaticPasskey(uint8_t* staticPasskeyP);

/**
* @brief Sets the Bluetooth appearance of the device
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] appearance: 2 byte Bluetooth appearance value (please check the Bluetooth Core
 *                        Specification: Core Specification Supplement, Part A, section 1.12
 *                        for permissible values)
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_SetAppearance(uint16_t appearance);

/**
* @brief Sets the base UUID of the SPP-like profile.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] uuidP: 16 byte UUID (LSB first)
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_SetSppBaseUuid(uint8_t* uuidP);

/**
* @brief Sets the service UUID of the SPP-like profile.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] uuidP: 2 byte UUID (LSB first)
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_SetSppServiceUuid(uint8_t* uuidP);

/**
* @brief Sets the RX UUID of the SPP-like profile.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] uuidP: 2 byte UUID (LSB first)
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_SetSppRxUuid(uint8_t* uuidP);

/**
* @brief Sets the TX UUID of the SPP-like profile.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] uuidP: 2 byte UUID (LSB first)
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_SetSppTxUuid(uint8_t* uuidP);

/* Read the non-volatile settings */

/**
* @brief Request the current user settings
 *
 * @param[in] userSetting: user setting to be requested
 * @param[out] responseP: Pointer of the memory to put the requested content
 * @param[out] responseLengthP: Length of the requested content
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_Get(ProteusE_UserSettings_t userSetting, uint8_t* responseP, uint16_t* responseLengthP);

/**
* @brief Request the 3 byte firmware version.
 *
 * @param[out] versionP: Pointer to the 3 byte firmware version, version is returned LSB first
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetFWVersion(uint8_t* versionP);

/**
* @brief Request device info.
 *
 * @param[out] deviceInfoP: Pointer to the device info structure
 *
 * @return True if request succeeded
 *         false otherwise
*/
extern bool ProteusE_GetDeviceInfo(ProteusE_DeviceInfo_t* deviceInfoP);

/**
* @brief Request the 3 byte serial number.
 *
 * @param[out] serialNumberP: Pointer to the 3 byte serial number (LSB first)
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetSerialNumber(uint8_t* serialNumberP);

/**
* @brief Request the current BLE device name.
 *
 * @param[out] deviceNameP: Pointer to device name (allowed characters are 0x20 - 0x7E)
 * @param[out] nameLengthP: Pointer to the length of the device name (max 31 characters)
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetDeviceName(uint8_t* deviceNameP, uint16_t* nameLengthP);

/**
* @brief Request the 8 digit MAC.
 *
 * @param[out] macP: Pointer to the MAC
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetMAC(uint8_t* macP);

/**
* @brief Request the 6 digit Bluetooth MAC.
 *
 * @param[out] btMacP: Pointer to the Bluetooth MAC
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetBTMAC(uint8_t* btMacP);

/**
* @brief Request the advertising timeout
 *
 * @param[out] advTimeoutP: Pointer to the advertising timeout
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetAdvertisingTimeout(uint16_t* advTimeoutP);

/**
* @brief Requests the connection interval.
 *
 * @param[out] minIntervalMsP: Pointer to the minimum connection interval
 * @param[out] maxIntervalMsP: Pointer to the maximum connection interval
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetConnectionInterval(uint16_t* minIntervalMsP, uint16_t* maxIntervalMsP);

/**
* @brief Requests the advertising interval.
 *
 * @param[out] intervalMsP: Pointer to the advertising interval
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetAdvertisingInterval(uint16_t* intervalMsP);

/**
* @brief Request the CFG flags (see ProteusE_CfgFlags_t)
 *
 * @param[out] cfgFlagsP: Pointer to the CFG flags (see ProteusE_CfgFlags_t)
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetCFGFlags(uint16_t* cfgFlagsP);

/**
* @brief Request the TX power
 *
 * @param[out] txPowerP: Pointer to the TX power
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetTXPower(ProteusE_TXPower_t* txPowerP);

/**
* @brief Request the security flags
 *
 * @param[out] secFlagsP: Pointer to the security flags
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetSecFlags(ProteusE_SecFlags_t* secFlagsP);

/**
* @brief Request the (custom) content of the advertising packet (persistent)
 *
 * @param[in] dataP: Pointer to (custom) content of advertising packet
 * @param[in] lengthP: Pointer to length of content
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetAdvertisingData(uint8_t* dataP, uint16_t* lengthP);

/**
* @brief Request the (custom) content of the scan response packet (persistent)
 *
 * @param[in] dataP: Pointer to (custom) content of scan response packet
 * @param[in] lengthP: Pointer to length of content
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetScanResponseData(uint8_t* dataP, uint16_t* lengthP);

/**
* @brief Request the UART baudrate index
 *
 * @param[out] baudrateP: Pointer to the UART baudrate index
 * @param[out] parityP: Pointer to the UART parity
 * @param[out] flowControlEnableP: Pointer to the UART flow control parameter
 *
 * @return True if request succeeded
 *         false otherwise
*/
extern bool ProteusE_GetBaudrateIndex(ProteusE_BaudRate_t* baudrateP, ProteusE_UartParity_t* parityP, bool* flowControlEnableP);

/**
* @brief Request the BLE static passkey
 *
 * @param[out] staticPasskeyP: Pointer to the static passkey (6 digits)
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetStaticPasskey(uint8_t* staticPasskeyP);

/**
* @brief Request the Bluetooth appearance of the device
 *
 * @param[out] appearanceP: Pointer to the Bluetooth appearance
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetAppearance(uint16_t* appearanceP);

/**
* @brief Request the base UUID of the SPP-like profile.
 *
 * @param[out] uuidP: Pointer to the 16 byte UUID (LSB first)
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetSppBaseUuid(uint8_t* uuidP);

/**
* @brief Request the service UUID of the SPP-like profile.
 *
 * @param[out] uuidP: Pointer to the 2 byte UUID (LSB first)
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetSppServiceUuid(uint8_t* uuidP);

/**
* @brief Request the RX UUID of the SPP-like profile.
 *
 * @param[out] uuidP: Pointer to the 2 byte UUID (LSB first)
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetSppRxUuid(uint8_t* uuidP);

/**
* @brief Request the TX UUID of the SPP-like profile.
 *
 * @param[out] uuidP: Pointer to the 2 byte UUID (LSB first)
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_GetSppTxUuid(uint8_t* uuidP);

/**
* @brief Enable the direct test mode (DTM)
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_DTMEnable();

/**
* @brief Run a direct test mode (DTM) command
 *
 * @param[in] command: Command
 * @param[in] channel_vendoroption: Channel or vendor option
 * @param[in] length_vendorcmd: Length or vendor command
 * @param[in] payload: Payload
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_DTMRun(ProteusE_DTMCommand_t command, uint8_t channel_vendoroption, uint8_t length_vendorcmd, uint8_t payload);

/**
* @brief Start the direct test mode (DTM) TX test
 *
 * @param[in] channel: Channel
 * @param[in] length: Number of patterns
 * @param[in] pattern: Pattern to send
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_DTMStartTX(uint8_t channel, uint8_t length, ProteusE_DTMTXPattern_t pattern);

/**
* @brief Start the direct test mode (DTM) TX carrier test
 *
 * @param[in] channel: Channel
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_DTMStartTXCarrier(uint8_t channel);

/**
* @brief Stop the current direct test mode (DTM) test
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_DTMStop();

/**
* @brief Set the phy for direct test mode (DTM) test
 *
 * @param[in] phy: Phy
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_DTMSetPhy(ProteusE_Phy_t phy);

/**
* @brief Set the TX power for direct test mode (DTM) test
 *
 * @param[in] power: Tx power
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool ProteusE_DTMSetTXPower(ProteusE_TXPower_t power);

#endif // PROTEUSE_H_INCLUDED

#ifdef __cplusplus
}
#endif
