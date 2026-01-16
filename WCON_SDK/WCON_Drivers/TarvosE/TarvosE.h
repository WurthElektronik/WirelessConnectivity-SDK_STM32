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
 * @file TarvosE.h
 * @brief Tarvos-E driver header file.
 */

#include <global/global_types.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef TARVOS_E_H_INCLUDED
#define TARVOS_E_H_INCLUDED

#define TARVOSE_BROADCASTADDRESS 0xFF
#define TARVOSE_MAX_USERSETTING_LENGTH 4

#define TARVOSE_DEFAULT_BAUDRATE (uint32_t)115200

typedef struct TarvosE_Pins_t
{
    WE_Pin_t TarvosE_Pin_Reset;
    WE_Pin_t TarvosE_Pin_SleepWakeUp;
    WE_Pin_t TarvosE_Pin_Boot;
    WE_Pin_t TarvosE_Pin_Mode;

} TarvosE_Pins_t;

typedef enum TarvosE_AddressMode_t
{
    TarvosE_AddressMode_0 = 0x00,
    TarvosE_AddressMode_1,
    TarvosE_AddressMode_2,
    TarvosE_AddressMode_3,
} TarvosE_AddressMode_t;

typedef enum TarvosE_UserSettings_t
{
    TarvosE_CMD_SETGET_OPTION_UARTBAUDRATE = 0x00,
    TarvosE_CMD_SETGET_OPTION_DEFAULTRFPROFILE = 0x01,
    TarvosE_CMD_SETGET_OPTION_DEFAULTRFTXPOWER = 0x02,
    TarvosE_CMD_SETGET_OPTION_DEFAULTRFCHANNEL = 0x03,
    TarvosE_CMD_SETGET_OPTION_DEFAULTADDRESSMODE = 0x04,
    TarvosE_CMD_SETGET_OPTION_NUMRETRYS = 0x06,
    TarvosE_CMD_SETGET_OPTION_DEFAULTDESTNETID = 0x07,
    TarvosE_CMD_SETGET_OPTION_DEFAULTDESTADDR = 0x08,
    TarvosE_CMD_SETGET_OPTION_SOURCENETID = 0x0A,
    TarvosE_CMD_SETGET_OPTION_SOURCEADDR = 0x0B,
    TarvosE_CMD_SETGET_OPTION_CFG_FLAGS = 0x0F,
    TarvosE_CMD_SETGET_OPTION_RP_FLAGS = 0x10,
    TarvosE_CMD_SETGET_OPTION_RP_NUMSLOTS = 0x11,
    TarvosE_CMD_SETGET_OPTION_LBT_OBSERVATION_PERIOD = 0x1A,
    TarvosE_CMD_SETGET_OPTION_LBT_THRESHOLD = 0x1B,
    TarvosE_CMD_SETGET_OPTION_FACTORYSETTINGS = 0x20,
    TarvosE_CMD_SETGET_OPTION_FWVERSION = 0x21,
} TarvosE_UserSettings_t;

/**
 * @brief Struct representing a user setting.
 */
typedef struct TarvosE_Configuration_t
{
    TarvosE_UserSettings_t usersetting;            /**< user setting */
    uint8_t value[TARVOSE_MAX_USERSETTING_LENGTH]; /**< value */
    uint8_t value_length;                          /**< length of the value */
} TarvosE_Configuration_t;

/**
 * @brief Initialize the TarvosE for serial interface.
 *
 * Caution: The parameters baudrate and addrmode must match the configured UserSettings of the TarvosE.
 *          The baudrate parameter must match to perform a successful UART communication.
 *          Updating this parameter during runtime may lead to communication errors.
 *          The addrmode must match when RF packet transmission or reception is performed.
 *          This parameter can be updated to the correct value (used in TarvosE_Init function) as soon as no RF packet transmission or reception was performed.
 *
 * @param[in] uartP: Definition of the uart connected to the module
 * @param[in] pinoutP: Definition of the gpios connected to the module
 * @param[in] addrmode: address mode of the TarvosE
 * @param[in] RXcb: RX callback function
 *
 * @return True if initialization succeeded,
 *         false otherwise
 */
extern bool TarvosE_Init(WE_UART_t* uartP, TarvosE_Pins_t* pinoutP, TarvosE_AddressMode_t addrmode, void (*RXcb)(uint8_t*, uint8_t, uint8_t, uint8_t, uint8_t, int8_t));

