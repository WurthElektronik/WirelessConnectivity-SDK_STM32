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
 * @brief Calypso Azure Plug and Play example.
 */
#include <Calypso/Examples/Calypso_Azure_PnP_Example/Calypso_Azure_PnP_Example.h>
#include <Calypso/Examples/Calypso_Examples.h>
#include <stdio.h>
#include <Calypso/Calypso.h>
#include <Calypso/ATCommands/ATEvent.h>
#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/ATCommands/ATMQTT.h>
#include <Calypso/ATCommands/ATNetCfg.h>
#include <Calypso/ATCommands/ATSocket.h>
#include <Calypso/ATCommands/ATWLAN.h>
#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/ATCommands/ATFile.h>
#include <Calypso/ATCommands/ATNetApp.h>
#include <utils/json-builder.h>

/**
 * @brief Contains information on last startup event (if any)
 */
Calypso_ATEvent_Startup_t Calypso_Azure_PnP_startupEvent = {
		0 };

/**
 * @brief Is set to true when a startup event is received
 */
bool Calypso_Azure_PnP_startupEventReceived = false;

/**
 * @brief Is set to true when an MQTT operation event is received
 */
bool Calypso_Azure_PnP_mqttOpEvent = false;

/**
 * @brief Is set to true when an MQTT recv event is received
 */
bool Calypso_Azure_PnP_mqttRecvEvent = false;

/**
 * @brief Contains information of the last MQTT recv event
 */
Calypso_ATEvent_MQTTRcvd_t recvEvent;

/**
 * @brief Request ID - Unique ID added to each request
 */
static uint8_t reqID = 0;

/**
 * @brief Address of the IoT hub to connect
 */
static char iotHubAddress[CALYPSO_MAX_HOST_NAME_LENGTH] = {
		0 };

static bool Calypso_Azure_PnP_WaitForMqttOpEvent(uint32_t timeoutMs);
static bool Calypso_Azure_PnP_WaitForStartup(uint32_t timeoutMs);
static bool Calypso_Azure_PnP_WaitForMqttRecvEvent(uint32_t timeoutMs);

static void Calypso_Azure_PnP_Print(char *str, bool success);
static void Calypso_Azure_PnP_EventCallback(char *eventText);

static bool Calypso_Azure_PnP_SNTP_Setup();

static bool Calypso_Azure_PnP_Write_Certs();

static bool Calypso_Azure_PnP_Provision();

static bool Calypso_Azure_PnP_Connect_To_IoT_Hub(uint8_t *mqttIndex);

static bool Calypso_Azure_PnP_Publish_Reg_Req(uint8_t mqttIndex);
static bool Calypso_Azure_PnP_Publish_Status_Req(uint8_t mqttIndex, char *operationID);
static bool Calypso_Azure_PnP_Publish_MAC_Adr(uint8_t mqttIndex);
static bool Calypso_Azure_PnP_Publish_Version(uint8_t mqttIndex);
static bool Calypso_Azure_PnP_Publish_UDID(uint8_t mqttIndex);

static bool Calypso_Azure_PnP_File_Exists(const char *fileName);
static bool Calypso_Azure_PnP_Write_File(const char *path, const char *data, uint16_t dataLength);
static bool Calypso_Azure_PnP_readFile(const char *path, char *data, uint16_t dataLength, uint16_t *outputLength);

static bool Calypso_Azure_PnP_MQTT_Publish(uint8_t mqttIndex, char *topic, uint8_t retain, char *data, int length, bool encode);

static char* Calypso_Azure_PnP_Serialize_Prov_Req();

/**
 * @brief MQTT example.
 */
