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
 * @brief Calypso P2P example.
 */
#include <Calypso/Calypso_P2P_Example.h>
#include <ctype.h>
#include <stdio.h>
#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/ATCommands/ATEvent.h>
#include <Calypso/ATCommands/ATNetCfg.h>
#include <Calypso/ATCommands/ATWLAN.h>
#include <Calypso/Calypso.h>
#include <Calypso/Calypso_Examples.h>

/**
 * @brief TCP port used for communicating with peer.
 */
static uint16_t p2pExamplePort = 8888;

/**
 * @brief Timeout for P2P WLAN connection after P2P connect request event has been triggered
 */
static uint32_t p2pConnectTimeoutMs = 20000;

/**
 * @brief Is set to true when a P2P device found event has been triggered
 */
static bool p2pDeviceFound = false;

/**
 * @brief Is set to true when a P2P connect request event has been triggered
 */
static bool p2pRequestReceived = false;

/**
 * @brief Is set to true when a P2P connect event has been triggered
 */
static bool p2pConnected = false;

/**
 * @brief Is set to true when a P2P connect failed event has been triggered
 */
static bool p2pConnectFail = false;

/**
 * @brief Is set to true when a P2P connection has been established with this device being group owner
 */
static bool p2pIsGroupOwner = false;

/**
 * @brief Is set to true when the TCP server has accepted a connection. Used in TCP server example.
 */
static bool p2pServerConnectionAccepted = false;

/**
 * @brief Last TCP server accept event (filled in when a client connects to the server). Used in TCP server example.
 */
static Calypso_ATEvent_SocketTCPAccept_t p2pServerAcceptEvent = {
		0 };

/**
 * @brief Is set to true when the TCP client has established a connection to the server. Used in TCP client example.
 */
static bool p2pClientConnectionEstablished = false;

/**
 * @brief Last TCP client connect event (filled in when the client has established a connection to the server). Used in TCP client example.
 */
static Calypso_ATEvent_SocketTCPConnect_t p2pClientConnectEvent = {
		0 };

/**
 * @brief Is set to true when a TCP connection has been established. Used in TCP server and client examples.
 */
static bool peerTcpConnected = false;

/**
 * @brief Is set to true when waiting for incoming data via TCP or UDP
 */
static bool p2pExampleWaitingForData = false;

/**
 * @brief Rx buffer for received TCP or UDP data. Note: Max recommended payload size is 1460 bytes.
 */
char p2pExampleReceiveBuffer[CALYPSO_MAX_PAYLOAD_SIZE + 1];

void Calypso_P2P_Example_EventCallback(char *eventText);
void Calypso_P2P_Example_OnDataReceived(Calypso_ATEvent_SocketRcvd_t *rcvdEvent);