/**
* @brief Deinitialize the TarvosE interface
 *
 * @return True if deinitialization succeeded,
 *         false otherwise
*/
extern bool TarvosE_Deinit(void);

/**
* @brief Reset the TarvosE by pin
 *
 * @return True if reset succeeded,
 *         false otherwise
*/
extern bool TarvosE_PinReset(void);

/**
* @brief Reset the TarvosE by command
 *
 * @return True if reset succeeded,
 *         false otherwise
*/
extern bool TarvosE_Reset(void);

/**
* @brief Transmit data
 *
 * @param[in] payload: Pointer to the data
 * @param[in] length: Length of the data
 * @param[in] channel: channel to be used
 * @param[in] dest_network_id: Destination network ID
 * @param[in] dest_address_lsb: Destination address lsb
 * @param[in] dest_address_msb: Destination address msb
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_Transmit_Extended(uint8_t* payload, uint8_t length, uint8_t channel, uint8_t dest_network_id, uint8_t dest_address_lsb, uint8_t dest_address_msb);

/**
* @brief Transmit data using the configured settings
 *
 * @param[in] payload: Pointer to the data
 * @param[in] length: Length of the data
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_Transmit(uint8_t* payload, uint8_t length);

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
extern bool TarvosE_Transparent_Transmit(const uint8_t* data, uint16_t dataLength);

/**
* @brief Switch the module to shutdown mode
 *
 * @return True if switching succeeded,
 *         false otherwise
*/
extern bool TarvosE_Shutdown(void);

/**
* @brief Switch the module to standby mode
 *
 * @return True if switching succeeded,
 *         false otherwise
*/
extern bool TarvosE_Standby(void);

/**
* @brief Wakeup the TarvosE from standby or shutdown mode by pin
 *
 * @param[in] standby: true, if wake-up from standby mode is made
 *                              false, if wake-up from shutdown mode is made
 *
 * @return True if wakeup succeeded,
 *         false otherwise
*/
extern bool TarvosE_PinWakeup(bool standby);

/* Read the non-volatile settings */

/**
* @brief Request the current TarvosE settings
 *
 * @param[in] us: user setting to be requested
 * @param[out] response: Pointer of the memory to put the request content
 * @param[out] response_length: Length of the request content
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_Get(TarvosE_UserSettings_t us, uint8_t* response, uint8_t* response_length);

/**
* @brief Request the 3 byte firmware version
 *
 * @param[out] fw: Pointer to the 3 byte firmware version
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_GetFirmwareVersion(uint8_t* fw);

/**
* @brief Request the 4 byte serial number
 *
 * @param[out] sn: Pointer to the 4 byte serial number
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_GetSerialNumber(uint8_t* sn);

/**
* @brief Request the default TX power
 *
 * @param[out] txpower: Pointer to the TXpower
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_GetDefaultTXPower(uint8_t* txpower);

/**
* @brief Get the default source address
 *
 * @param[out] srcaddr_lsb: LSB of the source address
 * @param[out] srcaddr_msb: MSB of the source address
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_GetSourceAddr(uint8_t* srcaddr_lsb, uint8_t* srcaddr_msb);

/**
* @brief Set the default source net id
 *
 * @param[out] srcnetid: source net id
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_GetSourceNetID(uint8_t* srcnetid);

/**
* @brief Get the default destination address
 *
 * @param[out] destaddr_lsb: LSB of the destination address
 * @param[out] destaddr_msb: MSB of the destination address
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_GetDefaultDestAddr(uint8_t* destaddr_lsb, uint8_t* destaddr_msb);

/**
* @brief Get the default destination address
 *
 * @param[out] destnetid: Destination net id
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_GetDefaultDestNetID(uint8_t* destnetid);

/**
* @brief Get the default RF channel
 *
 * @param[out] channel: channel
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_GetDefaultRFChannel(uint8_t* channel);

/**
* @brief Get the default RF profile
 *
 * @param[out] profile: RF profile
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_GetDefaultRFProfile(uint8_t* profile);

/**
* @brief Get the LBT observation period
 *
 * @param[out] period: LBT observation period in ms
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_GetLBTObservationPeriod(uint8_t* period);

/**
* @brief Get the LBT threshold value
 *
 * @param[out] threshold: LBT threshold value in dBm
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_GetLBTThreshold(int8_t* threshold);

/* Functions that write the non-volatile settings in the flash: After modification of any non-volatile setting,
 * the module must be reset such that the update takes effect.
 * IMPORTANT: Use only in rare cases, since flash can be written to only a limited number of times.
 */

