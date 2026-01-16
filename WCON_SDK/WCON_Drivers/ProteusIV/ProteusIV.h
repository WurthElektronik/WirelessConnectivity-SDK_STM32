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
 * @brief Proteus-IV driver header file.
 */

#include <global/global_types.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef PROTEUSIV_H_INCLUDED
#define PROTEUSIV_H_INCLUDED

/** Max. radio payload length for transmission. */
#define PROTEUSIV_MAX_RADIO_PAYLOAD_LENGTH ((uint16_t)244)

/** Max. command payload length for transmission. */
#define PROTEUSIV_MAX_CMD_PAYLOAD_LENGTH (uint16_t)(PROTEUSIV_MAX_RADIO_PAYLOAD_LENGTH + 7)

/** Time in ms required for booting: 75 ms + baudrate depending startup-message */
#define PROTEUSIV_BOOT_DURATION (uint16_t)(75 + 75)

/** Link is considered to be connected if status pin (LED_1) is on for this duration */
#define PROTEUSIV_STATUS_LED_CONNECTED_TIMEOUT_MS ((uint16_t)5)

/** Default UART baudrate of Proteus-IV module */
#define PROTEUSIV_DEFAULT_BAUDRATE ((uint32_t)115200)

/** UART baudrate of Proteus-IV module in config mode */
#define PROTEUSIV_CONFIGMODE_BAUDRATE ((uint32_t)9600)

/** Length of the Bluetooth MAC address */
#define PROTEUSIV_BTMAC_LENGTH (6)

/** Length of the pass key */
#define PROTEUSIV_PASSKEY_LENGTH (6)

/** Length of the UUID */
#define PROTEUSIV_UUID_LENGTH (16)

/** Length of the advertising and scan response data */
#define PROTEUSIV_ADVDATA_LENGTH (31)

/** Length of the device name */
#define PROTEUSIV_DEVICENAME_LENGTH (28)

/**
 * Max. number of devices that is used by the internal
 * buffer of the driver for scan information, this doesn't
 * affect the number of scan callback triggers that are generated
 * */
#define PROTEUSIV_MAX_NUMBER_OF_SCANDEVICES ((uint8_t)32)

/** Max. number of devices that may be queried using ProteusIV_GetConnectionInfo() */
#define PROTEUSIV_MAX_NUMBER_OF_CONNECTEDDEVICES ((uint8_t)10)

/** Note that this is the max. number of devices that may be queried
 * using ProteusIV_GetBonds() */
#define PROTEUSIV_MAX_BOND_DEVICES ((uint8_t)16)

/** Conn_ID indicating broadcast transmission */
#define PROTEUSIV_BROADCAST_CONN_ID ((uint8_t)0xFE)

/** Conn_ID indicating invalid connection */
#define PROTEUSIV_MULTICONN_INVALID_ID ((uint8_t)0xFF)

/**@brief Macro for converting milliseconds to 1.25 unit.
 *
 * @param[in] TIME: Number of milliseconds to convert.
 */
#define MSEC_TO_1250UNITS(TIME) (((TIME) * 1000) / (1250))

/**@brief Macro for converting milliseconds to 0.625 unit.
 *
 * @param[in] TIME: Number of milliseconds to convert.
 */
#define MSEC_TO_625UNITS(TIME) (((TIME) * 1000) / (625))

/**
 * @brief Operation mode definition
 */
typedef enum ProteusIV_OperationMode_t
{
    ProteusIV_OperationMode_CommandMode = (1 << 0),
    ProteusIV_OperationMode_TransparentMode = (1 << 1),
    ProteusIV_OperationMode_ConfigMode = (1 << 2),
    ProteusIV_OperationMode_BootMode = (1 << 3),
} ProteusIV_OperationMode_t;

/**
 * @brief Definition of the Bluetooth MAC address type
 */
typedef enum ProteusIV_Bluetooth_Address_Type_t
{
    ProteusIV_Bluetooth_Address_Type_Public = 0x00,
    ProteusIV_Bluetooth_Address_Type_Static_Random = 0x01,
} ProteusIV_Bluetooth_Address_Type_t;

/**
 * @brief Definition of the radio modules pins
 */
typedef struct ProteusIV_Pins_t
{
    WE_Pin_t ProteusIV_Pin_Reset;
    WE_Pin_t ProteusIV_Pin_Mode0;
    WE_Pin_t ProteusIV_Pin_Mode1;
    WE_Pin_t ProteusIV_Pin_Led0;
    WE_Pin_t ProteusIV_Pin_Led1;
    WE_Pin_t ProteusIV_UART_Enable;
} ProteusIV_Pins_t;

/**
 * @brief Scan info object, contains the needed information of a single scanned devices
 */
typedef struct ProteusIV_ScanDevice_t
{
    bool inUse;
    ProteusIV_Bluetooth_Address_Type_t addr_type;
    uint8_t btMac[PROTEUSIV_BTMAC_LENGTH];
    int8_t rssi;
    char deviceName[PROTEUSIV_DEVICENAME_LENGTH + 1];
} ProteusIV_ScanDevice_t;

/**
 * @brief Scan result, contains the needed information of all scanned devices
 */
typedef struct ProteusIV_ScanResult_t
{
    uint8_t numberOfDevices;
    ProteusIV_ScanDevice_t devices[PROTEUSIV_MAX_NUMBER_OF_SCANDEVICES];
} ProteusIV_ScanResult_t;

/**
 * @brief Definition of the radio physical layers
 */
