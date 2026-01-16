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
 * @file
 * @brief CordeliaI driver header file.
 */

#ifndef CORDELIAI_H_INCLUDED
#define CORDELIAI_H_INCLUDED
#include <global/global_types.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <utils/base64.h>

/**
 * @brief Max. length of sent commands and responses from CordeliaI.
 */
#define CORDELIAI_LINE_MAX_SIZE 2048

/**
 * @brief Max. length of response text (size of buffer storing responses received from CordeliaI).
 * @see CordeliaI_currentResponseText
 */
#define CORDELIAI_MAX_RESPONSE_TEXT_LENGTH CORDELIAI_LINE_MAX_SIZE

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief AT command confirmation status.
 */
typedef enum CordeliaI_CNFStatus_t
{
    CordeliaI_CNFStatus_Success,
    CordeliaI_CNFStatus_Failed,
    CordeliaI_CNFStatus_Invalid,
    /** @cond DOXYGEN_IGNORE */
    CordeliaI_CNFStatus_NumberOfValues
    /** @endcond */
} CordeliaI_CNFStatus_t;

/**
 * @brief Data format used for transferred data.
 */
typedef enum CordeliaI_DataFormat_t
{
    CordeliaI_DataFormat_Binary,
    CordeliaI_DataFormat_Base64,
    /** @cond DOXYGEN_IGNORE */
    CordeliaI_DataFormat_NumberOfValues,
    /** @endcond */
} CordeliaI_DataFormat_t;

/**
 * @brief Timeout categories (for responses to AT commands).
 * @see CordeliaI_SetTimeout(), CordeliaI_GetTimeout()
 */
typedef enum CordeliaI_Timeout_t
{
    CordeliaI_Timeout_General,
    CordeliaI_Timeout_FactoryReset,
    CordeliaI_Timeout_WlanAddProfile,
    CordeliaI_Timeout_WlanScan,
    CordeliaI_Timeout_FileIO,
    CordeliaI_Timeout_OTAVersion,
    /** @cond DOXYGEN_IGNORE */
    CordeliaI_Timeout_NumberOfValues
    /** @endcond */
} CordeliaI_Timeout_t;

/**
 * @brief Application modes. Used with CordeliaI_SetApplicationModePins().
 */
typedef enum CordeliaI_ApplicationMode_t
{
    CordeliaI_ApplicationMode_ATCommandMode = 0,
    CordeliaI_ApplicationMode_OTAUpdate = 1,
    CordeliaI_ApplicationMode_Provisioning = 2,
    CordeliaI_ApplicationMode_TransparentMode = 3
} CordeliaI_ApplicationMode_t;

/**
 * @brief Pins used by this driver.
 */
typedef struct CordeliaI_Pins_t
{
    WE_Pin_t CordeliaI_Pin_Reset;
    WE_Pin_t CordeliaI_Pin_WakeUp;
    WE_Pin_t CordeliaI_Pin_Boot;
    WE_Pin_t CordeliaI_Pin_AppMode0;
    WE_Pin_t CordeliaI_Pin_AppMode1;
    WE_Pin_t CordeliaI_Pin_StatusInd0;
    WE_Pin_t CordeliaI_Pin_StatusInd1;
} CordeliaI_Pins_t;

/**
 * @brief CordeliaI event callback.
 * Arguments: Event text
 */
typedef void (*CordeliaI_EventCallback_t)(char*);

/**
 * @brief CordeliaI line received callback.
 *
 * Can be used to intercept responses from CordeliaI.
 *
 * Arguments: Line text, length of text
 * Returns: true if the line should not be processed by the driver,
 *          false if the driver should process the line as usual
 *
 * @see CordeliaI_SetLineRxCallback()
 */
typedef bool (*CordeliaI_LineRxCallback_t)(char*, uint16_t);

/**
 * @brief Initializes the serial communication with the module
 *
 * @param[in] uartP: Definition of the uart connected to the module
 * @param[in] pinoutP: Definition of the gpios connected to the module
 * @param[in] eventCallback: Function pointer to event handler (optional)

 * @return True if successful, false otherwise
 */
extern bool CordeliaI_Init(WE_UART_t* uartP, CordeliaI_Pins_t* pinoutP, CordeliaI_EventCallback_t eventCallback);

/**
 * @brief Deinitializes the serial communication with the module.
 *
 * @return True if successful, false otherwise
 */
extern bool CordeliaI_Deinit(void);

