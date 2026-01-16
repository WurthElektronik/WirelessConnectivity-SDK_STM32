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
 * @brief Metis driver header file.
 */

#include <global/global_types.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * This driver supports the following AMBER modules:
 * AMB8425-M
 * TarvosI-M
 * TarvosIPlug-M
 * TarvosII-M
 * TarvosIIPlug-M
 * Titania-M
 * AMB3665-M
 */

#ifndef METIS_H_INCLUDED
#define METIS_H_INCLUDED

/**
 * Max. length of user settings (number of bytes).
 */
#define METIS_MAX_USERSETTING_LENGTH 4

/**
 * Default UART baudrate of Metis module
 */
#define METIS_DEFAULT_BAUDRATE ((uint32_t)9600)

typedef struct Metis_Pins_t
{
    WE_Pin_t Metis_Pin_Reset;
} Metis_Pins_t;

/**
 * @brief Enumeration for wM-Bus mode.
 *
 * By setting the mode preselect all RF parameters are set accordingly
 * and no further adaption is necessary.
 * There are different sets of modes for 868 MHz and 169 MHz respectively.
 */
typedef enum Metis_Mode_Preselect_t
{
    Metis_Mode_Preselect_868_S1 = 0x01,
    Metis_Mode_Preselect_868_S1_m = 0x02,
    Metis_Mode_Preselect_868_S2 = 0x03,
    Metis_Mode_Preselect_868_T1_meter = 0x05,
    Metis_Mode_Preselect_868_T1_other = 0x06,
    Metis_Mode_Preselect_868_T2_meter = 0x07,
    Metis_Mode_Preselect_868_T2_other = 0x08,
    Metis_Mode_Preselect_868_C2_T2_other = 0x09,
    Metis_Mode_Preselect_868_C1_meter = 0x0C,
    Metis_Mode_Preselect_868_C2_meter = 0x0D,
    Metis_Mode_Preselect_868_C2_other = 0x0E,

    Metis_Mode_Preselect_169_N1a = 0x01,
    Metis_Mode_Preselect_169_N2a = 0x02,
    Metis_Mode_Preselect_169_N1b = 0x03,
    Metis_Mode_Preselect_169_N2b = 0x04,
    Metis_Mode_Preselect_169_N1c = 0x05,
    Metis_Mode_Preselect_169_N2c = 0x06,
    Metis_Mode_Preselect_169_N1d = 0x07,
    Metis_Mode_Preselect_169_N2d = 0x08,
    Metis_Mode_Preselect_169_N1e = 0x09,
    Metis_Mode_Preselect_169_N2e = 0x0A,
    Metis_Mode_Preselect_169_N1f = 0x0B,
    Metis_Mode_Preselect_169_N2f = 0x0C,
} Metis_Mode_Preselect_t;

/**
 * @brief wM-Bus frequency.
 */
typedef enum Metis_Frequency_t
{
    Metis_Frequency_169,
    Metis_Frequency_868,
} Metis_Frequency_t;

/**
 * @brief Enumeration for baud rates supported by the module.
 *
 * Used to change the baud rate of the module via CMD_SetUartspeed
 * which expects predefined values as set here.
 */
typedef enum Metis_UartBaudrate_t
{
    Metis_UartBaudrate_1200 = 0x00,
    Metis_UartBaudrate_2400 = 0x01,
    Metis_UartBaudrate_4800 = 0x02,
    Metis_UartBaudrate_9600 = 0x03,
    Metis_UartBaudrate_19200 = 0x04,
    Metis_UartBaudrate_38400 = 0x05,
    Metis_UartBaudrate_56000 = 0x06,
    Metis_UartBaudrate_115200 = 0x07,
} Metis_UartBaudrate_t;

/**
 * @brief User settings memory locations.
 */
