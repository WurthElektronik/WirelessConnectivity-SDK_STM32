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
 * @brief Skoll-I Bluetooth LE example.
 */

#include <Skoll_I/Skoll_I.h>
#include <Skoll_I/Skoll_I_Examples.h>
#include <Skoll_I/Skoll_I_Examples_handler.h>
#include <global/global.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief This example shows the communication via Bluetooth LE CYSPP profile.
 *
 * @details: In peripheral mode it waits for incoming Bluetooth LE connection. In central mode it connects to an
 * external device with CYSPP profile. Please note that the central and peripheral key (see below) must be chosen accordingly.
 * In both cases, data is transmitted after the connection has been setup.
 * At the end the connection is closed.
 *
 * @param[in] role Switch whether to run in central or peripheral mode
 *
 * @return None
 */
void Test_BluetoothLE_CYSPP_Connection(Skoll_I_BluetoothLE_Role_t role)
{
    ezs_packet_t* packet;

    /* create an array with payload data */
    uint8_t data[512];
    unsigned int i = 0;
    for (i = 0; i < sizeof(data); i++)
    {
        data[i] = 0x30 + (i % 10);
    }

    /* initialize and reset the radio module */
    if (false == Skoll_I_Init(&Skoll_I_uart, &Skoll_I_pins, eventHandler, transparentDataHandler))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }
    if (false == Skoll_I_PinReset())
    {
        WE_APP_PRINT("Reset error\r\n");
        return;
    }

    uint32_t central_key = 0x11111111;
    uint32_t peripheral_key = 0x22222222;
    switch (role)
    {
        case Skoll_I_BluetoothLE_Role_Central:
        {
            /* set device name */
            uint8a_t name = {.length = 7, .data = "Central"};
            Examples_Print("gap_set_device_name", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_set_device_name(0, &name), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

            /* central, enable, allow sleep, no security, enable CYSPP flow control, keys set */
            Examples_Print("p_cyspp_set_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_p_cyspp_set_parameters(2, 1, WE_BLUETOOTH_COMPANY_IDENTIFIER, central_key, peripheral_key, 0, 1, 0, 2), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
            Examples_Print("set role to central", Skoll_I_BluetoothLE_PinSetRole(Skoll_I_BluetoothLE_Role_Central));
            break;
        }
        case Skoll_I_BluetoothLE_Role_Peripheral:
        {
            /* set device name */
            uint8a_t name = {.length = 10, .data = "Peripheral"};
            Examples_Print("gap_set_device_name", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_set_device_name(0, &name), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

            /* peripheral, enable, allow sleep, no security, enable CYSPP flow control, keys set */
            Examples_Print("p_cyspp_set_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_p_cyspp_set_parameters(2, 0, WE_BLUETOOTH_COMPANY_IDENTIFIER, peripheral_key, central_key, 0, 1, 0, 2), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
            Examples_Print("set role to peripheral", Skoll_I_BluetoothLE_PinSetRole(Skoll_I_BluetoothLE_Role_Peripheral));
            Examples_Print("gap_start_adv", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_start_adv(0, 3, 7, 0x0040, 60, 0x0040, 100, 1, 0, 0), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
            WE_Delay(500);
            break;
        }
        default:
        {
            /* error */
            return;
        }
    }

    /* start the cyspp function */
    Examples_Print("p_cyspp_start", (packet = EZS_SEND_AND_WAIT(ezs_cmd_p_cyspp_start(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    bool isChannelOpen;

    /* as soon as channel is open, start transmitting data */
    while (Skoll_I_PinIsChannelOpen(&isChannelOpen) && (isChannelOpen == false))
    {
        if (true == states.Common.pairing_requested)
        {
            /* accept pairing request */
            Examples_Print("smp_send_pairreq_response", (packet = EZS_SEND_AND_WAIT(ezs_cmd_smp_send_pairreq_response(states.LE.conn_handle, 1), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
            states.Common.pairing_requested = false;
        }

        if (true == states.LE.passkeyentry_requested)
        {
            /* please use correct passkey here */
            uint32_t passkey = 123123;
            Examples_Print("smp_send_passkeyreq_response", (packet = EZS_SEND_AND_WAIT(ezs_cmd_smp_send_passkeyreq_response(states.LE.conn_handle, passkey), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
            states.LE.passkeyentry_requested = false;
        }

        WE_APP_PRINT("Waiting for channel open\r\n");
        WE_Delay(5000);
    }

    i = 0;
    while ((Skoll_I_PinIsChannelOpen(&isChannelOpen) && (isChannelOpen)) && (i < 30))
    {
        WE_APP_PRINT("%d ", i);
        Examples_Print("Skoll_I_BluetoothLE_CYSPPTransparentTransmit", true == Skoll_I_BluetoothLE_CYSPPTransparentTransmit(sizeof(data), data));
        i++;
        WE_Delay(2500);
    }

    bool isConnectionOpen;

    /* disconnect at the end */
    if (Skoll_I_PinIsConnectionOpen(&isConnectionOpen) && (isConnectionOpen))
    {
        WE_Delay(1000);
        Examples_Print("Skoll_I_BluetoothLE_PinDisconnect", Skoll_I_BluetoothLE_PinDisconnect(states.LE.conn_handle));
    }
}

/**
 * @brief This example shows the communication via Bluetooth LE CYSPP profile.
 *
 * @details: In peripheral mode it waits for incoming Bluetooth LE connection. In central mode it connects to an
 * external device with CYSPP profile. Please note that the central and peripheral key (see below) must be chosen accordingly.
 * In both cases, data is transmitted with highest speed after the connection has been setup.
 * At the end, the connection is closed.
 *
 * Please note: For highest throughput, first configure UART to 2.666 MBaud with flow control enabled.
 *
 * @return None
 */
void Test_BluetoothLE_CYSPP_Throughput(Skoll_I_BluetoothLE_Role_t role)
{
    ezs_packet_t* packet;

    /* create an array with payload data */
    uint8_t data[512];
    unsigned int i = 0;
    for (i = 0; i < sizeof(data); i++)
    {
        data[i] = 0x30 + (i % 10);
    }

    /* initialize and reset the radio module */
    if (false == Skoll_I_Init(&Skoll_I_uart, &Skoll_I_pins, eventHandler, transparentDataHandler))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    uint32_t central_key = 0x11111111;
    uint32_t peripheral_key = 0x22222222;
    switch (role)
    {
        case Skoll_I_BluetoothLE_Role_Central:
        {
            /* set device name */
            uint8a_t name = {.length = 7, .data = "Central"};
            Examples_Print("gap_set_device_name", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_set_device_name(0, &name), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

            /* central, enable, allow sleep, no security, enable CYSPP flow control, keys set */
            Examples_Print("p_cyspp_set_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_p_cyspp_set_parameters(2, 1, WE_BLUETOOTH_COMPANY_IDENTIFIER, central_key, peripheral_key, 0, 1, 0, 2), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
            Examples_Print("set role to central", Skoll_I_BluetoothLE_PinSetRole(Skoll_I_BluetoothLE_Role_Central));
            break;
        }
        case Skoll_I_BluetoothLE_Role_Peripheral:
        {
            /* set device name */
            uint8a_t name = {.length = 10, .data = "Peripheral"};
            Examples_Print("gap_set_device_name", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_set_device_name(0, &name), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

            /* peripheral, enable, allow sleep, no security, enable CYSPP flow control, keys set */
            Examples_Print("p_cyspp_set_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_p_cyspp_set_parameters(2, 0, WE_BLUETOOTH_COMPANY_IDENTIFIER, peripheral_key, central_key, 0, 1, 0, 2), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
            Examples_Print("set role to peripheral", Skoll_I_BluetoothLE_PinSetRole(Skoll_I_BluetoothLE_Role_Peripheral));
            Examples_Print("gap_start_adv", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_start_adv(0, 3, 7, 0x0040, 60, 0x0040, 100, 1, 0, 0), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
            WE_Delay(500);
            break;
        }
        default:
        {
            /* error */
            return;
        }
    }

    /* start the cyspp function */
    Examples_Print("p_cyspp_start", (packet = EZS_SEND_AND_WAIT(ezs_cmd_p_cyspp_start(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    bool isChannelOpen;

    /* as soon as channel is open, start transmitting data */
    while (Skoll_I_PinIsChannelOpen(&isChannelOpen) && (isChannelOpen == false))
    {
        WE_APP_PRINT("Waiting for channel open\r\n");
        WE_Delay(5000);
    }

    i = 0;
    while ((Skoll_I_PinIsChannelOpen(&isChannelOpen) && (isChannelOpen)) && (i < 300))
    {
        Skoll_I_BluetoothLE_CYSPPTransparentTransmit(sizeof(data), data);
        i++;
    }

    bool isConnectionOpen;

    /* disconnect at the end */
    if (Skoll_I_PinIsConnectionOpen(&isConnectionOpen) && (isConnectionOpen))
    {
        WE_Delay(1000);
        Examples_Print("Skoll_I_BluetoothLE_PinDisconnect", Skoll_I_BluetoothLE_PinDisconnect(states.LE.conn_handle));
    }
}

/**
 * @brief This example shows the communication via Bluetooth LE WE SPP-like profile.
 *
 * @details: This example connects to a radio module of the from Würth electronics Proteus series
 * with the below MAC address. Then it discovers the WE SPP-like service, subscribes to it
 * and sends data to the Proteus module. At the end, it disconnects again.
 *
 * @return None
 */
uint8_t btle_peer_address[6] = {0x11, 0x22, 0x00, 0xDA, 0x18, 0x00}; /* MAC of the peer device to connect to */
void Test_BluetoothLE_ProteusSPPLike_Central(void)
{
    ezs_packet_t* packet;
    int i;

    /* initialize and reset the radio module */
    if (false == Skoll_I_Init(&Skoll_I_uart, &Skoll_I_pins, eventHandler, NULL))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }
    if (false == Skoll_I_PinReset())
    {
        WE_APP_PRINT("Reset error\r\n");
        return;
    }

    /* stop advertising and scan for Bluetooth LE devices */
    Examples_Print("gap_stop_adv", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_stop_adv(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    Examples_Print("gap_start_scan", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_start_scan(1, 0x0040, 0x0040, 1, 0, 1, 0), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    WE_Delay(2500);
    Examples_Print("gap_stop_scan", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_stop_scan(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    WE_Delay(100);

    /* try to connect to a device with btle_peer_address, this is a Proteus device supporting the SPPlike profile */
    Examples_Print("gap_connect", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_connect(btle_peer_address, 0, 6, 0, 0x64, 0x0040, 0x0040, 10), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    while (false == states.LE.connected)
    {
        WE_Delay(2500);
        WE_APP_PRINT("Wait for connection\r\n");
    }

    /* run the profile discovery to check whether the device provides the right characteristics of the SPPlike profile */
    //Examples_Print("gattc_discover_services", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gattc_discover_services(states.LE.conn_handle, 0, 0xFFFF), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    //WE_Delay(1000);
    Examples_Print("gattc_discover_characteristics", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gattc_discover_characteristics(states.LE.conn_handle, 0, 0xFFFF, 0), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    WE_Delay(1000);
    Examples_Print("gattc_discover_descriptors", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gattc_discover_descriptors(states.LE.conn_handle, 0, 0xFFFF, 0, 0), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    WE_Delay(1000);

    /* subscribe to notifications (write 0x0001 to the descriptor of the TX characteristic "1BC5D5A502003D95E51152C30300406E" */
    uint16_t desc_attr_handle = 0x0010;
    longuint8a_t data;
    data.length = 2;
    data.data[0] = 0x01;
    data.data[1] = 0x00;
    Examples_Print("gattc_write_handle", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gattc_write_handle(states.LE.conn_handle, desc_attr_handle, 1, &data), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    WE_Delay(500);

    //Examples_Print("gap_query_peer_address", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_query_peer_address(states.LE.conn_handle), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    //Examples_Print("gap_query_rssi", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_query_rssi(states.LE.conn_handle), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    //Examples_Print("gap_update_conn_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_update_conn_parameters(states.LE.conn_handle, 6, 0, 0x64), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    /* generate data with 0x01 header, which is needed for communication with Proteus devices */
    data.length = 128;
    for (i = 0; i < data.length; i++)
    {
        data.data[i] = 0x30 + (i / 10);
    }
    data.data[0] = 0x01;

    /* write data to the RX characteristic "1BC5D5A502003D95E51152C30200406E" */
    uint16_t rx_char_handle = 0x000d;
    i = 0;

    bool isConnectionOpen;

    while ((Skoll_I_PinIsConnectionOpen(&isConnectionOpen) && (isConnectionOpen)) && (i < 30))
    {
        WE_APP_PRINT("%d ", i);
        data.data[1] = 0x30 + (i % 10);
        Examples_Print("gattc_write_handle", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gattc_write_handle(states.LE.conn_handle, rx_char_handle, 1, &data), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
        i++;
        WE_Delay(500);
    }

    /* disconnect at the end */
    if (Skoll_I_PinIsConnectionOpen(&isConnectionOpen) && (isConnectionOpen))
    {
        WE_Delay(1000);
        Examples_Print("gap_disconnect", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_disconnect(states.LE.conn_handle), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    }
}

/**
 * @brief This example shows the communication via Bluetooth LE WE SPP-like profile.
 *
 * @details: This example adds the SPP-like custom Bluetooth LE profile (WE SPP-like profile) known from Würth electronics Proteus series
 * to the radio module and starts advertising.
 * As soon as a central device has connected to it, the radio module sends data via notifications
 * to the central device and disconnects at the end.
 *
 * Note that adding or removing the profile must be enabled by the 'run' parameter.
 *
 * @param[in] run Switch between the mode.
 * 0 - factory reset and return
 * 1 - add profile and run communication test (run this only once after factory reset)
 * 2 - only run communication test
 *
 * @return None
 */
void Test_BluetoothLE_ProteusSPPLike_Peripheral(uint8_t run)
{
    ezs_packet_t* packet;
    int i;

    /* initialize the radio module */
    if (false == Skoll_I_Init(&Skoll_I_uart, &Skoll_I_pins, eventHandler, NULL))
    {
        WE_APP_PRINT("Initialization error\r\n");
        return;
    }

    switch (run)
    {
        case 0: //factory reset
        {
            /* factory reset */
            ezs_cmd_system_factory_reset();
            Examples_Print("system_factory_reset", (packet = EZS_WAIT_FOR_EVENT(5000)) != (ezs_packet_t*)NULL);
            return;
        }

        case 1: //create profile
        {
            /* create profile */
            if (false == Skoll_I_PinReset())
            {
                WE_APP_PRINT("Reset error\r\n");
                return;
            }

            /* set device name */
            uint8a_t name = {.length = 10, .data = "Peripheral"};
            Examples_Print("gap_set_device_name", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_set_device_name(0, &name), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

            /* switch Bluetooth LE CYSPP off */
            Examples_Print("p_cyspp_set_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_p_cyspp_set_parameters(0, 0, WE_BLUETOOTH_COMPANY_IDENTIFIER, 0, 0, 0, 1, 0, 2), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

            /* Create service
         * /CAC,T=0,P=02,L=12,D=00281BC5D5A502003D95E51152C30100406E */
            longuint8a_t service = {.length = 0x12, .data = {0x00, 0x28, 0x1B, 0xC5, 0xD5, 0xA5, 0x02, 0x00, 0x3D, 0x95, 0xE5, 0x11, 0x52, 0xC3, 0x01, 0x00, 0x40, 0x6E}};
            Examples_Print("create service", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gatts_create_attr(0, 0x02, 0x12, &service), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

            /* Create a characteristic descriptor
         * /CAC,T=00,P=02,L=15,D=0328 0C 1F00 1BC5D5A502003D95E51152C30200406E */
            longuint8a_t rx_char = {.length = 0x15, .data = {0x03, 0x28, 0x0C, 0x1F, 0x00, 0x1B, 0xC5, 0xD5, 0xA5, 0x02, 0x00, 0x3D, 0x95, 0xE5, 0x11, 0x52, 0xC3, 0x02, 0x00, 0x40, 0x6E}};
            Examples_Print("create rx char", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gatts_create_attr(0, 0x02, 0x15, &rx_char), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

            /* Create a characteristic value descriptor, length 0x80 (128), without initial value
         * /CAC,T=01,P=8F,L=80,D= */
            longuint8a_t rx_char_value = {.length = 0x00, .data = {}};
            Examples_Print("create rx char value", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gatts_create_attr(1, 0x8F, 0x80, &rx_char_value), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

            /* Create a characteristic descriptor
         * /CAC,T=00,P=02,L=15,D=0328 10 2100 1BC5D5A502003D95E51152C30300406E  */
            longuint8a_t tx_char = {.length = 0x15, .data = {0x03, 0x28, 0x10, 0x21, 0x00, 0x1B, 0xC5, 0xD5, 0xA5, 0x02, 0x00, 0x3D, 0x95, 0xE5, 0x11, 0x52, 0xC3, 0x03, 0x00, 0x40, 0x6E}};
            Examples_Print("create tx char", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gatts_create_attr(0, 0x02, 0x15, &tx_char), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

            /* Create a characteristic value descriptor, length 0x80 (128), without initial value
         * /CAC,T=01,P=8B,L=80,D= */
            longuint8a_t tx_char_value = {.length = 0x00, .data = {}};
            Examples_Print("create tx char value", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gatts_create_attr(1, 0x8B, 0x80, &tx_char_value), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

            /* Create a CCCD, value 0
         * /CAC,T=00,P=0A,L=04,D=02290000 */
            longuint8a_t tx_char_cccd = {.length = 0x04, .data = {0x02, 0x29, 0x00, 0x00}};
            Examples_Print("create tx char cccd", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gatts_create_attr(0, 0x0A, 0x04, &tx_char_cccd), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
            WE_Delay(500);

            Examples_Print("gatts_validate_db", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gatts_validate_db(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
            WE_Delay(500);

            Examples_Print("system_store_config", (packet = EZS_SEND_AND_WAIT(ezs_cmd_system_store_config(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
            WE_Delay(500);
        }
        case 2:
        default:
        {
            /* run communication test */
            break;
        }
    }

    /* start advertising */
    Examples_Print("gap_start_adv", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_start_adv(0, 3, 7, 0x0040, 60, 0x0040, 100, 1, btle_peer_address, 0), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    WE_Delay(500);

    /* wait until connected */
    while (false == states.LE.connected)
    {
        WE_Delay(2500);
        WE_APP_PRINT("Wait for connection\r\n");
    }

    /* wait until subscribed */
    while (false == states.LE_SPPLike.peer_is_subscribed)
    {
        WE_Delay(1000);
        WE_APP_PRINT("Wait for channel open\r\n");
    }

    /* generate data with 0x01 header, which is needed for communication with Proteus devices */
    uint8a_t data;
    data.length = 64;
    for (i = 0; i < data.length; i++)
    {
        data.data[i] = 0x30 + (i / 10);
    }
    data.data[0] = 0x01;

    /* transmit data by notifying the handle of the TX-characteristic */
    i = 0;

    bool isConnectionOpen;

    while ((Skoll_I_PinIsConnectionOpen(&isConnectionOpen) && (isConnectionOpen)) && (states.LE_SPPLike.peer_is_subscribed) && (i < 30))
    {
        WE_APP_PRINT("%d ", i);
        data.data[1] = 0x30 + (i % 10);
        Examples_Print("gatts_notify_handle", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gatts_notify_handle(states.LE.conn_handle, 0x0021, &data), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

        i++;
        WE_Delay(500);
    }

    /* disconnect at the end */
    if (Skoll_I_PinIsConnectionOpen(&isConnectionOpen) && (isConnectionOpen))
    {
        WE_Delay(1000);
        Examples_Print("gap_disconnect", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_disconnect(states.LE.conn_handle), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    }
}
