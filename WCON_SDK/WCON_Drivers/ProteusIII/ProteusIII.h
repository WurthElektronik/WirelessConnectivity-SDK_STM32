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
 * @brief ProteusIII driver header file.
 */

#include <stdbool.h>
#include <stdint.h>
#include "../global/global.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ProteusIII_defined
#define _ProteusIII_defined

/* timings */
#define ProteusIII_BOOT_DURATION (uint16_t)75

#define ProteusIII_DEFAULT_BAUDRATE (uint32_t)115200


typedef struct ProteusIII_Device_t
{
    uint8_t btmac[6];
    int8_t rssi;
    int8_t txpower;
    uint8_t devicenamelength;
    uint8_t devicename[32];
} ProteusIII_Device_t;

#define MAX_NUMBER_OF_DEVICES (uint8_t)10

typedef struct ProteusIII_GetDevices_t
{
    uint8_t numberofdevices;
    ProteusIII_Device_t devices[MAX_NUMBER_OF_DEVICES];
} ProteusIII_GetDevices_t;

typedef enum ProteusIII_DisplayPasskeyAction_t
{
    ProteusIII_DisplayPasskeyAction_NoAction      = (uint8_t)0x00,
    ProteusIII_DisplayPasskeyAction_PleaseConfirm = (uint8_t)0x01
} ProteusIII_DisplayPasskeyAction_t;

#define ProteusIII_AMOUNT_GPIO_PINS    6

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
	ProteusIII_GPIO_IO_Disconnected  = (uint8_t)0x00,
    ProteusIII_GPIO_IO_Input         = (uint8_t)0x01,
    ProteusIII_GPIO_IO_Output        = (uint8_t)0x02,
    ProteusIII_GPIO_IO_PWM           = (uint8_t)0x03
} ProteusIII_GPIO_IO_t;

typedef enum ProteusIII_GPIO_Output_t
{
    ProteusIII_GPIO_Output_Low  = (uint8_t)0x00,
    ProteusIII_GPIO_Output_High = (uint8_t)0x01
} ProteusIII_GPIO_Output_t;

typedef enum ProteusIII_GPIO_Input_t
{
    ProteusIII_GPIO_Input_NoPull   = (uint8_t)0x00,
    ProteusIII_GPIO_Input_PullDown = (uint8_t)0x01,
    ProteusIII_GPIO_Input_PullUp   = (uint8_t)0x02
} ProteusIII_GPIO_Input_t;

typedef struct ProteusIII_GPIO_PwmValue_t
{
    uint16_t period; /* in ms */
    uint8_t  ratio;  /* 0-255 (0%-100%)*/
} ProteusIII_GPIO_PwmValue_t;

typedef struct ProteusIII_GPIOConfigBlock_t
{
    ProteusIII_GPIO_t GPIO_ID;
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
    ProteusIII_GPIO_t GPIO_ID;
    union /* 1Byte */
    {
        ProteusIII_GPIO_Output_t output;
		uint8_t ratio;  /* 0-255 (0%-100%)*/
    } value;
} ProteusIII_GPIOControlBlock_t;

typedef enum ProteusIII_States_t
{
    ProteusIII_State_BLE_Invalid =         (uint8_t)0x00,
    ProteusIII_State_BLE_Connected =       (uint8_t)0x01,
    ProteusIII_State_BLE_Channel_Open =    (uint8_t)0x02,
} ProteusIII_States_t;

typedef enum ProteusIII_Security_t
{
    ProteusIII_State_BLE_ReBonded = (uint8_t)0x00,
    ProteusIII_State_BLE_Bonded =   (uint8_t)0x01,
    ProteusIII_State_BLE_Paired =   (uint8_t)0x02,
} ProteusIII_Security_t;

typedef enum ProteusIII_BLE_Role_t
{
    ProteusIII_BLE_Role_None =         (uint8_t)0x00,
    ProteusIII_BLE_Role_Pheripheral =  (uint8_t)0x01,
    ProteusIII_BLE_Role_Central =      (uint8_t)0x02,
    ProteusIII_BLE_Role_DTM =          (uint8_t)0x10,
} ProteusIII_BLE_Role_t;