typedef enum ProteusIV_Phy_t
{
    ProteusIV_Phy_1MBit = (1 << 0),
    ProteusIV_Phy_2MBit = (1 << 1),
    ProteusIV_Phy_125kBit_LECoded = (1 << 2),
} ProteusIV_Phy_t;

/**
 * @brief Connection info object, contains the needed information of a single connected devices
 */
typedef struct ProteusIV_ConnectedDevice_t
{
    uint8_t conn_ID;
    ProteusIV_Bluetooth_Address_Type_t addr_type;
    uint8_t btMac[PROTEUSIV_BTMAC_LENGTH];
    bool is_linkopen;
    uint16_t max_payload;
    uint16_t connection_interval;
    ProteusIV_Phy_t phy_rx;
    ProteusIV_Phy_t phy_tx;
} ProteusIV_ConnectedDevice_t;

/**
 * @brief Connection result, contains the needed information of all connected devices stored in the radio module
 */
typedef struct ProteusIV_ConnectedDevices_t
{
    uint8_t numberOfDevices;
    ProteusIV_ConnectedDevice_t devices[PROTEUSIV_MAX_NUMBER_OF_CONNECTEDDEVICES];
} ProteusIV_ConnectedDevices_t;

/**
 * @brief Connection info structure
 */
typedef struct ProteusIV_ConnectionList_t
{
    bool is_connected;    /** Specify if connection is available */
    bool is_linkopen;     /** Specify if connection has an open Bluetooth LE link */
    uint16_t max_payload; /** Maximum payload of the connection */
} ProteusIV_ConnectionList_t;

/**
 * @brief Connection info object, contains the needed information of all connections stored in the driver
 */
extern ProteusIV_ConnectionList_t ProteusIV_connection_list[PROTEUSIV_MAX_NUMBER_OF_CONNECTEDDEVICES];

/**
 * @brief Definition of the actions for pass key exchange
 */
typedef enum ProteusIV_DisplayPasskeyAction_t
{
    ProteusIV_DisplayPasskeyAction_NoAction = (uint8_t)0x00,
    ProteusIV_DisplayPasskeyAction_PleaseConfirm = (uint8_t)0x01
} ProteusIV_DisplayPasskeyAction_t;

/**
 * @brief Definition of the disconnect reasons
 */
typedef enum ProteusIV_DisconnectReason_t
{
    ProteusIV_DisconnectReason_Unknown,
    ProteusIV_DisconnectReason_ConnectionTimeout,
    ProteusIV_DisconnectReason_UserTerminatedConnection,
    ProteusIV_DisconnectReason_HostTerminatedConnection,
    ProteusIV_DisconnectReason_ConnectionIntervalUnacceptable,
    ProteusIV_DisconnectReason_MicFailure,
    ProteusIV_DisconnectReason_ConnectionSetupFailed
} ProteusIV_DisconnectReason_t;

/**
 * @brief Definition of the internal module mode
 */
typedef enum ProteusIV_Internal_Mode_t
{
    ProteusIV_Internal_Mode_Error = (uint8_t)0x00,
    ProteusIV_Internal_Mode_Normal = (uint8_t)0x01,
    ProteusIV_Internal_Mode_Config = (uint8_t)0x02,
    ProteusIV_Internal_Mode_Boot = (uint8_t)0x03,
    ProteusIV_Internal_Mode_Off = (uint8_t)0x04
} ProteusIV_Internal_Mode_t;

/**
 * @brief Definition of the user settings (settings stored in non-volatile RRAM)
 */
typedef enum ProteusIV_UserSettings_t
{
    ProteusIV_USERSETTING_FS_FWVERSION = (uint8_t)0x01,
    ProteusIV_USERSETTING_RF_DEVICENAME = (uint8_t)0x02,
    ProteusIV_USERSETTING_FS_MAC = (uint8_t)0x03,
    ProteusIV_USERSETTING_FS_BTMAC = (uint8_t)0x04,
    ProteusIV_USERSETTING_CFG_FLAGS = (uint8_t)0x05,
    ProteusIV_USERSETTING_UART_TRANSP_ETX_CFG = (uint8_t)0x06,
    ProteusIV_USERSETTING_RF_ADVERTISINGINTERVAL = (uint8_t)0x07,
    ProteusIV_USERSETTING_RF_CONNECTIONINTERVAL = (uint8_t)0x08,
    ProteusIV_USERSETTING_UART_RECEIVE_TRANSP_ETX = (uint8_t)0x09,
    ProteusIV_USERSETTING_UART_TRANSMIT_TRANSP_ETX = (uint8_t)0x0A,
    ProteusIV_USERSETTING_UART_CONFIGINDEX = (uint8_t)0x0B,
    ProteusIV_USERSETTING_RF_SECFLAGS = (uint8_t)0x0C,
    ProteusIV_USERSETTING_RF_ADVERTISINGDATA = (uint8_t)0x0D,
    ProteusIV_USERSETTING_RF_SCANRESPONSEDATA = (uint8_t)0x0E,
    ProteusIV_USERSETTING_FS_DEVICEINFO = (uint8_t)0x0F,
    ProteusIV_USERSETTING_FS_SERIALNUMBER = (uint8_t)0x10,
    ProteusIV_USERSETTING_RF_TXPOWER = (uint8_t)0x11,
    ProteusIV_USERSETTING_RF_STATICPASSKEY = (uint8_t)0x12,
    ProteusIV_USERSETTING_RF_APPEARANCE = (uint8_t)0x19,
    ProteusIV_USERSETTING_RF_MAXPERIPHERALCONNECTIONS = (uint8_t)0x1E,
    ProteusIV_USERSETTING_RF_SPPSERVICEUUID = (uint8_t)0x20,
    ProteusIV_USERSETTING_RF_SPPRXUUID = (uint8_t)0x21,
    ProteusIV_USERSETTING_RF_SPPTXUUID = (uint8_t)0x22,
} ProteusIV_UserSettings_t;

