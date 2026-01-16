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
 * @file ThyoneE.h
 * @brief Thyone-e driver header file.
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef THYONEE_H_INCLUDED
#define THYONEE_H_INCLUDED

#include <global/global_types.h>
#include <stdbool.h>

/** Boot-up time */
#define THYONEE_BOOT_DURATION ((uint16_t)10)

/** Default UART baud rate */
#define THYONEE_DEFAULT_BAUDRATE ((uint32_t)115200)

/** Number of remote GPIOs */
#define THYONEE_AMOUNT_GPIO_PINS 2

/**
 * @brief Definition of the radio modules pins
 */
typedef struct ThyoneE_Pins_t
{
    WE_Pin_t ThyoneE_Pin_Reset;
    WE_Pin_t ThyoneE_Pin_Mode;
    WE_Pin_t ThyoneE_Pin_Busy;

} ThyoneE_Pins_t;

/**
 * @brief Definition of the remote GPIO IDs
 */
typedef enum ThyoneE_GPIO_t
{
    ThyoneE_GPIO_1 = (uint8_t)0x01,
    ThyoneE_GPIO_2 = (uint8_t)0x02
} ThyoneE_GPIO_t;

/**
 * @brief Definition of the remote GPIO types
 */
typedef enum ThyoneE_GPIO_IO_t
{
    ThyoneE_GPIO_IO_Disconnected = (uint8_t)0x00,
    ThyoneE_GPIO_IO_Input = (uint8_t)0x01,
    ThyoneE_GPIO_IO_Output = (uint8_t)0x02
} ThyoneE_GPIO_IO_t;

/**
 * @brief Definition of the remote GPIO output types
 */
typedef enum ThyoneE_GPIO_Output_t
{
    ThyoneE_GPIO_Output_Low = (uint8_t)0x00,
    ThyoneE_GPIO_Output_High = (uint8_t)0x01
} ThyoneE_GPIO_Output_t;

/**
 * @brief Definition of the remote GPIO input types
 */
typedef enum ThyoneE_GPIO_Input_t
{
    ThyoneE_GPIO_Input_NoPull = (uint8_t)0x00,
    ThyoneE_GPIO_Input_PullDown = (uint8_t)0x01,
    ThyoneE_GPIO_Input_PullUp = (uint8_t)0x02
} ThyoneE_GPIO_Input_t;

/**
 * @brief Definition of the remote GPIO configuration
 */
typedef struct ThyoneE_GPIOConfigBlock_t
{
    ThyoneE_GPIO_t GPIO_ID;
    ThyoneE_GPIO_IO_t function;
    union /* 3Byte */
    {
        ThyoneE_GPIO_Input_t input;
        ThyoneE_GPIO_Output_t output;
    } value;
} ThyoneE_GPIOConfigBlock_t;

/**
 * @brief Definition of the remote GPIO control
 */
typedef struct ThyoneE_GPIOControlBlock_t
{
    ThyoneE_GPIO_t GPIO_ID;
    union /* 1Byte */
    {
        ThyoneE_GPIO_Output_t output;
    } value;
} ThyoneE_GPIOControlBlock_t;

/**
 * @brief Reset reason
 */
typedef enum ThyoneE_ResetReason_t
{
    ThyoneE_ResetReason_PowerOn_WakeUp = (uint8_t)0x01,
    ThyoneE_ResetReason_PinReset = (uint8_t)0x02,
    ThyoneE_ResetReason_SoftReset = (uint8_t)0x04,
    ThyoneE_ResetReason_Invalid = (uint8_t)0xFF,
} ThyoneE_ResetReason_t;

/**
 * @brief Device states
 */
typedef enum ThyoneE_States_t
{
    ThyoneE_State_Application = (uint8_t)0x00,
    ThyoneE_State_Test = (uint8_t)0x01,
    ThyoneE_State_Invalid = (uint8_t)0xFF,
} ThyoneE_States_t;

/**
 * @brief Module mode
 */
typedef enum ThyoneE_ModuleMode_t
{
    ThyoneE_ModuleMode_Normal = (uint8_t)0,
    ThyoneE_ModuleMode_Sniffer = (uint8_t)1,
    ThyoneE_ModuleMode_Repeater = (uint8_t)2,
    ThyoneE_ModuleMode_Invalid = (uint8_t)0xFF,
} ThyoneE_ModuleMode_t;

