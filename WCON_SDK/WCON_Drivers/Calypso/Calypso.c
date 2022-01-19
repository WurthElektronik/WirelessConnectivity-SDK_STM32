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
 * @brief Calypso driver source file.
 */

#include "Calypso.h"

#include <stdio.h>
#include <stdlib.h>

#include "ATCommands/ATDevice.h"
#include "ATCommands/ATEvent.h"

static void Calypso_HandleRxByte(uint8_t receivedByte);
static void Calypso_HandleRxLine(char *rxPacket, uint16_t rxLength);

/**
 * @brief Base64 encoding table
 */
static uint8_t Calypso_base64EncTable[64] =  {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                              'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                              'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                              'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                              'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                              'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                              'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                              '4', '5', '6', '7', '8', '9', '+', '/'
                                             };

/**
 * @brief Base64 decoding table
 */
static uint8_t Calypso_base64DecTable[123] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,62,0,0,0,63,
                                              52,53,54,55,56,57,58,59,60,61, /* 0-9 */
                                              0,0,0,0,0,0,0,
                                              0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25, /* A-Z */
                                              0,0,0,0,0,0,
                                              26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51
                                             }; /* a-z */


static const char *Calypso_BooleanValueStrings[Calypso_BooleanValue_NumberOfValues] =
{
    "false",
    "true"
};

/**
 * @brief Timeouts for responses to AT commands (milliseconds).
 * Initialization is done in Calypso_Init().
 */
static uint32_t Calypso_timeouts[Calypso_Timeout_NumberOfValues] = {0};

/**
 * @brief Is set to true when sending an AT command and is reset to false when the response has been received.
 */
static bool Calypso_requestPending = false;

/**
 * @brief Name of the command last sent using Calypso_SendRequest() (without prefix "AT+").
 */
static char Calypso_pendingCommandName[64] = {0};

/**
 * @brief Length of Calypso_pendingCommandName.
 */
static size_t Calypso_pendingCommandNameLength = 0;

/**
 * @brief Buffer used for current response text.
 */
static char Calypso_currentResponseText[CALYPSO_MAX_RESPONSE_TEXT_LENGTH];

/**
 * @brief Length of text in current response text buffer.
 * @see Calypso_currentResponseText
 */
static size_t Calypso_currentResponseLength = 0;

/**
 * @brief Last error text (if any).
 */
static char Calypso_lastErrorText[32] = {0};

/**
 * @brief Last error code (if any).
 */
static int32_t Calypso_lastErrorCode = 0;

/**
 * @brief Confirmation status of the current (last issued) command.
 */
static Calypso_CNFStatus_t Calypso_cmdConfirmStatus;

/**
 * @brief Data buffer for received data.
 */
static char Calypso_rxBuffer[CALYPSO_LINE_MAX_SIZE];

/**
 * @brief Number of bytes in receive buffer.
 * @see Calypso_rxBuffer
 */
static uint16_t Calypso_rxByteCounter = 0;

/**
 * @brief Is set to true when the first EOL character (default: carriage return)
 * has been found in the current response text.
 */
static bool Calypso_eolChar1Found = false;

/**
 * @brief First EOL character expected for responses received from Calypso.
 * @see Calypso_eolChar2, Calypso_twoEolCharacters, Calypso_SetEolCharacters()
 */
static uint8_t Calypso_eolChar1 = '\r';

/**
 * @brief Second EOL character expected for responses received from Calypso.
 * Only applicable if Calypso_twoEolCharacters is true.
 * @see Calypso_eolChar1, Calypso_twoEolCharacters, Calypso_SetEolCharacters()
 */
static uint8_t Calypso_eolChar2 = '\n';

/**
 * @brief Controls whether a line of text received from Calypso is considered complete after one or two EOL characters.
 * @see Calypso_eolChar1, Calypso_eolChar2, Calypso_SetEolCharacters()
 */
static bool Calypso_twoEolCharacters = true;

/**
 * @brief Pin configuration array.
 * The values in Calypso_Pin_t are used to access the entries in this array.
 */
static WE_Pin_t Calypso_pins[Calypso_Pin_Count] = {0};

/**
 * @brief Time step (microseconds) when waiting for responses from Calypso.
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @see Calypso_SetTimingParameters
 */
static uint32_t Calypso_waitTimeStepUsec = 5 * 1000;

/**
 * @brief Minimum interval (microseconds) between subsequent commands sent to Calypso
 * (more precisely the minimum interval between the confirmation of the previous command
 * and the sending of the following command).
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @see Calypso_SetTimingParameters
 */
static uint32_t Calypso_minCommandIntervalUsec = 3 * 1000;

/**
 * @brief Time (microseconds) of last confirmation received from Calypso (if any).
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 */
static uint32_t Calypso_lastConfirmTimeUsec = 0;

/**
 * @brief Major firmware version of the connected module.
 *
 * Note that some functions may behave differently depending on the firmware version.
 *
 * The firmware version can be determined during runtime using
 * ATDevice_Get(ATDevice_GetId_General, ATDevice_GetGeneral_Version, &version) - this will
 * update the variables Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
 * Calypso_firmwareVersionPatch.
 */
uint8_t Calypso_firmwareVersionMajor = 2;

/**
 * @brief Minor firmware version of the connected module.
 *
 * Note that some functions may behave differently depending on the firmware version.
 *
 * The firmware version can be determined during runtime using
 * ATDevice_Get(ATDevice_GetId_General, ATDevice_GetGeneral_Version, &version) - this will
 * update the variables Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
 * Calypso_firmwareVersionPatch.
 */
uint8_t Calypso_firmwareVersionMinor = 0;

/**
 * @brief Patch firmware version of the connected module.
 *
 * Note that some functions may behave differently depending on the firmware version.
 *
 * The firmware version can be determined during runtime using
 * ATDevice_Get(ATDevice_GetId_General, ATDevice_GetGeneral_Version, &version) - this will
 * update the variables Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
 * Calypso_firmwareVersionPatch.
 */
uint8_t Calypso_firmwareVersionPatch = 0;

/**
 * @brief Callback function for events.
 */
