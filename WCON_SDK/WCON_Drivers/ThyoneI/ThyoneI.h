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
 * @file ThyoneI.h
 * @brief Thyone-I driver header file.
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef THYONEI_H_INCLUDED
#define THYONEI_H_INCLUDED

#include <global/global_types.h>
#include <stdbool.h>

/** Boot-up time */
#define THYONEI_BOOT_DURATION ((uint16_t)75)

/** Default UART baud rate */
#define THYONEI_DEFAULT_BAUDRATE ((uint32_t)115200)

/** Number of remote GPIOs */
#define THYONEI_AMOUNT_GPIO_PINS 6

/**
 * @brief Definition of the radio modules pins
 */
typedef struct ThyoneI_Pins_t
{
    WE_Pin_t ThyoneI_Pin_Reset;
    WE_Pin_t ThyoneI_Pin_SleepWakeUp;
    WE_Pin_t ThyoneI_Pin_Boot;
    WE_Pin_t ThyoneI_Pin_Mode;
    WE_Pin_t ThyoneI_Pin_Busy;

} ThyoneI_Pins_t;

/**
 * @brief Definition of the remote GPIO IDs
 */
typedef enum ThyoneI_GPIO_t
{
    ThyoneI_GPIO_1 = (uint8_t)0x01,
    ThyoneI_GPIO_2 = (uint8_t)0x02,
    ThyoneI_GPIO_3 = (uint8_t)0x03,
    ThyoneI_GPIO_4 = (uint8_t)0x04,
    ThyoneI_GPIO_5 = (uint8_t)0x05,
    ThyoneI_GPIO_6 = (uint8_t)0x06
} ThyoneI_GPIO_t;

/**
 * @brief Definition of the remote GPIO types
 */
typedef enum ThyoneI_GPIO_IO_t
{
    ThyoneI_GPIO_IO_Disconnected = (uint8_t)0x00,
    ThyoneI_GPIO_IO_Input = (uint8_t)0x01,
    ThyoneI_GPIO_IO_Output = (uint8_t)0x02,
    ThyoneI_GPIO_IO_PWM = (uint8_t)0x03
} ThyoneI_GPIO_IO_t;

/**
 * @brief Definition of the remote GPIO output types
 */
typedef enum ThyoneI_GPIO_Output_t
{
    ThyoneI_GPIO_Output_Low = (uint8_t)0x00,
    ThyoneI_GPIO_Output_High = (uint8_t)0x01
} ThyoneI_GPIO_Output_t;

/**
 * @brief Definition of the remote GPIO input types
 */
typedef enum ThyoneI_GPIO_Input_t
{
    ThyoneI_GPIO_Input_NoPull = (uint8_t)0x00,
    ThyoneI_GPIO_Input_PullDown = (uint8_t)0x01,
    ThyoneI_GPIO_Input_PullUp = (uint8_t)0x02
} ThyoneI_GPIO_Input_t;

/**
 * @brief Definition of the remote GPIO PWM types
 */
typedef struct ThyoneI_GPIO_PwmValue_t
{
    uint16_t period; /* in ms */
    uint8_t ratio;   /* 0-255 (0%-100%)*/
} ThyoneI_GPIO_PwmValue_t;

/**
 * @brief Definition of the remote GPIO configuration
 */
typedef struct ThyoneI_GPIOConfigBlock_t
{
    ThyoneI_GPIO_t GPIO_ID;
    ThyoneI_GPIO_IO_t function;
    union /* 3Byte */
    {
        ThyoneI_GPIO_PwmValue_t pwm;
        ThyoneI_GPIO_Input_t input;
        ThyoneI_GPIO_Output_t output;
    } value;
} ThyoneI_GPIOConfigBlock_t;

/**
 * @brief Definition of the remote GPIO control
 */