void Calypso_Azure_PnP_Example(void)
{
	printf("*** Start of Calypso Azure PnP example ***\r\n");

	bool ret = false;
	uint8_t mqttIndex;

	if (!Calypso_Init(&Calypso_uart, &Calypso_pins, &Calypso_Azure_PnP_EventCallback))
	{
		printf("Initialization error\r\n");
		return;
	}

	Calypso_PinReset();

	Calypso_Azure_PnP_WaitForStartup(5000);

	WE_Delay(2000);

	Calypso_ATDevice_Test();

	Calypso_ATWLAN_Disconnect();

	/* Set IPv4 address method DHCP */
	Calypso_ATNetCfg_IPv4Config_t newIpV4Config = {
			0 };
	newIpV4Config.method = Calypso_ATNetCfg_IPv4Method_Dhcp;
	ret = Calypso_ATNetCfg_SetIPv4AddressStation(&newIpV4Config);
	Calypso_Azure_PnP_Print("Set IPv4 DHCP", ret);

	/* No automatic connection */
	ret = Calypso_ATWLAN_SetConnectionPolicy(Calypso_ATWLAN_PolicyConnection_None);
	Calypso_Azure_PnP_Print("Set no WLAN connection policy", ret);

	/* WLAN station mode */
	ret = Calypso_ATWLAN_SetMode(Calypso_ATWLAN_SetMode_Station);
	Calypso_Azure_PnP_Print("Set WLAN station mode", ret);

	ret = Calypso_ATDevice_Restart(0);
	Calypso_Azure_PnP_Print("Restart network processor", ret);

	WE_Delay(1000);
	/*If files needs to be over written comment out the following if statement and call Calypso_Azure_PnP_Write_Certs()*/
	if ((false == Calypso_Azure_PnP_File_Exists(ROOT_CA_PATH)) || (false == Calypso_Azure_PnP_File_Exists(DEVICE_CERT_PATH)) || (false == Calypso_Azure_PnP_File_Exists(DEVICE_KEY_PATH)))
	{
		/*(One time only) Write the certificates to the file system*/
		Calypso_Azure_PnP_Write_Certs();
	}

	/* Connect to WLAN */
	Calypso_ATWLAN_ConnectionArguments_t connectArgs;
	memset(&connectArgs, 0, sizeof(connectArgs));
	strcpy(connectArgs.SSID, CALYPSO_AZURE_PNP_WLAN_SSID);
	connectArgs.securityParams.securityType = Calypso_ATWLAN_SecurityType_WPA_WPA2;
	strcpy(connectArgs.securityParams.securityKey, CALYPSO_AZURE_PNP_WLAN_PW);

	ret = Calypso_ATWLAN_Connect(connectArgs);
	Calypso_Azure_PnP_Print("Connect to WLAN", ret);

	WE_Delay(3000);

	if (ret)
	{
		/*Setup SNTP server and update time*/
		ret = Calypso_Azure_PnP_SNTP_Setup();
		Calypso_Azure_PnP_Print("SNTP set-up", ret);
		if (ret)
		{
			if (false == Calypso_Azure_PnP_File_Exists(DEVICE_IOT_HUB_ADDRESS))
			{
				/*(One time only) Provision the module using the Azure Device Provisioning service*/
				ret = Calypso_Azure_PnP_Provision();
				Calypso_Azure_PnP_Print("Device provisioning ", ret);
			}
			if (ret)
			{
				/*Connect to the provisioned IoT hub*/
				ret = Calypso_Azure_PnP_Connect_To_IoT_Hub(&mqttIndex);
				Calypso_Azure_PnP_Print("Connection to IoT hub", ret);
				if (ret)
				{
					ret = Calypso_Azure_PnP_Publish_MAC_Adr(mqttIndex);
					Calypso_Azure_PnP_Print("Publish property: MAC address", ret);
					ret = Calypso_Azure_PnP_Publish_UDID(mqttIndex);
					Calypso_Azure_PnP_Print("Publish property: UDID", ret);
					ret = Calypso_Azure_PnP_Publish_Version(mqttIndex);
					Calypso_Azure_PnP_Print("Publish property: software", ret);
				}
			}
		}

	}
	Calypso_Deinit();
}

/**
 * @brief Connect to the provisioned IoT hub
 * @param[out] mqttIndex MQTT socket index
 * @return true if successful, false otherwise
 */
