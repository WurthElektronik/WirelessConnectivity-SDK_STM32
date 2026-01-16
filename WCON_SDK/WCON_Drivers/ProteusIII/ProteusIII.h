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
 * @brief Proteus-III driver header file.
 */

#include <global/global_types.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef PROTEUSIII_H_INCLUDED
#define PROTEUSIII_H_INCLUDED

/* Max. radio payload length for transmission. Note that this is the max. length in high
 * throughput mode (see ProteusIII_CfgFlags_HighThroughputMode) - in normal mode,
 * the max. radio payload length is 243 bytes. */
#define PROTEUSIII_MAX_RADIO_PAYLOAD_LENGTH (uint16_t)964
#define PROTEUSIII_MAX_CMD_PAYLOAD_LENGTH (uint16_t)(PROTEUSIII_MAX_RADIO_PAYLOAD_LENGTH + 7)

/* Max. length of beacon data (custom data in scan response packet,
 * see ProteusIII_SetBeacon()) */
#define PROTEUSIII_MAX_BEACON_LENGTH (uint16_t)19

/* Time (ms) required for booting */
#define PROTEUSIII_BOOT_DURATION (uint16_t)75

/* Channel is considered to be connected if status pin (LED_2) is on for this duration */
#define PROTEUSIII_STATUS_LED_CONNECTED_TIMEOUT_MS (uint16_t)5

/* Default UART baudrate of Proteus-III module */
#define PROTEUSIII_DEFAULT_BAUDRATE (uint32_t)115200

/* Note that this is the max. number of devices that may be queried
 * using ProteusIII_GetBonds() - the module itself might in fact
 * support more devices. */
#define PROTEUSIII_MAX_BOND_DEVICES (uint8_t)20

typedef enum ProteusIII_OperationMode_t
{
    ProteusIII_OperationMode_CommandMode,
    ProteusIII_OperationMode_PeripheralOnlyMode
} ProteusIII_OperationMode_t;

typedef struct ProteusIII_Device_t
{
    uint8_t btmac[6];
    int8_t rssi;
    int8_t txPower;
    uint8_t deviceNameLength;
    uint8_t deviceName[32];
} ProteusIII_Device_t;

typedef struct ProteusIII_Pins_t
{
    WE_Pin_t ProteusIII_Pin_Reset;
    WE_Pin_t ProteusIII_Pin_SleepWakeUp;
    WE_Pin_t ProteusIII_Pin_Boot;
    WE_Pin_t ProteusIII_Pin_Mode;
    WE_Pin_t ProteusIII_Pin_Busy;
    WE_Pin_t ProteusIII_Pin_StatusLed2;
} ProteusIII_Pins_t;

/* Max. number of devices that may be queried using ProteusIII_GetDevices() */
#define PROTEUSIII_MAX_NUMBER_OF_DEVICES (uint8_t)10

typedef struct ProteusIII_GetDevices_t
{
    uint8_t numberOfDevices;
    ProteusIII_Device_t devices[PROTEUSIII_MAX_NUMBER_OF_DEVICES];
} ProteusIII_GetDevices_t;

typedef enum ProteusIII_DisplayPasskeyAction_t
{
    ProteusIII_DisplayPasskeyAction_NoAction = (uint8_t)0x00,
    ProteusIII_DisplayPasskeyAction_PleaseConfirm = (uint8_t)0x01
} ProteusIII_DisplayPasskeyAction_t;

/* Number of free GPIOs available on the module */
#define PROTEUSIII_AMOUNT_GPIO_PINS 6

typedef enum ProteusIII_GPIO_t
{
    ProteusIII_GPIO_1 = (uint8_t)0x01,
    ProteusIII_GPIO_2 = (uint8_t)0x02,
    ProteusIII_GPIO_3 = (uint8_t)0x03,
    ProteusIII_GPIO_4 = (uint8_t)0x04,
    ProteusIII_GPIO_5 = (uint8_t)0x05,
    ProteusIII_GPIO_6 = (uint8_t)0x06
} ProteusIII_GPIO_t;

typedef enum ProteusIII_GPIO_IO_t
{
    ProteusIII_GPIO_IO_Disconnected = (uint8_t)0x00,
    ProteusIII_GPIO_IO_Input = (uint8_t)0x01,
    ProteusIII_GPIO_IO_Output = (uint8_t)0x02,
    ProteusIII_GPIO_IO_PWM = (uint8_t)0x03
} ProteusIII_GPIO_IO_t;

typedef enum ProteusIII_GPIO_Output_t
{
    ProteusIII_GPIO_Output_Low = (uint8_t)0x00,
    ProteusIII_GPIO_Output_High = (uint8_t)0x01
} ProteusIII_GPIO_Output_t;

typedef enum ProteusIII_GPIO_Input_t
{
    ProteusIII_GPIO_Input_NoPull = (uint8_t)0x00,
    ProteusIII_GPIO_Input_PullDown = (uint8_t)0x01,
    ProteusIII_GPIO_Input_PullUp = (uint8_t)0x02
} ProteusIII_GPIO_Input_t;

typedef struct ProteusIII_GPIO_PwmValue_t
{
    uint16_t period; /* in ms */
    uint8_t ratio;   /* 0-255 (0%-100%)*/
} ProteusIII_GPIO_PwmValue_t;

typedef struct ProteusIII_GPIOConfigBlock_t
{
    ProteusIII_GPIO_t gpioId;
    ProteusIII_GPIO_IO_t function;
    union /* 3Byte */
    {
        ProteusIII_GPIO_PwmValue_t pwm;
        ProteusIII_GPIO_Input_t input;
        ProteusIII_GPIO_Output_t output;
    } value;
} ProteusIII_GPIOConfigBlock_t;

typedef struct ProteusIII_GPIOControlBlock_t
{
    ProteusIII_GPIO_t gpioId;
    union /* 1Byte */
    {
        ProteusIII_GPIO_Output_t output;
        uint8_t ratio; /* 0-255 (0%-100%)*/
    } value;
} ProteusIII_GPIOControlBlock_t;