typedef struct ThyoneI_GPIOControlBlock_t
{
    ThyoneI_GPIO_t GPIO_ID;
    union /* 1Byte */
    {
        ThyoneI_GPIO_Output_t output;
        uint8_t ratio; /* 0-255 (0%-100%)*/
    } value;
} ThyoneI_GPIOControlBlock_t;

/**
 * @brief Reset reason
 */
typedef enum ThyoneI_ResetReason_t
{
    ThyoneI_ResetReason_PowerOn_WakeUp = (uint8_t)0x01,
    ThyoneI_ResetReason_PinReset = (uint8_t)0x02,
    ThyoneI_ResetReason_SoftReset = (uint8_t)0x04,
    ThyoneI_ResetReason_Invalid = (uint8_t)0xFF,
} ThyoneI_ResetReason_t;

/**
 * @brief Device states
 */
typedef enum ThyoneI_States_t
{
    ThyoneI_State_Application = (uint8_t)0x00,
    ThyoneI_State_Test = (uint8_t)0x01,
    ThyoneI_State_Invalid = (uint8_t)0xFF,
} ThyoneI_States_t;

/**
 * @brief Module mode
 */
typedef enum ThyoneI_ModuleMode_t
{
    ThyoneI_ModuleMode_Normal = (uint8_t)0,
    ThyoneI_ModuleMode_Sniffer = (uint8_t)1,
    ThyoneI_ModuleMode_Repeater = (uint8_t)2,
    ThyoneI_ModuleMode_Invalid = (uint8_t)0xFF,
} ThyoneI_ModuleMode_t;

/**
 * @brief Operation mode
 */
typedef enum ThyoneI_OperationMode_t
{
    ThyoneI_OperationMode_CommandMode,
    ThyoneI_OperationMode_TransparentMode
} ThyoneI_OperationMode_t;

/**
 * @brief Definition of the user settings (settings stored in non-volatile flash)
 */
typedef enum ThyoneI_UserSettings_t
{
    ThyoneI_USERSETTING_INDEX_SERIAL_NUMBER = (uint8_t)0x01,
    ThyoneI_USERSETTING_INDEX_FW_VERSION = (uint8_t)0x02,
    ThyoneI_USERSETTING_INDEX_UART_CONFIG = (uint8_t)0x04,
    ThyoneI_USERSETTING_INDEX_RF_CHANNEL = (uint8_t)0x07,
    ThyoneI_USERSETTING_INDEX_ENCRYPTION_MODE = (uint8_t)0x08,
    ThyoneI_USERSETTING_INDEX_RF_PROFILE = (uint8_t)0x09,
    ThyoneI_USERSETTING_INDEX_RF_NUM_RETRIES = (uint8_t)0x0A,
    ThyoneI_USERSETTING_INDEX_RF_TX_POWER = (uint8_t)0x0B,
    ThyoneI_USERSETTING_INDEX_RF_RP_NUM_SLOTS = (uint8_t)0x0C,
    ThyoneI_USERSETTING_INDEX_MAC_SOURCE_ADDRESS = (uint8_t)0x10,
    ThyoneI_USERSETTING_INDEX_MAC_DESTINATION_ADDRESS = (uint8_t)0x11,
    ThyoneI_USERSETTING_INDEX_MAC_GROUP_ID = (uint8_t)0x12,
    ThyoneI_USERSETTING_INDEX_MAC_ENCRYPTION_KEY = (uint8_t)0x14,
    ThyoneI_USERSETTING_INDEX_MAC_TLL = (uint8_t)0x15,
    ThyoneI_USERSETTING_INDEX_CCA_MODE = (uint8_t)0x16,
    ThyoneI_USERSETTING_INDEX_CCA_THRESHOLD = (uint8_t)0x17,
    ThyoneI_USERSETTING_INDEX_REMOTE_GPIO_CONFIG = (uint8_t)0x18,
    ThyoneI_USERSETTING_INDEX_MAC_DOUBLEPACKETCLEARTIME = (uint8_t)0x1F,
    ThyoneI_USERSETTING_INDEX_MODULE_MODE = (uint8_t)0x20,
} ThyoneI_UserSettings_t;