typedef enum Metis_UserSettings_t
{
    Metis_USERSETTING_MEMPOSITION_UART_CMD_OUT_ENABLE = 0x05,
    Metis_USERSETTING_MEMPOSITION_APP_AES_ENABLE = 0X0B,
    Metis_USERSETTING_MEMPOSITION_DEFAULTRFTXPOWER = 0x3D,
    Metis_USERSETTING_MEMPOSITION_RSSI_ENABLE = 0x45,
    Metis_USERSETTING_MEMPOSITION_MODE_PRESELECT = 0X46,
    Metis_USERSETTING_MEMPOSITION_CFG_FLAGS = 0x50,
} Metis_UserSettings_t;

/**
 * @brief Struct representing a user setting with its value.
 */
typedef struct Metis_Configuration_t
{
    Metis_UserSettings_t usersetting;            /**< User setting */
    uint8_t value[METIS_MAX_USERSETTING_LENGTH]; /**< Value */
    uint8_t value_length;                        /**< Length of value */
} Metis_Configuration_t;

/* Callback for data received (CMD_DATA_IND).
 * Notice that the first byte of the frame data contains the length of the
 * following payload (i.e. the L field value). */
typedef void (*Metis_RxCallback_t)(uint8_t* frameData, uint8_t frameLength, int8_t rssi);

/**
 * @brief Initialize the AMB module for serial interface.
 *
 * Caution: The baudrate parameter must match the module's baudrate to perform a successful communication.
 *          Updating this parameter during runtime may lead to communication errors.
 *          The mode parameter must match the other participant of the RF communication.
 *          Check manual of the wM-Bus AMB modules for the suitable modes.
 *
 * @param[in] uartP: Definition of the uart connected to the module
 * @param[in] pinoutP: Definition of the gpios connected to the module
 * @param[in] freq: Frequency used by the AMBER module(AMB8xxx-M uses 868Mhz, AMB36xx-M uses 169MHz)
 * @param[in] mode: wM-Bus mode preselect of the AMBER module
 * @param[in] enable_rssi: Enable rssi in data reception
 * @param[in] RXcb: RX callback function
 *
 * @return True if initialization succeeded,
 *         false otherwise
 */
extern bool Metis_Init(WE_UART_t* uartP, Metis_Pins_t* pinoutP, Metis_Frequency_t freq, Metis_Mode_Preselect_t mode, bool enable_rssi, Metis_RxCallback_t RXcb);

/**
 * @brief Deinitialize the Metis interface.
 *
 * @return True if deinitialization succeeded,
 *         false otherwise
 */
extern bool Metis_Deinit(void);

/**
 * @brief Reset the Metis by pin
 *
 * @return True if reset succeeded
 *         false otherwise
 */
extern bool Metis_PinReset(void);

/**
 * @brief Reset the Metis by command
 *
 * @return True if reset succeeded,
 *         false otherwise
 */
extern bool Metis_Reset(void);

/**
 * @brief Transmit data using the configured settings
 *
 * @param[in] payloadP: Pointer to the data
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool Metis_Transmit(uint8_t* payloadP);

/**
 * @brief Transmit data in a transparent way
 *
 * @param[in] dataP: Pointer to the data
 * @param[in] dataLength: Length to the data
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool Metis_Transparent_Transmit(const uint8_t* dataP, uint16_t dataLength);

/**
 * @brief Request the current Metis settings.
 *
 * @param[in] us: User setting to be requested
 * @param[out] response: Pointer of the memory to put the request content
 * @param[out] response_length: Length of the request content
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool Metis_Get(Metis_UserSettings_t us, uint8_t* response, uint8_t* response_length);

/**
 * @brief Request multiple of the current Metis settings
 *
 * @param[in] startAddress: First usersetting to be read
 * @param[in] lengthToRead: Length of memory to be read
 * @param[out] response: Pointer of the memory to put the request content
 * @param[out] response_length: Length of the request content
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool Metis_GetMultiple(uint8_t startAddress, uint8_t lengthToRead, uint8_t* response, uint8_t* response_length);

/**
 * @brief Request the 3 byte firmware version.
 *
 * @param[out] fwP: Pointer to the 3 byte firmware version
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool Metis_GetFirmwareVersion(uint8_t* fwP);

/**
 * @brief Request the 4 byte serial number
 *
 * @param[out] snP: Pointer to the 4 byte serial number
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool Metis_GetSerialNumber(uint8_t* snP);

/**
 * @brief Request the default TX power.
 *
 * @param[out] txpowerP: Pointer to the TXpower
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool Metis_GetDefaultTXPower(int8_t* txpowerP);

/**
 * @brief Get the Uart Out Enable byte
 *
 * @param[out] uartEnableP: Pointer to uartEnable
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool Metis_GetUartOutEnable(uint8_t* uartEnableP);

/**
 * @brief Get the RSSI Enable byte
 *
 * @param[out] rssiEnableP: Pointer to rssiEnable
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool Metis_GetRSSIEnable(uint8_t* rssiEnableP);

/**
 * @brief Get the AES Enable byte
 *
 * @param[out] aesEnableP: Pointer to aes enable
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool Metis_GetAESEnable(uint8_t* aesEnableP);

/**
 * @brief Get the M-Bus mode preselect byte
 *
 * @param[out] modePreselectP: Pointer to mode Preselect
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool Metis_GetModePreselect(uint8_t* modePreselectP);

/**
 * @brief Factory reset the Metis
 *
 * Note: Use only in rare cases, since flash can be updated only a limited number of times.
 *       The factory reset must be followed by a reset such that the changes become effective.
 *
 * @return True if factory reset succeeded,
 *         false otherwise
 */