Calypso_EventCallback_t Calypso_eventCallback;

/**
 * @bief Is set to true if currently executing a (custom) event handler.
 */
static bool Calypso_executingEventCallback = false;

/**
 * @brief Callback function which is executed if a single byte has been received from Calypso.
 * The default callback is Calypso_HandleRxByte().
 * @see Calypso_SetByteRxCallback()
 */
Calypso_ByteRxCallback_t Calypso_byteRxCallback = NULL;

/**
 * @brief Optional callback function which is executed if a line is received from Calypso.
 * Can be used to intercept responses from Calypso.
 * @see Calypso_SetLineRxCallback()
 */
Calypso_LineRxCallback_t Calypso_lineRxCallback = NULL;

/**
 * @brief Initializes the serial communication with the module
 *
 * @param[in] baudrate       Baud rate of the serial communication
 * @param[in] flowControl    Flow control setting for the serial communication
 * @param[in] parity         Parity bit configuration for the serial communication
 * @param[in] eventCallback  Function pointer to event handler (optional)
 * @param[in] pins           Pin configuration, pointer to array with Calypso_Pin_Count elements
 *                           (optional, default configuration is used if NULL, see Calypso_GetDefaultPinConfig())

 * @return true if successful, false otherwise
 */
bool Calypso_Init(uint32_t baudrate,
                  WE_FlowControl_t flowControl,
                  WE_Parity_t parity,
                  Calypso_EventCallback_t eventCallback,
                  WE_Pin_t *pins)
{
    Calypso_requestPending = false;

    /* Callbacks */
    Calypso_byteRxCallback = Calypso_HandleRxByte;
    Calypso_lineRxCallback = NULL;
    Calypso_eventCallback = eventCallback;

    /* Initialize the pins */
    if (NULL == pins)
    {
        /* Use default pin config */
        Calypso_GetDefaultPinConfig(Calypso_pins);
    }
    else
    {
        /* Use the supplied pin config */
        for (uint8_t i = 0; i < Calypso_Pin_Count; i++)
        {
            Calypso_pins[i] = pins[i];
        }
    }

    if (false == WE_InitPins(Calypso_pins, Calypso_Pin_Count))
    {
        /* Error */
        return false;
    }

    /* Set initial pin levels */
    WE_SetPin(Calypso_pins[Calypso_Pin_Boot], WE_Pin_Level_Low);
    WE_SetPin(Calypso_pins[Calypso_Pin_WakeUp], WE_Pin_Level_Low);
    WE_SetPin(Calypso_pins[Calypso_Pin_Reset], WE_Pin_Level_High);
    WE_SetPin(Calypso_pins[Calypso_Pin_AppMode0], WE_Pin_Level_Low);
    WE_SetPin(Calypso_pins[Calypso_Pin_AppMode1], WE_Pin_Level_Low);

    WE_UART_Init(baudrate, flowControl, parity, true); /* Calypso default: 921600 Baud 8e1 */
    WE_Delay(10);

    /* Set response timeouts */
    Calypso_timeouts[Calypso_Timeout_General] = 1000;
    Calypso_timeouts[Calypso_Timeout_FactoryReset] = 100000;
    Calypso_timeouts[Calypso_Timeout_WlanAddProfile] = 5000;
    Calypso_timeouts[Calypso_Timeout_WlanScan] = 1000;
    Calypso_timeouts[Calypso_Timeout_NetAppUpdateTime] = 30000;
    Calypso_timeouts[Calypso_Timeout_NetAppHostLookUp] = 5000;
    Calypso_timeouts[Calypso_Timeout_HttpConnect] = 30000;
    Calypso_timeouts[Calypso_Timeout_HttpRequest] = 30000;
    Calypso_timeouts[Calypso_Timeout_FileIO] = 2000;
    Calypso_timeouts[Calypso_Timeout_GPIO] = 2000;

    return true;
}

/**
 * @brief Deinitializes the serial communication with the module.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_Deinit(void)
{
    Calypso_eventCallback = NULL;

    Calypso_rxByteCounter = 0;
    Calypso_eolChar1Found = 0;
    Calypso_requestPending = false;
    Calypso_currentResponseLength = 0;

    WE_UART_DeInit();

    return true;
}

/**
 * @brief Sets the Calypso's application mode pins APP_MODE_0 and APP_MODE_1.
 *
 * @param[in] appMode Application mode to set
 *
 * @return true if successful, false otherwise
 */
bool Calypso_SetApplicationModePins(Calypso_ApplicationMode_t appMode)
{
    if (!WE_SetPin(Calypso_pins[Calypso_Pin_AppMode0], (0 != (appMode & 0x01)) ? WE_Pin_Level_High : WE_Pin_Level_Low))
    {
        return false;
    }
    return WE_SetPin(Calypso_pins[Calypso_Pin_AppMode1], (0 != (appMode & 0x02)) ? WE_Pin_Level_High : WE_Pin_Level_Low);
}

/**
 * @brief Performs a reset of the module using the reset pin.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_PinReset(void)
{
    if (!WE_SetPin(Calypso_pins[Calypso_Pin_Reset], WE_Pin_Level_Low))
    {
        return false;
    }
    WE_Delay(5);
    return WE_SetPin(Calypso_pins[Calypso_Pin_Reset], WE_Pin_Level_High);
}

/**
 * @brief Wakes the module up from power save mode using the wake up pin.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_PinWakeUp(void)
{
    if (!WE_SetPin(Calypso_pins[Calypso_Pin_WakeUp], WE_Pin_Level_High))
    {
        return false;
    }
    WE_Delay(5);
    return WE_SetPin(Calypso_pins[Calypso_Pin_WakeUp], WE_Pin_Level_Low);
}

/**
 * @brief Sets pin level to high or low.
 *
 * @param[in] pin Output pin to be set
 * @param[in] level Output level to be set
 *
 * @return true if successful, false otherwise
 */
bool Calypso_SetPin(Calypso_Pin_t pin, WE_Pin_Level_t level)
{
    if (pin >= Calypso_Pin_Count)
    {
        return false;
    }
    return WE_SetPin(Calypso_pins[pin], level);
}

