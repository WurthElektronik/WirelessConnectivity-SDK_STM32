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
 * @brief Tarvos-III example.
 */

#include <TarvosIII/TarvosIII.h>
#include <global/global.h>
#include <global_platform_types.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Definition of the pins
 */
static TarvosIII_Pins_t TarvosIII_pins = {
    .TarvosIII_Pin_Reset = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_10)),
    .TarvosIII_Pin_SleepWakeUp = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_9)),
    .TarvosIII_Pin_Boot = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_7)),
    .TarvosIII_Pin_Mode = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_8)),
};

/**
 * @brief Definition of the uart
 */
static WE_UART_t TarvosIII_uart;

/* Pick the example to be executed in the main function. */
static void CommandModeExample();

/**
 * @brief Prints the supplied string, prefixed with OK or NOK (depending on the success parameter).
 *
 * @param str String to print
 * @param success Variable indicating if action was ok
 */
static void Examples_Print(char* str, bool success) { WE_DEBUG_PRINT("%s%s\r\n", success ? "OK    " : "NOK   ", str); }

/**
 * @brief Callback called when data has been received via radio
 */
static void RxCallback(uint8_t* payload, uint8_t payload_length, uint8_t dest_network_id, uint8_t dest_address_lsb, uint8_t dest_address_msb, int8_t rssi)
{
    uint8_t i = 0;
    WE_DEBUG_PRINT("Received data from address (NetID:0x%02x,Addr:0x%02x%02x) with %d dBm:\n-> ", dest_network_id, dest_address_lsb, dest_address_msb, rssi);
    WE_DEBUG_PRINT("0x");
    for (i = 0; i < payload_length; i++)
    {
        WE_DEBUG_PRINT("%02x", *(payload + i));
    }
    WE_DEBUG_PRINT(" (");
    for (i = 0; i < payload_length; i++)
    {
        WE_DEBUG_PRINT("%c", *(payload + i));
    }
    WE_DEBUG_PRINT(")\r\n");
}

void TarvosIII_Examples(void)
{

    TarvosIII_uart.baudrate = TARVOSIII_DEFAULT_BAUDRATE;
    TarvosIII_uart.flowControl = WE_FlowControl_NoFlowControl;
    TarvosIII_uart.parity = WE_Parity_None;
    TarvosIII_uart.uartInit = WE_UART1_Init;
    TarvosIII_uart.uartDeinit = WE_UART1_DeInit;
    TarvosIII_uart.uartTransmit = WE_UART1_Transmit;

    CommandModeExample();
}

/**
 * @brief Command mode example repeatedly transmitting data via radio
 */
void CommandModeExample(void)
{
    if (false == TarvosIII_Init(&TarvosIII_uart, &TarvosIII_pins, TarvosIII_AddressMode_0, RxCallback))
    {
        WE_DEBUG_PRINT("Initialization error\r\n");
        return;
    }

    uint8_t serialNr[4];
    Examples_Print("Read serial number", TarvosIII_GetSerialNumber(serialNr));
    WE_DEBUG_PRINT("Serial number is 0x%02x%02x%02x%02x\r\n", serialNr[0], serialNr[1], serialNr[2], serialNr[3]);
    WE_Delay(500);

    uint8_t fwVersion[3];
    Examples_Print("Read firmware version", TarvosIII_GetFirmwareVersion(fwVersion));
    WE_DEBUG_PRINT("Firmware version is %u.%u.%u\r\n", fwVersion[0], fwVersion[1], fwVersion[2]);
    WE_Delay(500);

    uint8_t data[4 * 16];
    for (uint16_t i = 0; i < sizeof(data); i++)
    {
        data[i] = (uint8_t)i;
    }

    while (1)
    {
        if (false == TarvosIII_Transmit(data, sizeof(data)))
        {
            WE_DEBUG_PRINT("Transmission error\r\n");
        }
        WE_Delay(500);
    }

    return;
}