void Calypso_P2P_Example(void)
{
	WE_DEBUG_PRINT("*** Start of Calypso P2P example ***\r\n");

	bool ret;

	if (!Calypso_Init(&Calypso_uart, &Calypso_pins, &Calypso_P2P_Example_EventCallback))
	{
		WE_DEBUG_PRINT("Initialization error\r\n");
		return;
	}

	Calypso_PinReset();

	Calypso_Examples_WaitForStartup(5000);

	/* Get version info. This retrieves Calypso's firmware version (amongst other version info) and
	 * stores the firmware version in Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
	 * Calypso_firmwareVersionPatch for later use. */
	Calypso_ATDevice_Value_t deviceValue;
	ret = Calypso_ATDevice_Get(Calypso_ATDevice_GetId_General, Calypso_ATDevice_GetGeneral_Version, &deviceValue);
	Calypso_Examples_Print("Get device version", ret);

	/* Set IPv4 address method DHCP */
	Calypso_ATNetCfg_IPv4Config_t newIpV4Config = {
			0 };
	newIpV4Config.method = Calypso_ATNetCfg_IPv4Method_Dhcp;
	ret = Calypso_ATNetCfg_SetIPv4AddressStation(&newIpV4Config);
	Calypso_Examples_Print("Set IPv4 DHCP", ret);

	/* Set WLAN policy to connect to the first P2P device that is discovered */
	ret = Calypso_ATWLAN_SetConnectionPolicy(Calypso_ATWLAN_PolicyConnection_P2P);
	Calypso_Examples_Print("Set P2P connection policy", ret);

	/* Set WLAN P2P policy to negotiate P2P role (instead of setting a fixed role) */
	ret = Calypso_ATWLAN_SetP2PPolicy(Calypso_ATWLAN_PolicyP2P_Negotiate, Calypso_ATWLAN_PolicyP2PValue_RandBackoff);
	Calypso_Examples_Print("Set P2P negotiation policy", ret);

	/* Enable P2P mode */
	ret = Calypso_ATWLAN_SetMode(Calypso_ATWLAN_SetMode_P2P);
	Calypso_Examples_Print("Set P2P mode", ret);

	/* Main P2P example loop - tries to establish a connection to the first peer being discovered,
	 * then creates a TCP port and waits for incoming connections. When the peer connects to the
	 * port, we wait for incoming characters and send the characters back (converted to uppercase).
	 * When the connection fails / is lost, the network processor is restarted and the sequence
	 * starts from the beginning. */
	while (true)
	{
		p2pConnected = false;
		p2pDeviceFound = false;
		p2pRequestReceived = false;
		p2pConnectFail = false;
		p2pIsGroupOwner = false;

		/* Restart the network processor */
		ret = Calypso_ATDevice_Restart(0);
		Calypso_Examples_Print("Restart NWP", ret);

		/* Scan for P2P devices until a P2P device found event has been received (or until the connection attempt fails) */
		Calypso_ATWLAN_ScanEntry_t scanEntries[5];
		uint8_t numScanEntries;
		while (!p2pDeviceFound && !p2pConnectFail)
		{
			ret = Calypso_ATWLAN_Scan(0, 5, scanEntries, &numScanEntries);
			Calypso_Examples_Print("Scan for P2P devices", ret);

			if (ret)
			{
				WE_DEBUG_PRINT("Number of scan entries: %d\r\n", numScanEntries);

				if (numScanEntries == 0)
				{
					break;
				}
			}

			WE_Delay(1000);
		}

		/* Wait for P2P request event (or until connection attempt fails / times out) */
		uint32_t t = WE_GetTick();
		while (!p2pRequestReceived && !p2pConnectFail)
		{
			WE_Delay(100);

			if (WE_GetTick() - t > p2pConnectTimeoutMs)
			{
				WE_DEBUG_PRINT("Timeout waiting for P2P request event.\r\n");
				p2pConnectFail = true;
				break;
			}
		}

		/* Wait for connection to be established (or until connection attempt fails / times out) */
		t = WE_GetTick();
		while (!p2pConnected && !p2pConnectFail)
		{
			WE_Delay(100);

			if (WE_GetTick() - t > p2pConnectTimeoutMs)
			{
				WE_DEBUG_PRINT("Timeout waiting for P2P connect event.\r\n");
				p2pConnectFail = true;
				break;
			}
		}

		if (p2pConnected)
		{
			/* Connection to peer has been established! */

			p2pServerConnectionAccepted = false;
			peerTcpConnected = false;
			p2pExampleWaitingForData = false;

			/* Get IPv4 configuration (group owner = AP, client = station) */
			Calypso_ATNetCfg_IPv4Config_t ipV4Config;
			if (p2pIsGroupOwner)
			{
				ret = Calypso_ATNetCfg_GetIPv4AddressAP(&ipV4Config);
				Calypso_Examples_Print("Get IPv4 config", ret);
			}
			else
			{
				ret = Calypso_ATNetCfg_GetIPv4AddressStation(&ipV4Config);
				Calypso_Examples_Print("Get IPv4 config", ret);
			}
			if (ret)
			{
				WE_DEBUG_PRINT("*** P2P IPv4 configuration ***\r\n");
				WE_DEBUG_PRINT("IPv4 address: %s\r\n", ipV4Config.ipAddress);
				WE_DEBUG_PRINT("Subnet mask: %s\r\n", ipV4Config.subnetMask);
				WE_DEBUG_PRINT("Gateway address: %s\r\n", ipV4Config.gatewayAddress);
				WE_DEBUG_PRINT("DNS address: %s\r\n", ipV4Config.dnsAddress);
			}

			/* Create TCP socket, bind it to a port and start listening for incoming connections */
			uint8_t socketID, tcpServerClientSocketID = 0;
			ret = Calypso_ATSocket_Create(Calypso_ATSocket_Family_INET, Calypso_ATSocket_Type_Stream, Calypso_ATSocket_Protocol_TCP, &socketID);
			Calypso_Examples_Print("Create socket", ret);

			Calypso_ATSocket_Descriptor_t socketDescriptor;
			socketDescriptor.family = Calypso_ATSocket_Family_INET;
			strcpy(socketDescriptor.address, ipV4Config.ipAddress);
			socketDescriptor.port = p2pExamplePort;
			ret = Calypso_ATSocket_Bind(socketID, socketDescriptor);
			Calypso_Examples_Print("Bind", ret);

			ret = Calypso_ATSocket_Listen(socketID, 10);
			Calypso_Examples_Print("Listen", ret);

			ret = Calypso_ATSocket_Accept(socketID, Calypso_ATSocket_Family_INET);
			Calypso_Examples_Print("Accept", ret);

			bool firstRun = true;
			while (p2pConnected)
			{
				if (p2pServerConnectionAccepted)
				{
					/* The peer has connected to the server */
					p2pServerConnectionAccepted = false;

					WE_DEBUG_PRINT("Peer %s:%d connected.\r\n", p2pServerAcceptEvent.clientAddress, p2pServerAcceptEvent.clientPort);

					/* This is the socket ID that can be used for communicating with the peer */
					tcpServerClientSocketID = p2pServerAcceptEvent.socketID;

					peerTcpConnected = true;
				}

				if (peerTcpConnected)
				{
					if (p2pExampleWaitingForData)
					{
						/* NOP */
					}
					else
					{
						/* Start waiting for data. An Calypso_ATEvent_SocketRcvd event is generated when new
						 * data is available (see Calypso_eventCallback()). */
						p2pExampleWaitingForData = true;
						Calypso_ATSocket_Receive(tcpServerClientSocketID, Calypso_DataFormat_Binary, CALYPSO_MAX_PAYLOAD_SIZE);

						size_t len = strlen(p2pExampleReceiveBuffer);
						if (!firstRun && len == 0)
						{
							/* Connection lost (receive event with length zero was triggered) */
							break;
						}

						firstRun = false;

						if (len > 0)
						{
							/* Data has been received - convert to uppercase and send back */
							for (size_t i = 0; i < len; i++)
							{
								p2pExampleReceiveBuffer[i] = toupper(p2pExampleReceiveBuffer[i]);
							}
							uint16_t bytesSent = 0;
							Calypso_ATSocket_Send(tcpServerClientSocketID, Calypso_DataFormat_Binary,
							false, len, p2pExampleReceiveBuffer, &bytesSent);
						}
					}
				}

				WE_Delay(100);
			}

			Calypso_ATWLAN_Disconnect();
		}
	}

	Calypso_Deinit();
}