/**
 * @brief Read current pin level.
 *
 * @param[in] pin Pin to be read
 *
 * @return Current level of pin
 */
WE_Pin_Level_t Calypso_GetPinLevel(Calypso_Pin_t pin)
{
    if (pin >= Calypso_Pin_Count)
    {
        return WE_Pin_Level_Low;
    }
    return WE_GetPinLevel(Calypso_pins[pin]);
}

/**
 * @brief Sends the supplied AT command to the module
 *
 * @param[in] data AT command to send. Note that the command has to end with "\r\n\0".
 *
 * @return true if successful, false otherwise
 */
bool Calypso_SendRequest(char *data)
{
    if (Calypso_executingEventCallback)
    {
        /* Don't allow sending AT commands from event handlers, as this will
         * mess up send/receive states and buffers. */
        return false;
    }

    Calypso_requestPending = true;
    Calypso_currentResponseLength = 0;
    *Calypso_lastErrorText = '\0';
    Calypso_lastErrorCode = 0;

    /* Make sure that the time between the last confirmation received from the module
     * and the next command sent to the module is not shorter than Calypso_minCommandIntervalUsec */
    uint32_t t = WE_GetTickMicroseconds() - Calypso_lastConfirmTimeUsec;
    if (t < Calypso_minCommandIntervalUsec)
    {
        WE_DelayMicroseconds(Calypso_minCommandIntervalUsec - t);
    }

    size_t dataLength = strlen(data);

    /* Get command name from request string (remove prefix "AT+" and parameters) */
    Calypso_pendingCommandName[0] = '\0';
    Calypso_pendingCommandNameLength = 0;
    if (dataLength > 3 &&
            (data[0] == 'a' || data[0] == 'A') &&
            (data[1] == 't' || data[1] == 'T') &&
            data[2] == '+')
    {
        char *pData = data + 3;
        if (Calypso_GetCmdName(&pData, Calypso_pendingCommandName, CALYPSO_COMMAND_DELIM, '\r'))
        {
            Calypso_pendingCommandNameLength = strlen(Calypso_pendingCommandName);
        }
    }

#ifdef WE_DEBUG
    fprintf(stdout, "> %s", data);
#endif

    Calypso_Transmit(data, dataLength);

    return true;
}

/**
 * @brief Sends raw data to Calypso via UART.
 *
 * This function sends data immediately without any processing and is used
 * internally for sending AT commands to Calypso.
 *
 * @param[in] data Pointer to data buffer (data to be sent)
 * @param[in] dataLength Number of bytes to be sent
 */
void Calypso_Transmit(const char *data, uint16_t dataLength)
{
    WE_UART_Transmit((uint8_t *) data, dataLength);
}

/**
 * @brief Waits for the response from the module after a request.
 *
 * @param[in] maxTimeMs Maximum wait time in milliseconds
 * @param[in] expectedStatus Status to wait for
 * @param[out] pOutResponse Received response text (if any) will be written to this buffer (optional)
 *
 * @return true if successful, false otherwise
 */
bool Calypso_WaitForConfirm(uint32_t maxTimeMs,
                            Calypso_CNFStatus_t expectedStatus,
                            char *pOutResponse)
{
    Calypso_cmdConfirmStatus = Calypso_CNFStatus_Invalid;

    uint32_t t0 = WE_GetTick();

    while (1)
    {
        if (Calypso_CNFStatus_Invalid != Calypso_cmdConfirmStatus)
        {
            /* Store current time to enable check for min. time between received confirm and next command. */
            Calypso_lastConfirmTimeUsec = WE_GetTickMicroseconds();

            Calypso_requestPending = false;

            if (Calypso_cmdConfirmStatus == expectedStatus)
            {
                if (NULL != pOutResponse)
                {
                    /* Copy response for further processing */
                    memcpy(pOutResponse, Calypso_currentResponseText, Calypso_currentResponseLength);
                }
                return true;
            }
            else
            {
                return false;
            }
        }

        uint32_t now = WE_GetTick();
        if (now - t0 > maxTimeMs)
        {
            /* Timeout */
            break;
        }

        if (Calypso_waitTimeStepUsec > 0)
        {
            WE_DelayMicroseconds(Calypso_waitTimeStepUsec);
        }
    }

    Calypso_requestPending = false;
    return false;
}

/**
 * @brief Returns the code of the last error (if any).
 *
 * @param[out] lastErrorText Text of last error (if any). See Calypso_lastErrorText for max. buffer size.
 *
 * @return Last error code (if any)
 */
int32_t Calypso_GetLastError(char *lastErrorText)
{
    if (NULL != lastErrorText)
    {
        strcpy(lastErrorText, Calypso_lastErrorText);
    }
    return Calypso_lastErrorCode;
}

/**
 * @brief Converts an integer to string.
 *
 * @param[out] outString Number converted to string
 * @param[in] number Integer value to convert to string
 * @param[in] intFlags Formatting flags
 *
 * @return true if successful, false otherwise
 */
bool Calypso_IntToString(char *outString, uint32_t number, uint16_t intFlags)
{
    if ((0 == (intFlags & CALYPSO_INTFLAGS_SIGN)) || (0 == (intFlags & CALYPSO_INTFLAGS_NOTATION)))
    {
        return false;
    }

    if (CALYPSO_INTFLAGS_NOTATION_HEX == (intFlags & CALYPSO_INTFLAGS_NOTATION))
    {
        /* HEX */
        sprintf(outString, "0x%lx", number);
    }
    else
    {
        /* DEC */

        if (CALYPSO_INTFLAGS_UNSIGNED == (intFlags & CALYPSO_INTFLAGS_SIGN))
        {
            /* UNSIGNED */
            sprintf(outString, "%lu", number);
        }
        else
        {
            /* SIGNED */
            sprintf(outString, "%ld", *(int32_t*) &number);
        }
    }

    return true;
}

/**
 * @brief Parses string to integer
 *
 * @param[out] number Parsed integer value
 * @param[in] inString String to be parsed
 * @param[in] intFlags Flags to determine how to parse
 *
 * @return true if successful, false otherwise
 */