/**
 * @brief Definition of the runtime settings (settings stored in volatile RAM)
 */
typedef enum ThyoneI_RuntimeSettings_t
{
    ThyoneI_RUNTIMESETTING_INDEX_CONTEXT = (uint8_t)0xFF,
} ThyoneI_RuntimeSettings_t;

/**
 * @brief Definition of the TX power user setting
 */
typedef enum ThyoneI_TXPower_t
{
    ThyoneI_TXPower_8 = (int8_t)8,
    ThyoneI_TXPower_4 = (int8_t)4,
    ThyoneI_TXPower_0 = (int8_t)0,
    ThyoneI_TXPower_minus4 = (int8_t)-4,
    ThyoneI_TXPower_minus8 = (int8_t)-8,
    ThyoneI_TXPower_minus12 = (int8_t)-12,
    ThyoneI_TXPower_minus16 = (int8_t)-16,
    ThyoneI_TXPower_minus20 = (int8_t)-20,
    ThyoneI_TXPower_minus40 = (int8_t)-40,
} ThyoneI_TXPower_t;

/**
 * @brief Definition of the UART baud rate
 */
typedef enum ThyoneI_BaudRateIndex_t
{
    ThyoneI_BaudRateIndex_1200 = (uint8_t)0,
    ThyoneI_BaudRateIndex_2400 = (uint8_t)2,
    ThyoneI_BaudRateIndex_4800 = (uint8_t)4,
    ThyoneI_BaudRateIndex_9600 = (uint8_t)6,
    ThyoneI_BaudRateIndex_14400 = (uint8_t)8,
    ThyoneI_BaudRateIndex_19200 = (uint8_t)10,
    ThyoneI_BaudRateIndex_28800 = (uint8_t)12,
    ThyoneI_BaudRateIndex_38400 = (uint8_t)14,
    ThyoneI_BaudRateIndex_56000 = (uint8_t)16,
    ThyoneI_BaudRateIndex_57600 = (uint8_t)18,
    ThyoneI_BaudRateIndex_76800 = (uint8_t)20,
    ThyoneI_BaudRateIndex_115200 = (uint8_t)22,
    ThyoneI_BaudRateIndex_230400 = (uint8_t)24,
    ThyoneI_BaudRateIndex_250000 = (uint8_t)26,
    ThyoneI_BaudRateIndex_460800 = (uint8_t)28,
    ThyoneI_BaudRateIndex_921600 = (uint8_t)30,
    ThyoneI_BaudRateIndex_1000000 = (uint8_t)32
} ThyoneI_BaudRateIndex_t;

/**
 * @brief Definition of the UART parity
 */
typedef enum ThyoneI_UartParity_t
{
    ThyoneI_UartParity_Even,
    ThyoneI_UartParity_None
} ThyoneI_UartParity_t;

/**
 * @brief Definition of the encryption mode user setting
 */
typedef enum ThyoneI_EncryptionMode_t
{
    ThyoneI_EncryptionMode_Unencrypted = (uint8_t)0,
    ThyoneI_EncryptionMode_TxEncrypted = (uint8_t)1,   /* Transmit encrypted messages */
    ThyoneI_EncryptionMode_RxEncrypted = (uint8_t)2,   /* Receive only encrypted messages. Discard all other messages*/
    ThyoneI_EncryptionMode_RxTxEncrypted = (uint8_t)3, /* Transmit and receive encrypted messages. Discard all other messages*/
    ThyoneI_EncryptionMode_Invalid = (uint8_t)0xFF,
} ThyoneI_EncryptionMode_t;

/**
 * @brief Definition of the radio profile user setting
 */
typedef enum ThyoneI_Profile_t
{
    ThyoneI_Profile_125kbit = (uint8_t)0,
    ThyoneI_Profile_500kbit = (uint8_t)1,
    ThyoneI_Profile_1000kbit = (uint8_t)2,
    ThyoneI_Profile_2000kbit = (uint8_t)3,
    ThyoneI_Profile_Invalid = (uint8_t)0xFF,
} ThyoneI_Profile_t;

