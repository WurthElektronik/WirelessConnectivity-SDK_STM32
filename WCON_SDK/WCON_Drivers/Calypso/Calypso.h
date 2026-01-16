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
 * @file Calypso.h
 * @brief Calypso driver header file.
 */

#ifndef CALYPSO_H_INCLUDED
#define CALYPSO_H_INCLUDED
#include <global/global_types.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <utils/base64.h>

/**
 * @brief Max recommended payload size is 1460 bytes.
 */
#define CALYPSO_MAX_PAYLOAD_SIZE 1460

/**
 * @brief Default receive buffer size (used when receiving data e.g. via sockets or HTTP requests).
 */
#define CALYPSO_RECEIVE_BUFFER_SIZE 2048

/**
 * @brief Max. length of sent commands and responses from Calypso.
 */
#define CALYPSO_LINE_MAX_SIZE 2048

/**
 * @brief Max. length of IP address strings.
 */
#define CALYPSO_MAX_IP_ADDRESS_LENGTH 44

/**
 * @brief Max. length of host name strings (e.g. URLs or IP addresses).
 */
#define CALYPSO_MAX_HOST_NAME_LENGTH 128

/**
 * @brief Max. length of response text (size of buffer storing responses received from Calypso).
 * @see Calypso_currentResponseText
 */
#define CALYPSO_MAX_RESPONSE_TEXT_LENGTH CALYPSO_LINE_MAX_SIZE

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief AT command confirmation status.
 */
typedef enum Calypso_CNFStatus_t
{
    Calypso_CNFStatus_Success,
    Calypso_CNFStatus_Failed,
    Calypso_CNFStatus_Invalid,
    /** @cond DOXYGEN_IGNORE */
    Calypso_CNFStatus_NumberOfValues
    /** @endcond */
} Calypso_CNFStatus_t;

/**
 * @brief Data format used for transferred data.
 */
typedef enum Calypso_DataFormat_t
{
    Calypso_DataFormat_Binary,
    Calypso_DataFormat_Base64,
    /** @cond DOXYGEN_IGNORE */
    Calypso_DataFormat_NumberOfValues,
    /** @endcond */
} Calypso_DataFormat_t;

/**
 * @brief Timeout categories (for responses to AT commands).
 * @see Calypso_SetTimeout(), Calypso_GetTimeout()
 */
typedef enum Calypso_Timeout_t
{
    Calypso_Timeout_General,
    Calypso_Timeout_FactoryReset,
    Calypso_Timeout_WlanAddProfile,
    Calypso_Timeout_WlanScan,
    Calypso_Timeout_NetAppUpdateTime,
    Calypso_Timeout_NetAppHostLookUp,
    Calypso_Timeout_HttpConnect,
    Calypso_Timeout_HttpRequest,
    Calypso_Timeout_FileIO,
    Calypso_Timeout_GPIO,
    /** @cond DOXYGEN_IGNORE */
    Calypso_Timeout_NumberOfValues
    /** @endcond */
} Calypso_Timeout_t;

/**
 * @brief Application modes. Used with Calypso_SetApplicationModePins().
 */
typedef enum Calypso_ApplicationMode_t
{
    Calypso_ApplicationMode_ATCommandMode = 0,
    Calypso_ApplicationMode_OTAUpdate = 1,
    Calypso_ApplicationMode_Provisioning = 2,
    Calypso_ApplicationMode_TransparentMode = 3
} Calypso_ApplicationMode_t;

/**
 * @brief Pins used by this driver.
 */
typedef struct Calypso_Pins_t
{
    WE_Pin_t Calypso_Pin_Reset;
    WE_Pin_t Calypso_Pin_WakeUp;
    WE_Pin_t Calypso_Pin_Boot;
    WE_Pin_t Calypso_Pin_AppMode0;
    WE_Pin_t Calypso_Pin_AppMode1;
    WE_Pin_t Calypso_Pin_StatusInd0;
    WE_Pin_t Calypso_Pin_StatusInd1;
} Calypso_Pins_t;

/**
 * @brief Calypso event callback.
 * Arguments: Event text
 */
typedef void (*Calypso_EventCallback_t)(char*);

/**
 * @brief Calypso line received callback.
 *
 * Can be used to intercept responses from Calypso.
 *
 * Arguments: Line text, length of text
 * Returns: true if the line should not be processed by the driver,
 *          false if the driver should process the line as usual
 *
 * @see Calypso_SetLineRxCallback()
 */
typedef bool (*Calypso_LineRxCallback_t)(char*, uint16_t);

extern uint8_t Calypso_firmwareVersionMajor;
extern uint8_t Calypso_firmwareVersionMinor;
extern uint8_t Calypso_firmwareVersionPatch;