bool Calypso_StringToInt(void *number, const char *inString, uint16_t intFlags)
{
    if ((NULL == inString) || (NULL == number))
    {
        return false;
    }

    bool hex = (0 == strncmp(inString, "0x", 2)) || ((intFlags & CALYPSO_INTFLAGS_NOTATION_HEX) != 0);
    bool signedDataType = (intFlags & CALYPSO_INTFLAGS_SIGNED) != 0;

    if (signedDataType)
    {
        long longNr = strtol(inString, NULL, (hex == true) ? 16 : 10);

        if ((intFlags & CALYPSO_INTFLAGS_SIZE8) != 0)
        {
            *((int8_t*) number) = longNr;
        }
        else if ((intFlags & CALYPSO_INTFLAGS_SIZE16) != 0)
        {
            *((int16_t*) number) = longNr;
        }
        else if ((intFlags & CALYPSO_INTFLAGS_SIZE32) != 0)
        {
            *((int32_t*) number) = longNr;
        }
    }
    else
    {
        unsigned long longNr = strtoul(inString, NULL, (hex == true) ? 16 : 10);

        if ((intFlags & CALYPSO_INTFLAGS_SIZE8) != 0)
        {
            *((uint8_t*) number) = longNr;
        }
        else if ((intFlags & CALYPSO_INTFLAGS_SIZE16) != 0)
        {
            *((uint16_t*) number) = longNr;
        }
        else if ((intFlags & CALYPSO_INTFLAGS_SIZE32) != 0)
        {
            *((uint32_t*) number) = longNr;
        }
    }

    return true;
}

/**
 * @brief Appends a byte array argument to the end of an AT command.
 *
 * @param[out] pOutString  AT command after appending argument
 * @param[in] pInArgument Pointer to byte array to be added
 * @param[in] numBytes    Number of bytes to add
 * @param[in] delimiter   Delimiter to append after argument
 *
 * @return true if successful, false otherwise
 */
bool Calypso_AppendArgumentBytes(char *pOutString,
                                 const char *pInArgument,
                                 uint16_t numBytes,
                                 char delimiter)
{
    if (NULL == pOutString)
    {
        return false;
    }

    size_t strLength = strlen(pOutString);
    if (NULL != pInArgument)
    {
        memcpy(&pOutString[strLength], pInArgument, numBytes);
        strLength += numBytes;
    }

    pOutString[strLength] = delimiter;

    return true;
}

/**
 * @brief Appends a string argument to the end of an AT command.
 *
 * The supplied delimiter is appended after the argument. A null byte ('\0')
 * is appended after the delimiter, if the delimiter itself is not a null byte.
 *
 * @param[out] pOutString AT command after appending argument
 * @param[in] pInArgument Argument to be added
 * @param[in] delimiter Delimiter to append after argument
 *
 * @return true if successful, false otherwise
 */
bool Calypso_AppendArgumentString(char *pOutString, const char *pInArgument, char delimiter)
{
    if (NULL == pOutString)
    {
        return false;
    }

    size_t outStrLength = strlen(pOutString);
    if (NULL != pInArgument)
    {
        size_t inStrLength = strlen(pInArgument);
        strcpy(&pOutString[outStrLength], pInArgument);
        outStrLength += inStrLength;
    }
    pOutString[outStrLength] = delimiter;
    if (delimiter != '\0')
    {
        pOutString[outStrLength + 1] = '\0';
    }

    return true;
}

/**
 * @brief Appends an integer argument to the end of an AT command
 *
 * @param[out] pOutString AT command after appending argument
 * @param[in] pInValue Argument to be added
 * @param[in] intFlags Integer formatting flags
 * @param[in] delimiter Delimiter to append after argument
 *
 * @return true if successful, false otherwise
 */
bool Calypso_AppendArgumentInt(char *pOutString, uint32_t pInValue, uint16_t intFlags, char delimiter)
{
    if (NULL == pOutString)
    {
        return false;
    }

    char tempString[12];

    if (Calypso_IntToString(tempString, pInValue, intFlags))
    {
        return Calypso_AppendArgumentString(pOutString, tempString, delimiter);
    }

    return false;
}

/**
 * @brief Appends bitmask strings to the supplied string.
 *
 * @param[out] pOutString AT command after appending argument
 * @param[in] stringList List of strings containing the string representations of the bits in the input bitmask
 * @param[in] numStrings Number of elements in stringList (max. number of bits in input bitmask)
 * @param[in] bitmask Input bitmask
 * @param[in] delimiter Delimiter to append after argument
 * @param[in] maxStringLength Max. length of output string
 *
 * @return true if successful, false otherwise
 */
bool Calypso_AppendArgumentBitmask(char *pOutString,
                                   const char *stringList[],
                                   uint8_t numStrings,
                                   uint32_t bitmask,
                                   char delimiter,
                                   uint16_t maxStringLength)
{
    size_t outStrLength = strlen(pOutString) + 1;
    bool empty = true;
    for (int i = 0; i < numStrings; i++)
    {
        if (0 != (bitmask & (1 << i)))
        {
            outStrLength += strlen(stringList[i]) + 1;
            if (outStrLength > maxStringLength)
            {
                return false;
            }
            if (!Calypso_AppendArgumentString(pOutString, stringList[i], CALYPSO_BITMASK_DELIM))
            {
                return false;
            }
            empty = false;
        }
    }
    pOutString[outStrLength - (empty ? 1 : 2)] = delimiter;
    pOutString[outStrLength - (empty ? 0 : 1)] = '\0';
    return true;
}

/**
 * @brief Appends a boolean string (true, false) to the supplied string.
 *
 * @param[out] pOutString AT command after appending argument
 * @param[in] inBool Value to append
 * @param[in] delimiter Delimiter to append after argument
 *
 * @return true if successful, false otherwise
 */
bool Calypso_AppendArgumentBoolean(char *pOutString, bool inBool, char delimiter)
{
    return Calypso_AppendArgumentString(pOutString, Calypso_BooleanValueStrings[(inBool == true) ? 1 : 0], delimiter);
}

