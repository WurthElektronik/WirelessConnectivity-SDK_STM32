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
 * @file DaphnisI.h
 * @brief DaphnisI driver header file.
 */

#ifndef DAPHNISI_H_INCLUDED
#define DAPHNISI_H_INCLUDED

#include <global/global_types.h>
#include <stdbool.h>

/**
 * @brief Minimum firmware version of the module that is supported by the driver.
 * @attention This can be overriden to a different firmware version
 */
#ifndef DAPHNISI_MIN_FW_VER
#define DAPHNISI_MIN_FW_VER FW(1, 4, 0)
#endif

/**
 * @brief Default UART baudrate used by the module
 */
#define DAPHNISI_DEFAULT_BAUDRATE (uint32_t)9600

/**
 * @brief Start up time of the module in milliseconds
 */
#define DAPHNISI_STARTUP_TIME (uint32_t)1400

/**
 * @brief Length of device address in bytes
 */
#define DAPHNISI_ADDRESS_LENGTH 4

/**
 * @brief Device Address
 */
typedef uint8_t DaphnisI_Device_Address_t[DAPHNISI_ADDRESS_LENGTH];

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Defines the pin of Daphnis-I
 */
typedef struct DaphnisI_Pins_t
{
    WE_Pin_t DaphnisI_Pin_Reset;  /**< Reset pin */
    WE_Pin_t DaphnisI_Pin_WakeUp; /**< Wakeup pin */
    WE_Pin_t DaphnisI_Pin_Boot;   /**< Boot pin */
} DaphnisI_Pins_t;

/**
 * @brief AT command confirmation status.
 */
typedef enum DaphnisI_CNFStatus_t
{
    DaphnisI_CNFStatus_Success, /**< Success Status */
    DaphnisI_CNFStatus_Failed,  /**< Failed Status */
    DaphnisI_CNFStatus_Invalid, /**< Invalid Status */
    /** @cond DOXYGEN_IGNORE */
    DaphnisI_CNFStatus_NumberOfValues
    /** @endcond */
} DaphnisI_CNFStatus_t;

/**
 * @brief AT command error messages.
 */
typedef enum DaphnisI_ErrorMessage_t
{
    DaphnisI_ErrorMessage_ERROR,                /**< Error */
    DaphnisI_ErrorMessage_PARAM_ERROR,          /**< Invalid parameters */
    DaphnisI_ErrorMessage_BUSY_ERROR,           /**< Busy */
    DaphnisI_ErrorMessage_TEST_PARAM_OVERFLOW,  /**< Module UART buffer full */
    DaphnisI_ErrorMessage_NO_NET_JOINED,        /**< No LoRaWAN network joined */
    DaphnisI_ErrorMessage_RX_ERROR,             /**< UART error while receiving the command */
    DaphnisI_ErrorMessage_DUTYCYCLE_RESTRICTED, /**< Duty cycle restriction */
    DaphnisI_ErrorMessage_CRYPTO_ERROR,         /**< Failed to encypt message */
#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)
    DaphnisI_ErrorMessage_INVALID_MODE, /**< Invalid @ref DaphnisI_Mode_t */
    DaphnisI_ErrorMessage_INVALID_ROLE, /**< Invalid @ref DaphnisI_P2P_Role_t */
#endif
    /** @cond DOXYGEN_IGNORE */
    DaphnisI_ErrorMessage_Count,
    DaphnisI_ErrorMessage_Invalid
    /** @endcond */
} DaphnisI_ErrorMessage_t;

/**
 * @brief Timeout categories (for responses to AT commands).
 * @see DaphnisI_SetTimeout(), DaphnisI_GetTimeout()
 */
typedef enum DaphnisI_Timeout_t
{
    DaphnisI_Timeout_General, /**< General timeout */
#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)
    DaphnisI_Timeout_P2P_Data,        /**< P2P data timeout */
    DaphnisI_Timeout_P2P_Remote_GPIO, /**< P2P remote gpio timeout */
#endif
    /** @cond DOXYGEN_IGNORE */
    DaphnisI_Timeout_NumberOfValues
    /** @endcond */
} DaphnisI_Timeout_t;

/**
 * @brief AT event IDs.
 */
