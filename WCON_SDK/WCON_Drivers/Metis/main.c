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
 * COPYRIGHT (c) 2023 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Metis example.
 */

#include <stdio.h>
#include <string.h>

#include "../../WCON_Drivers/Metis/Metis.h"
#include "../../WCON_Drivers/global/global.h"

static uint8_t APP_Data[140] = {
        0x48, /* Length Field */

        /*0*/
        0x44,                                                                                  /* C-Feld */
        0xA2,0x05,                                                                             /* M-Feld (AMB) */
        0x11,0x47,0x15,0x08,                                                                   /* ID (Funkmodul) */
        0x01,                                                                                  /* Version (2) */
        0x37,                                                                                  /* Radio Converter (meter side) */

        /*9*/
        0x72,                                                                                  /* CI-Feld (12 Byte Header) */
        0x78,0x56,0x34,0x12,                                                                   /* ID (Zähler) */
        0xA2,0x05,                                                                             /* M-Feld (AMB) */
        0x01,                                                                                  /* Version (1) */
        0x37,                                                                                  /* Radio Converter (meter side) */
        0x00,                                                                                  /* AccCounter */
        0x00,                                                                                  /* Statusbyte */
        0x00,                                                                                  /* Signaturwort, hier: encryption mode 5 ohne verlüsselte blöcke */
        0x05,

        /*22*/
        /*-*/
        0x2F,                                                                                  /* AES Verification / Idle Filler */
        0x2F,                                                                                  /* AES Verification / Idle Filler */

        /*24*/
        0x0E,                                                                                  /* DIV 12Stellige BCD Zahl */
        0x13,                                                                                  /* VIF Volumen in l */
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,

        /*32*/
        0x0D,                                                                                   /* DIV Variable Datenlänge */
        0xFD,                                                                                   /* VIF Zweite Erweiterungstabelle */
        0x11,                                                                                   /* VIFE Abnehmer */
        0x20,                                                                                   /* 23 Stellen ASCII String */
        'e',
        'd',
        /*-*/
        'o',
        'M',
        ' ',
        'S', /* = index 41 */
        ' ',
        ' ',
        'A',/* = index 44 */
        ' ',
        'F',
        'F',
        ' ',
        'H',
        'b',
        'm',
        'G',
        ' ',
        /*-*/
        's',
        's',
        'e',
        'l',
        'e',
        'r',
        'i',
        'w',
        ' ',
        'R',
        'E',
        'B',
        'M',
        'A',
        /*70*/

        0x2F,
        0x2F
    };

/**
 * @brief Callback for data reception
 */
static void RxCallback(uint8_t* payload, uint8_t payload_length, int8_t rssi)
{
    int i = 0;
    printf("Received data with %d dBm:\n-> ", rssi);
    printf("0x ");
    for(i=0; i<payload_length; i++)
    {
        printf("%02x ", *(payload+i));
    }
    printf("\n-> ");
    for(i=0; i<payload_length; i++)
    {
        printf("%c", *(payload+i));
    }
    printf("\n");
    fflush(stdout);
}

/**
 * @brief The application's main function.
 */
int main(void)
{
    /* Initialize platform (peripherals, flash interface, Systick, system clock) */
    WE_Platform_Init();

#ifdef WE_DEBUG
    WE_Debug_Init();
#endif

    uint8_t driverVersion[3];
    WE_GetDriverVersion(driverVersion);
    printf("Wuerth Elektronik eiSos Wireless Connectivity SDK version %d.%d.%d\r\n", driverVersion[0], driverVersion[1], driverVersion[2]);

    Metis_Init(9600, WE_FlowControl_NoFlowControl, MBus_Frequency_868, MBus_Mode_868_S2, true, RxCallback);

    while (1)
    {
        uint8_t serial_number[4];
        memset(serial_number,0,sizeof(serial_number));
        bool ret = Metis_GetSerialNumber(serial_number);
        WE_Delay(500);

        uint8_t firmware_version[3];
        memset(firmware_version,0,sizeof(firmware_version));
        ret = Metis_GetFirmwareVersion(firmware_version);
        WE_Delay(500);

        ret = Metis_Transmit(APP_Data);
        WE_Delay(500);

        ret = Metis_PinReset();
        WE_Delay(500);

        ret = Metis_Reset();
        WE_Delay(500);
    }
}