/**
 * @brief Definition of the address mode user setting
 */
typedef enum ThyoneI_AddressMode_t
{
    ThyoneI_AddressMode_Broadcast = (uint8_t)0,
    ThyoneI_AddressMode_Multicast = (uint8_t)1,
    ThyoneI_AddressMode_Unicast = (uint8_t)2,
} ThyoneI_AddressMode_t;

/**
 * @brief Initialize the ThyoneI interface for serial interface
 *
 * Caution: The parameter baudrate must match the configured UserSettings of the ThyoneI.
 *          The baudrate parameter must match to perform a successful FTDI communication.
 *          Updating this parameter during runtime may lead to communication errors.
 *
 * @param[in] uartP: Definition of the uart connected to the module
 * @param[in] pinoutP: Definition of the gpios connected to the module
 * @param[in] opMode: Operation mode
 * @param[in] RXcb: RX callback function
 *
 * @return True if initialization succeeded,
 *         false otherwise
 */
extern bool ThyoneI_Init(WE_UART_t* uartP, ThyoneI_Pins_t* pinoutP, ThyoneI_OperationMode_t opMode, void (*RXcb)(uint8_t*, uint16_t, uint32_t, int8_t));

/**
 * @brief Deinitialize the ThyoneI interface
 *
 * @return True if deinitialization succeeded,
 *         false otherwise
 */
extern bool ThyoneI_Deinit(void);

/**
 * @brief Reset the ThyoneI by pin
 *
 * @return True if reset succeeded,
 *         false otherwise
 */
extern bool ThyoneI_PinReset(void);

/**
 * @brief Reset the ThyoneI by command
 *
 * @return True if reset succeeded,
 *         false otherwise
 */
extern bool ThyoneI_Reset(void);

/**
 * @brief Put the ThyoneI into sleep mode
 *
 * @return True if succeeded,
 *         false otherwise
 */
extern bool ThyoneI_Sleep();

/**
 * @brief Wakeup the ThyoneI from sleep by pin
 *
 * @return True if wakeup succeeded,
 *         false otherwise
 */
extern bool ThyoneI_PinWakeup();

/**
 * @brief Returns the current level of the BUSY pin.
 *
 * @param[out] busyStateP: busy pin state
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_IsTransparentModeBusy(bool* busyStateP);

/**
 * @brief Transmit data as broadcast
 *
 * @param[in] payloadP: Pointer to the data to transmit
 * @param[in] length: Length of the data to transmit
 *
 * @return True if succeeded,
 *         false otherwise
 */
extern bool ThyoneI_TransmitBroadcast(uint8_t* payloadP, uint16_t length);

/**
 * @brief Transmit data as multicast
 *
 * @param[in] payloadP: Pointer to the data to transmit
 * @param[in] length: Length of the data to transmit
 *
 * @return True if succeeded,
 *         false otherwise
 */
extern bool ThyoneI_TransmitMulticast(uint8_t* payloadP, uint16_t length);

/**
 * @brief Transmit data as unicast
 *
 * @param[in] payloadP: Pointer to the data to transmit
 * @param[in] length: Length of the data to transmit
 *
 * @return True if succeeded,
 *         false otherwise
 */
extern bool ThyoneI_TransmitUnicast(uint8_t* payloadP, uint16_t length);

/**
 * @brief Transmitting the data via UART.
 *
 * @param[in] data: Pointer to the data.
 * @param[in] dataLength: Length of the data.
 *
 * @return True if transmission succeeded,
 *         false otherwise
 */
extern bool ThyoneI_Transparent_Transmit(const uint8_t* data, uint16_t dataLength);

/**
 * @brief Transmit data as multicast
 *
 * @param[in] groupID: groupID to multicast
 * @param[in] payloadP: Pointer to the data to transmit
 * @param[in] length: Length of the data to transmit
 *
 * @return True if succeeded,
 *         false otherwise
 */