/**
* @brief Factory reset the TarvosE
 *
 * Note: use only in rare cases, since flash can be updated only a limited number of times
 *
 * @return True if factory reset succeeded,
 *         false otherwise
*/
extern bool TarvosE_FactoryReset(void);

/**
* @brief Set a special TarvosE setting
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] us: user setting to be updated
 * @param[in] value: Pointer to the new settings value
 * @param[in] length: Length of the value
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_Set(TarvosE_UserSettings_t us, uint8_t* value, uint8_t length);

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
extern bool TarvosE_CheckNSet(TarvosE_UserSettings_t userSetting, uint8_t* valueP, uint8_t length);

/**
* @brief Configure the TarvosE
 *
 * @param[in] config: Pointer to the configuration struct
 * @param[in] config_length: Length of the configuration struct
 * @param[in] factory_reset: apply a factory reset before or not
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_Configure(TarvosE_Configuration_t* config, uint8_t config_length, bool factory_reset);

/**
* @brief Set the default TX power
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use TarvosE_SetVolatile_TXPower for frequent adaption of the TX power.
 *
 * @param[in] txpower: TXpower
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_SetDefaultTXPower(uint8_t txpower);

/**
* @brief Set the default source address
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] srcaddr_lsb: LSB of the source address
 * @param[in] srcaddr_msb: MSB of the source address
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_SetSourceAddr(uint8_t srcaddr_lsb, uint8_t srcaddr_msb);

/**
* @brief Set the default source net id
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] srcnetid: source net id
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_SetSourceNetID(uint8_t srcnetid);

/**
* @brief Set the default destination address
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use TarvosE_SetVolatile_DestAddr for frequent adaption of the destination address.
 *
 * @param[in] destaddr_lsb: LSB of the destination address
 * @param[in] destaddr_msb: MSB of the destination address
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_SetDefaultDestAddr(uint8_t destaddr_lsb, uint8_t destaddr_msb);

/**
* @brief Set the default destination address
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use TarvosE_SetVolatile_DestNetID for frequent adaption of the destination net id.
 *
 * @param[in] destnetid: Destination net id
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_SetDefaultDestNetID(uint8_t destnetid);

/**
* @brief Set the default RF channel
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 * Note: Use TarvosE_SetVolatile_Channel for frequent adaption of the channel.
 *
 * @param[in] channel: channel
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_SetDefaultRFChannel(uint8_t channel);

/**
* @brief Set the default RF profile
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] profile: RF profile
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_SetDefaultRFProfile(uint8_t profile);

/**
* @brief Set the LBT observation period
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] period: LBT observation period in ms
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_SetLBTObservationPeriod(uint8_t period);

/**
* @brief Set the LBT threshold
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @param[in] threshold: LBT threshold in dBm
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_SetLBTThreshold(int8_t threshold);

/**
* @brief Enables the Sniffer mode
 *
 * Note: Reset the module after the adaption of the setting so that it can take effect.
 * Note: Use this function only in rare case, since flash can be updated only a limited number of times.
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_EnableSnifferMode();

/* write volatile settings into RAM, these settings are lost after a reset */

/**
* @brief Set the volatile destination address
 *
 * @param[in] destaddr_lsb: Lsb of the new destination address value
 * @param[in] destaddr_msb: msb of the new destination address value
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_SetVolatile_DestAddr(uint8_t destaddr_lsb, uint8_t destaddr_msb);

/**
* @brief Set the volatile destination net ID
 *
 * @param[in] destnetid: new destination net ID
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_SetVolatile_DestNetID(uint8_t destnetid);

/**
* @brief Set the volatile TX power
 *
 * @param[in] power: new TX power value
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_SetVolatile_TXPower(uint8_t power);

/**
* @brief Set the volatile channel
 *
 * @param[in] channel: new channel value
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_SetVolatile_Channel(uint8_t channel);

/**
* @brief Use the ping test command
 *
 * Note: Do not use this command. Just used for internal purposes!
 *
 * @return True if request succeeded,
 *         false otherwise
*/
extern bool TarvosE_Ping();

#endif // TARVOS_E_H_INCLUDED

#ifdef __cplusplus
}
#endif
