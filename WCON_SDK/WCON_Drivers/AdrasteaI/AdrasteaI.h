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
 * @file AdrasteaI.h
 * @brief Adrastea driver header file.
 */

#ifndef ADRASTEAI_H_INCLUDED
#define ADRASTEAI_H_INCLUDED

#include <global/global_types.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Max recommended payload size is 1460 bytes.
 */
#define ADRASTEAI_MAX_PAYLOAD_SIZE 1460

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief AT command confirmation status.
 */
typedef enum AdrasteaI_CNFStatus_t
{
    AdrasteaI_CNFStatus_Success, /**< Success Status */
    AdrasteaI_CNFStatus_Failed,  /**< Failed Status */
    AdrasteaI_CNFStatus_Invalid, /**< Invalid Status */
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_CNFStatus_NumberOfValues
    /** @endcond */
} AdrasteaI_CNFStatus_t;

/**
 * @brief Timeout categories (for responses to AT commands).
 * @see AdrasteaI_SetTimeout(), AdrasteaI_GetTimeout()
 */
typedef enum AdrasteaI_Timeout_t
{
    AdrasteaI_Timeout_General,      /**< General timeout */
    AdrasteaI_Timeout_Device,       /**< Device timeout */
    AdrasteaI_Timeout_GNSS,         /**< GNSS timeout */
    AdrasteaI_Timeout_HTTP,         /**< HTTP timeout */
    AdrasteaI_Timeout_MQTT,         /**< MQTT timeout */
    AdrasteaI_Timeout_NetService,   /**< Net Service timeout */
    AdrasteaI_Timeout_PacketDomain, /**< Packet Domain timeout */
    AdrasteaI_Timeout_Proprietary,  /**< Proprietery timeout */
    AdrasteaI_Timeout_SIM,          /**< SIM timeout */
    AdrasteaI_Timeout_SMS,          /**< SMS timeout */
    AdrasteaI_Timeout_Socket,       /**< Socket timeout */
    AdrasteaI_Timeout_Power,        /**< Power timeout */
    /** @cond DOXYGEN_IGNORE */
    AdrasteaI_Timeout_NumberOfValues
    /** @endcond */
} AdrasteaI_Timeout_t;

/**
 * @brief Current ATMode of the module
 */
typedef enum AdrasteaI_ATMode_t
{
    AdrasteaI_ATMode_Off,   /**< no AT mode */
    AdrasteaI_ATMode_Map,   /**< AT mode on but not fully booted */
    AdrasteaI_ATMode_Ready, /**< AT mode on and ready */
} AdrasteaI_ATMode_t;

/**
 * @brief Pins used by this driver.
 */
typedef struct AdrasteaI_Pins_t
{
    WE_Pin_t AdrasteaI_Pin_Reset;  /**< Reset pin */
    WE_Pin_t AdrasteaI_Pin_WakeUp; /**< Wakeup pin */
} AdrasteaI_Pins_t;

/**
 * @brief Adrastea event callback.
 *
 * @param[in] event_text: Text of the event
 */
typedef void (*AdrasteaI_EventCallback_t)(char* event_text);

/** @cond DOXYGEN_INTERNAL */
extern uint8_t AdrasteaI_optionalParamsDelimCount;
/** @endcond */

/**
 * @brief Initializes the serial communication with the module
 *
 * @param[in] uartP: Definition of the uart connected to the module
 * @param[in] pinoutP: Definition of the gpios connected to the module
 * @param[in] eventCallback: Function pointer to event handler (optional)

 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_Init(WE_UART_t* uartP, AdrasteaI_Pins_t* pinoutP, AdrasteaI_EventCallback_t eventCallback);

/**
 * @brief Deinitializes the serial communication with the module.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_Deinit(void);

/**
 * @brief Performs a reset of the module using the reset pin.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_PinReset(void);

/**
 * @brief Wakes the module up from power save mode using the wake up pin.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_PinWakeUp(void);

/**
 * @brief Sends the supplied AT command to the module
 *
 * @param[in] data: AT command to send. Note that the command has to end with "\r\n\0".
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_SendRequest(char* data);

/**
 * @brief Waits for the response from the module after a request.
 *
 * @param[in] maxTimeMs: Maximum wait time in milliseconds
 * @param[in] expectedStatus: Status to wait for
 * @param[out] pOutResponse: Received response text (if any) will be written to this buffer (optional)
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_WaitForConfirm(uint32_t maxTimeMs, AdrasteaI_CNFStatus_t expectedStatus, char* pOutResponse);

/**
 * @brief Set timing parameters used by the DaphnisI driver.
 *
 * @note WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @param[in] waitTimeStepMicroseconds: Time step (microseconds) when waiting for responses from DaphnisI.
 * @param[in] minCommandIntervalMicroseconds: Minimum interval (microseconds) between subsequent commands sent to DaphnisI.
 *
 * @return True if successful, false otherwise
 */
extern bool AdrasteaI_SetTimingParameters(uint32_t waitTimeStepMicroseconds, uint32_t minCommandIntervalMicroseconds);

/**
 * @brief Sets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type: Timeout (i.e. command) type
 * @param[in] timeout: Timeout in milliseconds
 */
extern void AdrasteaI_SetTimeout(AdrasteaI_Timeout_t type, uint32_t timeout);

/**
 * @brief Gets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type: Timeout (i.e. command) type
 *
 * @return Timeout in milliseconds
 */
extern uint32_t AdrasteaI_GetTimeout(AdrasteaI_Timeout_t type);

/**
 * @brief Sends raw data to Adrastea via UART.
 *
 * This function sends data immediately without any processing and is used
 * internally for sending AT commands to Adrastea.
 *
 * @param[in] data: Pointer to data buffer (data to be sent)
 * @param[in] dataLength: Number of bytes to be sent
 */
extern bool AdrasteaI_Transparent_Transmit(const char* data, uint16_t dataLength);

/**
 * @brief Sets EOL character(s) used for interpreting responses from Adrastea.
 *
 * @param[in] eol1: First EOL character
 * @param[in] eol2: Second EOL character (is only used if twoEolCharacters is true)
 * @param[in] twoEolCharacters: Controls whether the two EOL characters eol1 and eol2 (true) or only eol1 (false) is used
 */
extern void AdrasteaI_SetEolCharacters(uint8_t eol1, uint8_t eol2, bool twoEolCharacters);

/**
 * @brief Get the ATmode of the module
 *
 * @return ATmode.
 */
extern AdrasteaI_ATMode_t AdrasteaI_CheckATMode();

#ifdef __cplusplus
}
#endif

#endif // ADRASTEAI_H_INCLUDED