/**
 * @brief Operation mode
 */
typedef enum ThyoneE_OperationMode_t
{
    ThyoneE_OperationMode_CommandMode,
    ThyoneE_OperationMode_TransparentMode
} ThyoneE_OperationMode_t;

/**
 * @brief Definition of the user settings (settings stored in non-volatile flash)
 */
typedef enum ThyoneE_UserSettings_t
{
    ThyoneE_USERSETTING_INDEX_SERIAL_NUMBER = (uint8_t)0x01,
    ThyoneE_USERSETTING_INDEX_FW_VERSION = (uint8_t)0x02,
    ThyoneE_USERSETTING_INDEX_UART_CONFIG = (uint8_t)0x04,
    ThyoneE_USERSETTING_INDEX_RF_CHANNEL = (uint8_t)0x07,
    ThyoneE_USERSETTING_INDEX_ENCRYPTION_MODE = (uint8_t)0x08,
    ThyoneE_USERSETTING_INDEX_RF_PROFILE = (uint8_t)0x09,
    ThyoneE_USERSETTING_INDEX_RF_NUM_RETRIES = (uint8_t)0x0A,
    ThyoneE_USERSETTING_INDEX_RF_TX_POWER = (uint8_t)0x0B,
    ThyoneE_USERSETTING_INDEX_RF_RP_NUM_SLOTS = (uint8_t)0x0C,
    ThyoneE_USERSETTING_INDEX_MAC_SOURCE_ADDRESS = (uint8_t)0x10,
    ThyoneE_USERSETTING_INDEX_MAC_DESTINATION_ADDRESS = (uint8_t)0x11,
    ThyoneE_USERSETTING_INDEX_MAC_GROUP_ID = (uint8_t)0x12,
    ThyoneE_USERSETTING_INDEX_MAC_ENCRYPTION_KEY = (uint8_t)0x14,
    ThyoneE_USERSETTING_INDEX_MAC_TLL = (uint8_t)0x15,
    ThyoneE_USERSETTING_INDEX_REMOTE_GPIO_CONFIG = (uint8_t)0x18,
    ThyoneE_USERSETTING_INDEX_MAC_DOUBLEPACKETCLEARTIME = (uint8_t)0x1F,
    ThyoneE_USERSETTING_INDEX_MODULE_MODE = (uint8_t)0x20,
} ThyoneE_UserSettings_t;

/**
 * @brief Definition of the runtime settings (settings stored in volatile RAM)
 */
typedef enum ThyoneE_RuntimeSettings_t
{
    ThyoneE_RUNTIMESETTING_INDEX_CONTEXT = (uint8_t)0xFF,
} ThyoneE_RuntimeSettings_t;

/**
 * @brief Definition of the TX power user setting
 */
typedef enum ThyoneE_TXPower_t
{
    ThyoneE_TXPower_4 = (int8_t)4,
    ThyoneE_TXPower_0 = (int8_t)0,
    ThyoneE_TXPower_minus4 = (int8_t)-4,
    ThyoneE_TXPower_minus8 = (int8_t)-8,
    ThyoneE_TXPower_minus12 = (int8_t)-12,
    ThyoneE_TXPower_minus16 = (int8_t)-16,
    ThyoneE_TXPower_minus20 = (int8_t)-20,
    ThyoneE_TXPower_minus40 = (int8_t)-40,
} ThyoneE_TXPower_t;

/**
 * @brief Definition of the UART baud rate
 */
typedef enum ThyoneE_BaudRateIndex_t
{
    ThyoneE_BaudRateIndex_1200 = (uint8_t)0,
    ThyoneE_BaudRateIndex_2400 = (uint8_t)2,
    ThyoneE_BaudRateIndex_4800 = (uint8_t)4,
    ThyoneE_BaudRateIndex_9600 = (uint8_t)6,
    ThyoneE_BaudRateIndex_14400 = (uint8_t)8,
    ThyoneE_BaudRateIndex_19200 = (uint8_t)10,
    ThyoneE_BaudRateIndex_28800 = (uint8_t)12,
    ThyoneE_BaudRateIndex_38400 = (uint8_t)14,
    ThyoneE_BaudRateIndex_56000 = (uint8_t)16,
    ThyoneE_BaudRateIndex_57600 = (uint8_t)18,
    ThyoneE_BaudRateIndex_76800 = (uint8_t)20,
    ThyoneE_BaudRateIndex_115200 = (uint8_t)22,
    ThyoneE_BaudRateIndex_230400 = (uint8_t)24,
    ThyoneE_BaudRateIndex_250000 = (uint8_t)26,
    ThyoneE_BaudRateIndex_460800 = (uint8_t)28,
    ThyoneE_BaudRateIndex_921600 = (uint8_t)30,
    ThyoneE_BaudRateIndex_1000000 = (uint8_t)32
} ThyoneE_BaudRateIndex_t;