/**
 * @brief Definition of the runtime settings (settings stored in volatile RAM)
 */
typedef enum ProteusIV_RuntimeSettings_t
{
    ProteusIV_RUNTIMESETTING_RF_ADVERTISINGDATA = (uint8_t)0x0D,
    ProteusIV_RUNTIMESETTING_RF_SCANRESPONSEDATA = (uint8_t)0x0E,
} ProteusIV_RuntimeSettings_t;

/**
 * @brief Definition of the Cfg flags user setting
 */
typedef enum ProteusIV_CfgFlags_t
{
    ProteusIV_CfgFlags_AutoStartAdvertising = (1 << 0),
    ProteusIV_CfgFlags_RecordRSSI = (1 << 1),
    ProteusIV_CfgFlags_AutoStartFOTA = (1 << 2),
    ProteusIV_CfgFlags_DoubleScanDeviceDetection = (1 << 3),
    ProteusIV_CfgFlags_NotConnectableAdvertising = (1 << 4),
    ProteusIV_CfgFlags_EnableTxCompleteMessage = (1 << 5),
} ProteusIV_CfgFlags_t;

/**
 * @brief Definition of the security flags user setting
 */
typedef enum ProteusIV_SecFlags_t
{
    ProteusIV_SecFlags_NO_IO_MINPERIPHERAL_LEVEL_L2 = (uint8_t)0x01,
    ProteusIV_SecFlags_IO_KEYBOARD_ONLY_MINPERIPHERAL_LEVEL_L2 = (uint8_t)0x02,
    ProteusIV_SecFlags_IO_KEYBOARD_ONLY_MINPERIPHERAL_LEVEL_L4 = (uint8_t)0x03,
    ProteusIV_SecFlags_IO_STATIC_DISPLAY_ONLY_MINPERIPHERAL_LEVEL_L2 = (uint8_t)0x04,
    ProteusIV_SecFlags_IO_STATIC_DISPLAY_ONLY_MINPERIPHERAL_LEVEL_L4 = (uint8_t)0x05,
    ProteusIV_SecFlags_IO_DISPLAY_ONLY_MINPERIPHERAL_LEVEL_L2 = (uint8_t)0x06,
    ProteusIV_SecFlags_IO_DISPLAY_ONLY_MINPERIPHERAL_LEVEL_L4 = (uint8_t)0x07,
    ProteusIV_SecFlags_IO_KEYBOARD_DISPLAY_MINPERIPHERAL_LEVEL_L2 = (uint8_t)0x08,
    ProteusIV_SecFlags_IO_KEYBOARD_DISPLAY_MINPERIPHERAL_LEVEL_L4 = (uint8_t)0x09,
} ProteusIV_SecFlags_t;

/**
 * @brief Definition of the TX power user setting
 */
typedef enum ProteusIV_TXPower_t
{
    ProteusIV_TXPower_8 = (int8_t)8,
    ProteusIV_TXPower_7 = (int8_t)7,
    ProteusIV_TXPower_6 = (int8_t)6,
    ProteusIV_TXPower_5 = (int8_t)5,
    ProteusIV_TXPower_4 = (int8_t)4,
    ProteusIV_TXPower_3 = (int8_t)3,
    ProteusIV_TXPower_2 = (int8_t)2,
    ProteusIV_TXPower_0 = (int8_t)0,
    ProteusIV_TXPower_minus4 = (int8_t)-4,
    ProteusIV_TXPower_minus8 = (int8_t)-8,
    ProteusIV_TXPower_minus12 = (int8_t)-12,
    ProteusIV_TXPower_minus16 = (int8_t)-16,
    ProteusIV_TXPower_minus20 = (int8_t)-20,
    ProteusIV_TXPower_minus40 = (int8_t)-40
} ProteusIV_TXPower_t;

/**
 * @brief Definition of the UART baud rate user setting
 */
typedef enum ProteusIV_BaudRate_t
{
    ProteusIV_BaudRateIndex_1200 = (uint8_t)0,
    ProteusIV_BaudRateIndex_2400 = (uint8_t)2,
    ProteusIV_BaudRateIndex_4800 = (uint8_t)4,
    ProteusIV_BaudRateIndex_9600 = (uint8_t)6,
    ProteusIV_BaudRateIndex_14400 = (uint8_t)8,
    ProteusIV_BaudRateIndex_19200 = (uint8_t)10,
    ProteusIV_BaudRateIndex_28800 = (uint8_t)12,
    ProteusIV_BaudRateIndex_38400 = (uint8_t)14,
    ProteusIV_BaudRateIndex_56000 = (uint8_t)16,
    ProteusIV_BaudRateIndex_57600 = (uint8_t)18,
    ProteusIV_BaudRateIndex_76800 = (uint8_t)20,
    ProteusIV_BaudRateIndex_115200 = (uint8_t)22,
    ProteusIV_BaudRateIndex_230400 = (uint8_t)24,
    ProteusIV_BaudRateIndex_250000 = (uint8_t)26,
    ProteusIV_BaudRateIndex_460800 = (uint8_t)28,
    ProteusIV_BaudRateIndex_921600 = (uint8_t)30,
    ProteusIV_BaudRateIndex_1000000 = (uint8_t)32
} ProteusIV_BaudRate_t;