typedef enum DaphnisI_ATEvent_t
{
    DaphnisI_ATEvent_Invalid = (uint16_t)0, /**< Invalid event */

    // General events
    DaphnisI_ATEvent_StartUp, /**< Startup event */

    // LoRaWAN events
    DaphnisI_ATEvent_LoRaWAN_RxInfo,                /**< LoRaWAN RX info received event (Use @ref DaphnisI_LoRaWAN_ParseRxInfoEvent to parse data) */
    DaphnisI_ATEvent_LoRaWAN_RxData,                /**< LoRaWAN RX data received event (Use @ref DaphnisI_LoRaWAN_ParseRxDataEvent to parse data) */
    DaphnisI_ATEvent_LoRaWAN_TxConf,                /**< LoRaWAN TX confirmation event */
    DaphnisI_ATEvent_LoRaWAN_Class,                 /**< LoRaWAN class change event (Use @ref DaphnisI_LoRaWAN_ParseClassEvent to parse data) */
    DaphnisI_ATEvent_LoRaWAN_BeaconInfo,            /**< LoRaWAN beacon info event (Use @ref DaphnisI_LoRaWAN_ParseBeaconInfoEvent to parse data) */
    DaphnisI_ATEvent_LoRaWAN_BeaconLost,            /**< LoRaWAN beacon lost event */
    DaphnisI_ATEvent_LoRaWAN_BeaconNotReceived,     /**< LoRaWAN beacon not received event */
    DaphnisI_ATEvent_LoRaWAN_ContextState_Stored,   /**< LoRaWAN context stored event */
    DaphnisI_ATEvent_LoRaWAN_ContextState_Restored, /**< LoRaWAN context restored event */
    DaphnisI_ATEvent_LoRaWAN_JoinState_Success,     /**< LoRaWAN successfull join event */
    DaphnisI_ATEvent_LoRaWAN_JoinState_Fail,        /**< LoRaWAN failed join event */

#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)
    // P2P events
    DaphnisI_ATEvent_P2P_TxConf_Success,                 /**< P2P TX success confirmation event */
    DaphnisI_ATEvent_P2P_TxConf_Error,                   /**< P2P TX error event */
    DaphnisI_ATEvent_P2P_TxConf_Busy,                    /**< P2P TX busy event */
    DaphnisI_ATEvent_P2P_TxTime,                         /**< P2P TX time on air event (Use @ref DaphnisI_P2P_ParseTXTimeEvent to parse data) */
    DaphnisI_ATEvent_P2P_TxResp_Success,                 /**< P2P TX success response event */
    DaphnisI_ATEvent_P2P_TxResp_Fail_Timeout,            /**< P2P TX failed due to timeout response event */
    DaphnisI_ATEvent_P2P_TxResp_Fail_NACK_Received,      /**< P2P TX failed due to NACK response event */
    DaphnisI_ATEvent_P2P_TxResp_Fail_Internal_Error,     /**< P2P TX failed due to internal error response event */
    DaphnisI_ATEvent_P2P_RxData,                         /**< P2P RX data received event (Use @ref DaphnisI_P2P_ParseRXDataEvent to parse data) */
    DaphnisI_ATEvent_P2P_GPIO_Remote_Cfg_Set_Response,   /**< P2P GPIO remote configuration set response event (Use @ref DaphnisI_P2P_GPIO_Remote_ParseCfgSetResponseEvent to parse data) */
    DaphnisI_ATEvent_P2P_GPIO_Remote_Cfg_Get_Response,   /**< P2P GPIO remote configuration get response event (Use @ref DaphnisI_P2P_GPIO_Remote_ParseCfgGetResponseEvent to parse data) */
    DaphnisI_ATEvent_P2P_GPIO_Remote_Value_Set_Response, /**< P2P GPIO remote value set response event (Use @ref DaphnisI_P2P_GPIO_Remote_ParseValueSetResponseEvent to parse data) */
    DaphnisI_ATEvent_P2P_GPIO_Remote_Value_Get_Response, /**< P2P GPIO remote value get response event (Use @ref DaphnisI_P2P_GPIO_Remote_ParseValueSetResponseEvent to parse data) */
    DaphnisI_ATEvent_P2P_GPIO_Remote_Cfg_Changed,        /**< P2P GPIO remote configuration changed event (Use @ref DaphnisI_P2P_GPIO_Remote_ParseCfgChangedEvent to parse data) */
    DaphnisI_ATEvent_P2P_GPIO_Remote_Value_Changed,      /**< P2P GPIO remote value changed event (Use @ref DaphnisI_P2P_GPIO_Remote_ParseValueChangedEvent to parse data) */
#endif
    /** @cond DOXYGEN_IGNORE */
    DaphnisI_ATEvent_NumberOfValues,
    DaphnisI_ATEvent_Max = UINT16_MAX
    /** @endcond */
} DaphnisI_ATEvent_t;