typedef enum ProteusIII_DriverState_t
{
    ProteusIII_DriverState_BLE_Idle = (uint8_t)0x00,
    ProteusIII_DriverState_BLE_Connected = (uint8_t)0x01,
    ProteusIII_DriverState_BLE_ChannelOpen = (uint8_t)0x02
} ProteusIII_DriverState_t;

typedef enum ProteusIII_SecurityState_t
{
    ProteusIII_SecurityState_BLE_ReBonded = (uint8_t)0x00,
    ProteusIII_SecurityState_BLE_Bonded = (uint8_t)0x01,
    ProteusIII_SecurityState_BLE_Paired = (uint8_t)0x02
} ProteusIII_SecurityState_t;

typedef enum ProteusIII_DisconnectReason_t
{
    ProteusIII_DisconnectReason_Unknown,
    ProteusIII_DisconnectReason_ConnectionTimeout,
    ProteusIII_DisconnectReason_UserTerminatedConnection,
    ProteusIII_DisconnectReason_HostTerminatedConnection,
    ProteusIII_DisconnectReason_ConnectionIntervalUnacceptable,
    ProteusIII_DisconnectReason_MicFailure,
    ProteusIII_DisconnectReason_ConnectionSetupFailed
} ProteusIII_DisconnectReason_t;

typedef enum ProteusIII_BLE_Role_t
{
    ProteusIII_BLE_Role_None = (uint8_t)0x00,
    ProteusIII_BLE_Role_Peripheral = (uint8_t)0x01,
    ProteusIII_BLE_Role_Central = (uint8_t)0x02,
    ProteusIII_BLE_Role_DTM = (uint8_t)0x10
} ProteusIII_BLE_Role_t;

typedef enum ProteusIII_BLE_Action_t
{
    ProteusIII_BLE_Action_None = (uint8_t)0x00,
    ProteusIII_BLE_Action_Idle = (uint8_t)0x01,
    ProteusIII_BLE_Action_Scanning = (uint8_t)0x02,
    ProteusIII_BLE_Action_Connected = (uint8_t)0x03,
    ProteusIII_BLE_Action_Sleep = (uint8_t)0x04,
    ProteusIII_BLE_Action_DTM = (uint8_t)0x05
} ProteusIII_BLE_Action_t;

/**
 * @brief User settings
 */
typedef enum ProteusIII_UserSettings_t
{
    ProteusIII_USERSETTING_FS_FWVersion = (uint8_t)0x01,
    ProteusIII_USERSETTING_RF_DEVICE_NAME = (uint8_t)0x02,
    ProteusIII_USERSETTING_FS_MAC = (uint8_t)0x03,
    ProteusIII_USERSETTING_FS_BTMAC = (uint8_t)0x04,
    ProteusIII_USERSETTING_UART_TRANSP_ETX = (uint8_t)0x05,
    ProteusIII_USERSETTING_UART_TRANSP_ETX_CFG = (uint8_t)0x06,
    ProteusIII_USERSETTING_RF_ADVERTISING_TIMEOUT = (uint8_t)0x07,
    ProteusIII_USERSETTING_RF_CONNECTION_TIMING = (uint8_t)0x08,
    ProteusIII_USERSETTING_RF_SCAN_TIMING = (uint8_t)0x09,
    ProteusIII_USERSETTING_RF_SCAN_FACTOR = (uint8_t)0x0A,
    ProteusIII_USERSETTING_UART_CONFIG_INDEX = (uint8_t)0x0B,
    ProteusIII_USERSETTING_RF_SEC_FLAGS = (uint8_t)0x0C,
    ProteusIII_USERSETTING_RF_SCAN_FLAGS = (uint8_t)0x0D,
    ProteusIII_USERSETTING_RF_BEACON_FLAGS = (uint8_t)0x0E,
    ProteusIII_USERSETTING_FS_DEVICE_INFO = (uint8_t)0x0F,
    ProteusIII_USERSETTING_FS_SERIAL_NUMBER = (uint8_t)0x10,
    ProteusIII_USERSETTING_RF_TX_POWER = (uint8_t)0x11,
    ProteusIII_USERSETTING_RF_STATIC_PASSKEY = (uint8_t)0x12,
    ProteusIII_USERSETTING_DIS_FLAGS = (uint8_t)0x13,
    ProteusIII_USERSETTING_DIS_MANUFACTURERNAME = (uint8_t)0x14,
    ProteusIII_USERSETTING_DIS_MODELNUMBER = (uint8_t)0x15,
    ProteusIII_USERSETTING_DIS_SERIALNUMBER = (uint8_t)0x16,
    ProteusIII_USERSETTING_DIS_HWVERSION = (uint8_t)0x17,
    ProteusIII_USERSETTING_DIS_SWVERSION = (uint8_t)0x18,
    ProteusIII_USERSETTING_RF_APPEARANCE = (uint8_t)0x19,
    ProteusIII_USERSETTING_RF_SPPBASEUUID = (uint8_t)0x1A,
    ProteusIII_USERSETTING_RF_CFGFLAGS = (uint8_t)0x1C,
    ProteusIII_USERSETTING_RF_ADVERTISING_FLAGS = (uint8_t)0x1D,
    ProteusIII_USERSETTING_RF_SPPServiceUUID = (uint8_t)0x20,
    ProteusIII_USERSETTING_RF_SPPRXUUID = (uint8_t)0x21,
    ProteusIII_USERSETTING_RF_SPPTXUUID = (uint8_t)0x22,
    ProteusIII_USERSETTING_OPTION_COMPANYID = (uint8_t)0x23,
    ProteusIII_USERSETTING_UART_TRANSPARENT_TIMEOUT = (uint8_t)0x24,
    ProteusIII_USERSETTING_UART_TRANSPARENT_MAXPAYLOAD = (uint8_t)0x25,
    ProteusIII_USERSETTING_RF_SECFLAGSPERONLY = (uint8_t)0x2C,
} ProteusIII_UserSettings_t;

/**
 * @brief Configuration flags to be used with ProteusIII_SetCFGFlags() and ProteusIII_GetCFGFlags()
 */