/**
 * @brief Definition of the UART parity user setting
 */
typedef enum ProteusIV_UartParity_t
{
    ProteusIV_UartParity_Even,
    ProteusIV_UartParity_None
} ProteusIV_UartParity_t;

/**
 * @brief Device info structure
 * @see ProteusIV_GetDeviceInfo()
 */
typedef struct ProteusIV_DeviceInfo_t
{
    uint32_t osVersion;
    uint32_t buildCode;
    uint16_t packageVariant;
    uint32_t chipId;
} ProteusIV_DeviceInfo_t;

/**
 * @brief Security information
 */
typedef struct ProteusIV_SecurityState_t
{
    uint8_t success;
    uint8_t level;
} ProteusIV_SecurityState_t;

/**
 * @brief Module connection state structure
 */
typedef union ProteusIV_ConnectionState_t
{
    uint8_t raw;
    struct
    {
        uint8_t advertising : 1;
        uint8_t scanning : 1;
        uint8_t connected : 1;
        uint8_t peripheral_link_open : 1;
        uint8_t central_link_open : 1;
        uint8_t reserved : 3;
    } flags;
} ProteusIV_ConnectionState_t;

/**
 * @brief Module state structure
 * @see ProteusIV_GetState()
 */
typedef struct ProteusIV_ModuleState_t
{
    ProteusIV_Internal_Mode_t internal_mode;
    ProteusIV_ConnectionState_t connection_state;
} ProteusIV_ModuleState_t;

/**
 * @brief Entry in ProteusIV_BondDatabase_t
 * @see ProteusIV_GetBonds()
 */
typedef struct ProteusIV_BondDatabaseEntry_t
{
    ProteusIV_Bluetooth_Address_Type_t addr_type;
    uint8_t btMac[PROTEUSIV_BTMAC_LENGTH];
} ProteusIV_BondDatabaseEntry_t;

/**
 * @brief List of bonded devices
 * @see ProteusIV_GetBonds()
 */
typedef struct ProteusIV_BondDatabase_t
{
    uint8_t numberOfDevices;
    ProteusIV_BondDatabaseEntry_t devices[PROTEUSIV_MAX_BOND_DEVICES];
} ProteusIV_BondDatabase_t;

/**
 * @brief This callback is called, when a CMD_DATA_IND message is sent from the module to the host.
 *
 * @param[in] conn_ID: ID of the connection
 * @param[in] payloadP: Pointer to the received payload data
 * @param[in] payloadLength: Length of the payload data
 * @param[in] rssi: Rssi of the received radio packet
 *
 * @return None
 */
typedef void (*ProteusIV_RxCallback_t)(uint8_t conn_ID, uint8_t* payloadP, uint16_t payloadLength, int8_t rssi);

/**
 * @brief This callback is called, when a CMD_CONNECT_IND message is sent from the module to the host.
 *
 * @param[in] conn_ID: ID of the connection
 * @param[in] success: True if connection has been setup successfully, false otherwise
 * @param[in] type: Type of the Bluetooth MAC address
 * @param[in] btMacP: Bluetooth MAC address
 *
 * @return None
 */
typedef void (*ProteusIV_ConnectCallback_t)(uint8_t conn_ID, bool success, ProteusIV_Bluetooth_Address_Type_t type, uint8_t* btMacP);

/**
 * @brief This callback is called, when a CMD_MAXPAYLOAD_IND message is sent from the module to the host.
 *
 * @param[in] conn_ID: ID of the connection
 * @param[in] max_payload: Maximum payload size of the Bluetooth link
 *
 * @return None
 */
typedef void (*ProteusIV_MaxPayloadCallback_t)(uint8_t conn_ID, uint16_t max_payload);

/**
 * @brief This callback is called, when a CMD_SECURITY_IND message is sent from the module to the host.
 *
 * @param[in] conn_ID: ID of the connection
 * @param[in] securityState: State of the connection's security level
 *
 * @return None
 */
typedef void (*ProteusIV_SecurityCallback_t)(uint8_t conn_ID, ProteusIV_SecurityState_t securityState);

/**
 * @brief This callback is called, when a CMD_PASSKEY_IND message is sent from the module to the host.
 *
 * @param[in] conn_ID: ID of the connection
 *
 * @return None
 */
typedef void (*ProteusIV_PasskeyCallback_t)(uint8_t conn_ID);

/**
 * @brief This callback is called, when a CMD_DISPLAY_PASSKEY_IND message is sent from the module to the host.
 *
 * @param[in] conn_ID: ID of the connection
 * @param[in] action: Determines, whether an action needs to be done
 * @param[in] passkeyP: Pointer to the pass key
 *
 * @return None
 */
typedef void (*ProteusIV_DisplayPasskeyCallback_t)(uint8_t conn_ID, ProteusIV_DisplayPasskeyAction_t action, uint8_t* passkeyP);

/**
 * @brief This callback is called, when a CMD_DISCONNECT_IND message is sent from the module to the host.
 *
 * @param[in] conn_ID: ID of the connection
 * @param[in] reason: Reason for the disconnection
 *
 * @return None
 */