bool Calypso_Azure_PnP_Connect_To_IoT_Hub(uint8_t *mqttIndex)
{
	bool ret = false;
	Calypso_ATMQTT_SetValues_t mqttSetParams;
	uint16_t addressLength;
	/*Read the stored IoT hub address*/
	if (!Calypso_Azure_PnP_readFile(DEVICE_IOT_HUB_ADDRESS, iotHubAddress, CALYPSO_MAX_HOST_NAME_LENGTH, &addressLength))
	{
		return false;
	}
	/* Create MQTT client */
	Calypso_ATMQTT_ServerInfo_t serverInfo;
	strcpy(serverInfo.address, iotHubAddress);
	serverInfo.port = MQTT_PORT;

	Calypso_ATMQTT_SecurityParams_t securityParams;
	securityParams.securityMethod = Calypso_ATMQTT_SecurityMethod_TLSV1_2;
	strcpy(securityParams.CAFile, ROOT_CA_PATH);
	strcpy(securityParams.certificateFile, DEVICE_CERT_PATH);
	strcpy(securityParams.DHKey, ATCOMMAND_STRING_EMPTY);
	strcpy(securityParams.privateKeyFile, DEVICE_KEY_PATH);
	securityParams.cipher = Calypso_ATSocket_Cipher_TLS_RSA_WITH_AES_256_CBC_SHA256;

	Calypso_ATMQTT_ConnectionParams_t connectionParams;
	connectionParams.format = Calypso_DataFormat_Base64;
	connectionParams.protocolVersion = Calypso_ATMQTT_ProtocolVersion_v3_1_1;
	connectionParams.blockingSend = 0;

	ret = Calypso_ATMQTT_Create(CALYPSO_AZURE_PNP_DEVICE_ID, Calypso_ATMQTT_CreateFlags_URL | Calypso_ATMQTT_CreateFlags_Secure, serverInfo, securityParams, connectionParams, mqttIndex);
	Calypso_Azure_PnP_Print("Create MQTT client", ret);

	/* Set user name for the MQTT client */
	sprintf(mqttSetParams.username, "%s/%s/?api-version=2021-04-12&model-id=%s", iotHubAddress, CALYPSO_AZURE_PNP_DEVICE_ID, MODEL_ID);
	ret = Calypso_ATMQTT_Set(*mqttIndex, Calypso_ATMQTT_SetOption_User, &mqttSetParams);
	Calypso_Azure_PnP_Print("Set MQTT user name", ret);

	/* Connect to DPS MQTT broker */
	ret = Calypso_ATMQTT_Connect(*mqttIndex);

	if (!Calypso_Azure_PnP_WaitForMqttOpEvent(5000))
	{
		return false;
	}
	/* Subscribe to topics */
	Calypso_ATMQTT_SubscribeTopic_t topics[3];
	strcpy(topics[0].topic, DEVICE_TWIN_RES_TOPIC);
	strcpy(topics[1].topic, DEVICE_TWIN_DESIRED_PROP_RES_TOPIC);
	strcpy(topics[2].topic, DEVICE_TWIN_DESIRED_PROP_RES_TOPIC);
	topics[0].QoS = Calypso_ATMQTT_QoS_QoS1;
	topics[1].QoS = Calypso_ATMQTT_QoS_QoS1;
	topics[2].QoS = Calypso_ATMQTT_QoS_QoS1;
	ret = Calypso_ATMQTT_Subscribe(*mqttIndex, 3, topics);
	Calypso_Azure_PnP_Print("Subscribe to topics", ret);

	return Calypso_Azure_PnP_WaitForMqttOpEvent(1000);
}

/**
 * @brief Publish MAC address read-only property
 * @param[in] mqttIndex MQTT socket index
 * @return true if successful, false otherwise
 */
bool Calypso_Azure_PnP_Publish_MAC_Adr(uint8_t mqttIndex)
{

	uint8_t macAddress[6];
	char macAdrString[32];
	char topic[128];
	if (!Calypso_ATNetCfg_GetMacAddress(macAddress))
	{
		return false;
	}

	sprintf(macAdrString, "%02X:%02X:%02X:%02X:%02X:%02X\r\n", macAddress[0], macAddress[1], macAddress[2], macAddress[3], macAddress[4], macAddress[5]);

	json_value *payload = json_object_new(1);
	json_object_push(payload, "MACAddress", json_string_new(macAdrString));

	reqID++;
	sprintf(topic, "%s%u", DEVICE_TWIN_MESSAGE_PATCH, reqID);
	char *buf = malloc(json_measure(payload));
	json_serialize(buf, payload);
	json_builder_free(payload);
	if (!Calypso_Azure_PnP_MQTT_Publish(mqttIndex, topic, 1, buf, strlen(buf), true))
	{
		free(buf);
		return false;
	}
	free(buf);
	return Calypso_Azure_PnP_WaitForMqttRecvEvent(2000);
}

/**
 * @brief Publish software version read-only property
 * @param[in] mqttIndex MQTT socket index
 * @return true if successful, false otherwise
 */