/**
 * @brief Gets the next string argument from the supplied AT command.
 *
 * @param[in,out] pInArguments AT command to get argument from
 * @param[out] pOutArgument Argument as string
 * @param[in] delimiter Delimiter which occurs after argument to get
 * @param[in] maxLength Max. length of string to get (including termination character).
 *
 * @return true if successful, false otherwise
 */
bool Calypso_GetNextArgumentString(char **pInArguments,
                                   char *pOutArgument,
                                   char delimiter,
                                   uint16_t maxLength)
{
    if ((NULL == pInArguments) || (NULL == pOutArgument))
    {
        return false;
    }

    size_t argumentLength = 0;
    size_t inputStringLength = strlen(*pInArguments);

    while (true)
    {
        if (argumentLength > maxLength - 1)
        {
            return false;
        }
        else if (((*pInArguments)[argumentLength] == delimiter))
        {
            /* If argument list is not empty, copy string to output arguments */
            memcpy(pOutArgument, *pInArguments, argumentLength);

            pOutArgument[argumentLength] = '\0';

            if (argumentLength > 0 || **pInArguments != '\0')
            {
                *pInArguments = &((*pInArguments)[argumentLength+1]);
            }

            return true;
        }
        else if (argumentLength > inputStringLength)
        {
            return false;
        }
        argumentLength++;
    }
}

/**
 * @brief Gets the next integer argument from the supplied AT command.
 *
 * @param[in,out] pInArguments AT command to get argument from
 * @param[out] pOutArgument Argument parsed as integer
 * @param[in] intFlags Flags to determine how to parse
 * @param[in] delimiter Delimiter which occurs after argument to get
 *
 * @return true if successful, false otherwise
 */
bool Calypso_GetNextArgumentInt(char **pInArguments,
                                void *pOutArgument,
                                uint16_t intFlags,
                                char delimiter)
{
    if ((NULL == pInArguments) || (NULL == pOutArgument))
    {
        return false;
    }

    char tempString[12];

    Calypso_GetNextArgumentString(pInArguments, tempString, delimiter, sizeof(tempString));
    return Calypso_StringToInt(pOutArgument, tempString, intFlags);
}

/**
 * @brief Gets the next enumeration argument from the supplied AT command.
 *
 * @param[in,out] pInArguments AT command to get argument from
 * @param[out] pOutArgument Enumeration value corresponding to the string read from the input AT command
 * @param[in] stringList List of strings containing the string representations of the enumeration's values
 * @param[in] numStrings Number of elements in stringList (number of elements in the enumeration)
 * @param[in] maxStringLength Max. length of individual enumeration value strings
 * @param[in] delimiter Delimiter which occurs after argument to get
 *
 * @return true if successful, false otherwise
 */
bool Calypso_GetNextArgumentEnum(char **pInArguments,
                                 uint8_t *pOutArgument,
                                 const char *stringList[],
                                 uint8_t numStrings,
                                 uint16_t maxStringLength,
                                 char delimiter)
{
    char tempString[maxStringLength];

    if (!Calypso_GetNextArgumentString(pInArguments,
                                       tempString,
                                       delimiter,
                                       sizeof(tempString)))
    {
        return false;
    }

    bool ok;
    *pOutArgument = Calypso_FindString(stringList,
                                       numStrings,
                                       tempString,
                                       0,
                                       &ok);
    return ok;
}

/**
 * @brief Gets the next bitmask argument from the supplied AT command.
 *
 * @param[in,out] pInArguments AT command to get argument from
 * @param[in] stringList List of strings containing the string representations of the bits in the output bitmask
 * @param[in] numStrings Number of elements in stringList (max. number of bits in output bitmask)
 * @param[in] maxStringLength Max. length of individual bitmask strings
 * @param[out] bitmask Output bitmask
 * @param[in] delimiter Delimiter which occurs after argument to get
 *
 * @return true if successful, false otherwise
 */
bool Calypso_GetNextArgumentBitmask(char **pInArguments,
                                    const char *stringList[],
                                    uint8_t numStrings,
                                    uint16_t maxStringLength,
                                    uint32_t *bitmask,
                                    char delimiter)
{

    bool ret = false;
    char tempString[maxStringLength];

    *bitmask = 0;

    while (**pInArguments != '\0' &&
            ((ret = Calypso_GetNextArgumentString(pInArguments, tempString, CALYPSO_BITMASK_DELIM, sizeof(tempString))) ||
             (ret = Calypso_GetNextArgumentString(pInArguments, tempString, delimiter, sizeof(tempString)))))
    {
        bool ok;
        uint8_t flag = Calypso_FindString(stringList, numStrings, tempString, 0, &ok);
        if (ok)
        {
            *bitmask |= (1 << flag);
        }
    }
    return true;
}

/**
 * @brief Gets the next boolean argument (true, false) from the supplied AT command.
 *
 * @param[in,out] pInArguments AT command to get argument from
 * @param[out] outBool Output boolean
 * @param[in] delimiter Delimiter which occurs after argument to get
 *
 * @return true if successful, false otherwise
 */
bool Calypso_GetNextArgumentBoolean(char **pInArguments, bool *outBool, char delimiter)
{
    uint8_t enumValue;
    bool ok = Calypso_GetNextArgumentEnum(pInArguments,
                                          &enumValue,
                                          Calypso_BooleanValueStrings,
                                          Calypso_BooleanValue_NumberOfValues,
                                          6,
                                          delimiter);
    if (ok)
    {
        *outBool = (enumValue == 1) ? true : false;
    }
    return ok;
}

/**
 * @param Gets the command name from an AT command.
 *
 * @param[in,out] pInAtCmd AT command to get command name from
 * @param[in] pCmdName Command name as string
 * @param[in] delimiter Delimiter which occurs after command name
 * @param[in] delimiter Alternative delimiter which may occur after command name
 *
 * @return true if successful, false otherwise
 */