typedef void (*ProteusIV_DisconnectCallback_t)(uint8_t conn_ID, ProteusIV_DisconnectReason_t reason);

/**
 * @brief This callback is called, when a CMD_LINKOPEN_RSP message is sent from the module to the host.
 *
 * @param[in] conn_ID: ID of the connection
 * @param[in] link_open: True if link has been opened, false if link has been closed
 *
 * @return None
 */
typedef void (*ProteusIV_LinkOpenCallback_t)(uint8_t conn_ID, bool link_open);

/**
 * @brief This callback is called, when a CMD_PHYUPDATE_IND message is sent from the module to the host.
 *
 * @param[in] conn_ID: ID of the connection
 * @param[in] phyTx: Bitmap of the new TX phy
 * @param[in] phyRx: Bitmap of the new RX phy
 *
 * @return None
 */
typedef void (*ProteusIV_PhyUpdateCallback_t)(uint8_t conn_ID, uint8_t phyTx, uint8_t phyRx);

/**
 * @brief This callback is called, when a CMD_SCAN_IND message is sent from the module to the host.
 *
 * @param[in] type: Type of the Bluetooth MAC address
 * @param[in] btMacP: Pointer to the Bluetooth MAC address
 * @param[in] rssi: Rssi of the scanned device
 * @param[in] devicenameP: Pointer to the name of the scanned device
 *
 * @return None
 */
typedef void (*ProteusIV_ScanCallback_t)(ProteusIV_Bluetooth_Address_Type_t type, uint8_t* btMacP, int8_t rssi, char* devicenameP);

/**
 * @brief Callback configuration structure. Used as argument for ProteusIV_Init().
 *
 * Caution: Please note that code in callbacks should be kept simple, as the callback
 * functions are called from ISRs.
 * For this reason, it is also not possible
 * to send requests to the Proteus-IV directly from inside a callback.
 */
typedef struct ProteusIV_CallbackConfig_t
{
    ProteusIV_RxCallback_t rxCb;                         /**< Callback for CMD_DATA_IND */
    ProteusIV_ConnectCallback_t connectCb;               /**< Callback for CMD_CONNECT_IND */
    ProteusIV_MaxPayloadCallback_t maxPayloadCb;         /**< Callback for CMD_MAXPAYLOAD_IND */
    ProteusIV_SecurityCallback_t securityCb;             /**< Callback for CMD_SECURITY_IND */
    ProteusIV_PasskeyCallback_t passkeyCb;               /**< Callback for CMD_PASSKEY_IND */
    ProteusIV_DisplayPasskeyCallback_t displayPasskeyCb; /**< Callback for CMD_DISPLAY_PASSKEY_IND */
    ProteusIV_DisconnectCallback_t disconnectCb;         /**< Callback for CMD_DISCONNECT_IND */
    ProteusIV_LinkOpenCallback_t linkOpenCb;             /**< Callback for CMD_LINKOPEN_RSP */
    ProteusIV_PhyUpdateCallback_t phyUpdateCb;           /**< Callback for CMD_PHYUPDATE_IND */
    ProteusIV_ScanCallback_t scanCb;                     /**< Callback for CMD_SCAN_IND */
    WE_UART_HandleRxByte_t transparentRxCb;              /**< Callback for transparent data */
} ProteusIV_CallbackConfig_t;

/**
 * @brief Initialize the Proteus-IV for serial interface.
 *
 * Caution: The parameter baudrate must match the configured UserSettings of the Proteus-IV.
 *
 * @param[in] uartP: Pointer to the definition of the uart connected to the module
 * @param[in] pinoutP: Pointer to the definition of the gpios connected to the module
 * @param[in] opMode: Operation mode
 * @param[in] callbackConfig: Callback configuration
 *
 * @return True if initialization succeeded,
 *         false otherwise
 */
extern bool ProteusIV_Init(WE_UART_t* uartP, ProteusIV_Pins_t* pinoutP, ProteusIV_OperationMode_t opMode, ProteusIV_CallbackConfig_t callbackConfig);

/**
 * @brief Deinitialize the Proteus-IV interface.
 *
 * @return True if deinitialization succeeded,
 *         false otherwise
 */
extern bool ProteusIV_Deinit(void);

/**
 * @brief Request the module state.
 *
 * @param[out] moduleStateP: Pointer to module state
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetState(ProteusIV_ModuleState_t* moduleStateP);

/**
 * @brief Returns the current level of the status pin (LED_1).
 * Is used as indication for link open.
 *
 * @param[out] Led1PinLevelP: Pointer to the pin level state
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetLed1PinLevel(WE_Pin_Level_t* Led1PinLevelP);

/**
 * @brief Reset the Proteus-IV via reset pin.
 *
 * @return True if reset succeeded,
 *         false otherwise
 */
extern bool ProteusIV_PinReset(void);

/**
 * @brief Reset the Proteus-IV by reset command.
 *
 * @return True if reset succeeded,
 *         false otherwise
 */
extern bool ProteusIV_Reset(void);

/**
 * @brief Set the Proteus-IV into sleep mode.
 *
 * @return True if succeeded,
 *         false otherwise
 */
extern bool ProteusIV_Sleep(void);

/**
 * @brief Disable the UART by command.
 *
 * @return True if succeeded,
 *         false otherwise
 */
extern bool ProteusIV_UartDisable(void);

/**
 * @brief Re-enable the UART by pin, in case it is disabled by CMD_UARTDISABLE_REQ command.
 *
 * @return True if re-enabling succeeded,
 *         false otherwise
 */