typedef enum ProteusIII_BLE_Action_t
{
    ProteusIII_BLE_Action_None =       (uint8_t)0x00,
    ProteusIII_BLE_Action_Idle =       (uint8_t)0x01,
    ProteusIII_BLE_Action_Scanning =   (uint8_t)0x02,
    ProteusIII_BLE_Action_Connected =  (uint8_t)0x03,
    ProteusIII_BLE_Action_Sleep =      (uint8_t)0x04,
    ProteusIII_BLE_Action_DTM =        (uint8_t)0x05,
} ProteusIII_BLE_Action_t;


/* user settings */
typedef enum ProteusIII_UserSettings_t
{
    ProteusIII_USERSETTING_POSITION_FS_FWVersion =             (uint8_t)0x01,
    ProteusIII_USERSETTING_POSITION_RF_DEVICE_NAME =           (uint8_t)0x02,
    ProteusIII_USERSETTING_POSITION_FS_MAC =                   (uint8_t)0x03,
    ProteusIII_USERSETTING_POSITION_FS_BTMAC =                 (uint8_t)0x04,
    ProteusIII_USERSETTING_POSITION_RF_ADVERTISING_TIMEOUT =   (uint8_t)0x07,
    ProteusIII_USERSETTING_POSITION_RF_CONNECTION_TIMING =     (uint8_t)0x08,
    ProteusIII_USERSETTING_POSITION_RF_SCAN_TIMING =           (uint8_t)0x09,
    ProteusIII_USERSETTING_POSITION_RF_SCAN_FACTOR =           (uint8_t)0x0A,
    ProteusIII_USERSETTING_POSITION_UART_CONFIG_INDEX   =      (uint8_t)0x0B,
    ProteusIII_USERSETTING_POSITION_RF_SEC_FLAGS =             (uint8_t)0x0C,
    ProteusIII_USERSETTING_POSITION_RF_SCAN_FLAGS =            (uint8_t)0x0D,
    ProteusIII_USERSETTING_POSITION_RF_BEACON_FLAGS =          (uint8_t)0x0E,
    ProteusIII_USERSETTING_POSITION_FS_DEVICE_INFO =           (uint8_t)0x0F,
    ProteusIII_USERSETTING_POSITION_FS_SERIAL_NUMBER =         (uint8_t)0x10,
    ProteusIII_USERSETTING_POSITION_RF_TX_POWER =              (uint8_t)0x11,
    ProteusIII_USERSETTING_POSITION_RF_STATIC_PASSKEY =        (uint8_t)0x12,
	ProteusIII_USERSETTING_POSITION_DIS_FLAGS =                (uint8_t)0x13,
	ProteusIII_USERSETTING_POSITION_DIS_MANUFACTURERNAME =     (uint8_t)0x14,
	ProteusIII_USERSETTING_POSITION_DIS_MODELNUMBER =          (uint8_t)0x15,
	ProteusIII_USERSETTING_POSITION_DIS_SERIALNUMBER =         (uint8_t)0x16,
	ProteusIII_USERSETTING_POSITION_DIS_HWVERSION =            (uint8_t)0x17,
	ProteusIII_USERSETTING_POSITION_DIS_SWVERSION =            (uint8_t)0x18,
	ProteusIII_USERSETTING_POSITION_RF_APPEARANCE =            (uint8_t)0x19,
	ProteusIII_USERSETTING_POSITION_RF_SPPBASEUUID =           (uint8_t)0x1A,
    ProteusIII_USERSETTING_POSITION_RF_CFGFLAGS =              (uint8_t)0x1C,
	ProteusIII_USERSETTING_POSITION_RF_ADVERTISING_FLAGS =     (uint8_t)0x1D,
    ProteusIII_USERSETTING_POSITION_RF_SPPServiceUUID =        (uint8_t)0x20,
    ProteusIII_USERSETTING_POSITION_RF_SPPRXUUID =             (uint8_t)0x21,
    ProteusIII_USERSETTING_POSITION_RF_SPPTXUUID =             (uint8_t)0x22,
} ProteusIII_UserSettings_t;

#define SEC_MODE_BONDING_ENABLE_MASK          (uint8_t)0x08
#define SEC_MODE_BONDEDCONNECTIONSONLY_ENABLE (uint8_t)0x10