/**
 * @brief DaphnisI event callback.
 * 
 * Called when one of the events in @ref DaphnisI_ATEvent_t is received
 * 
 * @param[in] event_type: Type of event
 * @param[in] event_data: Data of the event
 */
typedef void (*DaphnisI_EventCallback_t)(DaphnisI_ATEvent_t event_type, char* event_data);

/**
 * @brief Initializes the serial communication with the module
 *
 * @param[in] uartP: Definition of the uart connected to the module
 * @param[in] pinoutP: Definition of the gpios connected to the module
 * @param[in] eventCallback: Function pointer to event handler (optional)

 * @return True if successful, false otherwise
 */
extern bool DaphnisI_Init(WE_UART_t* uartP, DaphnisI_Pins_t* pinoutP, DaphnisI_EventCallback_t eventCallback);

/**
 * @brief Deinitializes the serial communication with the module.
 *
 * @return True if successful, false otherwise
 */
extern bool DaphnisI_Deinit(void);

/**
 * @brief Performs a reset of the module using the reset pin.
 *
 * @return True if successful, false otherwise
 */
extern bool DaphnisI_PinReset(void);

/**
 * @brief Wakes the module up from power save mode using the wake up pin.
 *
 * @return True if successful, false otherwise
 */
extern bool DaphnisI_PinWakeUp(void);

/**
 * @brief Sets pin level to high or low.
 *
 * @param[in] pin: Output pin to be set
 * @param[in] level: Output level to be set
 *
 * @return True if successful, false otherwise
 */
extern bool DaphnisI_SetPin(WE_Pin_t pin, WE_Pin_Level_t level);

/**
 * @brief Sends the supplied AT command to the module
 *
 * @param[in] data: AT command to send. Note that the command has to end with "\r\n\0".
 *
 * @return True if successful, false otherwise
 */
extern bool DaphnisI_SendRequest(char* data);

/**
 * @brief Waits for the response from the module after a request.
 *
 * @param[in] maxTimeMs: Maximum wait time in milliseconds
 * @param[in] expectedStatus: Status to wait for
 * @param[out] pOutResponse: Received response text (if any) will be written to this buffer (optional)
 *
 * @return True if successful, false otherwise
 */
extern bool DaphnisI_WaitForConfirm(uint32_t maxTimeMs, DaphnisI_CNFStatus_t expectedStatus, char* pOutResponse);

/**
 * @brief Waits for an event from the module.
 *
 * @param[in] maxTimeMs: Maximum wait time in milliseconds
 * @param[in] expectedEvents: Events to wait for
 * @param[in] eventsLength: Length of events to wait for
 * @param[in] resetEventsBuffer: boolean to indicate if the internal events buffer should reset or not
 *
 * @return the event if successful, @ref DaphnisI_ATEvent_Invalid otherwise
 */
extern DaphnisI_ATEvent_t DaphnisI_WaitForEvents(uint32_t maxTimeMs, DaphnisI_ATEvent_t* expectedEvents, uint8_t eventsLength, bool resetEventsBuffer);

/**
 * @brief Returns the code of the last error (if any).
 *
 * @param[out] lastError: value of last error (if any).
 *
 * @return True if successful, false otherwise
 */
extern bool DaphnisI_GetLastError(DaphnisI_ErrorMessage_t* lastError);

/**
 * @brief Set timing parameters used by the DaphnisI driver.
 *
 * @note WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @param[in] waitTimeStepUsec: Time step (microseconds) when waiting for responses from DaphnisI.
 * @param[in] minCommandIntervalUsec: Minimum interval (microseconds) between subsequent commands sent to DaphnisI.
 *
 * @return True if successful, false otherwise
 */
bool DaphnisI_SetTimingParameters(uint32_t waitTimeStepUsec, uint32_t minCommandIntervalUsec);

/**
 * @brief Sets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type: Timeout (i.e. command) type
 * @param[in] timeout: Timeout in milliseconds
 */
extern void DaphnisI_SetTimeout(DaphnisI_Timeout_t type, uint32_t timeout);

/**
 * @brief Gets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type: Timeout (i.e. command) type
 *
 * @return Timeout in milliseconds
 */
extern uint32_t DaphnisI_GetTimeout(DaphnisI_Timeout_t type);

#ifdef __cplusplus
}
#endif

#endif // DAPHNISI_H_INCLUDED