bool Calypso_Azure_PnP_Publish_Version(uint8_t mqttIndex)
{
	char topic[128];

	Calypso_ATDevice_Value_t deviceValue;
	if (!Calypso_ATDevice_Get(Calypso_ATDevice_GetId_General, Calypso_ATDevice_GetGeneral_Version, &deviceValue))
	{
		return false;
	}

	json_value *payload = json_object_new(1);
	json_object_push(payload, "swVersion", json_string_new(deviceValue.general.version.calypsoFirmwareVersion));

	reqID++;
	sprintf(topic, "%s%u", DEVICE_TWIN_MESSAGE_PATCH, reqID);
	char *buf = malloc(json_measure(payload));
	json_serialize(buf, payload);
	json_builder_free(payload);
	if (!Calypso_Azure_PnP_MQTT_Publish(mqttIndex, topic, 1, buf, strlen(buf), true))
	{
		free(buf);
		return false;
	}
	free(buf);
	return Calypso_Azure_PnP_WaitForMqttRecvEvent(2000);
}

/**
 * @brief Publish UDID read-only property
 * @param[in] mqttIndex MQTT socket index
 * @return true if successful, false otherwise
 */
bool Calypso_Azure_PnP_Publish_UDID(uint8_t mqttIndex)
{
	char topic[128];

	Calypso_ATDevice_Value_t deviceValue;
	if (!Calypso_ATDevice_Get(Calypso_ATDevice_GetId_IOT, Calypso_ATDevice_GetIot_UDID, &deviceValue))
	{
		return false;
	}

	json_value *payload = json_object_new(1);
	json_object_push(payload, "UDID", json_string_new(deviceValue.iot.udid));

	reqID++;
	sprintf(topic, "%s%u", DEVICE_TWIN_MESSAGE_PATCH, reqID);
	char *buf = malloc(json_measure(payload));
	json_serialize(buf, payload);
	json_builder_free(payload);

	printf("%s\r\n", buf);
	if (!Calypso_Azure_PnP_MQTT_Publish(mqttIndex, topic, 1, buf, strlen(buf), true))
	{
		free(buf);
		return false;
	}
	free(buf);
	return Calypso_Azure_PnP_WaitForMqttRecvEvent(2000);
}

/**
 * @brief Write certificates to the module's file system
 * @return true if successful, false otherwise
 */
bool Calypso_Azure_PnP_Write_Certs()
{
	if (!Calypso_Azure_PnP_Write_File(ROOT_CA_PATH, BALTIMORE_CYBERTRUST_ROOT_CERT, strlen(BALTIMORE_CYBERTRUST_ROOT_CERT)))
	{
		Calypso_Azure_PnP_Print("Unable to write root CA Certificate", false);
		return false;
	}

	if (!Calypso_Azure_PnP_Write_File(DEVICE_CERT_PATH, DEVICE_CERT, strlen(DEVICE_CERT)))
	{
		Calypso_Azure_PnP_Print("Unable to write device Certificate", false);
		return false;
	}

	if (!Calypso_Azure_PnP_Write_File(DEVICE_KEY_PATH, DEVICE_KEY, strlen(DEVICE_KEY)))
	{
		Calypso_Azure_PnP_Print("Unable to write device key", false);
		return false;
	}
	return true;
}

/**
 * @brief Set up SNTP parameters and update time
 * @return true if successful, false otherwise
 */
bool Calypso_Azure_PnP_SNTP_Setup()
{
	bool ret = false;
	Calypso_ATDevice_Value_t deviceValue;
	/* SNTP client start */
	ret = Calypso_ATNetApp_StartApplications(Calypso_ATNetApp_Application_SntpClient);
	Calypso_Azure_PnP_Print("Start SNTP client", ret);

	Calypso_ATNetApp_OptionValue_t value;
	memset(&value, 0, sizeof(value));
	value.sntp.updateInterval = 1;
	ret = Calypso_ATNetApp_Set(Calypso_ATNetApp_Application_SntpClient, Calypso_ATNetApp_SntpOption_UpdateInterval, &value);
	Calypso_Azure_PnP_Print("Set SNTP update interval", ret);

	memset(&value, 0, sizeof(value));
	value.sntp.timeZone = 60;
	ret = Calypso_ATNetApp_Set(Calypso_ATNetApp_Application_SntpClient, Calypso_ATNetApp_SntpOption_TimeZone, &value);
	Calypso_Azure_PnP_Print("Set SNTP time zone", ret);

	memset(&value, 0, sizeof(value));
	strcpy(value.sntp.servers[0], SNTP_SERVER_ADDRESS_1);
	strcpy(value.sntp.servers[1], SNTP_SERVER_ADDRESS_2);
	strcpy(value.sntp.servers[2], SNTP_SERVER_ADDRESS_3);
	ret = Calypso_ATNetApp_Set(Calypso_ATNetApp_Application_SntpClient, Calypso_ATNetApp_SntpOption_Servers, &value);
	Calypso_Azure_PnP_Print("Set SNTP servers", ret);

	WE_Delay(2000);

	ret = Calypso_ATNetApp_UpdateTime();
	Calypso_Azure_PnP_Print("Update device time", ret);

	ret = Calypso_ATDevice_Get(Calypso_ATDevice_GetId_General, Calypso_ATDevice_GetGeneral_Time, &deviceValue);
	Calypso_Azure_PnP_Print("Get device time", ret);
	if (ret)
	{
		printf("date(dd:mm:yy): %u.%u.%u time(hh:mm:ss): %u:%u:%u\r\n", deviceValue.general.time.day, deviceValue.general.time.month, deviceValue.general.time.year, deviceValue.general.time.hour, deviceValue.general.time.minute, deviceValue.general.time.second);
	}
	return ret;
}