bool Calypso_GetCmdName(char **pInAtCmd,
                        char *pCmdName,
                        char delimiter,
                        char alternativeDelimiter)
{
    if ((NULL == pInAtCmd) || (NULL == pCmdName))
    {
        return false;
    }

    size_t argumentLength = 0;

    while (true)
    {
        if ((*pInAtCmd)[argumentLength] == delimiter ||
                (*pInAtCmd)[argumentLength] == alternativeDelimiter)
        {
            /* if argument list not empty*/
            /* copy string to output arguments */
            memcpy(pCmdName, *pInAtCmd, argumentLength);

            pCmdName[argumentLength] = '\0';

            *pInAtCmd = &((*pInAtCmd)[argumentLength+1]);
            return true;
        }
        if ((*pInAtCmd)[argumentLength] == '\0')
        {
            return false;
        }
        argumentLength++;
    }
}

/**
 * @brief Looks up a string in a list of strings (case insensitive) and returns the
 * index of the string or the supplied default value, if the string is not found.
 *
 * @param[in] stringList List of strings to search in
 * @param[in] numStrings Number of strings in stringList
 * @param[in] str String to look for
 * @param[in] defaultValue Value to return if the string is not found
 * @param[out] ok Is set to true if the string is found. Optional.
 *
 * @return Index of the first occurrence of str in stringList or defaultValue, if string is not found
 */
uint8_t Calypso_FindString(const char *stringList[],
                           uint8_t numStrings,
                           const char *str,
                           uint8_t defaultValue,
                           bool *ok)
{
    for (uint8_t i = 0; i < numStrings; i++)
    {
        if (0 == strcasecmp(stringList[i], str))
        {
            if (ok)
            {
                *ok = true;
            }
            return i;
        }
    }

    if (ok)
    {
        *ok = false;
    }
    return defaultValue;
}

/**
 * @brief Set timing parameters used by the Calypso driver.
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @param[in] waitTimeStepUsec Time step (microseconds) when waiting for responses from Calypso.
 * @param[in] minCommandIntervalUsec Minimum interval (microseconds) between subsequent commands sent to Calypso.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_SetTimingParameters(uint32_t waitTimeStepUsec, uint32_t minCommandIntervalUsec)
{
    Calypso_waitTimeStepUsec = waitTimeStepUsec;
    Calypso_minCommandIntervalUsec = minCommandIntervalUsec;
    return true;
}

/**
 * @brief Sets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type Timeout (i.e. command) type
 * @param[in] timeout Timeout in milliseconds
 */
void Calypso_SetTimeout(Calypso_Timeout_t type, uint32_t timeout)
{
    Calypso_timeouts[type] = timeout;
}

/**
 * @brief Gets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type Timeout (i.e. command) type
 *
 * @return Timeout in milliseconds
 */
uint32_t Calypso_GetTimeout(Calypso_Timeout_t type)
{
    return Calypso_timeouts[type];
}

/**
 * @brief Returns the default pin configuration.
 *
 * @param[out] pins Pin configuration array of length Calypso_Pin_Count
 */
void Calypso_GetDefaultPinConfig(WE_Pin_t *pins)
{
    pins[Calypso_Pin_Reset].port = GPIOA;
    pins[Calypso_Pin_Reset].pin = GPIO_PIN_10;
    pins[Calypso_Pin_Reset].type = WE_Pin_Type_Output;
    pins[Calypso_Pin_WakeUp].port = GPIOA;
    pins[Calypso_Pin_WakeUp].pin = GPIO_PIN_9;
    pins[Calypso_Pin_WakeUp].type = WE_Pin_Type_Output;
    pins[Calypso_Pin_Boot].port = GPIOA;
    pins[Calypso_Pin_Boot].pin = GPIO_PIN_7;
    pins[Calypso_Pin_Boot].type = WE_Pin_Type_Output;
    pins[Calypso_Pin_AppMode0].port = GPIOA;
    pins[Calypso_Pin_AppMode0].pin = GPIO_PIN_0;
    pins[Calypso_Pin_AppMode0].type = WE_Pin_Type_Output;
    pins[Calypso_Pin_AppMode1].port = GPIOA;
    pins[Calypso_Pin_AppMode1].pin = GPIO_PIN_1;
    pins[Calypso_Pin_AppMode1].type = WE_Pin_Type_Output;
    pins[Calypso_Pin_StatusInd0].port = GPIOB;
    pins[Calypso_Pin_StatusInd0].pin = GPIO_PIN_8;
    pins[Calypso_Pin_StatusInd0].type = WE_Pin_Type_Input;
    pins[Calypso_Pin_StatusInd1].port = GPIOB;
    pins[Calypso_Pin_StatusInd1].pin = GPIO_PIN_9;
    pins[Calypso_Pin_StatusInd1].type = WE_Pin_Type_Input;
}

/**
 * @brief Get Base64 decoded buffer size.
 *
 * This routine calculates the expected raw data buffer size
 * for the given Base64 buffer and buffer size.
 *
 * Note that the returned size includes the string termination character ('\0'),
 * so the size of the actual data is one character less.
 *
 * @param[in] inputData Source buffer holding the base64 data
 * @param[in] inputLength Length of base64 data
 *
 * @return Decoded data size.
*/
uint32_t Calypso_GetBase64DecBufSize(uint8_t *inputData, uint32_t inputLength)
{
    uint32_t outputLength = inputLength / 4 * 3;

    if (outputLength == 0)
    {
        return 0;
    }
    if (inputData[inputLength - 1] == '=')
    {
        outputLength--;
    }
    if (inputData[inputLength - 2] == '=')
    {
        outputLength--;
    }

    /* Data size plus string termination character */
    return outputLength + 1;
}

/**
 * @brief Get Base64 encoded buffer size.
 *
 * This routine calculates the expected Base64 buffer size
 * for the given raw data size.
 *
 * Note that the returned size includes the string termination character ('\0'),
 * so the size of the actual data is one character less.
 *
 * @param[in] inputLength Raw buffer size
 *
 * @return Encoded data size.
*/
uint32_t Calypso_GetBase64EncBufSize(uint32_t inputLength)
{
    /* Data size plus string termination character */
    return (4 * ((inputLength + 2) / 3)) + 1;
}