/**
 * @brief Sets the CordeliaI's application mode pins APP_MODE_0 and APP_MODE_1.
 *
 * @param[in] appMode: Application mode to set
 *
 * @return True if successful, false otherwise
 */
extern bool CordeliaI_SetApplicationModePins(CordeliaI_ApplicationMode_t appMode);

/**
 * @brief Performs a reset of the module using the reset pin.
 *
 * @return True if successful, false otherwise
 */
extern bool CordeliaI_PinReset(void);

/**
 * @brief Wakes the module up from power save mode using the wake up pin.
 *
 * @return True if successful, false otherwise
 */
extern bool CordeliaI_PinWakeUp(void);

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
extern bool CordeliaI_GetPinLevel(WE_Pin_t pin, WE_Pin_Level_t* pin_levelP);

/**
 * @brief Sends the supplied AT command to the module
 *
 * @param[in] data: AT command to send. Note that the command has to end with "\r\n\0".
 *
 * @return True if successful, false otherwise
 */
extern bool CordeliaI_SendRequest(char* data);

/**
 * @brief Waits for the response from the module after a request.
 *
 * @param[in] maxTimeMs: Maximum wait time in milliseconds
 * @param[in] expectedStatus: Status to wait for
 * @param[out] pOutResponse: Received response text (if any) will be written to this buffer (optional)
 *
 * @return True if successful, false otherwise
 */
extern bool CordeliaI_WaitForConfirm(uint32_t maxTimeMs, CordeliaI_CNFStatus_t expectedStatus, char* pOutResponse);

/**
 * @brief Returns the code of the last error (if any).
 *
 * @param[out] lastErrorText: Text of last error (if any). See CordeliaI_lastErrorText for max. buffer size.
 *
 * @return Last error code (if any)
 */
extern int32_t CordeliaI_GetLastError(char* lastErrorText);

/**
 * @brief Set timing parameters used by the CordeliaI driver.
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @param[in] waitTimeStepMicroseconds: Time step (microseconds) when waiting for responses from CordeliaI.
 * @param[in] minCommandIntervalMicroseconds: Minimum interval (microseconds) between subsequent commands sent to CordeliaI.
 *
 * @return True if successful, false otherwise
 */
extern bool CordeliaI_SetTimingParameters(uint32_t waitTimeStepMicroseconds, uint32_t minCommandIntervalMicroseconds);

/**
 * @brief Sets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type: Timeout (i.e. command) type
 * @param[in] timeout: Timeout in milliseconds
 */
extern void CordeliaI_SetTimeout(CordeliaI_Timeout_t type, uint32_t timeout);

/**
 * @brief Gets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type: Timeout (i.e. command) type
 *
 * @return Timeout in milliseconds
 */
extern uint32_t CordeliaI_GetTimeout(CordeliaI_Timeout_t type);

/**
 * @brief Sends raw data to CordeliaI via UART.
 *
 * This function sends data immediately without any processing and is used
 * internally for sending AT commands to CordeliaI.
 *
 * @param[in] data: Pointer to data buffer (data to be sent)
 * @param[in] dataLength: Number of bytes to be sent
 *
 * @return True if successful, false otherwise
 */
extern bool CordeliaI_Transparent_Transmit(const char* data, uint16_t dataLength);

/**
 * @brief Sets the callback function which is executed if a byte has been received from CordeliaI.
 *
 * The default callback is CordeliaI_HandleRxByte().
 *
 * @param[in] callback: Pointer to byte received callback function (default callback is used if NULL)
 */
extern void CordeliaI_SetByteRxCallback(WE_UART_HandleRxByte_t callback);

/**
 * @brief Sets an optional callback function which is executed if a line has been received from CordeliaI.
 * Can be used to intercept responses from CordeliaI.
 *
 * The callback function must return True if the line should not be processed by the driver
 * and false if the driver should process the line as usual.
 *
 * @param[in] callback: Pointer to line received callback function
 */
extern void CordeliaI_SetLineRxCallback(CordeliaI_LineRxCallback_t callback);

/**
 * @brief Sets EOL character(s) used for interpreting responses from CordeliaI.
 *
 * @param[in] eol1: First EOL character
 * @param[in] eol2: Second EOL character (is only used if twoEolCharacters is true)
 * @param[in] twoEolCharacters: Controls whether the two EOL characters eol1 and eol2 (true) or only eol1 (false) is used
 */
extern void CordeliaI_SetEolCharacters(uint8_t eol1, uint8_t eol2, bool twoEolCharacters);

#ifdef __cplusplus
}
#endif

#endif // CORDELIAI_H_INCLUDED
