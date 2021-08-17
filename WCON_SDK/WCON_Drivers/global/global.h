/**
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
 * COPYRIGHT (c) 2021 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 **/


#include <stdbool.h>
#include "stm32l0xx_hal.h"

#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

/******************
 * driver version *
 ******************/
#define WIRELESS_CONNECTIVITY_SDK_VERSION {1,0,0}

#define fprintf(...)
#define  printf(...)
#define  fflush(...)

typedef struct Pin_t {
	GPIO_TypeDef *port;
	uint32_t pin;
} Pin_t;

typedef enum SetPin_InputOutput_t {
    SetPin_InputOutput_Input   = (uint8_t) 0,
    SetPin_InputOutput_Output  = (uint8_t) 1,
} SetPin_InputOutput_t;

typedef enum SetPin_Pull_t {
    SetPin_Pull_No   = (uint8_t) 0,
    SetPin_Pull_Up   = (uint8_t) 1,
    SetPin_Pull_Down = (uint8_t) 2,
} SetPin_Pull_t;

typedef enum SetPin_Out_t {
    SetPin_Out_Low  = (uint8_t) 0,
    SetPin_Out_High = (uint8_t) 1,
} SetPin_Out_t;

typedef enum FlowControl_t {
	No_flow_control  = (uint8_t) 0,
	RTS_Only         = (uint8_t) 1,
	CTS_Only         = (uint8_t) 2,
	RTS_and_CTS      = (uint8_t) 3,
} FlowControl_t;

extern UART_HandleTypeDef huart1;

/*
 *Initialize and start the UART
 *
 *input:
 * -baudrate:     baudrate of the serial interface
 * -flow_control: enable/disable flowcontrol
 *
 *return None
 */
extern void UART_Init(uint32_t baudrate, FlowControl_t flow_control);

/*
 *Deinitialize and stop the UART
 *
 *return None
 */
extern void UART_DeInit();

/*
 *Handle a single byte received via UART
 *
 *input:
 * -received_byte:  the received byte
 *
 *return None
 */
extern void UART_HandleRxByte(uint8_t received_byte);

/*
 *Transmit data via UART
 *
 *input:
 * -dataP:  pointer to the data buffer
 * -length: length of the data buffer
 *
 *return None
 */
extern void UART_Transmit(uint8_t *dataP, uint16_t length);

/*
 *Handling an error
 *
 *return None
 */
extern void Error_Handler(void);

/*
 *Configure the system clock
 *
 *return None
 */
extern void SystemClock_Config(void);

/*
 *Request the 3 byte driver version
 *
 *output:
 * -version: pointer to the 3 byte driver version
 *
 *return true if request succeeded
 *       false otherwise
 */
extern bool GetDriverVersion(uint8_t* version);


/* Switch pin to output high/low
 * input
 * - pin_number: number of pin
 * - out: output level high or low
 *
 * return: true, if success
 *        false, otherwise
*/
extern bool SetPin( Pin_t pin, SetPin_Out_t out);

/*Read out the pin level
 *input:
 *- pin_number: number of pin
 *
 * return: current level of pin
 */
extern SetPin_Out_t GetPinLevel(Pin_t pin);

/*
 * Sleep function for delay
 *
 * input:
 * - sleepFor: Time to delay in ms
 */
extern void delay(uint16_t sleepFor);

/*
 * Deinitialize a pin
 *
 * input:
 * - pin_number: pin number
 *
 * return: true, if success
 *         false, otherwise
 *
 */
extern bool DeinitPin(Pin_t pin);

#endif /* GLOBAL_H_INCLUDED */