/**
 * @brief Definition of the UART parity
 */
typedef enum ThyoneE_UartParity_t
{
    ThyoneE_UartParity_Even,
    ThyoneE_UartParity_None
} ThyoneE_UartParity_t;

/**
 * @brief Definition of the encryption mode user setting
 */
typedef enum ThyoneE_EncryptionMode_t
{
    ThyoneE_EncryptionMode_Unencrypted = (uint8_t)0,
    ThyoneE_EncryptionMode_TxEncrypted = (uint8_t)1,   /** Transmit encrypted messages */
    ThyoneE_EncryptionMode_RxEncrypted = (uint8_t)2,   /** Receive only encrypted messages. Discard all other messages */
    ThyoneE_EncryptionMode_RxTxEncrypted = (uint8_t)3, /** Transmit and receive encrypted messages. Discard all other messages */
    ThyoneE_EncryptionMode_Invalid = (uint8_t)0xFF,
} ThyoneE_EncryptionMode_t;

/**
 * @brief Definition of the radio profile user setting
 */
typedef enum ThyoneE_Profile_t
{
    ThyoneE_Profile_1000kbit = (uint8_t)2,
    ThyoneE_Profile_2000kbit = (uint8_t)3,
    ThyoneE_Profile_Invalid = (uint8_t)0xFF,
} ThyoneE_Profile_t;

/**
 * @brief Definition of the address mode user setting
 */
typedef enum ThyoneE_AddressMode_t
{
    ThyoneE_AddressMode_Broadcast = (uint8_t)0,
    ThyoneE_AddressMode_Multicast = (uint8_t)1,
    ThyoneE_AddressMode_Unicast = (uint8_t)2,
} ThyoneE_AddressMode_t;

/**
 * @brief Initialize the ThyoneE interface for serial interface
 *
 * Caution: The parameter baudrate must match the configured UserSettings of the ThyoneE.
 *          The baudrate parameter must match to perform a successful FTDI communication.
 *          Updating this parameter during runtime may lead to communication errors.
 *
 * @param[in] uartP: Pointer to definition of the uart connected to the module
 * @param[in] pinoutP: Pointer to the definition of the gpios connected to the module
 * @param[in] opMode: Operation mode
 * @param[in] RXcb: RX callback function
 *
 * @return True if initialization succeeded,
 *         false otherwise
 */
extern bool ThyoneE_Init(WE_UART_t* uartP, ThyoneE_Pins_t* pinoutP, ThyoneE_OperationMode_t opMode, void (*RXcb)(uint8_t*, uint16_t, uint32_t, int8_t));

/**
 * @brief Deinitialize the ThyoneE interface
 *
 * @return True if deinitialization succeeded,
 *         false otherwise
 */
extern bool ThyoneE_Deinit(void);

/**
 * @brief Reset the ThyoneE by pin
 *
 * @return True if reset succeeded,
 *         false otherwise
 */
extern bool ThyoneE_PinReset(void);

/**
 * @brief Reset the ThyoneE by command
 *
 * @return True if reset succeeded,
 *         false otherwise
 */
extern bool ThyoneE_Reset(void);

/**
 * @brief Put the ThyoneE into sleep mode
 *
 * @return True if succeeded,
 *         false otherwise
 */
extern bool ThyoneE_Sleep();

/**
 * @brief Returns the current level of the BUSY pin.
 *
 * @param[out] busyStateP: Pointer to the busy pin state
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_IsTransparentModeBusy(bool* busyStateP);

/**
 * @brief Transmit data as broadcast
 *
 * @param[in] payloadP: Pointer to the data to transmit
 * @param[in] length: Length of the data to transmit
 *
 * @return True if succeeded,
 *         false otherwise
 */
extern bool ThyoneE_TransmitBroadcast(uint8_t* payloadP, uint16_t length);