/**
 * @brief Initializes the serial communication with the module
 *
 * @param[in] uartP: Definition of the uart connected to the module
 * @param[in] pinoutP: Definition of the gpios connected to the module
 * @param[in] eventCallback: Function pointer to event handler (optional)

 * @return True if successful, false otherwise
 */
extern bool Calypso_Init(WE_UART_t* uartP, Calypso_Pins_t* pinoutP, Calypso_EventCallback_t eventCallback);

/**
 * @brief Deinitializes the serial communication with the module.
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_Deinit(void);

/**
 * @brief Sets the Calypso's application mode pins APP_MODE_0 and APP_MODE_1.
 *
 * @param[in] appMode: Application mode to set
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_SetApplicationModePins(Calypso_ApplicationMode_t appMode);

/**
 * @brief Performs a reset of the module using the reset pin.
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_PinReset(void);

/**
 * @brief Wakes the module up from power save mode using the wake up pin.
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_PinWakeUp(void);

/**
 * @brief Gets the pin level
 *
 * @param[in] pin: the pin to be checked
 *
 * @param[out] pin_levelP: the pin level
 *
 * @return True if request succeeded,
 *         false otherwise
 *
 */
extern bool Calypso_GetPinLevel(WE_Pin_t pin, WE_Pin_Level_t* pin_levelP);

/**
 * @brief Sends the supplied AT command to the module
 *
 * @param[in] data: AT command to send. Note that the command has to end with "\r\n\0".
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_SendRequest(char* data);

/**
 * @brief Waits for the response from the module after a request.
 *
 * @param[in] maxTimeMs: Maximum wait time in milliseconds
 * @param[in] expectedStatus: Status to wait for
 * @param[out] pOutResponse: Received response text (if any) will be written to this buffer (optional)
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_WaitForConfirm(uint32_t maxTimeMs, Calypso_CNFStatus_t expectedStatus, char* pOutResponse);

/**
 * @brief Returns the code of the last error (if any).
 *
 * @param[out] lastErrorText: Text of last error (if any). See Calypso_lastErrorText for max. buffer size.
 *
 * @return Last error code (if any)
 */
extern int32_t Calypso_GetLastError(char* lastErrorText);

/**
 * @brief Set timing parameters used by the Calypso driver.
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @param[in] minCommandIntervalMicroseconds: Time step (microseconds) when waiting for responses from Calypso.
 * @param[in] waitTimeStepMicroseconds: Minimum interval (microseconds) between subsequent commands sent to Calypso.
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_SetTimingParameters(uint32_t waitTimeStepMicroseconds, uint32_t minCommandIntervalMicroseconds);

/**
 * @brief Sets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type: Timeout (i.e. command) type
 * @param[in] timeout: Timeout in milliseconds
 */
extern void Calypso_SetTimeout(Calypso_Timeout_t type, uint32_t timeout);

/**
 * @brief Gets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type: Timeout (i.e. command) type
 *
 * @return Timeout in milliseconds
 */
extern uint32_t Calypso_GetTimeout(Calypso_Timeout_t type);

/**
 * @brief Sends raw data to Calypso via UART.
 *
 * This function sends data immediately without any processing and is used
 * internally for sending AT commands to Calypso.
 *
 * @param[in] data: Pointer to data buffer (data to be sent)
 * @param[in] dataLength: Number of bytes to be sent
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_Transparent_Transmit(const char* data, uint16_t dataLength);

/**
 * @brief Sets the callback function which is executed if a byte has been received from Calypso.
 *
 * The default callback is Calypso_HandleRxByte().
 *
 * @param[in] callback: Pointer to byte received callback function (default callback is used if NULL)
 */
extern void Calypso_SetByteRxCallback(WE_UART_HandleRxByte_t callback);

/**
 * @brief Sets an optional callback function which is executed if a line has been received from Calypso.
 * Can be used to intercept responses from Calypso.
 *
 * The callback function must return True if the line should not be processed by the driver
 * and false if the driver should process the line as usual.
 *
 * @param[in] callback: Pointer to line received callback function
 */
extern void Calypso_SetLineRxCallback(Calypso_LineRxCallback_t callback);

/**
 * @brief Sets EOL character(s) used for interpreting responses from Calypso.
 *
 * @param[in] eol1: First EOL character
 * @param[in] eol2: Second EOL character (is only used if twoEolCharacters is true)
 * @param[in] twoEolCharacters: Controls whether the two EOL characters eol1 and eol2 (true) or only eol1 (false) is used
 */
extern void Calypso_SetEolCharacters(uint8_t eol1, uint8_t eol2, bool twoEolCharacters);

#ifdef __cplusplus
}
#endif

#endif // CALYPSO_H_INCLUDED