/**
 * @brief Decode Base64 data.
 *
 * This routine decodes the supplied data in Base64 format to raw data
 * and writes it to the output buffer (outputData, must be pre-allocated).
 * The size of the output buffer is also returned.
 *
 * Note that the data is returned as a null terminated string, the destination
 * buffer must have size Calypso_GetBase64DecBufSize(inputData, inputLength).
 *
 * @param[in] inputData Source buffer (holding the Base64 data to be decoded)
 * @param[in] inputLength Source buffer size
 * @param[out] outputData Destination buffer (will contain the decoded data)
 * @param[out] outputLength Decoded data size
 *
 * @return true if successful, false otherwise
*/
bool Calypso_DecodeBase64(uint8_t *inputData,
                          uint32_t inputLength,
                          uint8_t *outputData,
                          uint32_t *outputLength)
{
    *outputLength = 0;

    uint32_t decode_value;
    uint32_t nibble6_1, nibble6_2, nibble6_3, nibble6_4;
    uint32_t i, j;

    if (inputLength % 4 != 0)
    {
        return false;
    }

    if (outputData == NULL)
    {
        return false;
    }

    *outputLength = Calypso_GetBase64DecBufSize(inputData, inputLength);

    for (i = 0, j = 0; i < inputLength;)
    {
        nibble6_1 = inputData[i] == '=' ? 0 & i++ : Calypso_base64DecTable[inputData[i++]];
        nibble6_2 = inputData[i] == '=' ? 0 & i++ : Calypso_base64DecTable[inputData[i++]];
        nibble6_3 = inputData[i] == '=' ? 0 & i++ : Calypso_base64DecTable[inputData[i++]];
        nibble6_4 = inputData[i] == '=' ? 0 & i++ : Calypso_base64DecTable[inputData[i++]];

        decode_value = (nibble6_1 << 3 * 6) + (nibble6_2 << 2 * 6) +
                (nibble6_3 << 1 * 6) + (nibble6_4 << 0 * 6);

        if (j < *outputLength - 1)
        {
            outputData[j++] = (decode_value >> 2 * 8) & 0xFF;
        }
        if (j < *outputLength - 1)
        {
            outputData[j++] = (decode_value >> 1 * 8) & 0xFF;
        }
        if (j < *outputLength - 1)
        {
            outputData[j++] = (decode_value >> 0 * 8) & 0xFF;
        }
    }
    outputData[j] = 0;

    return true;
}

/**
 * @brief Encodes raw data to Base64 format.
 *
 * This routine encodes the supplied raw data to base64 format and writes it to the
 * output buffer (outputData, must be pre-allocated).
 * The size of the output buffer is also returned.
 *
 * Note that the data is returned as a null terminated string, the destination
 * buffer must have size Calypso_GetBase64EncBufSize(inputLength).
 *
 * @param[in] inputData Source buffer holding the raw data
 * @param[in] inputLength Source buffer size
 * @param[out] outputData Destination buffer (will contain the Base64 encoded data)
 * @param[out] outputLength Encoded data size
 *
 * @return true if successful, false otherwise
 */
bool Calypso_EncodeBase64(uint8_t *inputData,
                          uint32_t inputLength,
                          uint8_t *outputData,
                          uint32_t *outputLength)
{
    uint32_t encodeValue;
    uint32_t nibble6_1, nibble6_2, nibble6_3;
    uint32_t i, j;

    *outputLength = Calypso_GetBase64EncBufSize(inputLength);

    if (outputData == NULL)
    {
        return false;;
    }

    for (i = 0, j = 0; i < inputLength;)
    {
        nibble6_1 = i < inputLength ? inputData[i++] : 0;
        nibble6_2 = i < inputLength ? inputData[i++] : 0;
        nibble6_3 = i < inputLength ? inputData[i++] : 0;

        encodeValue = (nibble6_1 << 0x10) + (nibble6_2 << 0x08) + nibble6_3;

        outputData[j++] = Calypso_base64EncTable[(encodeValue >> 3 * 6) & 0x3F];
        outputData[j++] = Calypso_base64EncTable[(encodeValue >> 2 * 6) & 0x3F];
        outputData[j++] = Calypso_base64EncTable[(encodeValue >> 1 * 6) & 0x3F];
        outputData[j++] = Calypso_base64EncTable[(encodeValue >> 0 * 6) & 0x3F];
    }

    if (inputLength % 3 >= 1)
    {
        outputData[*outputLength - 2] = '=';
    }
    if (inputLength % 3 == 1)
    {
        outputData[*outputLength - 3] = '=';
    }

    outputData[*outputLength - 1] = 0;

    return true;
}

void WE_UART_HandleRxByte(uint8_t receivedByte)
{
    Calypso_byteRxCallback(receivedByte);
}

/**
 * @brief Default byte received callback.
 *
 * Is called when a single byte has been received.
 *
 * @param[in] receivedByte The received byte.
 */
static void Calypso_HandleRxByte(uint8_t receivedByte)
{
    /* Interpret received byte */
    if (Calypso_rxByteCounter == 0)
    {
        /* This is the first character - i.e. the start of a new line */
        /* Possible responses: OK, Error, +[event], +[cmdResponse] */
        if (('O' == receivedByte) || ('o' == receivedByte) ||
                ('E' == receivedByte) || ('e' == receivedByte) ||
                ('+' == receivedByte))
        {
            Calypso_rxBuffer[Calypso_rxByteCounter] = receivedByte;
            Calypso_rxByteCounter++;
        }
    }
    else
    {
        if (Calypso_rxByteCounter >= CALYPSO_LINE_MAX_SIZE)
        {
            Calypso_rxByteCounter = 0;
            Calypso_eolChar1Found = false;
            return;
        }

        if (receivedByte == Calypso_eolChar1)
        {
            Calypso_eolChar1Found = true;

            if (!Calypso_twoEolCharacters)
            {
                /* Interpret it now */
                Calypso_rxBuffer[Calypso_rxByteCounter] = '\0';
                Calypso_rxByteCounter++;
                Calypso_HandleRxLine(Calypso_rxBuffer, Calypso_rxByteCounter);
                Calypso_eolChar1Found = false;
                Calypso_rxByteCounter = 0;
            }
        }
        else if (Calypso_eolChar1Found)
        {
            if (receivedByte == Calypso_eolChar2)
            {
                /* Interpret it now */
                Calypso_rxBuffer[Calypso_rxByteCounter] = '\0';
                Calypso_rxByteCounter++;
                Calypso_HandleRxLine(Calypso_rxBuffer, Calypso_rxByteCounter);
                Calypso_eolChar1Found = false;
                Calypso_rxByteCounter = 0;
            }
        }
        else
        {
            Calypso_rxBuffer[Calypso_rxByteCounter++] = receivedByte;
        }
    }
}

