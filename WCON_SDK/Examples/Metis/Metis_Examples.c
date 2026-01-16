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
 * @brief Metis example.
 */

#include <Metis/Metis.h>
#include <global/global.h>
#include <global_platform.h>
#include <print.h>
#include <stdio.h>
#include <string.h>

/* Pick the example to be executed in the main function. */
static void CommandModeExample();

/**
 * @brief Example wMBUS frame
 */
static uint8_t APP_Data[140] = {0x48, /* Length Field */

                                /*0*/
                                0x44,                   /* C-Feld */
                                0xA2, 0x05,             /* M-Feld (AMB) */
                                0x11, 0x47, 0x15, 0x08, /* ID (Funkmodul) */
                                0x01,                   /* Version (2) */
                                0x37,                   /* Radio Converter (meter side) */

                                /*9*/
                                0x72,                   /* CI-Feld (12 Byte Header) */
                                0x78, 0x56, 0x34, 0x12, /* ID (Zähler) */
                                0xA2, 0x05,             /* M-Feld (AMB) */
                                0x01,                   /* Version (1) */
                                0x37,                   /* Radio Converter (meter side) */
                                0x00,                   /* AccCounter */
                                0x00,                   /* Statusbyte */
                                0x00,                   /* Signaturwort, hier: encryption mode 5 ohne verlüsselte blöcke */
                                0x05,

                                /*22*/
                                /*-*/
                                0x2F, /* AES Verification / Idle Filler */
                                0x2F, /* AES Verification / Idle Filler */

                                /*24*/
                                0x0E, /* DIV 12Stellige BCD Zahl */
                                0x13, /* VIF Volumen in l */
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

                                /*32*/
                                0x0D, /* DIV Variable Datenlänge */
                                0xFD, /* VIF Zweite Erweiterungstabelle */
                                0x11, /* VIFE Abnehmer */
                                0x20, /* 23 Stellen ASCII String */
                                'e', 'd',
                                /*-*/
                                'o', 'M', ' ', 'S', /* = index 41 */
                                ' ', ' ', 'A',      /* = index 44 */
                                ' ', 'F', 'F', ' ', 'H', 'b', 'm', 'G', ' ',
                                /*-*/
                                's', 's', 'e', 'l', 'e', 'r', 'i', 'w', ' ', 'R', 'E', 'B', 'M', 'A',
                                /*70*/

                                0x2F, 0x2F};

/**
 * @brief Definition of the pins
 */
Metis_Pins_t Metis_pins = {
    .Metis_Pin_Reset = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_10)),
};

/**
 * @brief Definition of the uart
 */
WE_UART_t Metis_uart;

/**
 * @brief Prints the supplied string, prefixed with OK or NOK (depending on the success parameter).
 *
 * @param str String to print
 * @param success Variable indicating if action was ok
 */
static void Examples_Print(char* str, bool success) { WE_APP_PRINT("%s%s\r\n", success ? "OK    " : "NOK   ", str); }

/**
 * @brief Callback for data reception
 */
static void RxCallback(uint8_t* payload, uint8_t payload_length, int8_t rssi)
{
    uint8_t i = 0;
    WE_APP_PRINT("Received data with %d dBm:\n-> ", rssi);
    WE_APP_PRINT("0x");
    for (i = 0; i < payload_length; i++)
    {
        WE_APP_PRINT("%02x", *(payload + i));
    }
    WE_APP_PRINT(" (");
    for (i = 0; i < payload_length; i++)
    {
        WE_APP_PRINT("%c", *(payload + i));
    }
    WE_APP_PRINT(")\r\n");
}

void Metis_Examples(void)
{

    Metis_uart.baudrate = METIS_DEFAULT_BAUDRATE;
    Metis_uart.flowControl = WE_FlowControl_NoFlowControl;
    Metis_uart.parity = WE_Parity_None;
    Metis_uart.uartInit = WE_UART1_Init;
    Metis_uart.uartDeinit = WE_UART1_DeInit;
    Metis_uart.uartTransmit = WE_UART1_Transmit;

    CommandModeExample();
}

/**
 * @brief Example repeatedly transmitting data
 */
static void CommandModeExample()
{
    if (false == Metis_Init(&Metis_uart, &Metis_pins, Metis_Frequency_868, Metis_Mode_Preselect_868_S2, true, RxCallback))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    uint8_t serialNr[4];
    Examples_Print("Read serial number", Metis_GetSerialNumber(serialNr));
    WE_APP_PRINT("Serial number is 0x%02x%02x%02x%02x\r\n", serialNr[0], serialNr[1], serialNr[2], serialNr[3]);
    WE_Delay(500);

    uint8_t fwVersion[3];
    Examples_Print("Read firmware version", Metis_GetFirmwareVersion(fwVersion));
    WE_APP_PRINT("Firmware version is %u.%u.%u\r\n", fwVersion[0], fwVersion[1], fwVersion[2]);
    WE_Delay(500);

    while (1)
    {
        if (!Metis_Transmit(APP_Data))
        {
            WE_APP_PRINT("Transmission failed\r\n");
        }
        WE_Delay(500);
    }

    return;
}