typedef enum ProteusIII_CfgFlags_t
{
    ProteusIII_CfgFlags_HighThroughputMode = (1 << 0),
    ProteusIII_CfgFlags_LongRangeConnectionMode = (1 << 1),
    ProteusIII_CfgFlags_GPIORemoteConfig = (1 << 2)
} ProteusIII_CfgFlags_t;

/**
 * @brief Flags to be used with ProteusIII_SetScanFlags() and ProteusIII_GetScanFlags()
 */
typedef enum ProteusIII_ScanFlags_t
{
    ProteusIII_ScanFlags_None = 0,
    ProteusIII_ScanFlags_ActiveScanning = 1
} ProteusIII_ScanFlags_t;

/**
 * @brief Flags to be used with ProteusIII_SetBeaconFlags() and ProteusIII_GetBeaconFlags()
 */
typedef enum ProteusIII_BeaconFlags_t
{
    ProteusIII_BeaconFlags_ReceiveDisabled = 0,
    ProteusIII_BeaconFlags_ReceiveInterpreted = 1,
    ProteusIII_BeaconFlags_ReceiveRaw = 2,
    ProteusIII_BeaconFlags_ReceiveInterpretedDropDuplicates = 3,
    ProteusIII_BeaconFlags_ReceiveRssiIndications = 4
} ProteusIII_BeaconFlags_t;

/**
 * @brief Flags to be used with ProteusIII_SetAdvertisingFlags() and ProteusIII_GetAdvertisingFlags()
 */
typedef enum ProteusIII_AdvertisingFlags_t
{
    ProteusIII_AdvertisingFlags_Default = 0,
    ProteusIII_AdvertisingFlags_DeviceNameAndUuid = 1,
    ProteusIII_AdvertisingFlags_DeviceNameAndTxPower = 2
} ProteusIII_AdvertisingFlags_t;

#define PROTEUSIII_SEC_MODE_BONDING_ENABLE_MASK (uint8_t)0x08
#define PROTEUSIII_SEC_MODE_BONDEDCONNECTIONSONLY_ENABLE (uint8_t)0x10

typedef enum ProteusIII_SecFlags_t
{
    ProteusIII_SecFlags_NONE = (uint8_t)0x00,
    ProteusIII_SecFlags_LescJustWorks = (uint8_t)0x01,
    ProteusIII_SecFlags_JustWorks = (uint8_t)0x02,
    ProteusIII_SecFlags_StaticPassKey = (uint8_t)0x03,
    ProteusIII_SecFlags_LescNumCompare = (uint8_t)0x04,
    ProteusIII_SecFlags_LescPassKey = (uint8_t)0x05,
    ProteusIII_SecFlags_LescJustWorks_Bonding = (uint8_t)(ProteusIII_SecFlags_LescJustWorks | PROTEUSIII_SEC_MODE_BONDING_ENABLE_MASK),
    ProteusIII_SecFlags_JustWorks_Bonding = (uint8_t)(ProteusIII_SecFlags_JustWorks | PROTEUSIII_SEC_MODE_BONDING_ENABLE_MASK),
    ProteusIII_SecFlags_StaticPassKey_Bonding = (uint8_t)(ProteusIII_SecFlags_StaticPassKey | PROTEUSIII_SEC_MODE_BONDING_ENABLE_MASK),
    ProteusIII_SecFlags_LescNumCompare_Bonding = (uint8_t)(ProteusIII_SecFlags_LescNumCompare | PROTEUSIII_SEC_MODE_BONDING_ENABLE_MASK),
    ProteusIII_SecFlags_LescPassKey_Bonding = (uint8_t)(ProteusIII_SecFlags_LescPassKey | PROTEUSIII_SEC_MODE_BONDING_ENABLE_MASK),
    ProteusIII_SecFlags_LescJustWorks_BondingOnly = (uint8_t)(ProteusIII_SecFlags_LescJustWorks | PROTEUSIII_SEC_MODE_BONDING_ENABLE_MASK | PROTEUSIII_SEC_MODE_BONDEDCONNECTIONSONLY_ENABLE),
    ProteusIII_SecFlags_JustWorks_BondingOnly = (uint8_t)(ProteusIII_SecFlags_JustWorks | PROTEUSIII_SEC_MODE_BONDING_ENABLE_MASK | PROTEUSIII_SEC_MODE_BONDEDCONNECTIONSONLY_ENABLE),
    ProteusIII_SecFlags_StaticPassKey_BondingOnly = (uint8_t)(ProteusIII_SecFlags_StaticPassKey | PROTEUSIII_SEC_MODE_BONDING_ENABLE_MASK | PROTEUSIII_SEC_MODE_BONDEDCONNECTIONSONLY_ENABLE),
    ProteusIII_SecFlags_LescNumCompare_BondingOnly = (uint8_t)(ProteusIII_SecFlags_LescNumCompare | PROTEUSIII_SEC_MODE_BONDING_ENABLE_MASK | PROTEUSIII_SEC_MODE_BONDEDCONNECTIONSONLY_ENABLE),
    ProteusIII_SecFlags_LescPassKey_BondingOnly = (uint8_t)(ProteusIII_SecFlags_LescPassKey | PROTEUSIII_SEC_MODE_BONDING_ENABLE_MASK | PROTEUSIII_SEC_MODE_BONDEDCONNECTIONSONLY_ENABLE)
} ProteusIII_SecFlags_t;

typedef enum ProteusIII_ConnectionTiming_t
{
    ProteusIII_ConnectionTiming_0 = (uint8_t)0x00,
    ProteusIII_ConnectionTiming_1 = (uint8_t)0x01,
    ProteusIII_ConnectionTiming_2 = (uint8_t)0x02,
    ProteusIII_ConnectionTiming_3 = (uint8_t)0x03,
    ProteusIII_ConnectionTiming_4 = (uint8_t)0x04,
    ProteusIII_ConnectionTiming_5 = (uint8_t)0x05,
    ProteusIII_ConnectionTiming_6 = (uint8_t)0x06,
    ProteusIII_ConnectionTiming_7 = (uint8_t)0x07,
    ProteusIII_ConnectionTiming_8 = (uint8_t)0x08,
    ProteusIII_ConnectionTiming_9 = (uint8_t)0x09,
    ProteusIII_ConnectionTiming_10 = (uint8_t)0x0A,
    ProteusIII_ConnectionTiming_11 = (uint8_t)0x0B,
    ProteusIII_ConnectionTiming_12 = (uint8_t)0x0C
} ProteusIII_ConnectionTiming_t;