extern bool ThyoneI_TransmitMulticastExtended(uint8_t groupID, uint8_t* payloadP, uint16_t length);

/**
 * @brief Transmit data as unicast
 *
 * @param[in] address: address to sent to
 * @param[in] payloadP: Pointer to the data to transmit
 * @param[in] length: Length of the data to transmit
 *
 * @return True if succeeded,
 *         false otherwise
 */
extern bool ThyoneI_TransmitUnicastExtended(uint32_t address, uint8_t* payloadP, uint16_t length);

/**
 * @brief Configure the local GPIO of the module
 *
 * @param[in] configP: Pointer to one or more pin configurations
 * @param[in] numberOfControls: Number of entries in configP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GPIOLocalSetConfig(ThyoneI_GPIOConfigBlock_t* configP, uint16_t numberOfControls);

/**
 * @brief Read the local GPIO configuration of the module
 *
 * @param[out] configP: Pointer to one or more pin configurations
 * @param[out] numberOfControlsP: Pointer to number of entries in configP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GPIOLocalGetConfig(ThyoneI_GPIOConfigBlock_t* configP, uint16_t* numberOfControlsP);

/**
 * @brief Set the output value of the local pin. Pin has to be configured first.
 * See ThyoneI_GPIOLocalWriteConfig
 *
 * @param[in] controlP: Pointer to one or more pin controls
 * @param[in] numberOfControls: Number of entries in controlP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GPIOLocalWrite(ThyoneI_GPIOControlBlock_t* controlP, uint16_t numberOfControls);

/**
 * @brief Read the input of the pin. Pin has to be configured first.
 * See ThyoneI_GPIOLocalWriteConfig
 *
 * @param[in] GPIOToReadP: One or more pins to read.
 * @param[in] amountGPIOToRead: amount of pins to read and therefore length of GPIOToRead
 * @param[out] controlP: Pointer to controlBlock
 * @param[out] numberOfControlsP: Pointer to number of entries in controlP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GPIOLocalRead(uint8_t* GPIOToReadP, uint8_t amountGPIOToRead, ThyoneI_GPIOControlBlock_t* controlP, uint16_t* numberOfControlsP);

/**
 * @brief Configure the remote GPIO of the module
 *
 * @param[in] destAddress: Destination address of the remote Thyone-I device
 * @param[in] configP: Pointer to one or more pin configurations
 * @param[in] numberOfControls: Number of entries in configP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GPIORemoteSetConfig(uint32_t destAddress, ThyoneI_GPIOConfigBlock_t* configP, uint16_t numberOfControls);

/**
 * @brief Read the remote GPIO configuration of the module
 *
 * @param[out] destAddress: Destination address of the remote Thyone-I device
 * @param[out] configP: Pointer to one or more pin configurations
 * @param[out] numberOfControlsP: Pointer to number of entries in configP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GPIORemoteGetConfig(uint32_t destAddress, ThyoneI_GPIOConfigBlock_t* configP, uint16_t* numberOfControlsP);

/**
 * @brief Set the output value of the remote pin. Pin has to be configured first.
 * See ThyoneI_GPIORemoteWriteConfig
 *
 * @param[in] destAddress: Destination address of the remote Thyone-I device
 * @param[in] controlP: Pointer to one or more pin controls
 * @param[in] numberOfControls: Number of entries in controlP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GPIORemoteWrite(uint32_t destAddress, ThyoneI_GPIOControlBlock_t* controlP, uint16_t numberOfControls);

/**
 * @brief Read the input of the pins. Pin has to be configured first.
 * See ThyoneI_GPIORemoteWriteConfig
 *
 * @param[in] destAddress: Destination address of the remote Thyone-I device
 * @param[in] GPIOToReadP: One or more pins to read.
 * @param[in] amountGPIOToRead: amount of pins to read and therefore length of GPIOToReadP
 * @param[out] controlP: Pointer to controlBlock
 * @param[out] numberOfControlsP: Pointer to number of entries in controlP array
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GPIORemoteRead(uint32_t destAddress, uint8_t* GPIOToReadP, uint8_t amountGPIOToRead, ThyoneI_GPIOControlBlock_t* controlP, uint16_t* numberOfControlsP);

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
extern bool ThyoneI_SetRam(ThyoneI_RuntimeSettings_t runtimeSetting, uint8_t* valueP, uint8_t length);

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
extern bool ThyoneI_GetRam(ThyoneI_RuntimeSettings_t runtimeSetting, uint8_t* responseP, uint16_t* lengthP);

/**
 * @brief Factory reset the module
 *
 * @return True if succeeded,
 *         false otherwise
 */