extern bool ProteusIV_UartPinEnable();

/**
 * @brief Connect to the Bluetooth LE device with the corresponding Bluetooth MAC address.
 *
 * @param[in] type: Type of the Bluetooth MAC address
 * @param[in] btMacP: Pointer to Bluetooth MAC address
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_Connect(ProteusIV_Bluetooth_Address_Type_t type, uint8_t* btMacP);

/**
 * @brief Disconnect the Proteus-IV connection if open.
 *
 * @param[in] conn_ID: ID of the connection
 *
 * @return True if disconnect succeeded,
 *         false otherwise
 */
extern bool ProteusIV_Disconnect(uint8_t conn_ID);

/**
 * @brief Request the scan results.
 *
 * @param[in] conn_ID: ID of the connection
 * @param[out] devicesP: Pointer to scan result
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetConnectionInfo(uint8_t conn_ID, ProteusIV_ConnectedDevices_t* devicesP);

/**
 * @brief Start advertising.
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_AdvertisingStart(void);

/**
 * @brief Stop a advertising.
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_AdvertisingStop(void);

/**
 * @brief Start scan.
 *
 * @param[out] scanresultP: Pointer to the buffer to store the scan result
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_ScanStart(ProteusIV_ScanResult_t* scanresultP);

/**
 * @brief Stop a scan.
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_ScanStop(void);

/**
 * @brief Transmit data if a connection is open.
 *
 * @details: For broadcast transmission use PROTEUSIV_BROADCAST_CONN_ID .
 *
 * @param[in] conn_ID: ID of the connection
 * @param[in] payloadP: Pointer to the data to transmit
 * @param[in] length: Length of the data to transmit
 *
 * @return True if succeeded,
 *         false otherwise
 */
extern bool ProteusIV_Transmit(uint8_t conn_ID, uint8_t* payloadP, uint16_t length);

/**
 * @brief Transmitting the data via UART in transparent mode.
 *
 * @details: In transparent mode the ETX characters must be added to trigger RF transmission.
 *
 * @param[in] payloadP: Pointer to the data to transmit
 * @param[in] length: Length of the data to transmit
 * @param[in] ETX: ETX characters
 *
 * @return True if transmission succeeded,
 *         false otherwise
 */
extern bool ProteusIV_Transparent_Transmit(const uint8_t* payloadP, uint16_t length, uint16_t ETX);

/**
 * @brief Answer on a passkey request with a passkey to setup a connection.
 *
 * @param[in] conn_ID: ID of the connection
 * @param[in] passkeyP: Pointer to passkey
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_Passkey(uint8_t conn_ID, uint8_t* passkeyP);

/**
 * @brief Answer on a numeric comparison request.
 *
 * @param[in] conn_ID: ID of the connection
 * @param[in] keyIsOk: Boolean to confirm if the key shown is correct
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_NumericCompareConfirm(uint8_t conn_ID, bool keyIsOk);

/**
 * @brief Update the phy during an open connection.
 *
 * @param[in] conn_ID: ID of the connection
 * @param[in] phy: New phy
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_PhyUpdate(uint8_t conn_ID, ProteusIV_Phy_t phy);

/**
 * @brief Requests the Bluetooth MAC addresses of all bonded devices.
 *
 * Caution: Note that this function supports a maximum of PROTEUSIV_MAX_BOND_DEVICES
 * returned devices. The Proteus-IV module itself might be capable of
 * bonding more devices.
 *
 * @param[out] bondDatabaseP: Pointer to bond database
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetBonds(ProteusIV_BondDatabase_t* bondDatabaseP);

/**
 * @brief Removes all bonding data.
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_DeleteBonds();

/**
 * @brief Removes the bonding information for a single device.
 *
 * @param[in] type: Type of the Bluetooth MAC address
 * @param[in] btMacP: Pointer to the Bluetooth MAC (of 6 bytes)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_DeleteBond(ProteusIV_Bluetooth_Address_Type_t type, uint8_t* btMacP);

/*
 * @brief Factory reset of the module.
 *
 * @return True if succeeded,
 *         false otherwise
 */
extern bool ProteusIV_FactoryReset();

/**
 * @brief Set a special user setting.
 *
 * @param[in] userSetting: User setting to be updated
 * @param[in] valueP: Pointer to the new settings value
 * @param[in] length: Length of the value
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_Set(ProteusIV_UserSettings_t userSetting, uint8_t* valueP, uint8_t length);

/**
 * @brief Set the Bluetooth LE device name.
 *
 * @param[in] deviceNameP: Pointer to the device name
 * @param[in] nameLength: Length of the device name
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_SetDeviceName(uint8_t* deviceNameP, uint8_t nameLength);

/**
 * @brief Set the CFG flags.
 *
 * @param[in] cfgFlags: Bitmap of the CFG flags (see ProteusIV_CfgFlags_t)
 *
 * @return True if request succeeded
 *         false otherwise
 */
extern bool ProteusIV_SetCFGFlags(uint16_t cfgFlags);

/**
 * @brief Sets the Bluetooth MAC address.
 *
 * @param[in] btMacP: Pointer to the Bluetooth MAC address
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_SetBTMAC(uint8_t* btMacP);

/**
 * @brief Set the Bluetooth LE TX power.
 *
 * @param[in] txPower: TX power
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_SetTXPower(ProteusIV_TXPower_t txPower);

/**
 * @brief Set the advertising interval.
 *
 * @param[in] min: Minimum value (unit 0.625ms)
 * @param[in] max: Maximum value (unit 0.625ms)
 *
 * @return True if request succeeded
 *         false otherwise
 */