/**
 * @brief Provision the module using the Azure DPS
 * @return true if successful, false otherwise
 */
bool Calypso_Azure_PnP_Provision()
{
	bool ret = false;
	bool provDone = false;

	Calypso_ATMQTT_SetValues_t mqttSetParams;
	/* Create MQTT client */
	Calypso_ATMQTT_ServerInfo_t serverInfo;
	strcpy(serverInfo.address, DPS_SERVER_ADDRESS);
	serverInfo.port = MQTT_PORT;

	Calypso_ATMQTT_SecurityParams_t securityParams;
	securityParams.securityMethod = Calypso_ATMQTT_SecurityMethod_TLSV1_2;
	strcpy(securityParams.CAFile, ROOT_CA_PATH);
	strcpy(securityParams.certificateFile, DEVICE_CERT_PATH);
	strcpy(securityParams.DHKey, ATCOMMAND_STRING_EMPTY);
	strcpy(securityParams.privateKeyFile, DEVICE_KEY_PATH);
	securityParams.cipher = Calypso_ATSocket_Cipher_TLS_RSA_WITH_AES_256_CBC_SHA256;

	Calypso_ATMQTT_ConnectionParams_t connectionParams;
	connectionParams.format = Calypso_DataFormat_Base64;
	connectionParams.protocolVersion = Calypso_ATMQTT_ProtocolVersion_v3_1_1;
	connectionParams.blockingSend = 0;

	uint8_t mqttIndex = 0;
	ret = Calypso_ATMQTT_Create(CALYPSO_AZURE_PNP_DEVICE_ID, Calypso_ATMQTT_CreateFlags_URL | Calypso_ATMQTT_CreateFlags_Secure, serverInfo, securityParams, connectionParams, &mqttIndex);
	Calypso_Azure_PnP_Print("Create MQTT client", ret);

	/* Set user name for the MQTT client */
	sprintf(mqttSetParams.username, "%s/registrations/%s/api-version=2019-03-31&model-id=%s", SCOPE_ID,
	CALYPSO_AZURE_PNP_DEVICE_ID, MODEL_ID);
	ret = Calypso_ATMQTT_Set(mqttIndex, Calypso_ATMQTT_SetOption_User, &mqttSetParams);
	Calypso_Azure_PnP_Print("Set MQTT user name", ret);

	/* Connect to DPS MQTT broker */
	ret = Calypso_ATMQTT_Connect(mqttIndex);

	Calypso_Azure_PnP_WaitForMqttOpEvent(2000);

	/* Subscribe to provisioning response topic */
	Calypso_ATMQTT_SubscribeTopic_t provResp;
	strcpy(provResp.topic, PROVISIONING_RESP_TOPIC);
	provResp.QoS = Calypso_ATMQTT_QoS_QoS1;
	ret = Calypso_ATMQTT_Subscribe(mqttIndex, 1, &provResp);
	Calypso_Azure_PnP_Print("Subscribe to provisioning response topic", ret);

	Calypso_Azure_PnP_WaitForMqttOpEvent(1000);

	/*Publish register request to DPS*/
	ret = Calypso_Azure_PnP_Publish_Reg_Req(mqttIndex);
	Calypso_Azure_PnP_Print("Published register request", ret);

	Calypso_Azure_PnP_WaitForMqttOpEvent(1000);

	/*Poll for provisioning complete response (status = "assigned")*/
	if (Calypso_Azure_PnP_WaitForMqttRecvEvent(3000))
	{
		json_value *response = NULL;
		response = json_parse(recvEvent.data, recvEvent.dataLength - 1);
		while (!provDone)
		{
			WE_Delay(2000);
			ret = Calypso_Azure_PnP_Publish_Status_Req(mqttIndex, response->u.object.values[0].value->u.string.ptr);
			if (Calypso_Azure_PnP_WaitForMqttRecvEvent(3000))
			{
				json_value *status = NULL;
				status = json_parse(recvEvent.data, recvEvent.dataLength - 1);
				if (0 == strncmp(status->u.object.values[1].value->u.string.ptr, "assigned", strlen("assigned")))
				{
					provDone = true;
					strcpy(iotHubAddress, status->u.object.values[2].value->u.object.values[3].value->u.string.ptr);
					printf("IoT hub address to connect: %s\r\n", iotHubAddress);
					if (!Calypso_Azure_PnP_Write_File(DEVICE_IOT_HUB_ADDRESS, iotHubAddress, strlen(iotHubAddress)))
					{
						Calypso_Azure_PnP_Print("Unable to write hub address to file", false);
					}
				}
				json_value_free(status);
			}
			else
			{
				break;
			}

		}
	}

	/* Disconnect and delete MQTT client */
	ret = Calypso_ATMQTT_Disconnect(mqttIndex);
	Calypso_Azure_PnP_Print("MQTT disconnect", ret);

	WE_Delay(1000);

	ret = Calypso_ATMQTT_Delete(mqttIndex);
	Calypso_Azure_PnP_Print("MQTT delete", ret);
	return ret;
}