extern bool ThyoneI_FactoryReset();

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
extern bool ThyoneI_Set(ThyoneI_UserSettings_t userSetting, uint8_t* valueP, uint8_t length);

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
extern bool ThyoneI_CheckNSet(ThyoneI_UserSettings_t userSetting, uint8_t* valueP, uint8_t length);

/**
 * @brief Set the UART baudrate index
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] baudrate: UART baudrate index
 * @param[in] parity: UART parity
 * @param[in] flowcontrolEnable: true to enable UART flow control, false otherwise
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_SetBaudrateIndex(ThyoneI_BaudRateIndex_t baudrate, ThyoneI_UartParity_t parity, bool flowcontrolEnable);

/**
 * @brief Set the encryption mode
 *
 * @param[in] encryptionMode: encryptionMode
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_SetEncryptionMode(ThyoneI_EncryptionMode_t encryptionMode);

/**
 * @brief Set the rf profile
 *
 * @param[in] profile: rf profile
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_SetRfProfile(ThyoneI_Profile_t profile);

/**
 * @brief Set the RF channel
 *
 * @param[in] channel: Radio channel
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_SetRFChannel(uint8_t channel);

/**
 * @brief Set the RF channel on-the-fly without changing the user settings (volatile,
 * channel is reverted to the value stored in flash after a reset of the module).
 *
 * @param[in] channel: Radio channel
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_SetRFChannelRuntime(uint8_t channel);

/**
 * @brief Set the number of retries
 *
 * @param[in] numRetries: number of retries
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_SetNumRetries(uint8_t numRetries);

/**
 * @brief Set the tx power
 *
 * @param[in] txPower: transmit power
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_SetTXPower(ThyoneI_TXPower_t txPower);

/**
 * @brief Set the number of time slots
 *
 * @param[in] numSlots: number of time slots
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_SetRpNumSlots(uint8_t numSlots);

/**
 * @brief Set the source address
 *
 * @param[in] sourceAddress: source address
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_SetSourceAddress(uint32_t sourceAddress);

/**
 * @brief Set the destination address
 *
 * @param[in] destinationAddress: Destination address
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_SetDestinationAddress(uint32_t destinationAddress);

/**
 * @brief Set the group id
 *
 * @param[in] groupId: groupID
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_SetGroupID(uint8_t groupId);

/**
 * @brief Set the encryption key
 *
 * @param[in] keyP: Pointer to 16-byte key
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_SetEncryptionKey(uint8_t* keyP);

/**
 * @brief Set the time-to-live defining the maximum of hops if repeating
 *
 * @param[in] ttl: time-to-live value
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_SetTimeToLive(uint8_t ttl);

/**
 * @brief Set clear channel assessement mode
 *
 * @param[in] ccaMode: clear channel assessment mode
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_SetCCAMode(uint8_t ccaMode);

/**
 * @brief Set threshold for clear channel assessement
 *
 * @param[in] ccaThreshold: threshold for clear channel assessement
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_SetCCAThreshold(uint8_t ccaThreshold);

/**
 * @brief Set remote gpio config
 *
 * @param[in] remoteConfig: remote gpio config
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_SetGPIOBlockRemoteConfig(uint8_t remoteConfig);

/**
 * @brief Set module mode
 *
 * @param[in] moduleMode: operation mode of the module
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_SetModuleMode(ThyoneI_ModuleMode_t moduleMode);

/**
 * @brief Request the current user settings
 *
 * @param[in] userSetting: user setting to be requested
 * @param[out] responseP: Pointer of the memory to put the requested content
 * @param[out] lengthP: Length of the requested content
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_Get(ThyoneI_UserSettings_t userSetting, uint8_t* responseP, uint16_t* lengthP);

/**
 * @brief Request the 4 byte serial number
 *
 * @param[out] serialNumberP: Pointer to the 4 byte serial number
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GetSerialNumber(uint8_t* serialNumberP);

/**
 * @brief Request the 3 byte firmware version
 *
 * @param[out] versionP: Pointer to the 3 byte firmware version
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GetFWVersion(uint8_t* versionP);

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
extern bool ThyoneI_GetBaudrateIndex(ThyoneI_BaudRateIndex_t* baudrateP, ThyoneI_UartParity_t* parityP, bool* flowcontrolEnableP);

/**
 * @brief Get the encryption mode
 *
 * @param[out] encryptionModeP: Pointer to encryptionMode
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GetEncryptionMode(ThyoneI_EncryptionMode_t* encryptionModeP);

/**
 * @brief Get the rf profile
 *
 * *output:
 * @param[out] profileP: Pointer to rf profile
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GetRfProfile(ThyoneI_Profile_t* profileP);

/**
 * @brief Get the rf channel
 *
 * @param[out] channelP: Pointer to rf channel
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GetRFChannel(uint8_t* channelP);

/**
 * @brief Get the number of retries
 *
 * @param[out] numRetriesP: Pointer to number of retries
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GetNumRetries(uint8_t* numRetriesP);

/**
 * @brief Request the TX power
 *
 * @param[out] txpowerP: Pointer to the TX power
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GetTXPower(ThyoneI_TXPower_t* txpowerP);

/**
 * @brief Get the number of time slots
 *
 * @param[out] numSlotsP: Pointer of number of time slots
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GetRpNumSlots(uint8_t* numSlotsP);

/**
 * @brief Get the source address
 *
 * @param[out] sourceAddressP: Pointer to source address
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GetSourceAddress(uint32_t* sourceAddressP);

/**
 * @brief Get the destination address
 *
 * @param[out] destinationAddressP: Pointer to destination address
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GetDestinationAddress(uint32_t* destinationAddressP);

/**
 * @brief Get the group id
 *
 * @param[out] groupIdP: Pointer to groupID
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GetGroupID(uint8_t* groupIdP);

/**
 * @brief Get the time-to-live defining the maximum of hops if repeating
 *
 * @param[out] ttlP: Pointer to time-to-live
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GetTimeToLive(uint8_t* ttlP);

/**
 * @brief Get clear channel assessment mode
 *
 * @param[out] ccaModeP: Pointer to clear channel assessment mode
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GetCCAMode(uint8_t* ccaModeP);

/**
 * @brief Get threshold for clear channel assessment
 *
 * @param[out] ccaThresholdP: threshold for clear channel assessement
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GetCCAThreshold(uint8_t* ccaThresholdP);

/**
 * @brief Get remote gpio config
 *
 * @param[out] remoteConfigP: Pointer to remote gpio config
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GetGPIOBlockRemoteConfig(uint8_t* remoteConfigP);

/**
 * @brief Get module mode
 *
 * @param[out] moduleModeP: operation mode of the module
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GetModuleMode(ThyoneI_ModuleMode_t* moduleModeP);

/**
 * @brief Request the module state
 *
 * @param[out] stateP: Current state of the module
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool ThyoneI_GetState(ThyoneI_States_t* stateP);

#endif // THYONEI_H_INCLUDED

#ifdef __cplusplus
}
#endif