extern bool ProteusIV_SetAdvertisingInterval(uint16_t min, uint16_t max);

/**
 * @brief Set the connection interval.
 *
 * @param[in] min: Minimum value (unit 1.25ms)
 * @param[in] max: Maximum value (unit 1.25ms)
 *
 * @return True if request succeeded
 *         false otherwise
 */
extern bool ProteusIV_SetConnectionInterval(uint16_t min, uint16_t max);

/**
 * @brief Set the ETX configuration.
 *
 * @param[in] etxconfig: ETX configuration
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_SetETXConfig(uint8_t etxconfig);

/**
 * @brief Set the receive ETX.
 *
 * @param[in] etx: Receive ETX characters
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_SetReceiveETX(uint16_t etx);

/**
 * @brief Set the transmit ETX.
 *
 * @param[in] etx: Transmit ETX characters
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_SetTransmitETX(uint16_t etx);

/**
 * @brief Set the Bluetooth LE security flags.
 *
 * @param[in] secFlags: Security flags
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_SetSecFlags(ProteusIV_SecFlags_t secFlags);

/**
 * @brief Set the UART baudrate index.
 *
 * @param[in] baudrate: UART baud rate
 * @param[in] parity: Parity setting
 * @param[in] flowControlEnable: enable/disable flow control
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_SetBaudrateIndex(ProteusIV_BaudRate_t baudrate, ProteusIV_UartParity_t parity, bool flowControlEnable);

/**
 * @brief Set the Bluetooth LE static pass key.
 *
 * @param[in] staticPasskeyP: Pointer to the static passkey (6 digits)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_SetStaticPasskey(uint8_t* staticPasskeyP);

/**
 * @brief Sets the Bluetooth appearance of the device.
 *
 * @param[in] appearance: 2 byte Bluetooth appearance value (please check the Bluetooth Core
 *                        Specification: Core Specification Supplement, Part A, section 1.12
 *                        for permissible values)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_SetAppearance(uint16_t appearance);

/**
 * @brief Sets the service UUID of the SPP-like profile.
 *
 * @param[in] uuidP: Pointer to the 16 byte UUID (MSB first)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_SetSppServiceUuid(uint8_t* uuidP);

/**
 * @brief Sets the RX UUID of the SPP-like profile.
 *
 * @param[in] uuidP: Pointer to the 16 byte UUID (MSB first)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_SetSppRxUuid(uint8_t* uuidP);

/**
 * @brief Sets the TX UUID of the SPP-like profile.
 *
 * @param[in] uuidP: Pointer to the 16 byte UUID (MSB first)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_SetSppTxUuid(uint8_t* uuidP);

/**
 * @brief Set the maximum peripheral connection.
 *
 * @param[in] max: Maximum number of peripheral connections
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_SetMaxPeripheralConnections(uint8_t max);

/**
 * @brief Sets the advertising data.
 *
 * @param[in] dataP: Pointer to the advertising data
 * @param[in] length: Length of the advertising data
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_SetAdvertisingData(uint8_t* dataP, uint16_t length);

/**
 * @brief Sets the scan response data.
 *
 * @param[in] dataP: Pointer to the scan response data
 * @param[in] length: Length of the scan response data
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_SetScanResponseData(uint8_t* dataP, uint16_t length);

/**
 * @brief Request the current user settings.
 *
 * @param[in] userSetting: User setting to be requested
 * @param[out] responseP: Pointer of the memory to put the requested content
 * @param[in,out] responseLengthP: Length of the provided buffer, length of the returned content
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_Get(ProteusIV_UserSettings_t userSetting, uint8_t* responseP, uint16_t* responseLengthP);

/**
 * @brief Request the 3 byte firmware version.
 *
 * @param[out] versionP: Pointer to the 3 byte firmware version
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetFWVersion(uint8_t* versionP);

/**
 * @brief Request device info.
 *
 * @param[out] deviceInfoP: Pointer to the device info
 *
 * @return True if request succeeded
 *         false otherwise
 */
extern bool ProteusIV_GetDeviceInfo(ProteusIV_DeviceInfo_t* deviceInfoP);