typedef enum ProteusIII_SecFlags_t
{
    ProteusIII_SecFlags_NONE =                       (uint8_t)0x00,
    ProteusIII_SecFlags_LescJustWorks =              (uint8_t)0x01,
    ProteusIII_SecFlags_JustWorks =                  (uint8_t)0x02,
    ProteusIII_SecFlags_StaticPassKey =              (uint8_t)0x03,
    ProteusIII_SecFlags_LescNumCompare =             (uint8_t)0x04,
    ProteusIII_SecFlags_LescPassKey =                (uint8_t)0x05,
    ProteusIII_SecFlags_LescJustWorks_Bonding =      (uint8_t)(ProteusIII_SecFlags_LescJustWorks | SEC_MODE_BONDING_ENABLE_MASK),
    ProteusIII_SecFlags_JustWorks_Bonding =          (uint8_t)(ProteusIII_SecFlags_JustWorks | SEC_MODE_BONDING_ENABLE_MASK),
    ProteusIII_SecFlags_StaticPassKey_Bonding =      (uint8_t)(ProteusIII_SecFlags_StaticPassKey | SEC_MODE_BONDING_ENABLE_MASK),
	ProteusIII_SecFlags_LescNumCompare_Bonding =     (uint8_t)(ProteusIII_SecFlags_LescNumCompare | SEC_MODE_BONDING_ENABLE_MASK),
    ProteusIII_SecFlags_LescPassKey_Bonding =        (uint8_t)(ProteusIII_SecFlags_LescPassKey | SEC_MODE_BONDING_ENABLE_MASK),
    ProteusIII_SecFlags_LescJustWorks_BondingOnly =  (uint8_t)(ProteusIII_SecFlags_LescJustWorks | SEC_MODE_BONDING_ENABLE_MASK | SEC_MODE_BONDEDCONNECTIONSONLY_ENABLE),
    ProteusIII_SecFlags_JustWorks_BondingOnly =      (uint8_t)(ProteusIII_SecFlags_JustWorks | SEC_MODE_BONDING_ENABLE_MASK | SEC_MODE_BONDEDCONNECTIONSONLY_ENABLE),
    ProteusIII_SecFlags_StaticPassKey_BondingOnly =  (uint8_t)(ProteusIII_SecFlags_StaticPassKey | SEC_MODE_BONDING_ENABLE_MASK | SEC_MODE_BONDEDCONNECTIONSONLY_ENABLE),
	ProteusIII_SecFlags_LescNumCompare_BondingOnly = (uint8_t)(ProteusIII_SecFlags_LescNumCompare | SEC_MODE_BONDING_ENABLE_MASK | SEC_MODE_BONDEDCONNECTIONSONLY_ENABLE),
    ProteusIII_SecFlags_LescPassKey_BondingOnly =    (uint8_t)(ProteusIII_SecFlags_LescPassKey | SEC_MODE_BONDING_ENABLE_MASK | SEC_MODE_BONDEDCONNECTIONSONLY_ENABLE),
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
    ProteusIII_ConnectionTiming_8 = (uint8_t)0x08
} ProteusIII_ConnectionTiming_t;

typedef enum ProteusIII_ScanTiming_t
{
    ProteusIII_ScanTiming_0  = (uint8_t)0x00,
    ProteusIII_ScanTiming_1  = (uint8_t)0x01,
    ProteusIII_ScanTiming_2  = (uint8_t)0x02,
    ProteusIII_ScanTiming_3  = (uint8_t)0x03,
    ProteusIII_ScanTiming_4  = (uint8_t)0x04,
    ProteusIII_ScanTiming_5  = (uint8_t)0x05,
    ProteusIII_ScanTiming_6  = (uint8_t)0x06,
    ProteusIII_ScanTiming_7  = (uint8_t)0x07,
    ProteusIII_ScanTiming_8  = (uint8_t)0x08,
    ProteusIII_ScanTiming_9  = (uint8_t)0x09,
    ProteusIII_ScanTiming_10 = (uint8_t)0x0A,
    ProteusIII_ScanTiming_11 = (uint8_t)0x0B
} ProteusIII_ScanTiming_t;

typedef enum ProteusIII_TXPower_t
{
	ProteusIII_TXPower_8       = (int8_t) 8,
    ProteusIII_TXPower_7       = (int8_t) 7,
    ProteusIII_TXPower_6       = (int8_t) 6,
    ProteusIII_TXPower_5       = (int8_t) 5,
    ProteusIII_TXPower_4       = (int8_t) 4,
    ProteusIII_TXPower_3       = (int8_t) 3,
	ProteusIII_TXPower_2       = (int8_t) 2,
	ProteusIII_TXPower_0       = (int8_t) 0,
    ProteusIII_TXPower_minus4  = (int8_t)-4,
    ProteusIII_TXPower_minus8  = (int8_t)-8,
    ProteusIII_TXPower_minus12 = (int8_t)-12,
} ProteusIII_TXPower_t;

