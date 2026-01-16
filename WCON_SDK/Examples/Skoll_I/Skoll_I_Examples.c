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
 * @brief Skoll-I example.
 */

#include <Skoll_I/Skoll_I.h>
#include <Skoll_I/Skoll_I_BluetoothClassic_Examples.h>
#include <Skoll_I/Skoll_I_BluetoothLE_Examples.h>
#include <Skoll_I/Skoll_I_Examples_handler.h>
#include <global/global.h>
#include <global_platform.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Definition of the pins
 */
Skoll_I_Pins_t Skoll_I_pins = {
    .Skoll_I_Pin_Reset = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_10)),
    .Skoll_I_Pin_SPP = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_7)),
    .Skoll_I_Pin_Connection = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_9)),
    .Skoll_I_Pin_Role = WE_PIN((void*)&WE_STM32_PIN(GPIOA, GPIO_PIN_8)),
};

/**
 * @brief Definition of the uart
 */
WE_UART_t Skoll_I_uart;

static void Test_Compiler_Packing();
static void Test_System();
static void Test_Settings();

/**
 * @brief The application's main function.
 */
void Skoll_I_Examples(void)
{

    /* define and configure the UART */
    Skoll_I_uart.baudrate = SKOLL_I_DEFAULT_BAUDRATE;
    Skoll_I_uart.flowControl = WE_FlowControl_NoFlowControl;
#if 0
    /* use UART settings for high throughput */
    Skoll_I_uart.baudrate = 2666666;
    Skoll_I_uart.flowControl = WE_FlowControl_RTSAndCTS;
#endif
    Skoll_I_uart.parity = WE_Parity_None;
    Skoll_I_uart.uartInit = WE_UART1_Init;
    Skoll_I_uart.uartDeinit = WE_UART1_DeInit;
    Skoll_I_uart.uartTransmit = WE_UART1_Transmit;

#if 0
    /* reset */

    if (false == Skoll_I_Init(&Skoll_I_uart, &Skoll_I_pins, eventHandler, NULL))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    Skoll_I_PinReset();
    WE_Delay(1000);
    return;

#elif 0
    /* enables flow control and uses 2.666 MBaud */

    if (false == Skoll_I_Init(&Skoll_I_uart, &Skoll_I_pins, eventHandler, NULL))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    /* set UART values, 2.666 MBaud 8N1, flow control enabled */
    Examples_Print("system_get_uart_parameters", (EZS_SEND_AND_WAIT(ezs_cmd_system_get_uart_parameters(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    Examples_Print("system_set_uart_parameters 2.666 MBaud, flow control enabled ", (EZS_SEND_AND_WAIT(ezs_cmd_system_set_uart_parameters(2666666, 0, 0, 1, 8, 0, 1), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    WE_Delay(1000);
    return;
#elif 0
    /* factory reset */

    if (false == Skoll_I_Init(&Skoll_I_uart, &Skoll_I_pins, eventHandler, NULL))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }
    ezs_cmd_system_factory_reset();
    Examples_Print("system_factory_reset", (EZS_WAIT_FOR_EVENT(5000)) != (ezs_packet_t*)NULL);
    return;
#endif

    /* choose/uncomment the example you want to run */
    //Test_Compiler_Packing();
    //Test_System();
    //Test_Settings();
    Test_BluetoothClassic_SPP_TransparentConnection(Skoll_I_BluetoothClassic_Role_Slave);
    //Test_BluetoothClassic_SPP_Connection(Skoll_I_BluetoothClassic_Role_Slave);
    //Test_BluetoothClassic_SPP_Slave_Secure_Connection();
    //Test_BluetoothClassic_SPP_TransparentThroughput(Skoll_I_BluetoothClassic_Role_Slave);
    //Test_BluetoothLE_CYSPP_Connection(Skoll_I_BluetoothLE_Role_Peripheral);
    //Test_BluetoothLE_CYSPP_Throughput(Skoll_I_BluetoothLE_Role_Peripheral);
    //Test_BluetoothLE_ProteusSPPLike_Peripheral(1);
    //Test_BluetoothLE_ProteusSPPLike_Central();

    return;
}

/* This example tests all commands related to system */
static void Test_System()
{
    ezs_packet_t* packet;

    /* initialize the radio module */
    if (false == Skoll_I_Init(&Skoll_I_uart, &Skoll_I_pins, eventHandler, NULL))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    Examples_Print("system_ping", (packet = EZS_SEND_AND_WAIT(ezs_cmd_system_ping(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    /* print runtime debug data */
    Examples_Print("system_dump", (packet = EZS_SEND_AND_WAIT(ezs_cmd_system_dump(0), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    WE_Delay(2000);

    Examples_Print("system_query_firmware_version", (packet = EZS_SEND_AND_WAIT(ezs_cmd_system_query_firmware_version(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    Examples_Print("system_query_random_number", (packet = EZS_SEND_AND_WAIT(ezs_cmd_system_query_random_number(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    uint8a_t user_data = {.length = 3, .data = {0x12, 0x34, 0x56}};
    Examples_Print("system_write_user_data", (packet = EZS_SEND_AND_WAIT(ezs_cmd_system_write_user_data(0x10, 0, &user_data), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    Examples_Print("system_read_user_data", (packet = EZS_SEND_AND_WAIT(ezs_cmd_system_read_user_data(0x10, 0, 3), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    Examples_Print("system_get_transport", (packet = EZS_SEND_AND_WAIT(ezs_cmd_system_get_transport(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    uint8a_t event = {.length = 12, .data = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    Examples_Print("system_set_transport", (packet = EZS_SEND_AND_WAIT(ezs_cmd_system_set_transport(1, 17, 17, 17, 0x80, 0x80, 5, 0, &event), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
}

/* This example tests all commands related to settings */
static void Test_Settings()
{
    ezs_packet_t* packet;

    /* initialize the radio module */
    if (false == Skoll_I_Init(&Skoll_I_uart, &Skoll_I_pins, eventHandler, NULL))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    /* binary mode */
    //Examples_Print("protocol_set_parse_mode", (packet = EZS_SEND_AND_WAIT(ezs_cmd_protocol_set_parse_mode(1), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    //Examples_Print("protocol_get_parse_mode", (packet = EZS_SEND_AND_WAIT(ezs_cmd_protocol_get_parse_mode(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    /* echo off */
    //Examples_Print("protocol_set_echo_mode", (packet = EZS_SEND_AND_WAIT(ezs_cmd_protocol_set_echo_mode(0), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    //Examples_Print("protocol_get_echo_mode", (packet = EZS_SEND_AND_WAIT(ezs_cmd_protocol_get_echo_mode(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    //uint8_t whitelisted_address[6] = {0x00, 0x18, 0xDA, 0x34, 0xAA, 0x34};
    //Examples_Print("gap_add_whitelist_entry", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_add_whitelist_entry(whitelisted_address, 0), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    //Examples_Print("gap_query_whitelist", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_query_whitelist(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    //Examples_Print("gap_delete_whitelist_entry", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_delete_whitelist_entry(whitelisted_address, 0), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    Examples_Print("gap_query_whitelist", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_query_whitelist(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    Examples_Print("smp_query_bonds", (packet = EZS_SEND_AND_WAIT(ezs_cmd_smp_query_bonds(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    Examples_Print("smp_query_random_address", (packet = EZS_SEND_AND_WAIT(ezs_cmd_smp_query_random_address(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    Examples_Print("gattc_set_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gattc_set_parameters(1), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    Examples_Print("gattc_get_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gattc_get_parameters(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    Examples_Print("smp_set_privacy_mode", (packet = EZS_SEND_AND_WAIT(ezs_cmd_smp_set_privacy_mode(0x04, 60), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    Examples_Print("smp_get_privacy_mode", (packet = EZS_SEND_AND_WAIT(ezs_cmd_smp_get_privacy_mode(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    Examples_Print("smp_set_fixed_passkey", (packet = EZS_SEND_AND_WAIT(ezs_cmd_smp_set_fixed_passkey(0x1212), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    Examples_Print("smp_get_fixed_passkey", (packet = EZS_SEND_AND_WAIT(ezs_cmd_smp_get_fixed_passkey(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    Examples_Print("gatts_set_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gatts_set_parameters(1), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    Examples_Print("gatts_get_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gatts_get_parameters(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    /* set address */
    uint8_t address[6] = {0x00, 0x18, 0xDA, 0x11, 0x22, 0x33};
    Examples_Print("system_set_bluetooth_address", (packet = EZS_SEND_AND_WAIT(ezs_cmd_system_set_bluetooth_address(address), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    Examples_Print("system_get_bluetooth_address", (packet = EZS_SEND_AND_WAIT(ezs_cmd_system_get_bluetooth_address(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    /* EPDS if possible, no HID sleep time */
    Examples_Print("system_set_sleep_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_system_set_sleep_parameters(1, 0), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    Examples_Print("system_get_sleep_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_system_get_sleep_parameters(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    /* use maximum output power */
    Examples_Print("system_set_tx_power", (packet = EZS_SEND_AND_WAIT(ezs_cmd_system_set_tx_power(8), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    Examples_Print("system_get_tx_power", (packet = EZS_SEND_AND_WAIT(ezs_cmd_system_get_tx_power(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    Examples_Print("system_get_transport", (packet = EZS_SEND_AND_WAIT(ezs_cmd_system_get_transport(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    /* set UART values, 115200 8N1, no flow control */
    Examples_Print("system_set_uart_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_system_set_uart_parameters(115200, 0, 0, 0, 8, 0, 1), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    WE_Delay(1000);
    Examples_Print("system_get_uart_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_system_get_uart_parameters(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    /* set device name */
    uint8a_t name = {.length = 11, .data = "my name is"};
    Examples_Print("gap_set_device_name", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_set_device_name(0, &name), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    Examples_Print("gap_get_device_name", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_get_device_name(0), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    Examples_Print("gap_set_device_appearance", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_set_device_appearance(0), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    Examples_Print("gap_get_device_appearance", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_get_device_appearance(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    //Examples_Print("gap_set_adv_data", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_set_adv_data(data), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    Examples_Print("gap_get_adv_data", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_get_adv_data(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    //Examples_Print("gap_set_sr_data", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_set_sr_data(data), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    Examples_Print("gap_get_sr_data", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_get_sr_data(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    /* Undirected low duty, all channels, 60s with 40ms, then 5min with 40ms, auto-enable advertising disabled */
    Examples_Print("gap_set_adv_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_set_adv_parameters(0, 4, 7, 64, 60, 64, 300, 0, 0, 0), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    Examples_Print("gap_get_adv_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_get_adv_parameters(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    /* 160ms scan window and interval, active scan, accept all, filter double, stop after 10s */
    Examples_Print("gap_set_scan_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_set_scan_parameters(0, 256, 256, 1, 0, 1, 10), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    Examples_Print("gap_get_scan_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_get_scan_parameters(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    /* 25ms connection interval, slave latency off, 1s super vision timeout, 160ms scan window and interval, timeout 5s */
    Examples_Print("gap_set_conn_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_set_conn_parameters(20, 0, 100, 256, 256, 5), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    Examples_Print("gap_get_conn_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_get_conn_parameters(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    /* set device class to 0x00240417 */
    Examples_Print("bt_set_device_class", (packet = EZS_SEND_AND_WAIT(ezs_cmd_bt_set_device_class(0x00240417), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    Examples_Print("bt_get_device_class", (packet = EZS_SEND_AND_WAIT(ezs_cmd_bt_get_device_class(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    /* set parameters: timeout 20s, general discoverable, connectable, active */
    Examples_Print("bt_set_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_bt_set_parameters(0x7D00, 2, 1, 0, 2, 0, 0), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    Examples_Print("bt_get_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_bt_get_parameters(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

#if 0
    Examples_Print("system_store_config", (packet = EZS_SEND_AND_WAIT(ezs_cmd_system_store_config(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    WE_Delay(1000);

    ezs_cmd_system_factory_reset();
    Examples_Print("system_factory_reset", (packet = EZS_WAIT_FOR_EVENT(5000)) != (ezs_packet_t*)NULL);
#else
    if (false == Skoll_I_PinReset())
    {
        WE_APP_PRINT("Reset error\r\n");
        return;
    }
#endif
}

/* This function tests whether the compiler does the packing as supposed by the driver */
static void Test_Compiler_Packing()
{
    /* manually build "rsp_system_get_uart_parameters" response packet */
    uint8_t test_compiler_packing[] = {
        0xC0, 0x0C, 0x02, 0x1A, /* system_get_uart_parameters response header (0x1A020CC0 little-endian) */
        0x11, 0x22,             /* result = 0x2211 test value */
        0x00, 0xC2, 0x01, 0x00, /* baud = 0x0001C200 (115200) test value */
        0xAA,                   /* autobaud test value */
        0xBB,                   /* autocorrect test value */
        0xCC,                   /* flow test value */
        0xDD,                   /* databits test value */
        0xEE,                   /* parity test value */
        0xFF                    /* stopbits test value */
    };

    /* create packet structure pointer to buffer */
    ezs_packet_t* test_packet = (ezs_packet_t*)test_compiler_packing;

    /* verify endianness */
    if (test_packet->int_header != 0x1A020CC0)
    {
        /* endianness failed, compiler is using big-endian byte ordering */
        WE_APP_PRINT("\r\nINCOMPATIBLE COMPILER BEHAVIOR:\r\n");
        WE_APP_PRINT("EZ-Serial API byte stream requires little-endian, compiler is big-endian.\r\n");
        WE_APP_PRINT("Please review compiler flags for your toolchain to verify whether it is\r\n");
        WE_APP_PRINT("possible to switch to little-endian data storage.\r\n\r\n");
        while (1)
        {
            /* loop forever, cannot communicate */
        }
    }
    else
    {
        /* successful 32-bit integer byte order comparison */
        WE_APP_PRINT("Compiler endianness passes verification\r\n");
    }

    /* verify packing/alignment */
    if (test_packet->payload.rsp_system_get_uart_parameters.result != 0x2211 || test_packet->payload.rsp_system_get_uart_parameters.baud != 0x0001C200 || test_packet->payload.rsp_system_get_uart_parameters.autobaud != 0xAA || test_packet->payload.rsp_system_get_uart_parameters.autocorrect != 0xBB || test_packet->payload.rsp_system_get_uart_parameters.flow != 0xCC ||
        test_packet->payload.rsp_system_get_uart_parameters.databits != 0xDD || test_packet->payload.rsp_system_get_uart_parameters.parity != 0xEE || test_packet->payload.rsp_system_get_uart_parameters.stopbits != 0xFF)
    {
        /* packing failed, compiler is not tightly packing and/or properly aligning structures */
        WE_APP_PRINT("\r\nINCOMPATIBLE COMPILER BEHAVIOR:\r\n");
        WE_APP_PRINT("Structures must be fully packed, but compiler is generating padding.\r\n");
        WE_APP_PRINT("Please review and modify the __PACKDEF macro definition in ezsapi.h.\r\n\r\n");
        while (1)
        {
            /* loop forever, cannot communicate */
        }
    }
    else
    {
        /* successful read back from packed structure */
        WE_APP_PRINT("Compiler structure packing passes verification\r\n");
    }
}