typedef enum ProteusIII_ScanTiming_t
{
    ProteusIII_ScanTiming_0 = (uint8_t)0x00,
    ProteusIII_ScanTiming_1 = (uint8_t)0x01,
    ProteusIII_ScanTiming_2 = (uint8_t)0x02,
    ProteusIII_ScanTiming_3 = (uint8_t)0x03,
    ProteusIII_ScanTiming_4 = (uint8_t)0x04,
    ProteusIII_ScanTiming_5 = (uint8_t)0x05,
    ProteusIII_ScanTiming_6 = (uint8_t)0x06,
    ProteusIII_ScanTiming_7 = (uint8_t)0x07,
    ProteusIII_ScanTiming_8 = (uint8_t)0x08,
    ProteusIII_ScanTiming_9 = (uint8_t)0x09,
    ProteusIII_ScanTiming_10 = (uint8_t)0x0A,
    ProteusIII_ScanTiming_11 = (uint8_t)0x0B
} ProteusIII_ScanTiming_t;

typedef enum ProteusIII_TXPower_t
{
    ProteusIII_TXPower_8 = (int8_t)8,
    ProteusIII_TXPower_7 = (int8_t)7,
    ProteusIII_TXPower_6 = (int8_t)6,
    ProteusIII_TXPower_5 = (int8_t)5,
    ProteusIII_TXPower_4 = (int8_t)4,
    ProteusIII_TXPower_3 = (int8_t)3,
    ProteusIII_TXPower_2 = (int8_t)2,
    ProteusIII_TXPower_0 = (int8_t)0,
    ProteusIII_TXPower_minus4 = (int8_t)-4,
    ProteusIII_TXPower_minus8 = (int8_t)-8,
    ProteusIII_TXPower_minus12 = (int8_t)-12,
    ProteusIII_TXPower_minus16 = (int8_t)-16,
    ProteusIII_TXPower_minus20 = (int8_t)-20,
    ProteusIII_TXPower_minus40 = (int8_t)-40
} ProteusIII_TXPower_t;

typedef enum ProteusIII_BaudRate_t
{
    ProteusIII_BaudRateIndex_1200 = (uint8_t)0,
    ProteusIII_BaudRateIndex_2400 = (uint8_t)2,
    ProteusIII_BaudRateIndex_4800 = (uint8_t)4,
    ProteusIII_BaudRateIndex_9600 = (uint8_t)6,
    ProteusIII_BaudRateIndex_14400 = (uint8_t)8,
    ProteusIII_BaudRateIndex_19200 = (uint8_t)10,
    ProteusIII_BaudRateIndex_28800 = (uint8_t)12,
    ProteusIII_BaudRateIndex_38400 = (uint8_t)14,
    ProteusIII_BaudRateIndex_56000 = (uint8_t)16,
    ProteusIII_BaudRateIndex_57600 = (uint8_t)18,
    ProteusIII_BaudRateIndex_76800 = (uint8_t)20,
    ProteusIII_BaudRateIndex_115200 = (uint8_t)22,
    ProteusIII_BaudRateIndex_230400 = (uint8_t)24,
    ProteusIII_BaudRateIndex_250000 = (uint8_t)26,
    ProteusIII_BaudRateIndex_460800 = (uint8_t)28,
    ProteusIII_BaudRateIndex_921600 = (uint8_t)30,
    ProteusIII_BaudRateIndex_1000000 = (uint8_t)32
} ProteusIII_BaudRate_t;

typedef enum ProteusIII_UartParity_t
{
    ProteusIII_UartParity_Even,
    ProteusIII_UartParity_None
} ProteusIII_UartParity_t;

typedef enum ProteusIII_Phy_t
{
    ProteusIII_Phy_1MBit = 0x01,
    ProteusIII_Phy_2MBit = 0x02,
    ProteusIII_Phy_125kBit_LECoded = 0x04
} ProteusIII_Phy_t;

/**
 * @brief Device info structure.
 * @see ProteusIII_GetDeviceInfo()
 */
typedef struct ProteusIII_DeviceInfo_t
{
    uint16_t osVersion;
    uint32_t buildCode;
    uint16_t packageVariant;
    uint32_t chipId;
} ProteusIII_DeviceInfo_t;

/**
 * @brief Module state structure.
 * @see ProteusIII_GetState()
 */
typedef struct ProteusIII_ModuleState_t
{
    ProteusIII_BLE_Role_t role;
    ProteusIII_BLE_Action_t action;
    uint8_t connectedDeviceBtMac[6];
} ProteusIII_ModuleState_t;

/**
 * @brief Entry in ProteusIII_BondDatabase_t.
 * @see ProteusIII_GetBonds()
 */
typedef struct ProteusIII_BondDatabaseEntry_t
{
    uint16_t id;
    uint8_t btMac[6];
} ProteusIII_BondDatabaseEntry_t;

/**
 * @brief List of bonded devices.
 * @see ProteusIII_GetBonds()
 */
typedef struct ProteusIII_BondDatabase_t
{
    uint8_t nrOfDevices;
    ProteusIII_BondDatabaseEntry_t devices[PROTEUSIII_MAX_BOND_DEVICES];
} ProteusIII_BondDatabase_t;

/**
 * @brief DTM command type
 */
typedef enum ProteusIII_DTMCommand_t
{
    ProteusIII_DTMCommand_Setup = 0x00,
    ProteusIII_DTMCommand_StartRX = 0x01,
    ProteusIII_DTMCommand_StartTX = 0x02,
    ProteusIII_DTMCommand_Stop = 0x03,
} ProteusIII_DTMCommand_t;

/**
 * @brief DTM TX pattern
 */
typedef enum ProteusIII_DTMTXPattern_t
{
    ProteusIII_DTMTXPattern_PRBS9 = 0x00,
    ProteusIII_DTMTXPattern_0x0F = 0x01,
    ProteusIII_DTMTXPattern_0x55 = 0x02,
} ProteusIII_DTMTXPattern_t;

