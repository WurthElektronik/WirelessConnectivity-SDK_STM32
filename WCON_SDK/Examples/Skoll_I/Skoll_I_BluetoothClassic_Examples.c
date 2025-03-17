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
 * @brief Skoll-I Bluetooth Classic example.
 */

#include <Skoll_I/Skoll_I.h>
#include <Skoll_I/Skoll_I_Examples.h>
#include <Skoll_I/Skoll_I_Examples_handler.h>
#include <global/global.h>
#include <stdio.h>
#include <string.h>

/* MAC of the peer device to connect to */
uint8_t btclassic_peer_address[6] = {0xcc, 0xbb, 0xaa, 0x33, 0x22, 0xd1};

/**
 * @brief This example shows the communication via Bluetooth Classic SPP profile.
 *
 * @details: In slave mode it waits for incoming Bluetooth Classic connections, in master mode it connects to an
 * external device with SPP profile.
 * In both cases, data is transmitted after the connection has been setup.
 * For data transmission the transparent data mode is used.
 * At the end, the connection is closed.
 *
 * @param[in] role Switch whether to run in master of slave mode
 *
 * @return None
 */
void Test_BluetoothClassic_SPP_TransparentConnection(Skoll_I_BluetoothClassic_Role_t role)
{
    ezs_packet_t* packet;

    /* generate a payload data array */
    uint8_t data[512];
    unsigned int i = 0;
    for (i = 0; i < sizeof(data); i++)
    {
        data[i] = 0x30 + (i % 10);
    }

    /* initialize and reset the radio module */
    if (false == Skoll_I_Init(&Skoll_I_uart, &Skoll_I_pins, eventHandler, transparentDataHandler))
    {
        WE_DEBUG_PRINT("Initialization error\r\n");
        return;
    }
    if (false == Skoll_I_PinReset())
    {
        WE_DEBUG_PRINT("Reset error\r\n");
        return;
    }

    /* switch Bluetooth LE CYSPP off */
    Examples_Print("p_cyspp_set_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_p_cyspp_set_parameters(0, 0, WE_BLUETOOTH_COMPANY_IDENTIFIER, 0, 0, 0, 1, 0, 2), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    switch (role)
    {
        case Skoll_I_BluetoothClassic_Role_Master:
        {
            /* set device name */
            uint8a_t name = {.length = 6, .data = "Master"};
            Examples_Print("gap_set_device_name", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_set_device_name(1, &name), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

            /* search for 5s */
            Examples_Print("bt_start_inquiry", (packet = EZS_SEND_AND_WAIT(ezs_cmd_bt_start_inquiry(5, 0), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

            while (states.Classic.inquiry_running)
            {
                WE_Delay(500);
            }

#if 0
			Examples_Print("bt_query_name", (packet = EZS_SEND_AND_WAIT(ezs_cmd_bt_query_name(address), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
			Examples_Print("bt_query_connections", (packet = EZS_SEND_AND_WAIT(ezs_cmd_bt_query_connections(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
			Examples_Print("bt_query_rssi", (packet = EZS_SEND_AND_WAIT(ezs_cmd_bt_query_rssi(btclassic_peer_address), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
#endif

            /* connect to peer device */
            Examples_Print("bt_connect", (packet = EZS_SEND_AND_WAIT(ezs_cmd_bt_connect(btclassic_peer_address), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

            break;
        }
        case Skoll_I_BluetoothClassic_Role_Slave:
        {
            /* set device name */
            uint8a_t name = {.length = 5, .data = "Slave"};
            Examples_Print("gap_set_device_name", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_set_device_name(1, &name), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
            break;
        }
        default:
        {
            /* error */
            return;
        }
    }

    bool isChannelOpen;

    /* wait till connection is open */
    while (Skoll_I_PinIsChannelOpen(&isChannelOpen) && (isChannelOpen == false))
    {
        switch (role)
        {
            case Skoll_I_BluetoothClassic_Role_Master:
            {
                WE_DEBUG_PRINT("Setting up connection\r\n");
                break;
            }
            case Skoll_I_BluetoothClassic_Role_Slave:
            {
                WE_DEBUG_PRINT("Waiting for connection setup\r\n");
                break;
            }
            default:
            {
                /* error */
                return;
            }
        }
        WE_Delay(5000);
    }

    /* transmit data */
    i = 0;
    while (Skoll_I_PinIsChannelOpen(&isChannelOpen) && (isChannelOpen) && (i < 30))
    {
        WE_DEBUG_PRINT("%d ", i);
        Examples_Print("Skoll_I_BluetoothClassic_SPPTransparentTransmit", true == Skoll_I_BluetoothClassic_SPPTransparentTransmit(sizeof(data), data));
        i++;
        WE_Delay(500);
    }

    bool isConnectionOpen;

    /* disconnect at the end */
    if (Skoll_I_PinIsConnectionOpen(&isConnectionOpen) && (isConnectionOpen))
    {
        WE_Delay(1000);
        Examples_Print("Skoll_I_BluetoothClassic_PinDisconnect", Skoll_I_BluetoothClassic_PinDisconnect());
    }
}

/**
 * @brief This example shows the communication via Bluetooth Classic SPP profile.
 *
 * @details: In slave mode it waits for incoming Bluetooth Classic connection, in master mode it connects to an
 * external device with SPP profile.
 * In both cases, data is transmitted after the connection has been setup.
 * For data transmission commands are used.
 * At the end, the connection is closed.
 *
 * @param[in] role Switch whether to run in master of slave mode
 *
 * @return None
 */
void Test_BluetoothClassic_SPP_Connection(Skoll_I_BluetoothClassic_Role_t role)
{
    ezs_packet_t* packet;
    int i = 0;

    /* create payload data */
    longuint8a_t data;
    data.length = 300; //300 is maximum
    for (i = 0; i < data.length; i++)
    {
        data.data[i] = 0x30 + (i / 10);
    }

    /* initialize and reset the radio module */
    if (false == Skoll_I_Init(&Skoll_I_uart, &Skoll_I_pins, eventHandler, NULL))
    {
        WE_DEBUG_PRINT("Initialization error\r\n");
        return;
    }
    if (false == Skoll_I_PinReset())
    {
        WE_DEBUG_PRINT("Reset error\r\n");
        return;
    }

    /* switch Bluetooth LE CYSPP off */
    Examples_Print("p_cyspp_set_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_p_cyspp_set_parameters(0, 0, WE_BLUETOOTH_COMPANY_IDENTIFIER, 0, 0, 0, 1, 0, 2), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    switch (role)
    {
        case Skoll_I_BluetoothClassic_Role_Master:
        {
            /* set device name */
            uint8a_t name = {.length = 6, .data = "Master"};
            Examples_Print("gap_set_device_name", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_set_device_name(1, &name), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

            /* search for 5s */
            Examples_Print("bt_start_inquiry", (packet = EZS_SEND_AND_WAIT(ezs_cmd_bt_start_inquiry(5, 0), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

            while (states.Classic.inquiry_running)
            {
                WE_Delay(500);
            }

#if 0
			Examples_Print("bt_query_name", (packet = EZS_SEND_AND_WAIT(ezs_cmd_bt_query_name(address), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
#endif
            Examples_Print("bt_query_connections", (packet = EZS_SEND_AND_WAIT(ezs_cmd_bt_query_connections(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
            Examples_Print("bt_query_rssi", (packet = EZS_SEND_AND_WAIT(ezs_cmd_bt_query_rssi(btclassic_peer_address), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

            /* connect to peer device */
            Examples_Print("bt_connect", (packet = EZS_SEND_AND_WAIT(ezs_cmd_bt_connect(btclassic_peer_address), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

            break;
        }
        case Skoll_I_BluetoothClassic_Role_Slave:
        {
            /* set device name */
            uint8a_t name = {.length = 5, .data = "Slave"};
            Examples_Print("gap_set_device_name", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_set_device_name(1, &name), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
            break;
        }
        default:
        {
            /* error */
            return;
        }
    }

    bool isChannelOpen;

    /* wait till connection is open */
    while (Skoll_I_PinIsChannelOpen(&isChannelOpen) && (isChannelOpen == false))
    {
        switch (role)
        {
            case Skoll_I_BluetoothClassic_Role_Master:
            {
                WE_DEBUG_PRINT("Setting up connection\r\n");
                break;
            }
            case Skoll_I_BluetoothClassic_Role_Slave:
            {
                WE_DEBUG_PRINT("Waiting for connection setup\r\n");
                break;
            }
            default:
            {
                /* error */
                return;
            }
        }
        WE_Delay(5000);
    }

    /* transmit data */
    i = 0;
    while (Skoll_I_PinIsChannelOpen(&isChannelOpen) && (isChannelOpen) && (i < 30))
    {
        WE_DEBUG_PRINT("%d ", i);
        data.data[0] = 0x30 + (i % 10);
        Examples_Print("spp_send_command", (packet = EZS_SEND_AND_WAIT(ezs_cmd_spp_send_command(states.Classic.conn_handle, &data), 2 * SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

        i++;
        WE_Delay(500);
    }

    bool isConnectionOpen;

    /* disconnect at the end */
    if (Skoll_I_PinIsConnectionOpen(&isConnectionOpen) && (isConnectionOpen))
    {
        WE_Delay(1000);
        Examples_Print("bt_disconnect", (packet = EZS_SEND_AND_WAIT(ezs_cmd_bt_disconnect(states.Classic.conn_handle), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    }
}

/**
 * @brief This example shows the communication via Bluetooth Classic SPP profile.
 *
 * @details: This example sets the device name and security settings for Bluetooth Classic
 * to display the pass key, when pairing is done. It waits for a pairing request of a
 * peer device. The pass key is shown on the module's UART as well as on the peer device (i.e. smart phone).
 * As soon as the connection has been setup, it disconnects after a few seconds.
 *
 * @return None
 */
void Test_BluetoothClassic_SPP_Slave_Secure_Connection(void)
{
    ezs_packet_t* packet;

    /* initialize and reset the radio module */
    if (false == Skoll_I_Init(&Skoll_I_uart, &Skoll_I_pins, eventHandler, NULL))
    {
        WE_DEBUG_PRINT("Initialization error\r\n");
        return;
    }
    if (false == Skoll_I_PinReset())
    {
        WE_DEBUG_PRINT("Reset error\r\n");
        return;
    }

    /* set device name */
    uint8a_t name = {.length = 6, .data = "Secure"};
    Examples_Print("gap_set_device_name", (packet = EZS_SEND_AND_WAIT(ezs_cmd_gap_set_device_name(1, &name), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    /* set security settings: display key */
    Examples_Print("smp_set_security_parameters", (packet = EZS_SEND_AND_WAIT(ezs_cmd_smp_set_security_parameters(0x5D, 1, 0x10, 0, 1, 0x03), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

    /* wait until connected */
    while (false == states.Classic.connected)
    {
        WE_DEBUG_PRINT("Wait for connection\r\n");
        WE_Delay(7500);
    }

    /* disconnect after a while */
    if (true == states.Classic.connected)
    {
        int i = 60;
        while (i > 0)
        {
            WE_DEBUG_PRINT("Disconnect in %d seconds\r\n", i);
            i -= 5;
            WE_Delay(5000);
        }
        Examples_Print("bt_disconnect", (packet = EZS_SEND_AND_WAIT(ezs_cmd_bt_disconnect(states.Classic.conn_handle), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    }
}

/**
 * @brief This example shows the communication via Bluetooth Classic SPP profile.
 *
 * @details: In slave mode it waits for incoming Bluetooth Classic connection, in master mode it connects to an
 * external device with SPP profile.
 * In both cases, data is transmitted with highest speed after the connection has been setup.
 * At the end, the connection is closed.
 *
 * Please note: For highest throughput, first configure UART to 2.666 MBaud with flow control enabled.
 *
 * @param[in] role Switch whether to run in master of slave mode
 *
 * @return None
 */
#pragma message("Please note: For highest throughput, first configure UART to 2.666 MBaud with flow control enabled.")
void Test_BluetoothClassic_SPP_TransparentThroughput(Skoll_I_BluetoothClassic_Role_t role)
{
    ezs_packet_t* packet;

    /* create an array of payload data */
    uint8_t data[512];
    unsigned int i = 0;
    for (i = 0; i < sizeof(data); i++)
    {
        data[i] = 0x30 + (i % 10);
    }

    /* initialize the radio module */
    if (false == Skoll_I_Init(&Skoll_I_uart, &Skoll_I_pins, eventHandler, transparentDataHandler))
    {
        WE_DEBUG_PRINT("Initialization error\r\n");
        return;
    }

    if (role == Skoll_I_BluetoothClassic_Role_Master)
    {
        /* number of connections 1 */
        Examples_Print("cmd_spp_get_config", (packet = EZS_SEND_AND_WAIT(ezs_cmd_spp_get_config(), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
        Examples_Print("cmd_spp_set_config", (packet = EZS_SEND_AND_WAIT(ezs_cmd_spp_set_config(1), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

        /* search for 5s */
        Examples_Print("bt_start_inquiry", (packet = EZS_SEND_AND_WAIT(ezs_cmd_bt_start_inquiry(5, 0), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);

        while (states.Classic.inquiry_running)
        {
            WE_Delay(500);
        }

        /* connect to peer device */
        Examples_Print("bt_connect", (packet = EZS_SEND_AND_WAIT(ezs_cmd_bt_connect(btclassic_peer_address), SKOLL_I_COMMAND_TIMEOUT_MS)) != (ezs_packet_t*)NULL);
    }

    bool isChannelOpen;

    /* wait till connection is open */
    while (Skoll_I_PinIsChannelOpen(&isChannelOpen) && (isChannelOpen == false))
    {
        switch (role)
        {
            case Skoll_I_BluetoothClassic_Role_Master:
            {
                WE_Delay(1000);
                break;
            }
            case Skoll_I_BluetoothClassic_Role_Slave:
            {
                WE_DEBUG_PRINT("Waiting for connection\r\n");
                WE_Delay(2500);
                break;
            }
            default:
            {
                /* error */
                return;
            }
        }
    }

    /* push payload data without pause */
    i = 0;
    while ((i < 300))
    {
        i++;
        Skoll_I_uart.uartTransmit(data, sizeof(data));
    }

    bool isConnectionOpen;

    /* disconnect at the end */
    if (Skoll_I_PinIsConnectionOpen(&isConnectionOpen) && (isConnectionOpen))
    {
        WE_Delay(1000);
        Examples_Print("Skoll_I_BluetoothClassic_PinDisconnect", Skoll_I_BluetoothClassic_PinDisconnect());
    }
}