/**
 * @brief Is called when a complete line has been received.
 *
 * @param[in] rxPacket Received text
 * @param[in] rxLength Received text length
 */
static void Calypso_HandleRxLine(char *rxPacket, uint16_t rxLength)
{
#ifdef WE_DEBUG
    fprintf(stdout, "< %s\r\n", rxPacket);
#endif

    /* Check if a custom line rx callback is specified and call it if so */
    if (NULL != Calypso_lineRxCallback)
    {
        /* Custom callback returns true if the line has been handled */
        bool handled = Calypso_lineRxCallback(rxPacket, rxLength);
        if (handled)
        {
            return;
        }
    }
    if (Calypso_requestPending)
    {
        /* AT command was sent to module. Waiting for response. */

        /* If starts with 'O', check if response is "OK\r\n" */
        if (('O' == rxPacket[0]) || ('o' == rxPacket[0]))
        {
            if (0 == strncasecmp(&rxPacket[0], CALYPSO_RESPONSE_OK, strlen(CALYPSO_RESPONSE_OK)))
            {
                Calypso_cmdConfirmStatus = Calypso_CNFStatus_Success;
            }
            else
            {
                Calypso_cmdConfirmStatus = Calypso_CNFStatus_Failed;
            }
        }
        /* If starts with 'E', check if response is "Error:[arguments]\r\n" */
        else if (('E' == rxPacket[0]) || ('e' == rxPacket[0]))
        {
            if (!(0 == strncasecmp(&rxPacket[0], CALYPSO_RESPONSE_ERROR, strlen(CALYPSO_RESPONSE_ERROR))))
            {

                Calypso_cmdConfirmStatus = Calypso_CNFStatus_Success;
            }
            else
            {
                char* packetPtr = rxPacket;
                Calypso_GetNextArgumentString(&packetPtr, Calypso_lastErrorText, ':', sizeof(Calypso_lastErrorText));

                while (*packetPtr == ' ')
                {
                    packetPtr++;
                }

                Calypso_GetNextArgumentString(&packetPtr, Calypso_lastErrorText, CALYPSO_ARGUMENT_DELIM, sizeof(Calypso_lastErrorText));
                Calypso_GetNextArgumentInt(&packetPtr,
                                           &Calypso_lastErrorCode,
                                           CALYPSO_INTFLAGS_NOTATION_DEC | CALYPSO_INTFLAGS_SIGNED | CALYPSO_INTFLAGS_SIZE32,
                                           CALYPSO_STRING_TERMINATE);

                Calypso_cmdConfirmStatus = Calypso_CNFStatus_Failed;
            }
        }
        else
        {
            /* Doesn't start with o or e - copy to response text buffer, if the start
             * of the response matches the pending command name preceded by '+' */
            if (rxLength < CALYPSO_LINE_MAX_SIZE &&
                    rxLength > 1 &&
                    Calypso_rxBuffer[0] == '+' &&
                    Calypso_pendingCommandName[0] != '\0' &&
                    0 == strncasecmp(Calypso_pendingCommandName, Calypso_rxBuffer + 1, Calypso_pendingCommandNameLength))
            {
                /* Copy to response text buffer, taking care not to exceed buffer size */
                uint16_t chunkLength = rxLength;
                if (Calypso_currentResponseLength + chunkLength >= CALYPSO_MAX_RESPONSE_TEXT_LENGTH)
                {
                    chunkLength = CALYPSO_MAX_RESPONSE_TEXT_LENGTH - Calypso_currentResponseLength;
                }
                memcpy(&Calypso_currentResponseText[Calypso_currentResponseLength], Calypso_rxBuffer, chunkLength);
                Calypso_currentResponseLength += chunkLength;
            }
        }
    }

    if ('+' == rxPacket[0])
    {
        /* An event occurred. Execute callback (if specified). */
        if (NULL != Calypso_eventCallback)
        {
            Calypso_executingEventCallback = true;
            Calypso_eventCallback(Calypso_rxBuffer);
            Calypso_executingEventCallback = false;
        }
    }
}

/**
 * @brief Sets the callback function which is executed if a byte has been received from Calypso.
 *
 * The default callback is Calypso_HandleRxByte().
 *
 * @param[in] callback Pointer to byte received callback function (default callback is used if NULL)
 */
void Calypso_SetByteRxCallback(Calypso_ByteRxCallback_t callback)
{
    Calypso_byteRxCallback = (callback == NULL) ? Calypso_HandleRxByte : callback;
}

/**
 * @brief Sets an optional callback function which is executed if a line has been received from Calypso.
 * Can be used to intercept responses from Calypso.
 *
 * The callback function must return true if the line should not be processed by the driver
 * and false if the driver should process the line as usual.
 *
 * @param[in] callback Pointer to line received callback function
 */
void Calypso_SetLineRxCallback(Calypso_LineRxCallback_t callback)
{
    Calypso_lineRxCallback = callback;
}

/**
 * @brief Sets EOL character(s) used for interpreting responses from Calypso.
 *
 * @param[in] eol1 First EOL character
 * @param[in] eol2 Second EOL character (is only used if twoEolCharacters is true)
 * @param[in] twoEolCharacters Controls whether the two EOL characters eol1 and eol2 (true) or only eol1 (false) is used
 */
void Calypso_SetEolCharacters(uint8_t eol1, uint8_t eol2, bool twoEolCharacters)
{
    Calypso_eolChar1 = eol1;
    Calypso_eolChar2 = eol2;
    Calypso_twoEolCharacters = twoEolCharacters;
}