typedef enum ProteusIII_BaudRate_t
{
    ProteusIII_BaudRateIndex_1200    = (uint8_t)0,
    ProteusIII_BaudRateIndex_2400    = (uint8_t)2,
    ProteusIII_BaudRateIndex_4800    = (uint8_t)4,
    ProteusIII_BaudRateIndex_9600    = (uint8_t)6,
    ProteusIII_BaudRateIndex_14400   = (uint8_t)8,
    ProteusIII_BaudRateIndex_19200   = (uint8_t)10,
    ProteusIII_BaudRateIndex_28800   = (uint8_t)12,
    ProteusIII_BaudRateIndex_38400   = (uint8_t)14,
    ProteusIII_BaudRateIndex_56000   = (uint8_t)16,
    ProteusIII_BaudRateIndex_57600   = (uint8_t)18,
    ProteusIII_BaudRateIndex_76800   = (uint8_t)20,
    ProteusIII_BaudRateIndex_115200  = (uint8_t)22,
    ProteusIII_BaudRateIndex_230400  = (uint8_t)24,
    ProteusIII_BaudRateIndex_250000  = (uint8_t)26,
    ProteusIII_BaudRateIndex_460800  = (uint8_t)28,
    ProteusIII_BaudRateIndex_921600  = (uint8_t)30,
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
	ProteusIII_Phy_125kBit_LECoded = 0x04,
} ProteusIII_Phy_t;

/* Callback definition */

typedef void (*ProteusIII_RxCallback)(uint8_t* payload, uint16_t payload_length, uint8_t* BTMAC, int8_t rssi);
typedef void (*ProteusIII_ConnectCallback)(uint8_t* BTMAC);
typedef void (*ProteusIII_SecurityCallback)(uint8_t* BTMAC, ProteusIII_Security_t security_state);
typedef void (*ProteusIII_PasskeyCallback)(uint8_t* BTMAC);
typedef void (*ProteusIII_DisplayPasskeyCallback)(ProteusIII_DisplayPasskeyAction_t action, uint8_t* BTMAC, uint8_t* passkey);
typedef void (*ProteusIII_DisconnectCallback)();
typedef void (*ProteusIII_ChannelopenCallback)(uint8_t* BTMAC, uint16_t max_payload);
typedef void (*ProteusIII_PhyupdateCallback)(uint8_t* BTMAC, uint8_t phy_rx, uint8_t phy_tx);

typedef struct ProteusIII_CallbackConfig_t
{
    ProteusIII_RxCallback              rxCb;
    ProteusIII_ConnectCallback         connectCb;
    ProteusIII_SecurityCallback        securityCb;
    ProteusIII_PasskeyCallback         passkeyCb;
    ProteusIII_DisplayPasskeyCallback  displayPasskeyCb;
    ProteusIII_DisconnectCallback      disconnectCb;
    ProteusIII_ChannelopenCallback     channelOpenCb;
    ProteusIII_PhyupdateCallback       phyUpdateCb;
} ProteusIII_CallbackConfig_t;

extern bool ProteusIII_Init(uint32_t baudrate, WE_FlowControl_t flow_control, ProteusIII_CallbackConfig_t callbackConfig);
extern bool ProteusIII_Deinit(void);

extern bool ProteusIII_PinReset(void);
extern bool ProteusIII_Reset(void);

extern bool ProteusIII_Sleep();
extern bool ProteusIII_PinWakeup();

extern bool ProteusIII_Connect(uint8_t *btmacP);
extern bool ProteusIII_Disconnect();

extern bool ProteusIII_Scanstart();
extern bool ProteusIII_Scanstop();
extern bool ProteusIII_GetDevices(ProteusIII_GetDevices_t* devicesP);

extern bool ProteusIII_Transmit(uint8_t* PayloadP, uint16_t length);

extern bool ProteusIII_Passkey(uint8_t* passkeyP);
extern bool ProteusIII_NumericCompareConfirm(bool keyIsOk);

extern bool ProteusIII_PhyUpdate(ProteusIII_Phy_t phy);

extern ProteusIII_States_t ProteusIII_GetDriverState();