/* Callback definition */

typedef void (*ProteusIII_RxCallback_t)(uint8_t* payload, uint16_t payloadLength, uint8_t* btMac, int8_t rssi);
/* Note that btMac is not provided if success == false (is set to all zeros) */
typedef void (*ProteusIII_ConnectCallback_t)(bool success, uint8_t* btMac);
typedef void (*ProteusIII_SecurityCallback_t)(uint8_t* btMac, ProteusIII_SecurityState_t securityState);
typedef void (*ProteusIII_PasskeyCallback_t)(uint8_t* btMac);
typedef void (*ProteusIII_DisplayPasskeyCallback_t)(ProteusIII_DisplayPasskeyAction_t action, uint8_t* btMac, uint8_t* passkey);
typedef void (*ProteusIII_DisconnectCallback_t)(ProteusIII_DisconnectReason_t reason);
typedef void (*ProteusIII_ChannelOpenCallback_t)(uint8_t* btMac, uint16_t maxPayload);
/* Note that btMac is not provided if success == false (is set to all zeros) */
typedef void (*ProteusIII_PhyUpdateCallback_t)(bool success, uint8_t* btMac, uint8_t phyRx, uint8_t phyTx);
typedef void (*ProteusIII_SleepCallback_t)();
typedef void (*ProteusIII_RssiCallback_t)(uint8_t* btMac, int8_t rssi, int8_t txPower);
/* Note that the gpioId parameter is of type uint8_t instead of ProteusIII_GPIO_t, as the
 * remote device may support other GPIOs than this device. */
typedef void (*ProteusIII_GpioWriteCallback_t)(bool remote, uint8_t gpioId, uint8_t value);
typedef void (*ProteusIII_GpioRemoteConfigCallback_t)(ProteusIII_GPIOConfigBlock_t* gpioConfig);
typedef void (*ProteusIII_ErrorCallback_t)(uint8_t errorCode);

/**
 * @brief Callback configuration structure. Used as argument for ProteusIII_Init().
 *
 * Please note that code in callbacks should be kept simple, as the callback
 * functions are called from ISRs. For this reason, it is also not possible
 * to send requests to the Proteus-III directly from inside a callback.
 */
typedef struct ProteusIII_CallbackConfig_t
{
    ProteusIII_RxCallback_t rxCb;                             /**< Callback for CMD_DATA_IND */
    ProteusIII_RxCallback_t beaconRxCb;                       /**< Callback for CMD_BEACON_IND and CMD_BEACON_RSP */
    ProteusIII_ConnectCallback_t connectCb;                   /**< Callback for CMD_CONNECT_IND */
    ProteusIII_SecurityCallback_t securityCb;                 /**< Callback for CMD_SECURITY_IND */
    ProteusIII_PasskeyCallback_t passkeyCb;                   /**< Callback for CMD_PASSKEY_IND */
    ProteusIII_DisplayPasskeyCallback_t displayPasskeyCb;     /**< Callback for CMD_DISPLAY_PASSKEY_IND */
    ProteusIII_DisconnectCallback_t disconnectCb;             /**< Callback for CMD_DISCONNECT_IND */
    ProteusIII_ChannelOpenCallback_t channelOpenCb;           /**< Callback for CMD_CHANNELOPEN_RSP */
    ProteusIII_PhyUpdateCallback_t phyUpdateCb;               /**< Callback for CMD_PHYUPDATE_IND */
    ProteusIII_SleepCallback_t sleepCb;                       /**< Callback for CMD_SLEEP_IND */
    ProteusIII_RssiCallback_t rssiCb;                         /**< Callback for CMD_RSSI_IND */
    ProteusIII_GpioWriteCallback_t gpioWriteCb;               /**< Callback for CMD_GPIO_LOCAL_WRITE_IND and CMD_GPIO_REMOTE_WRITE_IND */
    ProteusIII_GpioRemoteConfigCallback_t gpioRemoteConfigCb; /**< Callback for CMD_GPIO_REMOTE_WRITECONFIG_IND */
    ProteusIII_ErrorCallback_t errorCb;                       /**< Callback for CMD_ERROR_IND */
} ProteusIII_CallbackConfig_t;

/**
 * @brief Initialize the ProteusIII for serial interface.
 *
 * Caution: The parameter baudrate must match the configured UserSettings of the ProteusIII.
 *          The baudrate parameter must match to perform a successful FTDI communication.
 *          Updating this parameter during runtime may lead to communication errors.
 *
 * @param[in] uartP: Definition of the uart connected to the module
 * @param[in] pinoutP: Definition of the gpios connected to the module
 * @param[in] opMode: operation mode
 * @param[in] callbackConfig: Callback configuration
 *
 * @return True if initialization succeeded,
 *         false otherwise
 */
extern bool ProteusIII_Init(WE_UART_t* uartP, ProteusIII_Pins_t* pinoutP, ProteusIII_OperationMode_t opMode, ProteusIII_CallbackConfig_t callbackConfig);

/**
 * @brief Deinitialize the ProteusIII interface.
 *
 * @return True if deinitialization succeeded,
 *         false otherwise
 */
extern bool ProteusIII_Deinit(void);

/**
 * @brief Request the module state
 *
 * @param[out] moduleStateP: Pointer to module state
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetState(ProteusIII_ModuleState_t* moduleStateP);

/**
 * @brief Reset the ProteusIII by pin.
 *
 * @return True if reset succeeded,
 *         false otherwise
 */
extern bool ProteusIII_PinReset(void);

/**
 * @brief Reset the ProteusIII by command
 *
 * @return True if reset succeeded,
 *         false otherwise
 */
extern bool ProteusIII_Reset(void);

/**
 * @brief Put the ProteusIII into sleep mode.
 *
 * @return True if succeeded,
 *         false otherwise
 */
extern bool ProteusIII_Sleep();

/**
 * @brief Wake up the ProteusIII from sleep by pin.
 *
 * Please note that the WAKE_UP pin is also used for re-enabling the UART using
 * the function ProteusIII_PinUartEnable(). In that case, the module answers
 * with a different response. The two functions are therefore not interchangeable.
 *
 * @return True if wake-up succeeded,
 *         false otherwise
 */