/**
 * @brief Request the 3 byte serial number.
 *
 * @param[out] serialNumberP: Pointer to the 3 byte serial number (MSB first)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetSerialNumber(uint8_t* serialNumberP);

/**
 * @brief Request the current Bluetooth LE device name.
 *
 * @param[out] deviceNameP: Pointer to device name
 * @param[out] nameLengthP: Pointer to the length of the device name
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetDeviceName(uint8_t* deviceNameP, uint16_t* nameLengthP);

/**
 * @brief Request the 8 digit MAC.
 *
 * @param[out] macP: Pointer to the MAC
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetMAC(uint8_t* macP);

/**
 * @brief Request the 6 digit Bluetooth MAC.
 *
 * @param[out] btMacP: Pointer to the Bluetooth MAC
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetBTMAC(uint8_t* btMacP);

/**
 * @brief Request the CFG flags.
 *
 * @param[out] cfgFlagsP: Pointer to the bitmap of the CFG flags (see ProteusIV_CfgFlags_t)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetCFGFlags(uint16_t* cfgFlagsP);

/**
 * @brief Request the ETX configuration.
 *
 * @param[out] etxconfigP: Pointer to the ETX config
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetETXConfig(uint8_t* etxconfigP);

/**
 * @brief Request the receive ETX.
 *
 * @param[out] etxP: Pointer to the receive ETX characters
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetReceiveETX(uint16_t* etxP);

/**
 * @brief Request the transmit ETX.
 *
 * @param[out] etxP: Pointer to the transmit ETX characters
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetTransmitETX(uint16_t* etxP);

/**
 * @brief Request the TX power.
 *
 * @param[out] txPowerP: Pointer to the TX power
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetTXPower(ProteusIV_TXPower_t* txPowerP);

/**
 * @brief Request the advertising interval.
 *
 * @param[out] minP: Pointer to the minimum value (unit 0.625ms)
 * @param[out] maxP: Pointer to the maximum value (unit 0.625ms)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetAdvertisingInterval(uint16_t* minP, uint16_t* maxP);

/**
 * @brief Request the connection interval.
 *
 * @param[out] minP: Pointer to the minimum value (unit 1.25ms)
 * @param[out] maxP: Pointer to the maximum value (unit 1.25ms)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetConnectionInterval(uint16_t* minP, uint16_t* maxP);

/**
 * @brief Request the security flags.
 *
 * @param[out] secFlagsP: Pointer to the security flags
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetSecFlags(ProteusIV_SecFlags_t* secFlagsP);

/**
 * @brief Request the UART configuration index.
 *
 * @param[out] baudrateP: Pointer to the UART baud rate index
 * @param[out] parityP: Pointer to the UART parity
 * @param[out] flowControlEnableP: Pointer to the UART flow control parameter
 *
 * @return True if request succeeded
 *         false otherwise
 */
extern bool ProteusIV_GetBaudrateIndex(ProteusIV_BaudRate_t* baudrateP, ProteusIV_UartParity_t* parityP, bool* flowControlEnableP);

/**
 * @brief Request the Bluetooth LE static pass key.
 *
 * @param[out] staticPasskeyP: Pointer to the static passkey (6 digits)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetStaticPasskey(uint8_t* staticPasskeyP);

/**
 * @brief Request the Bluetooth appearance of the device.
 *
 * @param[out] appearanceP: Pointer to the Bluetooth appearance
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetAppearance(uint16_t* appearanceP);

/**
 * @brief Request the service UUID of the SPP-like profile.
 *
 * @param[out] uuidP: Pointer to the 16 byte UUID (MSB first)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetSppServiceUuid(uint8_t* uuidP);

/**
 * @brief Request the RX UUID of the SPP-like profile.
 *
 * @param[out] uuidP: Pointer to the 16 byte UUID (MSB first)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetSppRxUuid(uint8_t* uuidP);

/**
 * @brief Request the TX UUID of the SPP-like profile.
 *
 * @param[out] uuidP: Pointer to the 16 byte UUID (MSB first)
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetSppTxUuid(uint8_t* uuidP);

/**
 * @brief Request the maximum peripheral connections.
 *
 * @param[out] maxP: Pointer to the maximum value
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetMaxPeripheralConnections(uint8_t* maxP);

/**
 * @brief Request the advertising data.
 *
 * @param[out] dataP: Pointer to the advertising data
 * @param[out] lengthP: Pointer to the length of the advertising data
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetAdvertisingData(uint8_t* dataP, uint16_t* lengthP);

/**
 * @brief Request the scan response data.
 *
 * @param[out] dataP: Pointer to the scan response data
 * @param[out] lengthP: Pointer to the length of the scan response data
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetScanResponseData(uint8_t* dataP, uint16_t* lengthP);

/**
 * @brief Request the current runtime settings.
 *
 * @param[in] runtimeSetting: User setting to be requested
 * @param[out] responseP: Pointer of the memory to put the requested content
 * @param[in,out] responseLengthP: Length of the provided buffer, length of the returned content
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetRAM(ProteusIV_RuntimeSettings_t runtimeSetting, uint8_t* responseP, uint16_t* responseLengthP);

/**
 * @brief Request the advertising data from RAM.
 *
 * @param[out] dataP: Pointer to the advertising data
 * @param[out] lengthP: Pointer to the length of the advertising data
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetRAMAdvertisingData(uint8_t* dataP, uint16_t* lengthP);

/**
 * @brief Request the scan response data from RAM.
 *
 * @param[out] dataP: Pointer to the scan response data
 * @param[out] lengthP: Pointer to the length of the scan response data
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_GetRAMScanResponseData(uint8_t* dataP, uint16_t* lengthP);

/**
 * @brief Set a runtime setting.
 *
 * @param[in] runtimeSetting: Runtime setting to be updated
 * @param[in] valueP: Pointer to the new settings value
 * @param[in] length: Length of the value
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_SetRAM(ProteusIV_RuntimeSettings_t runtimeSetting, uint8_t* valueP, uint8_t length);

/**
 * @brief Sets the advertising data to RAM.
 *
 * @param[in] dataP: Pointer to the advertising data
 * @param[in] length: Length of the advertising data
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_SetRAMAdvertisingData(uint8_t* dataP, uint16_t length);

/**
 * @brief Sets the scan response data to RAM.
 *
 * @param[in] dataP: Pointer to the scan response data
 * @param[in] length: Length of the scan response data
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ProteusIV_SetRAMScanResponseData(uint8_t* dataP, uint16_t length);

#endif // PROTEUSIV_H_INCLUDED

#ifdef __cplusplus
}
#endif