/**
 * @brief Transmit data as multicast
 *
 * @param[in] payloadP: Pointer to the data to transmit
 * @param[in] length: Length of the data to transmit
 *
 * @return True if succeeded,
 *         false otherwise
 */
extern bool ThyoneE_TransmitMulticast(uint8_t* payloadP, uint16_t length);

/**
 * @brief Transmit data as unicast
 *
 * @param[in] payloadP: Pointer to the data to transmit
 * @param[in] length: Length of the data to transmit
 *
 * @return True if succeeded,
 *         false otherwise
 */
extern bool ThyoneE_TransmitUnicast(uint8_t* payloadP, uint16_t length);
extern bool ThyoneE_Transparent_Transmit(const uint8_t* data, uint16_t dataLength);

/**
 * @brief Transmit data as multicast
 *
 * @param[in] groupID: Group ID to multicast
 * @param[in] payloadP: Pointer to the data to transmit
 * @param[in] length: Length of the data to transmit
 *
 * @return True if succeeded,
 *         false otherwise
 */
extern bool ThyoneE_TransmitMulticastExtended(uint8_t groupID, uint8_t* payloadP, uint16_t length);

/**
 * @brief Transmit data as unicast
 *
 * @param[in] address: Address to sent to
 * @param[in] payloadP: Pointer to the data to transmit
 * @param[in] length: Length of the data to transmit
 *
 * @return True if succeeded,
 *         false otherwise
 */
extern bool ThyoneE_TransmitUnicastExtended(uint32_t address, uint8_t* payloadP, uint16_t length);

/* Functions to control the GPIO feature */

/**
 * @brief Configure the local GPIO of the module
 *
 * @param[in] configP: Pointer to one or more pin configurations
 * @param[in] numberOfConfigs: number of entries in configP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GPIOLocalSetConfig(ThyoneE_GPIOConfigBlock_t* configP, uint16_t numberOfConfigs);

/**
 * @brief Read the local GPIO configuration of the module
 *
 * @param[out] configP: Pointer to one or more pin configurations
 * @param[out] numberOfConfigsP: Pointer to number of entries in configP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GPIOLocalGetConfig(ThyoneE_GPIOConfigBlock_t* configP, uint16_t* numberOfConfigsP);

/**
 * @brief Set the output value of the local pin. Pin has to be configured first.
 * See ThyoneE_GPIOLocalWriteConfig
 *
 * @param[in] controlP: Pointer to one or more pin controls
 * @param[in] numberOfControls: number of entries in controlP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GPIOLocalWrite(ThyoneE_GPIOControlBlock_t* controlP, uint16_t numberOfControls);

/**
 * @brief Read the input of the pin. Pin has to be configured first.
 * See ThyoneE_GPIOLocalWriteConfig
 *
 * @param[in] GPIOToReadP: One or more pins to read.
 * @param[in] amountGPIOToRead: amount of pins to read and therefore length of GPIOToRead
 * @param[out] controlP: Pointer to controlBlock
 * @param[out] numberOfControlsP: Pointer to number of entries in controlP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GPIOLocalRead(uint8_t* GPIOToReadP, uint8_t amountGPIOToRead, ThyoneE_GPIOControlBlock_t* controlP, uint16_t* numberOfControlsP);

/**
 * @brief Configure the remote GPIO of the module
 *
 * @param[in] destAddress: Destination address of the remote Thyone-e device
 * @param[in] configP: Pointer to one or more pin configurations
 * @param[in] numberOfConfigs: number of entries in configP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GPIORemoteSetConfig(uint32_t destAddress, ThyoneE_GPIOConfigBlock_t* configP, uint16_t numberOfConfigs);

/**
 * @brief Read the remote GPIO configuration of the module
 *
 * @param[out] destAddress: Destination address of the remote Thyone-e device
 * @param[out] configP: Pointer to one or more pin configurations
 * @param[out] numberOfConfigsP: Pointer to number of entries in configP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GPIORemoteGetConfig(uint32_t destAddress, ThyoneE_GPIOConfigBlock_t* configP, uint16_t* numberOfConfigsP);

/**
 * @brief Set the output value of the remote pin. Pin has to be configured first.
 * See ThyoneE_GPIORemoteWriteConfig
 *
 * @param[in] destAddress: Destination address of the remote Thyone-e device
 * @param[in] controlP: Pointer to one or more pin controls
 * @param[in] numberOfControls: number of entries in controlP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GPIORemoteWrite(uint32_t destAddress, ThyoneE_GPIOControlBlock_t* controlP, uint16_t numberOfControls);

/**
 * @brief Read the input of the pins. Pin has to be configured first.
 * See ThyoneE_GPIORemoteWriteConfig
 *
 * @param[in] destAddress: Destination address of the remote Thyone-e device
 * @param[in] GPIOToReadP: One or more pins to read.
 * @param[in] amountGPIOToRead: Amount of pins to read and therefore length of GPIOToReadP
 * @param[out] controlP: Pointer to controlBlock
 * @param[out] numberOfControlsP: Pointer to number of entries in controlP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GPIORemoteRead(uint32_t destAddress, uint8_t* GPIOToReadP, uint8_t amountGPIOToRead, ThyoneE_GPIOControlBlock_t* controlP, uint16_t* numberOfControlsP);

/**
 * @brief Set a special runtime setting
 *
 * @param[in] runtimeSetting: Runtime setting to be updated
 * @param[in] valueP: Pointer to the new settings value
 * @param[in] length: Length of the value
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_SetRam(ThyoneE_RuntimeSettings_t runtimeSetting, uint8_t* valueP, uint8_t length);

/**
 * @brief Request the current runtime settings
 *
 * @param[in] runtimeSetting: Runtime setting to be requested
 * @param[out] responseP: Pointer of the memory to put the requested content
 * @param[out] lengthP: Length of the requested content
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GetRam(ThyoneE_RuntimeSettings_t runtimeSetting, uint8_t* responseP, uint16_t* lengthP);

/* Functions that write the non-volatile settings in the flash: After modification of any non-volatile setting,
 * the module must be reset such that the update takes effect.
 * IMPORTANT: Use only in rare cases, since flash can be written to only a limited number of times.
 */