extern bool ProteusIII_PinWakeup();

/**
 * @brief Disables the UART of the Proteus-III.
 *
 * It will be re-enabled when the module has to send data to the host (e.g. data was received
 * via radio or a state is indicated) or it can be manually re-enabled using ProteusIII_PinWakeup().
 *
 * @return True if disable succeeded,
 *         false otherwise
 */
extern bool ProteusIII_UartDisable(void);

/**
 * @brief Re-enables the module's UART using the WAKE_UP pin after having disabled the
 * UART using ProteusIII_UartDisable().
 *
 * Please note that the WAKE_UP pin is also used for waking up the module from sleep
 * mode using the function ProteusIII_PinWakeup(). In that case, the module answers
 * with a different response. The two functions are therefore not interchangeable.
 *
 * @return True if enabling UART succeeded,
 *         false otherwise
 */
extern bool ProteusIII_PinUartEnable(void);

/**
 * @brief Connect to the BLE device with the corresponding BTMAC
 *
 * @param[in] btMacP: Pointer to btmac
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_Connect(uint8_t* btMacP);

/**
 * @brief Disconnect the ProteusIII connection if open.
 *
 * @return True if disconnect succeeded,
 *         false otherwise
 */
extern bool ProteusIII_Disconnect();

/**
 * @brief Request the current state of the driver
 *
 * @return driver state
 */
extern ProteusIII_DriverState_t ProteusIII_GetDriverState();

/**
 * @brief Start scan
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_ScanStart();

/**
 * @brief Stop a scan
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_ScanStop();

/**
 * @brief Request the scan results
 *
 * @param[out] devicesP: Pointer to scan result struct
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetDevices(ProteusIII_GetDevices_t* devicesP);

/**
 * @brief Transmit data if a connection is open
 *
 * @param[in] payloadP: Pointer to the data to transmit
 * @param[in] length: Length of the data to transmit
 *
 * @return True if succeeded,
 *         false otherwise
 */
extern bool ProteusIII_Transmit(uint8_t* payloadP, uint16_t length);

/**
 * @brief Transmitting the data via UART.
 *
 * @param[in] data: Pointer to the data.
 * @param[in] dataLength: Length of the data.
 *
 * @return True if transmission succeeded,
 *         false otherwise
 */
extern bool ProteusIII_Transparent_Transmit(const uint8_t* data, uint16_t dataLength);

/**
 * @brief Places user data in the scan response packet.
 *
 * @param[in] beaconDataP: Pointer to the data to put in scan response packet
 * @param[in] length: Length of the data to put in scan response packet
 *
 * @return True if succeeded,
 *         false otherwise
 */
extern bool ProteusIII_SetBeacon(uint8_t* beaconDataP, uint16_t length);

/**
 * @brief Answer on a passkey request with a passkey to setup a connection
 *
 * @param[in] passkeyP: Pointer to passkey
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_Passkey(uint8_t* passkeyP);

/**
 * @brief Answer on a numeric comparison request
 *
 * @param[in] keyIsOk: boolean to confirm if the key shown is correct
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_NumericCompareConfirm(bool keyIsOk);

/**
 * @brief Update the phy during an open connection
 *
 * @param[in] phy: new phy
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_PhyUpdate(ProteusIII_Phy_t phy);

extern ProteusIII_DriverState_t ProteusIII_GetDriverState();

/**
 * @brief Returns the current level of the status pin (LED_2).
 * Is used as indication for channel open in peripheral only mode.
 *
 * @param[out] statusLed2LevelP: status pin level
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetStatusLed2PinLevel(WE_Pin_Level_t* statusLed2LevelP);

/**
 * @brief Returns the current level of the BUSY pin.
 *
 * @param[out] busyStateP: busy pin state
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_IsPeripheralOnlyModeBusy(bool* busyStateP);

/* functions to control the GPIO feature */

/**
 * @brief Sets the callback function which is executed if a byte has been received from Proteus-III.
 *
 * The default callback is ProteusIII_HandleRxByte().
 *
 * @param[in] callback: Pointer to byte received callback function (default callback is used if NULL)
 */
extern void ProteusIII_SetByteRxCallback(WE_UART_HandleRxByte_t callback);