/* functions to control the GPIO feature */
extern bool ProteusIII_GPIOLocalWriteConfig(ProteusIII_GPIOConfigBlock_t* configP, uint16_t number_of_configs);
extern bool ProteusIII_GPIOLocalReadConfig(ProteusIII_GPIOConfigBlock_t* configP, uint16_t* number_of_configsP);
extern bool ProteusIII_GPIOLocalWrite(ProteusIII_GPIOControlBlock_t* controlP, uint16_t number_of_controls);
extern bool ProteusIII_GPIOLocalRead(uint8_t *GPIOToReadP, uint8_t amountGPIOToRead, ProteusIII_GPIOControlBlock_t* controlP, uint16_t* number_of_controlsP);

extern bool ProteusIII_GPIORemoteWriteConfig(ProteusIII_GPIOConfigBlock_t* configP, uint16_t number_of_configs);
extern bool ProteusIII_GPIORemoteReadConfig(ProteusIII_GPIOConfigBlock_t* configP, uint16_t* number_of_configsP);
extern bool ProteusIII_GPIORemoteWrite(ProteusIII_GPIOControlBlock_t* controlP, uint16_t number_of_controls);
extern bool ProteusIII_GPIORemoteRead(uint8_t *GPIOToReadP, uint8_t amountGPIOToRead, ProteusIII_GPIOControlBlock_t* controlP, uint16_t* number_of_controlsP);

extern bool ProteusIII_Allowunbondedconnections();

/* Functions that write the non-volatile settings in the flash: After modification of any non-volatile setting,
 * the module must be reset such that the update takes effect.
 * IMPORTANT: Use only in rare cases, since flash can be written to only a limited number of times.
 */
extern bool ProteusIII_FactoryReset();
extern bool ProteusIII_Set(ProteusIII_UserSettings_t userSetting, uint8_t *ValueP, uint8_t length);
extern bool ProteusIII_SetDeviceName(uint8_t *deviceNameP, uint8_t nameLength);
extern bool ProteusIII_SetAdvertisingTimeout(uint16_t advTimeout);
extern bool ProteusIII_SetCFGFlags(uint16_t cfgflags);
extern bool ProteusIII_SetConnectionTiming(ProteusIII_ConnectionTiming_t connectionTiming);
extern bool ProteusIII_SetScanTiming(ProteusIII_ScanTiming_t scanTiming);
extern bool ProteusIII_SetTXPower(ProteusIII_TXPower_t txpower);
extern bool ProteusIII_SetSecFlags(ProteusIII_SecFlags_t secflags);
extern bool ProteusIII_SetBaudrateIndex(ProteusIII_BaudRate_t baudrate, ProteusIII_UartParity_t parity, bool flowcontrolEnable);
extern bool ProteusIII_SetStaticPasskey(uint8_t *staticPasskeyP);

/* Read the non-volatile settings */
extern bool ProteusIII_Get(ProteusIII_UserSettings_t userSetting, uint8_t *responseP, uint16_t *response_LengthP);
extern bool ProteusIII_GetFWVersion(uint8_t *versionP);
extern bool ProteusIII_GetDeviceName(uint8_t *deviceNameP, uint16_t *nameLengthP);
extern bool ProteusIII_GetMAC(uint8_t *macP);
extern bool ProteusIII_GetBTMAC(uint8_t *BTMacP);
extern bool ProteusIII_GetAdvertisingTimeout(uint16_t *advTimeoutP);
extern bool ProteusIII_GetCFGFlags(uint16_t *cfgflags);
extern bool ProteusIII_GetConnectionTiming(ProteusIII_ConnectionTiming_t *connectionTimingP);
extern bool ProteusIII_GetScanTiming(ProteusIII_ScanTiming_t *scanTimingP);
extern bool ProteusIII_GetTXPower(ProteusIII_TXPower_t *txpowerP);
extern bool ProteusIII_GetSecFlags(ProteusIII_SecFlags_t *secflagsP);
extern bool ProteusIII_GetBaudrateIndex(ProteusIII_BaudRate_t *baudrateP, ProteusIII_UartParity_t *parityP, bool *flowcontrolEnableP);
extern bool ProteusIII_GetStaticPasskey(uint8_t *staticPasskeyP);
extern bool ProteusIII_GetState(ProteusIII_BLE_Role_t *BLE_roleP, ProteusIII_BLE_Action_t *BLE_actionP, uint8_t *InfoP, uint8_t *LengthP);

#endif // _ProteusIII_defined

#ifdef __cplusplus
}
#endif