/**
 * @brief Publish request to get the status of registration request
 * @param[in]mqttIndex MQTT socket index
 * @param[in]operationID Operation identifier
 * @return true if successful, false otherwise
 */
bool Calypso_Azure_PnP_Publish_Status_Req(uint8_t mqttIndex, char *operationID)
{

	bool ret = false;
	char provStatusTopic[256];
	char *payload = 0;

	reqID++;
	sprintf(provStatusTopic, "%s%u&operationId=%s", PROVISIONING_STATUS_REQ_TOPIC, reqID, operationID);

	if (!Calypso_Azure_PnP_MQTT_Publish(mqttIndex, provStatusTopic, 1, payload, 0, true))
	{
		ret = false;
	}
	else
	{
		ret = true;
	}
	return ret;
}

/**
 * @brief Publish request to register the module
 * @param[in]mqttIndex MQTT socket index
 * @return true if successful, false otherwise
 */
bool Calypso_Azure_PnP_Publish_Reg_Req(uint8_t mqttIndex)
{

	bool ret = false;
	char provReqTopic[128];

	reqID++;
	sprintf(provReqTopic, "%s%u", PROVISIONING_REG_REQ_TOPIC, reqID);

	char *provReq = Calypso_Azure_PnP_Serialize_Prov_Req();

	if (!Calypso_Azure_PnP_MQTT_Publish(mqttIndex, provReqTopic, 1, provReq, strlen(provReq), true))
	{
		ret = false;
	}
	else
	{
		ret = true;
	}
	free(provReq);
	return ret;
}
/**
 * @brief Publish a MQTT message
 * @param[in]mqttIndex MQTT socket index
 * @param[in]topic Topic to publish to
 * @param[in]retain 1 = retain message, 0 = do not retain message
 * @param[in]data Data to publish
 * @param[in]length Number of bytes to publish
 * @param[in]encode 1 = base64 encode data, 0 = do not encode
 * @return true if successful, false otherwise
 */
bool Calypso_Azure_PnP_MQTT_Publish(uint8_t mqttIndex, char *topic, uint8_t retain, char *data, int length, bool encode)
{
	bool ret = false;

	if (encode)
	{
		uint32_t elen = CALYPSO_LINE_MAX_SIZE;
		char out[CALYPSO_LINE_MAX_SIZE];
		Base64_Encode((uint8_t*) data, length, (uint8_t*) out, &elen);
		ret = Calypso_ATMQTT_Publish(mqttIndex, topic, Calypso_ATMQTT_QoS_QoS1, 1, elen - 1, out);
	}
	else
	{
		ret = Calypso_ATMQTT_Publish(mqttIndex, topic, Calypso_ATMQTT_QoS_QoS1, 1, length, data);
	}
	return ret;
}

/**
 * @brief Create a string containing registration data
 * @return JSON string
 */