/**
 * @brief Configure the local GPIO of the module
 *
 * @param[in] configP: Pointer to one or more pin configurations
 * @param[in] numberOfConfigs: number of entries in configP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GPIOLocalWriteConfig(ProteusIII_GPIOConfigBlock_t* configP, uint16_t numberOfConfigs);

/**
 * @brief Read the local GPIO configuration of the module
 *
 * @param[out] configP: Pointer to one or more pin configurations
 * @param[out] numberOfConfigsP: Pointer to number of entries in configP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GPIOLocalReadConfig(ProteusIII_GPIOConfigBlock_t* configP, uint16_t* numberOfConfigsP);

/**
 * @brief Set the output value of the local pin. Pin has to be configured first.
 * See ProteusIII_GPIOLocalWriteConfig
 *
 * @param[in] controlP: Pointer to one or more pin controls
 * @param[in] numberOfControls: number of entries in controlP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GPIOLocalWrite(ProteusIII_GPIOControlBlock_t* controlP, uint16_t numberOfControls);

/**
 * @brief Read the input of the pin. Pin has to be configured first.
 * See ProteusIII_GPIOLocalWriteConfig
 *
 * @param[in] gpioToReadP: One or more pins to read.
 * @param[in] amountGPIOToRead: amount of pins to read and therefore length of GPIOToRead
 * @param[out] controlP: Pointer to controlBlock
 * @param[out] numberOfControlsP: Pointer to number of entries in controlP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GPIOLocalRead(uint8_t* gpioToReadP, uint8_t amountGPIOToRead, ProteusIII_GPIOControlBlock_t* controlP, uint16_t* numberOfControlsP);

/**
 * @brief Configure the remote GPIO of the module
 *
 * @param[in] configP: Pointer to one or more pin configurations
 * @param[in] numberOfConfigs: number of entries in configP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GPIORemoteWriteConfig(ProteusIII_GPIOConfigBlock_t* configP, uint16_t numberOfConfigs);

/**
 * @brief Read the remote GPIO configuration of the module
 *
 * @param[out] configP: Pointer to one or more pin configurations
 * @param[out] numberOfConfigsP: Pointer to number of entries in configP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GPIORemoteReadConfig(ProteusIII_GPIOConfigBlock_t* configP, uint16_t* numberOfConfigsP);

/**
 * @brief Set the output value of the remote pin. Pin has to be configured first.
 * See ProteusIII_GPIORemoteWriteConfig
 *
 * @param[in] controlP: Pointer to one or more pin controls
 * @param[in] numberOfControls: number of entries in controlP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GPIORemoteWrite(ProteusIII_GPIOControlBlock_t* controlP, uint16_t numberOfControls);

/**
 * @brief Read the input of the pins. Pin has to be configured first.
 * See ProteusIII_GPIORemoteWriteConfig
 *
 * @param[in] gpioToReadP: One or more pins to read.
 * @param[in] amountGPIOToRead: amount of pins to read and therefore length of gpioToReadP
 * @param[out] controlP: Pointer to controlBlock
 * @param[out] numberOfControlsP: Pointer to number of entries in controlP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GPIORemoteRead(uint8_t* gpioToReadP, uint8_t amountGPIOToRead, ProteusIII_GPIOControlBlock_t* controlP, uint16_t* numberOfControlsP);

/**
 * @brief Requests the BTMAC addresses of all bonded devices.
 *
 * Note that this function supports a maximum of PROTEUSIII_MAX_BOND_DEVICES
 * returned devices. The Proteus-III module itself might be capable of
 * bonding more devices.
 *
 * @param[out] bondDatabaseP: Pointer to bond database
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetBonds(ProteusIII_BondDatabase_t* bondDatabaseP);

/**
 * @brief Removes all bonding data.
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_DeleteBonds();

/**
 * @brief Removes the bonding information for a single device.
 *
 * @param[in] bondId: bond ID of the device to be removed
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_DeleteBond(uint8_t bondId);

/**
 * @brief Temporarily allow unbonded connections, in case only bonded connections have been configured
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_AllowUnbondedConnections();

/* Functions that write the non-volatile settings in the flash: After modification of any non-volatile setting,
 * the module must be reset such that the update takes effect.
 * IMPORTANT: Use only in rare cases, since flash can be written to only a limited number of times.
 */
extern bool ProteusIII_FactoryReset();

/**
 * @brief Set a special user setting.
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
extern bool ProteusIII_Set(ProteusIII_UserSettings_t userSetting, uint8_t* valueP, uint8_t length);

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
extern bool ProteusIII_CheckNSet(ProteusIII_UserSettings_t userSetting, uint8_t* valueP, uint8_t length);

/**
 * @brief Set the BLE device name.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] deviceNameP: Pointer to the device name
 * @param[in] nameLength: Length of the device name
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_SetDeviceName(uint8_t* deviceNameP, uint8_t nameLength);

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
extern bool ProteusIII_SetAdvertisingTimeout(uint16_t advTimeout);

/**
 * @brief Set the advertising flags.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] advFlags: Advertising flags
 *
 * @return True if request succeeded
 *         false otherwise
 */
extern bool ProteusIII_SetAdvertisingFlags(ProteusIII_AdvertisingFlags_t advFlags);

/**
 * @brief Set the scan flags (see ProteusIII_ScanFlags_t).
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] scanFlags: Scan flags (see ProteusIII_ScanFlags_t)
 *
 * @return True if request succeeded
 *         false otherwise
 */
extern bool ProteusIII_SetScanFlags(uint8_t scanFlags);

/**
 * @brief Set the beacon flags.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] beaconFlags: Beacon flags
 *
 * @return True if request succeeded
 *         false otherwise
 */
extern bool ProteusIII_SetBeaconFlags(ProteusIII_BeaconFlags_t beaconFlags);

/**
 * @brief Set the CFG flags (see ProteusIII_CfgFlags_t)
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] cfgFlags: CFG flags (see ProteusIII_CfgFlags_t)
 *
 * @return True if request succeeded
 *         false otherwise
 */
extern bool ProteusIII_SetCFGFlags(uint16_t cfgFlags);

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
extern bool ProteusIII_SetBTMAC(uint8_t* btMacP);

/**
 * @brief Set the BLE connection timing.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] connectionTiming: connection timing
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_SetConnectionTiming(ProteusIII_ConnectionTiming_t connectionTiming);

/**
 * @brief Set the BLE scan timing
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] scanTiming: scan timing
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_SetScanTiming(ProteusIII_ScanTiming_t scanTiming);

/**
 * @brief Set the BLE scan factor
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] scanFactor: scan factor
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_SetScanFactor(uint8_t scanFactor);

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
extern bool ProteusIII_SetTXPower(ProteusIII_TXPower_t txPower);

/**
 * @brief Set the BLE security flags.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] secFlags: security flags
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_SetSecFlags(ProteusIII_SecFlags_t secFlags);

/**
 * @brief Set the BLE security flags for peripheral only mode.
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] secFlags: security flags
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_SetSecFlagsPeripheralOnly(ProteusIII_SecFlags_t secFlags);

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
extern bool ProteusIII_SetBaudrateIndex(ProteusIII_BaudRate_t baudrate, ProteusIII_UartParity_t parity, bool flowControlEnable);

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
extern bool ProteusIII_SetStaticPasskey(uint8_t* staticPasskeyP);

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
extern bool ProteusIII_SetAppearance(uint16_t appearance);

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
extern bool ProteusIII_SetSppBaseUuid(uint8_t* uuidP);

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
extern bool ProteusIII_SetSppServiceUuid(uint8_t* uuidP);

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
extern bool ProteusIII_SetSppRxUuid(uint8_t* uuidP);

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
extern bool ProteusIII_SetSppTxUuid(uint8_t* uuidP);

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
extern bool ProteusIII_Get(ProteusIII_UserSettings_t userSetting, uint8_t* responseP, uint16_t* responseLengthP);

/**
 * @brief Request the 3 byte firmware version.
 *
 * @param[out] versionP: Pointer to the 3 byte firmware version, version is returned LSB first
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetFWVersion(uint8_t* versionP);

/**
 * @brief Request device info.
 *
 * @param[out] deviceInfoP: Pointer to the device info structure
 *
 * @return True if request succeeded
 *         false otherwise
 */