extern bool Metis_FactoryReset(void);

/**
 * @brief Sets the baud rate of the module
 *
 * Note: Use only in rare cases, since flash can be updated only a limited number of times.
 *
 * @param[in] baudrate: Baud rate
 *
 * @return True if setting baud rate succeeded,
 *         false otherwise
 */
extern bool Metis_SetUartSpeed(Metis_UartBaudrate_t baudrate);

/**
 * @brief Set a special Metis setting
 *
 * Note: Reset the module after the adaption of the setting such that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] us: User setting to be updated
 * @param[in] valueP: Pointer to the new settings value
 * @param[in] length: Length of the value
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool Metis_Set(Metis_UserSettings_t us, uint8_t* valueP, uint8_t length);

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
extern bool Metis_CheckNSet(Metis_UserSettings_t userSetting, uint8_t* valueP, uint8_t length);

/**
 * @brief Configure the Metis
 *
 * @param[in] configP: Pointer to the configuration struct
 * @param[in] config_length: Length of the configuration struct
 * @param[in] factory_reset: Apply a factory reset before or not
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool Metis_Configure(Metis_Configuration_t* configP, uint8_t config_length, bool factory_reset);

/**
 * @brief Set the default TX power
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use Metis_SetVolatile_TXPower for frequent adaption of the TX power.
 *
 * @param[in] txpower: TXpower
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool Metis_SetDefaultTXPower(int8_t txpower);

/**
 * @brief Set the Uart Out Enable
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times
 *
 * @param[in] uartEnable: Uart enable
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool Metis_SetUartOutEnable(uint8_t uartEnable);

/**
 * @brief Set the RSSI Enable byte
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] rssiEnable: Rssi enable
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool Metis_SetRSSIEnable(uint8_t rssiEnable);

/**
 * @brief Set the AES Enable byte
 *
 * Note: Functions for AES are not implemented. Can only be used for disabling AES.
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] aesEnable: AES enable
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool Metis_SetAESEnable(uint8_t aesEnable);

/**
 * @brief Set the default M-Bus mode preselect
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use Metis_SetVolatile_ModePreselect for frequent adaption of the channel.
 *
 * @param[in] modePreselect: M-Bus mode preselect

 * @return True if request succeeded,
 *         false otherwise
 */
extern bool Metis_SetModePreselect(Metis_Mode_Preselect_t modePreselect);

/**
 * @brief Set the volatile mode preselect
 *
 * @param[in] modePreselect: new mode preselect value
 *
 * @return True if request succeeded,
 *         false otherwise
 */
extern bool Metis_SetVolatile_ModePreselect(Metis_Mode_Preselect_t modePreselect);

#endif // METIS_H_INCLUDED
#ifdef __cplusplus
}
#endif