/**
 * @brief Factory reset the module
 *
 * @return True if succeeded,
 *         false otherwise
 */
extern bool ThyoneE_FactoryReset();

/**
 * @brief Set a special user setting
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] userSetting: User setting to be updated
 * @param[in] valueP: Pointer to the new settings value
 * @param[in] length: Length of the value
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_Set(ThyoneE_UserSettings_t userSetting, uint8_t* valueP, uint8_t length);

/**
 * @brief Set a special user setting, but checks first if the value is already ok
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] userSetting: User setting to be updated
 * @param[in] valueP: Pointer to the new settings value
 * @param[in] length: Length of the value
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_CheckNSet(ThyoneE_UserSettings_t userSetting, uint8_t* valueP, uint8_t length);

/**
 * @brief Set the UART baudrate index
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] baudrate: UART baud rate index
 * @param[in] parity: UART parity
 * @param[in] flowcontrolEnable: true, if flow control is enabled, false otherwise
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_SetBaudrateIndex(ThyoneE_BaudRateIndex_t baudrate, ThyoneE_UartParity_t parity, bool flowcontrolEnable);

/**
 * @brief Set the encryption mode
 *
 * @param[in] encryptionMode: EncryptionMode
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_SetEncryptionMode(ThyoneE_EncryptionMode_t encryptionMode);

/**
 * @brief Set the rf profile
 *
 * @param[in] profile: Rf profile
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_SetRfProfile(ThyoneE_Profile_t profile);

/**
 * @brief Set the RF channel
 *
 * @param[in] channel: Radio channel
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_SetRFChannel(uint8_t channel);

/**
 * @brief Set the RF channel on-the-fly without changing the user settings (volatile,
 * channel is reverted to the value stored in flash after a reset of the module).
 *
 * @param[in] channel: Radio channel
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_SetRFChannelRuntime(uint8_t channel);

/**
 * @brief Set the number of retries
 *
 * @param[in] numRetries: Number of retries
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_SetNumRetries(uint8_t numRetries);

/**
 * @brief Set the tx power
 *
 * @param[in] txPower: Transmit power
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_SetTXPower(ThyoneE_TXPower_t txPower);

/**
 * @brief Set the number of time slots
 *
 * @param[in] numSlots: Number of time slots
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_SetRpNumSlots(uint8_t numSlots);

/**
 * @brief Set the source address
 *
 * @param[in] sourceAddress: Source address
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_SetSourceAddress(uint32_t sourceAddress);

/**
 * @brief Set the destination address
 *
 * @param[in] destinationAddress: Destination address
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_SetDestinationAddress(uint32_t destinationAddress);

/**
 * @brief Set the group id
 *
 * @param[in] groupId: Group ID
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_SetGroupID(uint8_t groupId);

/**
 * @brief Set the encryption key
 *
 * @param[in] keyP: Pointer to 16-byte key
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_SetEncryptionKey(uint8_t* keyP);

/**
 * @brief Set the time-to-live defining the maximum of hops if repeating
 *
 * @param[in] ttl: Time-to-live value
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_SetTimeToLive(uint8_t ttl);

/**
 * @brief Set remote gpio config
 *
 * @param[in] remoteConfig: Remote gpio config
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_SetGPIOBlockRemoteConfig(uint8_t remoteConfig);

/**
 * @brief Set module mode
 *
 * @param[in] moduleMode: Operation mode of the module
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_SetModuleMode(ThyoneE_ModuleMode_t moduleMode);

/* Read the non-volatile settings */