char* Calypso_Azure_PnP_Serialize_Prov_Req()
{

	json_value *prov_payload_ = json_object_new(1);
	json_value *prov_modelID_ = json_object_new(1);

	json_object_push(prov_modelID_, "modelId", json_string_new(MODEL_ID));
	json_object_push(prov_payload_, "registrationId", json_string_new(CALYPSO_AZURE_PNP_DEVICE_ID));
	json_object_push(prov_payload_, "payload", prov_modelID_);

	char *buf = malloc(json_measure(prov_payload_));
	json_serialize(buf, prov_payload_);

	json_builder_free(prov_payload_);
	json_builder_free(prov_modelID_);

	return buf;
}

/**
 * @brief Check if a given file exists in the file system
 * @param[in]fileName Full path
 * @return true if successful, false otherwise
 */
bool Calypso_Azure_PnP_File_Exists(const char *fileName)
{
	Calypso_ATFile_FileInfo_t info;
	return Calypso_ATFile_GetInfo(fileName, 0, &info);
}

/**
 * @brief Create a file and write to it
 * @param[in]fileName Full path
 * @param[in]data Data to write
 * @param[in]dataLength Number of bytes to write
 * @return true if successful, false otherwise
 */
bool Calypso_Azure_PnP_Write_File(const char *path, const char *data, uint16_t dataLength)
{
	bool ret = false;
	uint32_t fileID;
	uint32_t sToken;
	uint16_t bytesRemaining = dataLength;
	uint16_t fileLength;
	uint16_t startIdx = 0;
	uint16_t writeSize = 0;
	uint16_t bytesWritten = 0;

	char dataToWrite[ATFILE_FILE_MAX_CHUNK_SIZE ] = {
			0 };

	if (dataLength < ATFILE_FILE_MIN_SIZE)
	{
		fileLength = ATFILE_FILE_MIN_SIZE;
	}
	else
	{
		fileLength = dataLength;
	}
	ret = Calypso_ATFile_Open(path, Calypso_ATFile_OpenFlags_Create | Calypso_ATFile_OpenFlags_Overwrite, fileLength, &fileID, &sToken);
	if (ret)
	{
		while (bytesRemaining > 0)
		{
			if (bytesRemaining > ATFILE_FILE_MAX_CHUNK_SIZE)
			{
				writeSize = ATFILE_FILE_MAX_CHUNK_SIZE - 1;
			}
			else
			{
				writeSize = bytesRemaining;
			}
			memcpy(dataToWrite, data + startIdx, writeSize);
			dataToWrite[writeSize] = '\0';
			ret = Calypso_ATFile_Write(fileID, startIdx, Calypso_DataFormat_Binary,
			false, writeSize, dataToWrite, &bytesWritten);
			if (!ret)
			{
				break;
			}
			startIdx = startIdx + bytesWritten;
			bytesRemaining = bytesRemaining - bytesWritten;
		}
	}
	if (ret)
	{
		return Calypso_ATFile_Close(fileID, NULL, NULL);
	}
	else
	{
		ret = Calypso_ATFile_Close(fileID, NULL, NULL);
		return false;
	}

}

/**
 * @brief Read data from a file
 * @param[in]fileName Full path
 * @param[in]data Buffer to store the data
 * @param[in]dataLength Length of the buffer available
 * @param[out]outputLength Length of the data read
 * @return true if successful, false otherwise
 */
