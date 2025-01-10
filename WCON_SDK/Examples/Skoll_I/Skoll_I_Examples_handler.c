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
 * @brief Skoll-I example handler.
 */

#include <Skoll_I/Skoll_I.h>
#include <Skoll_I/Skoll_I_Examples_handler.h>
#include <global/global.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Definition of the initial module states
 */
Skoll_I_States_t states = {
		.Classic.inquiry_running = false,
		.Classic.connected = false,
		.Classic.pinentry_requested = false,

		.Common.pairing_requested = false,

		.LE.scan_running = false,
		.LE.connected = false,

		.LE_SPPLike.peer_is_subscribed = false,
		.LE_CYSPP.peer_is_subscribed = false, };

void eventHandler(ezs_packet_t *packet)
{
	switch (packet->tbl_index)
	{
	case EZS_IDX_RSP_SYSTEM_READ_USER_DATA:
		WE_DEBUG_PRINT("RX: rsp_system_read_user_data: result=0x%04x, data=", packet->payload.rsp_system_read_user_data.result);
		print_hexstring((char*) packet->payload.rsp_system_read_user_data.data.data, packet->payload.rsp_system_read_user_data.data.length);
		break;

	case EZS_IDX_RSP_SYSTEM_WRITE_USER_DATA:
		WE_DEBUG_PRINT("RX: rsp_system_write_user_data: result=0x%04x", packet->payload.rsp_system_write_user_data.result);
		break;

	case EZS_IDX_RSP_GATTS_SET_PARAMETERS:
		WE_DEBUG_PRINT("RX: rsp_gatts_set_parameters: result=0x%04x", packet->payload.rsp_gatts_set_parameters.result);
		break;

	case EZS_IDX_RSP_GATTS_GET_PARAMETERS:
		WE_DEBUG_PRINT("RX: rsp_gatts_get_parameters: result=0x%04x, flags=%d", packet->payload.rsp_gatts_get_parameters.result, packet->payload.rsp_gatts_get_parameters.flags);
		break;

	case EZS_IDX_RSP_SMP_QUERY_RANDOM_ADDRESS:
		WE_DEBUG_PRINT("RX: rsp_smp_query_random_address: result=0x%04x, address=", packet->payload.rsp_smp_query_random_address.result);
		print_hexstring((char*) packet->payload.rsp_smp_query_random_address.address.addr, 6);
		break;

	case EZS_IDX_RSP_SMP_QUERY_BONDS:
		WE_DEBUG_PRINT("RX: rsp_smp_query_bonds: result=0x%04x, count=%d", packet->payload.rsp_smp_query_bonds.result, packet->payload.rsp_smp_query_bonds.count);
		break;

	case EZS_IDX_RSP_SMP_DELETE_BOND:
		WE_DEBUG_PRINT("RX: rsp_smp_delete_bond: result=0x%04x, count=%d", packet->payload.rsp_smp_delete_bond.result, packet->payload.rsp_smp_delete_bond.count);
		break;

	case EZS_IDX_RSP_SMP_SET_FIXED_PASSKEY:
		WE_DEBUG_PRINT("RX: rsp_smp_set_fixed_passkey: result=0x%04x", packet->payload.rsp_smp_set_fixed_passkey.result);
		break;

	case EZS_IDX_RSP_SMP_GET_FIXED_PASSKEY:
		WE_DEBUG_PRINT("RX: rsp_smp_get_fixed_passkey: result=0x%04x, key=%lu / 0x%x", packet->payload.rsp_smp_get_fixed_passkey.result, packet->payload.rsp_smp_get_fixed_passkey.passkey, (unsigned int) packet->payload.rsp_smp_get_fixed_passkey.passkey);
		break;

	case EZS_IDX_RSP_SMP_SET_PRIVACY_MODE:
		WE_DEBUG_PRINT("RX: rsp_smp_set_privacy_mode: result=0x%04x", packet->payload.rsp_smp_set_privacy_mode.result);
		break;

	case EZS_IDX_RSP_SMP_GET_PRIVACY_MODE:
		WE_DEBUG_PRINT("RX: rsp_smp_get_privacy_mode: result=0x%04x, mode=%d, interval=%d", packet->payload.rsp_smp_get_privacy_mode.result, packet->payload.rsp_smp_get_privacy_mode.mode, packet->payload.rsp_smp_get_privacy_mode.interval);
		break;

	case EZS_IDX_RSP_GATTC_SET_PARAMETERS:
		WE_DEBUG_PRINT("RX: rsp_gattc_set_parameters: result=0x%04x", packet->payload.rsp_gattc_set_parameters.result);
		break;

	case EZS_IDX_RSP_GATTC_GET_PARAMETERS:
		WE_DEBUG_PRINT("RX: rsp_gattc_get_parameters: result=0x%04x, flags=%d", packet->payload.rsp_gattc_get_parameters.result, packet->payload.rsp_gattc_get_parameters.flags);
		break;

	case EZS_IDX_EVT_GAP_WHITELIST_ENTRY:
		WE_DEBUG_PRINT("RX: evt_gap_whitelist_entry: type=%d, address=", packet->payload.evt_gap_whitelist_entry.type);
		print_hexstring((char*) packet->payload.evt_gap_whitelist_entry.address.addr, 6);
		break;

	case EZS_IDX_RSP_GAP_ADD_WHITELIST_ENTRY:
		WE_DEBUG_PRINT("RX: rsp_gap_add_whitelist_entry: result=0x%04x, count=%d", packet->payload.rsp_gap_add_whitelist_entry.result, packet->payload.rsp_gap_add_whitelist_entry.count);
		break;

	case EZS_IDX_RSP_GAP_DELETE_WHITELIST_ENTRY:
		WE_DEBUG_PRINT("RX: rsp_gap_delete_whitelist_entry: result=0x%04x, count=%d", packet->payload.rsp_gap_delete_whitelist_entry.result, packet->payload.rsp_gap_delete_whitelist_entry.count);
		break;

	case EZS_IDX_RSP_GAP_UPDATE_CONN_PARAMETERS:
		WE_DEBUG_PRINT("RX: rsp_gap_update_conn_parameters: result=0x%04x", packet->payload.rsp_gap_update_conn_parameters.result);
		break;

	case EZS_IDX_RSP_GAP_QUERY_PEER_ADDRESS:
		WE_DEBUG_PRINT("RX: rsp_gap_query_peer_address: result=0x%04x, address=", packet->payload.rsp_gap_query_peer_address.result);
		print_hexstring((char*) packet->payload.rsp_gap_query_peer_address.address.addr, 6);
		break;

	case EZS_IDX_RSP_GAP_QUERY_RSSI:
		WE_DEBUG_PRINT("RX: rsp_gap_query_rssi: result=0x%04x, rssi=%d", packet->payload.rsp_gap_query_rssi.result, packet->payload.rsp_gap_query_rssi.rssi);
		break;

	case EZS_IDX_RSP_GAP_QUERY_WHITELIST:
		WE_DEBUG_PRINT("RX: rsp_gap_query_whitelist: result=0x%04x, count=%d", packet->payload.rsp_gap_query_whitelist.result, packet->payload.rsp_gap_query_whitelist.count);
		break;

	case EZS_IDX_RSP_GATTC_READ_HANDLE:
		WE_DEBUG_PRINT("RX: rsp_gattc_read_handle: result=0x%04x", packet->payload.rsp_gattc_read_handle.result);
		break;

	case EZS_IDX_RSP_GATTC_WRITE_HANDLE:
		WE_DEBUG_PRINT("RX: rsp_gattc_write_handle: result=0x%04x", packet->payload.rsp_gattc_write_handle.result);
		break;

	case EZS_IDX_RSP_GATTC_DISCOVER_SERVICES:
		WE_DEBUG_PRINT("RX: rsp_gattc_discover_services: result=0x%04x", packet->payload.rsp_gattc_discover_services.result);
		break;

	case EZS_IDX_RSP_GATTC_DISCOVER_CHARACTERISTICS:
		WE_DEBUG_PRINT("RX: rsp_gattc_discover_characteristics: result=0x%04x", packet->payload.rsp_gattc_discover_characteristics.result);
		break;

	case EZS_IDX_RSP_GATTC_DISCOVER_DESCRIPTORS:
		WE_DEBUG_PRINT("RX: rsp_gattc_discover_descriptors: result=0x%04x", packet->payload.rsp_gattc_discover_descriptors.result);
		break;

	case EZS_IDX_RSP_GATTS_NOTIFY_HANDLE:
		WE_DEBUG_PRINT("RX: rsp_gatts_notify_handle: result=0x%04x", packet->payload.rsp_gatts_notify_handle.result);
		break;

	case EZS_IDX_RSP_GATTS_CREATE_ATTR:
		WE_DEBUG_PRINT("RX: rsp_gatts_create_attr: result=0x%04x", packet->payload.rsp_gatts_create_attr.result);
		break;

	case EZS_IDX_RSP_GATTS_DELETE_ATTR:
		WE_DEBUG_PRINT("RX: rsp_gatts_delete_attr: result=0x%04x", packet->payload.rsp_gatts_delete_attr.result);
		break;

	case EZS_IDX_RSP_GATTS_VALIDATE_DB:
		WE_DEBUG_PRINT("RX: rsp_gatts_validate_db: result=0x%04x", packet->payload.rsp_gatts_validate_db.result);
		break;

	case EZS_IDX_RSP_GATTS_STORE_DB:
		WE_DEBUG_PRINT("RX: rsp_gatts_store_db: result=0x%04x", packet->payload.rsp_gatts_store_db.result);
		break;

	case EZS_IDX_RSP_GATTS_DISCOVER_SERVICES:
		WE_DEBUG_PRINT("RX: rsp_gatts_discover_services: result=0x%04x", packet->payload.rsp_gatts_discover_services.result);
		break;

	case EZS_IDX_RSP_GATTS_DISCOVER_CHARACTERISTICS:
		WE_DEBUG_PRINT("RX: rsp_gatts_discover_characteristics: result=0x%04x", packet->payload.rsp_gatts_discover_characteristics.result);
		break;

	case EZS_IDX_RSP_GATTS_DISCOVER_DESCRIPTORS:
		WE_DEBUG_PRINT("RX: rsp_gatts_discover_descriptors: result=0x%04x", packet->payload.rsp_gatts_discover_descriptors.result);
		break;

	case EZS_IDX_RSP_GAP_START_ADV:
		WE_DEBUG_PRINT("RX: rsp_gap_start_adv: result=0x%04x", packet->payload.rsp_gap_start_adv.result);
		break;

	case EZS_IDX_RSP_GAP_STOP_ADV:
		WE_DEBUG_PRINT("RX: rsp_gap_stop_adv: result=0x%04x", packet->payload.rsp_gap_stop_adv.result);
		break;

	case EZS_IDX_RSP_GAP_CONNECT:
		WE_DEBUG_PRINT("RX: rsp_gap_connect: conn_handle=0x%04x, result=0x%04x", packet->payload.rsp_gap_connect.conn_handle, packet->payload.rsp_gap_connect.result);
		break;

	case EZS_IDX_RSP_GAP_DISCONNECT:
		WE_DEBUG_PRINT("RX: rsp_gap_disconnect: result=0x%04x", packet->payload.rsp_gap_disconnect.result);
		break;

	case EZS_IDX_RSP_GAP_START_SCAN:
		WE_DEBUG_PRINT("RX: rsp_gap_start_scan: result=0x%04x", packet->payload.rsp_gap_start_scan.result);
		break;

	case EZS_IDX_RSP_GAP_STOP_SCAN:
		WE_DEBUG_PRINT("RX: rsp_gap_stop_scan: result=0x%04x", packet->payload.rsp_gap_stop_scan.result);
		break;

	case EZS_IDX_RSP_SPP_SEND_COMMAND:
		WE_DEBUG_PRINT("RX: rsp_spp_send_command: result=0x%04x", packet->payload.rsp_spp_send_command.result);
		break;

	case EZS_IDX_EVT_DFU_BOOT:
		WE_DEBUG_PRINT("RX: evt_dfu_boot");
		break;

	case EZS_IDX_EVT_SYSTEM_DUMP_BLOB:
		WE_DEBUG_PRINT("RX: evt_system_dump_blob: type=%d", packet->payload.evt_system_dump_blob.type);
		break;

	case EZS_IDX_EVT_SYSTEM_FACTORY_RESET_COMPLETE:
		WE_DEBUG_PRINT("RX: evt_system_factory_reset_complete");
		break;

	case EZS_IDX_RSP_SYSTEM_STORE_CONFIG:
		WE_DEBUG_PRINT("RX: rsp_system_store_config: result=0x%04x", packet->payload.rsp_system_store_config.result);
		break;

	case EZS_IDX_RSP_SYSTEM_FACTORY_RESET:
		WE_DEBUG_PRINT("RX: rsp_system_factory_reset: result=0x%04x", packet->payload.rsp_system_factory_reset.result);
		break;

	case EZS_IDX_RSP_SYSTEM_DUMP:
		WE_DEBUG_PRINT("RX: rsp_system_dump: result=0x%04x, length=%d", packet->payload.rsp_system_dump.result, packet->payload.rsp_system_dump.length);
		break;

	case EZS_IDX_RSP_GAP_GET_ADV_PARAMETERS:
		WE_DEBUG_PRINT("RX: rsp_gap_get_adv_parameters: result=0x%04x", packet->payload.rsp_gap_get_adv_parameters.result);
		break;

	case EZS_IDX_RSP_GAP_SET_ADV_PARAMETERS:
		WE_DEBUG_PRINT("RX: rsp_gap_set_adv_parameters: result=0x%04x", packet->payload.rsp_gap_set_adv_parameters.result);
		break;

	case EZS_IDX_RSP_GAP_GET_SCAN_PARAMETERS:
		WE_DEBUG_PRINT("RX: rsp_gap_get_scan_parameters: result=0x%04x", packet->payload.rsp_gap_get_scan_parameters.result);
		break;

	case EZS_IDX_RSP_GAP_SET_SCAN_PARAMETERS:
		WE_DEBUG_PRINT("RX: rsp_gap_set_scan_parameters: result=0x%04x", packet->payload.rsp_gap_set_scan_parameters.result);
		break;

	case EZS_IDX_RSP_GAP_GET_CONN_PARAMETERS:
		WE_DEBUG_PRINT("RX: rsp_gap_get_conn_parameters: result=0x%04x", packet->payload.rsp_gap_get_conn_parameters.result);
		break;

	case EZS_IDX_RSP_GAP_SET_CONN_PARAMETERS:
		WE_DEBUG_PRINT("RX: rsp_gap_set_conn_parameters: result=0x%04x", packet->payload.rsp_gap_set_conn_parameters.result);
		break;

	case EZS_IDX_RSP_GAP_GET_ADV_DATA:
		WE_DEBUG_PRINT("RX: rsp_gap_get_adv_data: result=0x%04x, data=", packet->payload.rsp_gap_get_adv_data.result);
		print_hexstring((char*) packet->payload.rsp_gap_get_adv_data.data.data, packet->payload.rsp_gap_get_adv_data.data.length);
		break;

	case EZS_IDX_RSP_GAP_GET_SR_DATA:
		WE_DEBUG_PRINT("RX: rsp_gap_get_sr_data: result=0x%04x, data=", packet->payload.rsp_gap_get_sr_data.result);
		print_hexstring((char*) packet->payload.rsp_gap_get_sr_data.data.data, packet->payload.rsp_gap_get_sr_data.data.length);
		break;

	case EZS_IDX_RSP_SYSTEM_GET_SLEEP_PARAMETERS:
		WE_DEBUG_PRINT("RX: rsp_system_get_sleep_parameters: result=0x%04x, level=%d", packet->payload.rsp_system_get_sleep_parameters.result, packet->payload.rsp_system_get_sleep_parameters.level);
		break;

	case EZS_IDX_RSP_SYSTEM_SET_SLEEP_PARAMETERS:
		WE_DEBUG_PRINT("RX: rsp_system_set_sleep_parameters: result=0x%04x", packet->payload.rsp_system_set_sleep_parameters.result);
		break;

	case EZS_IDX_RSP_SYSTEM_GET_TX_POWER:
		WE_DEBUG_PRINT("RX: rsp_system_get_tx_power: result=0x%04x, power=%d", packet->payload.rsp_system_get_tx_power.result, packet->payload.rsp_system_get_tx_power.power);
		break;

	case EZS_IDX_RSP_SYSTEM_SET_TX_POWER:
		WE_DEBUG_PRINT("RX: rsp_system_set_tx_power: result=0x%04x", packet->payload.rsp_system_set_tx_power.result);
		break;

	case EZS_IDX_RSP_SYSTEM_GET_TRANSPORT:
		WE_DEBUG_PRINT("RX: rsp_system_get_transport: result=0x%04x, interface=%d", packet->payload.rsp_system_get_transport.result, packet->payload.rsp_system_get_transport.interface);
		break;

	case EZS_IDX_RSP_SYSTEM_SET_TRANSPORT:
		WE_DEBUG_PRINT("RX: rsp_system_set_transport: result=0x%04x", packet->payload.rsp_system_set_transport.result);
		break;

	case EZS_IDX_RSP_SYSTEM_GET_UART_PARAMETERS:
		WE_DEBUG_PRINT("RX: rsp_system_get_uart_parameters: result=0x%04x, baudrate=%lu, flowcontrol=%d", packet->payload.rsp_system_get_uart_parameters.result, packet->payload.rsp_system_get_uart_parameters.baud, packet->payload.rsp_system_get_uart_parameters.flow);
		break;

	case EZS_IDX_RSP_SYSTEM_SET_UART_PARAMETERS:
		WE_DEBUG_PRINT("RX: rsp_system_set_uart_parameters: result=0x%04x", packet->payload.rsp_system_set_uart_parameters.result);
		break;

	case EZS_IDX_RSP_SYSTEM_GET_BLUETOOTH_ADDRESS:
		WE_DEBUG_PRINT("RX: rsp_system_get_bluetooth_address: result=0x%04x, addr=", packet->payload.rsp_system_get_bluetooth_address.result);
		print_hexstring((char*) packet->payload.rsp_system_get_bluetooth_address.address.addr, 6);
		break;

	case EZS_IDX_RSP_SYSTEM_SET_BLUETOOTH_ADDRESS:
		WE_DEBUG_PRINT("RX: rsp_system_set_bluetooth_address: result=0x%04x", packet->payload.rsp_system_set_bluetooth_address.result);
		break;

	case EZS_IDX_RSP_SYSTEM_QUERY_RANDOM_NUMBER:
		WE_DEBUG_PRINT("RX: rsp_system_query_random_number: result=0x%04x", packet->payload.rsp_system_query_random_number.result);
		break;

	case EZS_IDX_RSP_PROTOCOL_GET_ECHO_MODE:
		WE_DEBUG_PRINT("RX: rsp_protocol_get_echo_mode: result=0x%04x, mode=%d", packet->payload.rsp_protocol_get_echo_mode.result, packet->payload.rsp_protocol_get_echo_mode.mode);
		break;

	case EZS_IDX_RSP_PROTOCOL_SET_ECHO_MODE:
		WE_DEBUG_PRINT("RX: rsp_protocol_set_echo_mode: result=0x%04x", packet->payload.rsp_protocol_set_echo_mode.result);
		break;

	case EZS_IDX_RSP_PROTOCOL_GET_PARSE_MODE:
		WE_DEBUG_PRINT("RX: rsp_protocol_get_parse_mode: result=0x%04x, mode=%d", packet->payload.rsp_protocol_get_parse_mode.result, packet->payload.rsp_protocol_get_parse_mode.mode);
		break;

	case EZS_IDX_RSP_PROTOCOL_SET_PARSE_MODE:
		WE_DEBUG_PRINT("RX: rsp_protocol_set_parse_mode: result=0x%04x", packet->payload.rsp_protocol_set_parse_mode.result);
		break;

	case EZS_IDX_CMD_GAP_GET_DEVICE_APPEARANCE:
		WE_DEBUG_PRINT("RX: rsp_gap_get_device_appearance: result=0x%04x, appearance=%d", packet->payload.rsp_gap_get_device_appearance.result, packet->payload.rsp_gap_get_device_appearance.appearance);
		break;

	case EZS_IDX_CMD_GAP_SET_DEVICE_APPEARANCE:
		WE_DEBUG_PRINT("RX: rsp_gap_set_device_appearance: result=0x%04x", packet->payload.rsp_gap_get_device_appearance.result);
		break;

	case EZS_IDX_RSP_GAP_GET_DEVICE_NAME:
		packet->payload.rsp_gap_get_device_name.name.data[packet->payload.rsp_gap_get_device_name.name.length] = '\0';
		WE_DEBUG_PRINT("RX: rsp_gap_get_device_name: result=0x%04x, name=%s", packet->payload.rsp_gap_get_device_name.result, packet->payload.rsp_gap_get_device_name.name.data);
		break;

	case EZS_IDX_RSP_GAP_SET_DEVICE_NAME:
		WE_DEBUG_PRINT("RX: rsp_gap_set_device_name: result=0x%04x", packet->payload.rsp_gap_set_device_name.result);
		break;

	case EZS_IDX_RSP_SYSTEM_PING:
		WE_DEBUG_PRINT("RX: rsp_system_ping: result=0x%04x, runtime=%lu, fraction=%d", packet->payload.rsp_system_ping.result, packet->payload.rsp_system_ping.runtime, packet->payload.rsp_system_ping.fraction);
		break;

	case EZS_IDX_RSP_SYSTEM_QUERY_FIRMWARE_VERSION:
		WE_DEBUG_PRINT("RX: rsp_system_query_firmware_version: app=%08lx, stack=%08lx, protocol=%d, hardware=%d", packet->payload.rsp_system_query_firmware_version.app, packet->payload.rsp_system_query_firmware_version.stack, packet->payload.rsp_system_query_firmware_version.protocol, packet->payload.rsp_system_query_firmware_version.hardware);

		/* check for protocol version older than v1.3 */
		if (packet->payload.rsp_system_query_firmware_version.protocol < 0x0103)
		{
			WE_DEBUG_PRINT("\r\n*** PLEASE UPDATE TARGET MODULE TO LATEST VERISON OF EZ-SERIAL FIRMWARE");
		}
		break;

	case EZS_IDX_RSP_SYSTEM_REBOOT:
		WE_DEBUG_PRINT("RX: rsp_system_reboot: result=0x%04x", packet->payload.rsp_system_reboot.result);
		break;

	case EZS_IDX_EVT_SYSTEM_BOOT:
		WE_DEBUG_PRINT("RX: evt_system_boot: app=%lu, stack=%lu, protocol=%d, hardware=%d, cause=%d", packet->payload.evt_system_boot.app, packet->payload.evt_system_boot.stack, packet->payload.evt_system_boot.protocol, packet->payload.evt_system_boot.hardware, packet->payload.evt_system_boot.cause);
		//WE_DEBUG_PRINT(", address=%d");
		//printHexMac(packet->payload.evt_system_boot.address);
		WE_DEBUG_PRINT(", firmware=");
		print_ascii((char*) packet->payload.evt_system_boot.firmware.data, packet->payload.evt_system_boot.firmware.length);

		states.Classic.inquiry_running = false;
		states.Classic.connected = false;
		states.Classic.pinentry_requested = false;
		states.Common.pairing_requested = false;
		states.LE.scan_running = false;
		states.LE.connected = false;
		states.LE.passkeyentry_requested = false;
		states.LE.conn_handle = 0;
		states.LE_SPPLike.peer_is_subscribed = false;
		states.LE_CYSPP.peer_is_subscribed = false;
		break;

	case EZS_IDX_EVT_GAP_ADV_STATE_CHANGED:
		WE_DEBUG_PRINT("RX: evt_gap_adv_state_changed: state=%d, reason=%d", packet->payload.evt_gap_adv_state_changed.state, packet->payload.evt_gap_adv_state_changed.reason);
		break;

	case EZS_IDX_EVT_GAP_SCAN_STATE_CHANGED:
		WE_DEBUG_PRINT("RX: evt_gap_scan_state_changed: state=%d, reason=%d", packet->payload.evt_gap_scan_state_changed.state, packet->payload.evt_gap_scan_state_changed.reason);
		break;

	case EZS_IDX_EVT_GAP_CONNECTED:
		WE_DEBUG_PRINT("RX: evt_gap_connected: conn_handle=%d, type=%d, interval=%d, slave_latency=%d, supervision_timeout=%d, bond=%d, address=", packet->payload.evt_gap_connected.conn_handle, packet->payload.evt_gap_connected.type, packet->payload.evt_gap_connected.interval, packet->payload.evt_gap_connected.slave_latency, packet->payload.evt_gap_connected.supervision_timeout, packet->payload.evt_gap_connected.bond);
		print_hexstring((char*) packet->payload.evt_gap_connected.address.addr, 6);
		states.LE.connected = true;
		states.LE.conn_handle = packet->payload.evt_gap_connected.conn_handle;
		states.Common.pairing_requested = false;
		break;

	case EZS_IDX_EVT_GAP_DISCONNECTED:
		WE_DEBUG_PRINT("RX: evt_gap_disconnected: conn_handle=%d, reason=%d", packet->payload.evt_gap_disconnected.conn_handle, packet->payload.evt_gap_disconnected.reason);
		states.LE.connected = false;
		states.LE.passkeyentry_requested = false;
		states.Common.pairing_requested = false;
		states.LE.conn_handle = 0;
		states.LE_CYSPP.peer_is_subscribed = false;
		states.LE_SPPLike.peer_is_subscribed = false;
		break;

	case EZS_IDX_EVT_SYSTEM_ERROR:
		WE_DEBUG_PRINT("RX: evt_system_error: error=%d / 0x%4x", packet->payload.evt_system_error.error, packet->payload.evt_system_error.error);
		break;

	case EZS_IDX_EVT_GAP_SCAN_RESULT:
		WE_DEBUG_PRINT("RX: evt_gap_scan_result: result=0x%04x, address=", packet->payload.evt_gap_scan_result.result_type);
		print_hexstring((char*) packet->payload.evt_gap_scan_result.address.addr, 6);
		break;

	case EZS_IDX_RSP_BT_SET_DEVICE_CLASS:
		WE_DEBUG_PRINT("RX: rsp_bt_set_device_class: result=0x%04x", packet->payload.rsp_bt_set_device_class.result);
		break;

	case EZS_IDX_RSP_BT_GET_DEVICE_CLASS:
		WE_DEBUG_PRINT("RX: rsp_bt_get_device_class: result=0x%04x, cod=%08x", packet->payload.rsp_bt_get_device_class.result, (unsigned int) packet->payload.rsp_bt_get_device_class.cod);
		break;

	case EZS_IDX_RSP_BT_SET_PARAMETERS:
		WE_DEBUG_PRINT("RX: rsp_bt_set_parameters: result=0x%04x", packet->payload.rsp_bt_set_parameters.result);
		break;

	case EZS_IDX_RSP_BT_GET_PARAMETERS:
		WE_DEBUG_PRINT("RX: rsp_bt_get_parameters: result=0x%04x, active_conn=%d, active_disco=%d, conn=%d, disco=%d, flags=%d, link_timeout=%d, scn=%d", packet->payload.rsp_bt_get_parameters.result, packet->payload.rsp_bt_get_parameters.active_bt_connectability, packet->payload.rsp_bt_get_parameters.active_bt_discoverability, packet->payload.rsp_bt_get_parameters.connectable, packet->payload.rsp_bt_get_parameters.discoverable, packet->payload.rsp_bt_get_parameters.flags, packet->payload.rsp_bt_get_parameters.link_super_time_out, packet->payload.rsp_bt_get_parameters.scn);
		break;

	case EZS_IDX_RSP_BT_START_INQUIRY:
		WE_DEBUG_PRINT("RX: rsp_bt_start_inquiry: result=0x%04x", packet->payload.rsp_bt_start_inquiry.result);
		states.Classic.inquiry_running = (0x0000 == packet->payload.rsp_bt_start_inquiry.result);
		break;

	case EZS_IDX_EVT_BT_NAME_RESULT:
		WE_DEBUG_PRINT("RX: evt_bt_name_result: bond=%d, address=", packet->payload.evt_bt_name_result.bond);
		print_hexstring((char*) packet->payload.evt_bt_name_result.address.addr, 6);
		WE_DEBUG_PRINT(", name=");
		print_ascii((char*) packet->payload.evt_bt_name_result.name.data, packet->payload.evt_bt_name_result.name.length);
		break;

	case EZS_IDX_EVT_BT_INQUIRY_RESULT:
		WE_DEBUG_PRINT("RX: evt_bt_inquiry_result: bond=%d, cod=%08x, address=", packet->payload.evt_bt_inquiry_result.bond, (unsigned int) packet->payload.evt_bt_inquiry_result.cod);
		print_hexstring((char*) packet->payload.evt_bt_inquiry_result.address.addr, 6);
		break;

	case EZS_IDX_EVT_BT_INQUIRY_COMPLETE:
		WE_DEBUG_PRINT("RX: evt_bt_inquiry_complete");
		states.Classic.inquiry_running = false;
		break;

	case EZS_IDX_RSP_BT_CANCEL_INQUIRY:
		WE_DEBUG_PRINT("RX: rsp_bt_cancel_inquiry: result=0x%04x", packet->payload.rsp_bt_cancel_inquiry.result);
		states.Classic.inquiry_running = false;
		break;

	case EZS_IDX_RSP_BT_QUERY_NAME:
		WE_DEBUG_PRINT("RX: rsp_bt_query_name: result=0x%04x", packet->payload.rsp_bt_query_name.result);
		break;

	case EZS_IDX_RSP_BT_CONNECT:
		WE_DEBUG_PRINT("RX: rsp_bt_connect, result=0x%04x", packet->payload.rsp_bt_connect.result);
		states.Classic.pinentry_requested = false;
		states.Common.pairing_requested = false;
		break;

	case EZS_IDX_RSP_BT_DISCONNECT:
		WE_DEBUG_PRINT("RX: rsp_bt_disconnect, result=0x%04x", packet->payload.rsp_bt_disconnect.result);
		break;

	case EZS_IDX_EVT_BT_CONNECTED:
		WE_DEBUG_PRINT("RX: evt_bt_connected, handle=%d, address=", packet->payload.evt_bt_connected.conn_handle);
		print_hexstring((char*) packet->payload.evt_bt_connected.address.addr, 6);
		states.Classic.conn_handle = packet->payload.evt_bt_connected.conn_handle;
		states.Classic.connected = true;
		break;

	case EZS_IDX_EVT_BT_CONNECTION_FAILED:
		WE_DEBUG_PRINT("RX: evt_bt_connection_failed, handle=%d, reason=%d", packet->payload.evt_bt_connection_failed.conn_handle, packet->payload.evt_bt_connection_failed.reason);
		states.Classic.connected = false;
		states.Classic.conn_handle = 0;
		break;

	case EZS_IDX_EVT_BT_DISCONNECTED:
		WE_DEBUG_PRINT("RX: evt_bt_disconnected, handle=%d, reason=%d", packet->payload.evt_bt_disconnected.conn_handle, packet->payload.evt_bt_disconnected.reason);
		states.Classic.connected = false;
		states.Common.pairing_requested = false;
		states.Classic.pinentry_requested = false;
		states.Classic.conn_handle = 0;
		break;

	case EZS_IDX_RSP_BT_QUERY_CONNECTIONS:
		WE_DEBUG_PRINT("RX: rsp_bt_query_connections, result=0x%04x, connections=%d", packet->payload.rsp_bt_query_connections.result, packet->payload.rsp_bt_query_connections.count);
		break;

	case EZS_IDX_RSP_BT_QUERY_RSSI:
		WE_DEBUG_PRINT("RX: rsp_bt_query_rssi, result=0x%04x, rssi=%d", packet->payload.rsp_bt_query_rssi.result, packet->payload.rsp_bt_query_rssi.rssi);
		break;

	case EZS_IDX_EVT_SMP_BOND_ENTRY:
		WE_DEBUG_PRINT("RX: evt_smp_bond_entry, handle=%d, address=", packet->payload.evt_smp_bond_entry.handle);
		print_hexstring((char*) packet->payload.evt_smp_bond_entry.address.addr, 6);
		break;

	case EZS_IDX_EVT_SMP_PAIRING_REQUESTED:
		WE_DEBUG_PRINT("RX: evt_smp_pairing_requested, handle=%d", packet->payload.evt_smp_pairing_requested.conn_handle);
		states.Common.pairing_requested = true;
		//respond with smp_send_pairreq_response
		break;

	case EZS_IDX_EVT_SMP_PAIRING_RESULT:
		WE_DEBUG_PRINT("RX: evt_smp_pairing_result, handle=%d, result=0x%04x", packet->payload.evt_smp_pairing_result.conn_handle, packet->payload.evt_smp_pairing_result.result);
		states.Common.pairing_requested = false;
		states.Classic.connected = (packet->payload.evt_smp_pairing_result.result == 0x00);
		states.Classic.conn_handle = packet->payload.evt_smp_pairing_result.conn_handle;
		break;

	case EZS_IDX_EVT_SMP_ENCRYPTION_STATUS:
		WE_DEBUG_PRINT("RX: evt_smp_encryption_status, handle=%d, status=%d", packet->payload.evt_smp_encryption_status.conn_handle, packet->payload.evt_smp_encryption_status.status);
		break;

	case EZS_IDX_EVT_SMP_PASSKEY_DISPLAY_REQUESTED:
		WE_DEBUG_PRINT("RX: evt_smp_passkey_display_requested, handle=%d, key=%lu / 0x%x", packet->payload.evt_smp_passkey_display_requested.conn_handle, packet->payload.evt_smp_passkey_display_requested.passkey, (unsigned int) packet->payload.evt_smp_passkey_display_requested.passkey);
		break;

	case EZS_IDX_EVT_SMP_PASSKEY_ENTRY_REQUESTED:
		WE_DEBUG_PRINT("RX: evt_smp_passkey_entry_requested, handle=%d", packet->payload.evt_smp_passkey_entry_requested.conn_handle);
		states.LE.passkeyentry_requested = true;
		//respond with ezs_cmd_smp_send_passkeyreq_response
		break;

	case EZS_IDX_EVT_SMP_PIN_ENTRY_REQUESTED:
		WE_DEBUG_PRINT("RX: evt_smp_pin_entry_requested, address=");
		print_hexstring((char*) packet->payload.evt_smp_pin_entry_requested.address.addr, 6);
		states.Classic.pinentry_requested = true;
		//respond with smp_send_pinreq_response
		break;

	case EZS_IDX_EVT_SPP_DATA_RECEIVED:
		WE_DEBUG_PRINT("RX: evt_spp_data_received, handle=%d: ", packet->payload.evt_spp_data_received.conn_handle);
		print_hexstring((char*) packet->payload.evt_spp_data_received.data.data, packet->payload.evt_spp_data_received.data.length);
		break;

	case EZS_IDX_RSP_SMP_GET_SECURITY_PARAMETERS:
		WE_DEBUG_PRINT("RX: rsp_smp_get_security_parameters, result=0x%04x", packet->payload.rsp_smp_get_security_parameters.result);
		break;

	case EZS_IDX_RSP_SMP_SET_SECURITY_PARAMETERS:
		WE_DEBUG_PRINT("RX: rsp_smp_set_security_parameters, result=0x%04x", packet->payload.rsp_smp_set_security_parameters.result);
		break;

	case EZS_IDX_RSP_SMP_PAIR:
		WE_DEBUG_PRINT("RX: rsp_smp_pair, result=0x%04x", packet->payload.rsp_smp_pair.result);
		break;

	case EZS_IDX_EVT_GAP_CONNECTION_UPDATED:
		WE_DEBUG_PRINT("RX: evt_gap_connection_updated, handle=%d, interval=%d", packet->payload.evt_gap_connection_updated.conn_handle, packet->payload.evt_gap_connection_updated.interval);
		break;

	case EZS_IDX_RSP_SPP_SET_CONFIG:
		WE_DEBUG_PRINT("RX: rsp_spp_set_config, result=0x%04x", packet->payload.rsp_spp_set_config.result);
		break;

	case EZS_IDX_RSP_SPP_GET_CONFIG:
		WE_DEBUG_PRINT("RX: rsp_spp_get_config, result=0x%04x, connections=%d", packet->payload.rsp_spp_get_config.result, packet->payload.rsp_spp_get_config.connections);
		break;

	case EZS_IDX_RSP_P_CYSPP_START:
		WE_DEBUG_PRINT("RX: rsp_p_cyspp_start, result=0x%04x", packet->payload.rsp_p_cyspp_start.result);
		break;

	case EZS_IDX_RSP_P_CYSPP_SET_PARAMETERS:
		WE_DEBUG_PRINT("RX: rsp_p_cyspp_set_parameters, result=0x%04x", packet->payload.rsp_p_cyspp_set_parameters.result);
		break;

	case EZS_IDX_RSP_P_CYSPP_GET_PARAMETERS:
		WE_DEBUG_PRINT("RX: rsp_p_cyspp_get_parameters, result=0x%04x, role=%d, enable=%d", packet->payload.rsp_p_cyspp_get_parameters.result, packet->payload.rsp_p_cyspp_get_parameters.role, packet->payload.rsp_p_cyspp_get_parameters.enable);
		break;

	case EZS_IDX_RSP_P_CYSPP_SET_CLIENT_HANDLES:
		WE_DEBUG_PRINT("RX: rsp_p_cyspp_set_client_handles, result=0x%04x", packet->payload.rsp_p_cyspp_set_client_handles.result);
		break;

	case EZS_IDX_RSP_P_CYSPP_GET_CLIENT_HANDLES:
		WE_DEBUG_PRINT("RX: rsp_p_cyspp_get_client_handles, result=0x%04x", packet->payload.rsp_p_cyspp_get_client_handles.result);
		break;

	case EZS_IDX_RSP_P_CYSPP_SET_PACKETIZATION:
		WE_DEBUG_PRINT("RX: rsp_p_cyspp_set_packetization, result=0x%04x", packet->payload.rsp_p_cyspp_set_packetization.result);
		break;

	case EZS_IDX_RSP_P_CYSPP_GET_PACKETIZATION:
		WE_DEBUG_PRINT("RX: rsp_p_cyspp_get_packetization, result=0x%04x, mode=%d", packet->payload.rsp_p_cyspp_get_packetization.result, packet->payload.rsp_p_cyspp_get_packetization.mode);
		break;

	case EZS_IDX_EVT_P_CYCOMMAND_STATUS:
		WE_DEBUG_PRINT("RX: evt_p_cycommand_status, status=0x%04x", packet->payload.evt_p_cycommand_status.status);
		break;

	case EZS_IDX_EVT_P_CYSPP_STATUS:
		states.LE_CYSPP.peer_is_subscribed = false;
		WE_DEBUG_PRINT("RX: evt_p_cyspp_status, status=0x%04x\r\n", packet->payload.evt_p_cyspp_status.status);
		for (int i = 0; i < 8; i++)
		{
			if ((0x01 & (packet->payload.evt_p_cyspp_status.status >> i)) == 0x01)
			{
				switch (i)
				{
				case 0:
					WE_DEBUG_PRINT("Unacknowledged data subscribed\r\n");
					states.LE_CYSPP.peer_is_subscribed = true;
					break;
				case 1:
					WE_DEBUG_PRINT("Acknowledged data subscribed\r\n");
					states.LE_CYSPP.peer_is_subscribed = true;
					break;
				case 2:
					WE_DEBUG_PRINT("RX flow subscribed\r\n");
					break;
				case 3:
					WE_DEBUG_PRINT("RX flow blocked by remote Server\r\n");
					break;
				case 4:
					WE_DEBUG_PRINT("CYSPP peer support verified\r\n");
					break;
				case 5:
					WE_DEBUG_PRINT("Data mode active\r\n");
					break;
				default:
					break;
				}
			}
		}
		break;

	case EZS_IDX_EVT_GATTS_DATA_WRITTEN:
		WE_DEBUG_PRINT("RX: evt_gatts_data_written, %d bytes, data=", packet->payload.evt_gatts_data_written.data.length);
		print_hexstring((char*) packet->payload.evt_gatts_data_written.data.data, packet->payload.evt_gatts_data_written.data.length);

#if 1
		/* Proteus SPP-like profile */
#define HANDLE_SPPLIKE_CCCD_TX_CHAR 0x0022
		if ((packet->payload.evt_gatts_data_written.attr_handle == HANDLE_SPPLIKE_CCCD_TX_CHAR) && (packet->payload.evt_gatts_data_written.data.length == 2))
		{
			states.LE_SPPLike.peer_is_subscribed = (0x01 == (packet->payload.evt_gatts_data_written.data.data[0] & 0x01));
		}
#endif
		break;

	case EZS_IDX_EVT_GATTC_DISCOVER_RESULT:
		WE_DEBUG_PRINT("RX: evt_gattc_discover_result, attr_handle=0x%04x, attr_handle_rel=0x%04x, type=0x%02x, uuid=", packet->payload.evt_gattc_discover_result.attr_handle, packet->payload.evt_gattc_discover_result.attr_handle_rel, packet->payload.evt_gattc_discover_result.type);
		print_hexstring((char*) packet->payload.evt_gattc_discover_result.uuid.data, packet->payload.evt_gattc_discover_result.uuid.length);
		break;

	case EZS_IDX_EVT_GATTC_REMOTE_PROCEDURE_COMPLETE:
		WE_DEBUG_PRINT("RX: evt_gattc_remote_procedure_complete, result=0x%04x", packet->payload.evt_gattc_remote_procedure_complete.result);
		break;

	case EZS_IDX_EVT_GATTC_DATA_RECEIVED:
		WE_DEBUG_PRINT("RX: evt_gattc_data_received, conn_handle=0x%04x, attr_handle=0x%04x: ", packet->payload.evt_gattc_data_received.conn_handle, packet->payload.evt_gattc_data_received.attr_handle);
		print_hexstring((char*) packet->payload.evt_gattc_data_received.data.data, packet->payload.evt_gattc_data_received.data.length);
		break;

	case EZS_IDX_EVT_GATTC_WRITE_RESPONSE:
		WE_DEBUG_PRINT("RX: evt_gattc_write_response, result=0x%04x", packet->payload.evt_gattc_write_response.result);
		break;

	case EZS_IDX_RSP_GPIO_GET_INTERRUPT_MODE:
		WE_DEBUG_PRINT("RX: rsp_gpio_get_interrupt_mode, result=0x%04x", packet->payload.rsp_gpio_get_interrupt_mode.result);
		break;

	case EZS_IDX_RSP_GPIO_SET_INTERRUPT_MODE:
		WE_DEBUG_PRINT("RX: rsp_gpio_set_interrupt_mode, result=0x%04x", packet->payload.rsp_gpio_set_interrupt_mode.result);
		break;

	case EZS_IDX_RSP_GPIO_GET_FUNCTION:
		WE_DEBUG_PRINT("RX: rsp_gpio_get_function, result=0x%04x", packet->payload.rsp_gpio_get_function.result);
		break;

	case EZS_IDX_RSP_GPIO_SET_FUNCTION:
		WE_DEBUG_PRINT("RX: rsp_gpio_set_function, result=0x%04x", packet->payload.rsp_gpio_set_function.result);
		break;

	case EZS_IDX_RSP_GPIO_QUERY_ADC:
		WE_DEBUG_PRINT("RX: rsp_gpio_query_adc, result=0x%04x, value=0x%04x", packet->payload.rsp_gpio_query_adc.result, packet->payload.rsp_gpio_query_adc.value);
		break;

	case EZS_IDX_RSP_GPIO_GET_DRIVE:
		WE_DEBUG_PRINT("RX: rsp_gpio_get_drive, result=0x%04x", packet->payload.rsp_gpio_get_drive.result);
		break;

	case EZS_IDX_RSP_GPIO_SET_DRIVE:
		WE_DEBUG_PRINT("RX: rsp_gpio_set_drive, result=0x%04x", packet->payload.rsp_gpio_set_drive.result);
		break;

	case EZS_IDX_RSP_GPIO_GET_LOGIC:
		WE_DEBUG_PRINT("RX: rsp_gpio_get_logic, result=0x%04x", packet->payload.rsp_gpio_get_logic.result);
		break;

	case EZS_IDX_RSP_GPIO_SET_LOGIC:
		WE_DEBUG_PRINT("RX: rsp_gpio_set_logic, result=0x%04x", packet->payload.rsp_gpio_set_logic.result);
		break;

	case EZS_IDX_EVT_GATTS_DISCOVER_RESULT:
		WE_DEBUG_PRINT("RX: evt_gatts_discover_result, attr_handle=0x%04x, attr_handle_rel=0x%04x, type=0x%04x, uuid=", packet->payload.evt_gatts_discover_result.attr_handle, packet->payload.evt_gatts_discover_result.attr_handle_rel, packet->payload.evt_gatts_discover_result.type);
		print_hexstring((char*) packet->payload.evt_gatts_discover_result.uuid.data, packet->payload.evt_gatts_discover_result.uuid.length);
		break;

	case EZS_IDX_EVT_GATTS_INDICATION_CONFIRMED:
		WE_DEBUG_PRINT("RX: evt_gatts_indication_confirmed, conn_handle=0x%04x", packet->payload.evt_gatts_indication_confirmed.conn_handle);
		break;

	case EZS_IDX_EVT_GATTS_DB_ENTRY_BLOB:
		WE_DEBUG_PRINT("RX: evt_gatts_db_entry_blob, length=0x%04x", packet->payload.evt_gatts_db_entry_blob.length);
		break;

	default:
		WE_DEBUG_PRINT("RX: unhandled packet: %d/%d", packet->header.group, packet->header.id);
		break;
	}

	WE_DEBUG_PRINT("\r\n");
}

void transparentDataHandler(uint8_t *payload, uint16_t payloadLength)
{
	WE_DEBUG_PRINT("Transparent data received:");
	print_hexstring((char*) payload, payloadLength);
	WE_DEBUG_PRINT("\r\n");
}

void print_ascii(const char *text, size_t width)
{
	for (size_t idx = 0; idx < width; ++idx)
		WE_DEBUG_PRINT("%c", text[idx]);
}

void print_hexstring(const char *text, size_t width)
{
	WE_DEBUG_PRINT("0x");
	for (size_t idx = 0; idx < width; ++idx)
		WE_DEBUG_PRINT("%02x", text[idx]);
}

void Examples_Print(char *str, bool success)
{
	WE_DEBUG_PRINT("%s%s\r\n", success ? "OK    " : "NOK   ", str);
}