/**
 * @brief Request the current user settings
 *
 * @param[in] userSetting: User setting to be requested
 * @param[out] responseP: Pointer of the memory to put the requested content
 * @param[out] lengthP: Pointer to the length of the requested content
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_Get(ThyoneE_UserSettings_t userSetting, uint8_t* responseP, uint16_t* lengthP);

/**
 * @brief Request the 4 byte serial number
 *
 * @param[out] serialNumberP: Pointer to the 4 byte serial number
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GetSerialNumber(uint8_t* serialNumberP);

/**
 * @brief Request the 3 byte firmware version
 *
 * @param[out] versionP: Pointer to the 3 byte firmware version
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GetFWVersion(uint8_t* versionP);

/**
 * @brief Request the UART baudrate index
 *
 * @param[out] baudrateP: Pointer to the UART baudrate index
 * @param[out] parityP: Pointer to the UART parity
 * @param[out] flowcontrolEnableP: Pointer to the UART flow control parameter
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GetBaudrateIndex(ThyoneE_BaudRateIndex_t* baudrateP, ThyoneE_UartParity_t* parityP, bool* flowcontrolEnableP);

/**
 * @brief Get the encryption mode
 *
 * @param[out] encryptionModeP: Pointer to encryptionMode
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GetEncryptionMode(ThyoneE_EncryptionMode_t* encryptionModeP);

/**
 * @brief Get the rf profile
 *
 * *output:
 * @param[out] profileP: Pointer to rf profile
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GetRfProfile(ThyoneE_Profile_t* profileP);

/**
 * @brief Get the rf channel
 *
 * @param[out] channelP: Pointer to rf channel
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GetRFChannel(uint8_t* channelP);

/**
 * @brief Get the number of retries
 *
 * @param[out] numRetriesP: Pointer to number of retries
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GetNumRetries(uint8_t* numRetriesP);

/**
 * @brief Request the TX power
 *
 * @param[out] txpowerP: Pointer to the TX power
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GetTXPower(ThyoneE_TXPower_t* txpowerP);

/**
 * @brief Get the number of time slots
 *
 * @param[out] numSlotsP: Pointer of number of time slots
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GetRpNumSlots(uint8_t* numSlotsP);

/**
 * @brief Get the source address
 *
 * @param[out] sourceAddressP: Pointer to source address
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GetSourceAddress(uint32_t* sourceAddressP);

/**
 * @brief Get the destination address
 *
 * @param[out] destinationAddressP: Pointer to destination address
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GetDestinationAddress(uint32_t* destinationAddressP);

/**
 * @brief Get the group id
 *
 * @param[out] groupIdP: Pointer to groupID
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GetGroupID(uint8_t* groupIdP);

/**
 * @brief Get the time-to-live defining the maximum of hops if repeating
 *
 * @param[out] ttlP: Pointer to time-to-live
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GetTimeToLive(uint8_t* ttlP);

/**
 * @brief Get remote gpio config
 *
 * @param[out] remoteConfigP: Pointer to remote gpio config
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GetGPIOBlockRemoteConfig(uint8_t* remoteConfigP);

/**
 * @brief Get module mode
 *
 * @param[out] moduleModeP: Pointer to the operation mode of the module
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneE_GetModuleMode(ThyoneE_ModuleMode_t* moduleModeP);

#endif // THYONEE_H_INCLUDED

#ifdef __cplusplus
}
#endif