bool Calypso_Azure_PnP_readFile(const char *path, char *data, uint16_t dataLength, uint16_t *outputLength)
{
	bool ret = false;
	uint32_t fileID;
	uint32_t sToken;
	uint16_t bytesRead;
	if (Calypso_ATFile_Open(path, Calypso_ATFile_OpenFlags_Read, 0, &fileID, &sToken))
	{
		if (Calypso_ATFile_Read(fileID, 0, Calypso_DataFormat_Binary, false, dataLength, data, &bytesRead))
		{
			*outputLength = bytesRead;
			ret = true;
		}
		Calypso_ATFile_Close(fileID, NULL, NULL);
	}
	return ret;
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
void Calypso_Azure_PnP_EventCallback(char *eventText)
{
	Calypso_ATEvent_t event;
	if (false == Calypso_ATEvent_ParseEventType(&eventText, &event))
	{
		return;
	}

	switch (event)
	{
	case Calypso_ATEvent_Startup:
		if (Calypso_ATEvent_ParseStartUpEvent(&eventText, &Calypso_Azure_PnP_startupEvent))
		{
			printf("Startup event received. "
					"Article nr: %s, "
					"Chip ID: %s, "
					"MAC address: %s, "
					"Firmware version: %d.%d.%d\r\n", Calypso_Azure_PnP_startupEvent.articleNr, Calypso_Azure_PnP_startupEvent.chipID, Calypso_Azure_PnP_startupEvent.MACAddress, Calypso_Azure_PnP_startupEvent.firmwareVersion[0], Calypso_Azure_PnP_startupEvent.firmwareVersion[1], Calypso_Azure_PnP_startupEvent.firmwareVersion[2]);
		}
		Calypso_Azure_PnP_startupEventReceived = true;
		break;

	case Calypso_ATEvent_NetappIP4Acquired:
	case Calypso_ATEvent_WakeUp:
	case Calypso_ATEvent_Ping:
	case Calypso_ATEvent_SocketTxFailed:
	case Calypso_ATEvent_SocketAsyncEvent:
	case Calypso_ATEvent_SocketTCPConnect:
	case Calypso_ATEvent_SocketTCPAccept:
	case Calypso_ATEvent_SocketRcvd:
	case Calypso_ATEvent_SocketRcvdFrom:
	case Calypso_ATEvent_WlanP2PConnect:
	case Calypso_ATEvent_WlanP2PDisconnect:
	case Calypso_ATEvent_WlanP2PClientAdded:
	case Calypso_ATEvent_WlanP2PClientRemoved:
	case Calypso_ATEvent_WlanP2PDevFound:
	case Calypso_ATEvent_WlanP2PRequest:
	case Calypso_ATEvent_WlanP2PConnectFail:
	case Calypso_ATEvent_GeneralResetRequest:
	case Calypso_ATEvent_GeneralError:
	case Calypso_ATEvent_WlanConnect:
	case Calypso_ATEvent_WlanDisconnect:
	case Calypso_ATEvent_WlanStaAdded:
	case Calypso_ATEvent_WlanStaRemoved:
	case Calypso_ATEvent_WlanProvisioningStatus:
	case Calypso_ATEvent_WlanProvisioningProfileAdded:
	case Calypso_ATEvent_NetappIP6Acquired:
	case Calypso_ATEvent_NetappIPCollision:
	case Calypso_ATEvent_NetappDHCPv4_leased:
	case Calypso_ATEvent_NetappDHCPv4_released:
	case Calypso_ATEvent_NetappIPv4Lost:
	case Calypso_ATEvent_NetappDHCPIPv4AcquireTimeout:
	case Calypso_ATEvent_NetappIPv6Lost:
	case Calypso_ATEvent_MQTTOperation:
		Calypso_Azure_PnP_mqttOpEvent = true;
		break;
	case Calypso_ATEvent_MQTTRecv:
		if (Calypso_ATEvent_ParseSocketMQTTRcvdEvent(&eventText, &recvEvent))
		{
			Calypso_Azure_PnP_mqttRecvEvent = true;
		}
		break;
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
 * @brief Waits for the MQTT recv event.
 */
bool Calypso_Azure_PnP_WaitForMqttRecvEvent(uint32_t timeoutMs)
{
	uint32_t t0 = WE_GetTick();
	Calypso_Azure_PnP_mqttRecvEvent = false;
	while (false == Calypso_Azure_PnP_mqttRecvEvent && (WE_GetTick() - t0) < timeoutMs)
	{
	}
	return Calypso_Azure_PnP_mqttRecvEvent;
}

/**
 * @brief Waits for the MQTT operation event.
 */
bool Calypso_Azure_PnP_WaitForMqttOpEvent(uint32_t timeoutMs)
{
	uint32_t t0 = WE_GetTick();
	Calypso_Azure_PnP_mqttOpEvent = false;
	while (false == Calypso_Azure_PnP_mqttOpEvent && (WE_GetTick() - t0) < timeoutMs)
	{
	}
	return Calypso_Azure_PnP_mqttOpEvent;
}

/**
 * @brief Waits for the startup event to be received.
 */
bool Calypso_Azure_PnP_WaitForStartup(uint32_t timeoutMs)
{
	uint32_t t0 = WE_GetTick();
	Calypso_Azure_PnP_startupEventReceived = false;
	while (false == Calypso_Azure_PnP_startupEventReceived && (WE_GetTick() - t0) < timeoutMs)
	{
	}
	return Calypso_Azure_PnP_startupEventReceived;
}

/**
 * @brief Prints the supplied string, prefixed with OK or NOK (depending on the success parameter).
 *
 * @param str String to print
 * @param success Variable indicating if action was ok
 */
void Calypso_Azure_PnP_Print(char *str, bool success)
{
	printf("%s%s\r\n", success ? "OK    " : "NOK   ", str);
}