extern bool ProteusIII_GetDeviceInfo(ProteusIII_DeviceInfo_t* deviceInfoP);

/**
 * @brief Request the 3 byte serial number.
 *
 * @param[out] serialNumberP: Pointer to the 3 byte serial number (LSB first)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetSerialNumber(uint8_t* serialNumberP);

/**
 * @brief Request the current BLE device name.
 *
 * @param[out] deviceNameP: Pointer to device name
 * @param[out] nameLengthP: Pointer to the length of the device name
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetDeviceName(uint8_t* deviceNameP, uint16_t* nameLengthP);

/**
 * @brief Request the 8 digit MAC.
 *
 * @param[out] macP: Pointer to the MAC
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetMAC(uint8_t* macP);

/**
 * @brief Request the 6 digit Bluetooth MAC.
 *
 * @param[out] btMacP: Pointer to the Bluetooth MAC
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetBTMAC(uint8_t* btMacP);

/**
 * @brief Request the advertising timeout
 *
 * @param[out] advTimeoutP: Pointer to the advertising timeout
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetAdvertisingTimeout(uint16_t* advTimeoutP);

/**
 * @brief Request the advertising flags.
 *
 * @param[out] advFlagsP: Pointer to the advertising flags
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetAdvertisingFlags(ProteusIII_AdvertisingFlags_t* advFlagsP);

/**
 * @brief Request the scan flags (see ProteusIII_ScanFlags_t).
 *
 * @param[out] scanFlagsP: Pointer to the scan flags (see ProteusIII_ScanFlags_t)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetScanFlags(uint8_t* scanFlagsP);

/**
 * @brief Request the beacon flags.
 *
 * @param[out] beaconFlagsP: Pointer to the beacon flags
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetBeaconFlags(ProteusIII_BeaconFlags_t* beaconFlagsP);

/**
 * @brief Request the CFG flags (see ProteusIII_CfgFlags_t)
 *
 * @param[out] cfgFlagsP: Pointer to the CFG flags (see ProteusIII_CfgFlags_t)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetCFGFlags(uint16_t* cfgFlagsP);

/**
 * @brief Request the connection timing
 *
 * @param[out] connectionTimingP: Pointer to the connection timing
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetConnectionTiming(ProteusIII_ConnectionTiming_t* connectionTimingP);

/**
 * @brief Request the scan timing
 *
 * @param[out] scanTimingP: Pointer to the scan timing
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetScanTiming(ProteusIII_ScanTiming_t* scanTimingP);

/**
 * @brief Request the scan factor
 *
 * @param[out] scanFactorP: Pointer to the scan factor
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetScanFactor(uint8_t* scanFactorP);

/**
 * @brief Request the TX power
 *
 * @param[out] txPowerP: Pointer to the TX power
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetTXPower(ProteusIII_TXPower_t* txPowerP);

/**
 * @brief Request the security flags
 *
 * @param[out] secFlagsP: Pointer to the security flags
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetSecFlags(ProteusIII_SecFlags_t* secFlagsP);

/**
 * @brief Request the security flags for peripheral only mode
 *
 * @param[out] secFlagsP: Pointer to the security flags
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetSecFlagsPeripheralOnly(ProteusIII_SecFlags_t* secFlagsP);

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
extern bool ProteusIII_GetBaudrateIndex(ProteusIII_BaudRate_t* baudrateP, ProteusIII_UartParity_t* parityP, bool* flowControlEnableP);

/**
 * @brief Request the BLE static passkey
 *
 * @param[out] staticPasskeyP: Pointer to the static passkey (6 digits)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetStaticPasskey(uint8_t* staticPasskeyP);

/**
 * @brief Request the Bluetooth appearance of the device
 *
 * @param[out] appearanceP: Pointer to the Bluetooth appearance
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetAppearance(uint16_t* appearanceP);

/**
 * @brief Request the base UUID of the SPP-like profile.
 *
 * @param[out] uuidP: Pointer to the 16 byte UUID (LSB first)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetSppBaseUuid(uint8_t* uuidP);

/**
 * @brief Request the service UUID of the SPP-like profile.
 *
 * @param[out] uuidP: Pointer to the 2 byte UUID (LSB first)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetSppServiceUuid(uint8_t* uuidP);

/**
 * @brief Request the RX UUID of the SPP-like profile.
 *
 * @param[out] uuidP: Pointer to the 2 byte UUID (LSB first)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetSppRxUuid(uint8_t* uuidP);

/**
 * @brief Request the TX UUID of the SPP-like profile.
 *
 * @param[out] uuidP: Pointer to the 2 byte UUID (LSB first)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_GetSppTxUuid(uint8_t* uuidP);

/**
 * @brief Enable the direct test mode (DTM)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_DTMEnable();

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
extern bool ProteusIII_DTMRun(ProteusIII_DTMCommand_t command, uint8_t channel_vendoroption, uint8_t length_vendorcmd, uint8_t payload);

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
extern bool ProteusIII_DTMStartTX(uint8_t channel, uint8_t length, ProteusIII_DTMTXPattern_t pattern);

/**
 * @brief Start the direct test mode (DTM) TX carrier test
 *
 * @param[in] channel: Channel
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_DTMStartTXCarrier(uint8_t channel);

/**
 * @brief Stop the current direct test mode (DTM) test
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_DTMStop();

/**
 * @brief Set the phy for direct test mode (DTM) test
 *
 * @param[in] phy: Phy
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_DTMSetPhy(ProteusIII_Phy_t phy);

/**
 * @brief Set the TX power for direct test mode (DTM) test
 *
 * @param[in] power: Tx power
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIII_DTMSetTXPower(ProteusIII_TXPower_t power);

#endif // PROTEUSIII_H_INCLUDED

#ifdef __cplusplus
}
#endif