/**
 * @brief Is called when an event notification has been received.
 *
 * Note that this function is called from an interrupt - code in this function
 * should thus be kept simple.
 *
 * Note in particular that it is not possible to send AT commands to Calypso
 * from within this event handler.
 *
 * Also note that not all calls of this handler necessarily correspond to valid
 * events (i.e. events from Calypso_ATEvent_t). Some events might in fact be responses
 * to AT commands that are not included in Calypso_ATEvent_t.
 */
void Calypso_P2P_Example_EventCallback(char *eventText)
{
	Calypso_Examples_EventCallback(eventText);

	Calypso_ATEvent_t event;
	if (false == Calypso_ATEvent_ParseEventType(&eventText, &event))
	{
		return;
	}

	switch (event)
	{
	case Calypso_ATEvent_SocketTxFailed:
		break;

	case Calypso_ATEvent_SocketAsyncEvent:
		break;

	case Calypso_ATEvent_SocketTCPConnect:
		if (Calypso_ATEvent_ParseSocketTCPConnectEvent(&eventText, &p2pClientConnectEvent))
		{
			p2pClientConnectionEstablished = true;
		}
		break;

	case Calypso_ATEvent_SocketTCPAccept:
		if (Calypso_ATEvent_ParseSocketTCPAcceptEvent(&eventText, &p2pServerAcceptEvent))
		{
			p2pServerConnectionAccepted = true;
		}
		break;

	case Calypso_ATEvent_SocketRcvd:
	case Calypso_ATEvent_SocketRcvdFrom:
	{
		Calypso_ATEvent_SocketRcvd_t rcvdEvent;
		if (Calypso_ATEvent_ParseSocketRcvdEvent(&eventText, false, &rcvdEvent))
		{
			Calypso_P2P_Example_OnDataReceived(&rcvdEvent);
		}
		break;
	}

	case Calypso_ATEvent_WlanP2PConnect:
		p2pIsGroupOwner = false;
		p2pConnected = true;
		p2pConnectFail = false;
		break;

	case Calypso_ATEvent_WlanP2PDisconnect:
		p2pConnected = false;
		break;

	case Calypso_ATEvent_WlanP2PClientAdded:
		p2pIsGroupOwner = true;
		p2pConnected = true;
		break;

	case Calypso_ATEvent_WlanP2PClientRemoved:
		p2pConnected = false;
		break;

	case Calypso_ATEvent_WlanP2PDevFound:
		p2pDeviceFound = true;
		break;

	case Calypso_ATEvent_WlanP2PRequest:
		p2pRequestReceived = true;
		break;

	case Calypso_ATEvent_WlanP2PConnectFail:
		p2pConnectFail = true;
		break;

	case Calypso_ATEvent_Startup:
	case Calypso_ATEvent_WakeUp:
	case Calypso_ATEvent_Ping:
	case Calypso_ATEvent_GeneralResetRequest:
	case Calypso_ATEvent_GeneralError:
	case Calypso_ATEvent_WlanConnect:
	case Calypso_ATEvent_WlanDisconnect:
	case Calypso_ATEvent_WlanStaAdded:
	case Calypso_ATEvent_WlanStaRemoved:
	case Calypso_ATEvent_WlanProvisioningStatus:
	case Calypso_ATEvent_WlanProvisioningProfileAdded:
	case Calypso_ATEvent_NetappIP4Acquired:
	case Calypso_ATEvent_NetappIP6Acquired:
	case Calypso_ATEvent_NetappIPCollision:
	case Calypso_ATEvent_NetappDHCPv4_leased:
	case Calypso_ATEvent_NetappDHCPv4_released:
	case Calypso_ATEvent_NetappIPv4Lost:
	case Calypso_ATEvent_NetappDHCPIPv4AcquireTimeout:
	case Calypso_ATEvent_NetappIPv6Lost:
	case Calypso_ATEvent_MQTTOperation:
	case Calypso_ATEvent_MQTTRecv:
	case Calypso_ATEvent_MQTTDisconnect:
	case Calypso_ATEvent_FileListEntry:
	case Calypso_ATEvent_HTTPGet:
	case Calypso_ATEvent_CustomGPIO:
	case Calypso_ATEvent_CustomHTTPPost:
	case Calypso_ATEvent_FatalErrorDeviceAbort:
	case Calypso_ATEvent_FatalErrorDriverAbort:
	case Calypso_ATEvent_FatalErrorSyncLost:
	case Calypso_ATEvent_FatalErrorNoCmdAck:
	case Calypso_ATEvent_FatalErrorCmdTimeout:
	default:
		break;
	}
}

/**
 * @brief TCP / UDP data received callback.
 */
void Calypso_P2P_Example_OnDataReceived(Calypso_ATEvent_SocketRcvd_t *rcvdEvent)
{
	memcpy(p2pExampleReceiveBuffer, rcvdEvent->data, rcvdEvent->length);
	p2pExampleReceiveBuffer[rcvdEvent->length] = '\0';
	WE_DEBUG_PRINT("RECEIVED %s\r\n", p2pExampleReceiveBuffer);

	p2pExampleWaitingForData = false;
}
