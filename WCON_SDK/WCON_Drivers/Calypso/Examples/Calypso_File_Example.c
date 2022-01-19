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
 * COPYRIGHT (c) 2022 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Calypso file example.
 */

#include "Calypso_File_Example.h"

#include <stdio.h>

#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/ATCommands/ATFile.h>

#include "Calypso_Examples.h"

void Calypso_File_Example_EventCallback(char* eventText);

/**
 * @brief File handling example.
 */
void Calypso_File_Example(void)
{
    printf("*** Start of Calypso ATFile example ***\r\n");

    bool ret = false;

    if (!Calypso_Init(Calypso_Examples_baudRate, Calypso_Examples_flowControl, Calypso_Examples_parity, &Calypso_File_Example_EventCallback, NULL))
    {
        return;
    }

    Calypso_PinReset();

    WE_Delay(2000);

    /* Get version info. This retrieves Calypso's firmware version (amongst other version info) and
     * stores the firmware version in Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
     * Calypso_firmwareVersionPatch for later use. */
    ATDevice_Value_t deviceValue;
    ret = ATDevice_Get(ATDevice_GetId_General, ATDevice_GetGeneral_Version, &deviceValue);
    Calypso_Examples_Print("Get device version", ret);

    uint32_t fileID;
    uint32_t secureToken;
    const char *fileName = "testfile";
    char *fileContent = "Hello World";
    bool encodeAsBase64 = true;

    ret = ATFile_Open(fileName, (ATFile_OpenFlags_Create | ATFile_OpenFlags_Write), ATFILE_FILE_MIN_SIZE, &fileID, &secureToken);
    Calypso_Examples_Print("Open file for writing", ret);

    if (ret)
    {
        uint16_t bytesWritten = 0;
        ret = ATFile_Write(fileID,
                           0,
                           encodeAsBase64 ? Calypso_DataFormat_Base64 : Calypso_DataFormat_Binary,
                           encodeAsBase64,
                           strlen(fileContent),
                           fileContent,
                           &bytesWritten);
        Calypso_Examples_Print("Write file", ret);
        printf("Wrote \"%s\" (length %d) in %s format\r\n", fileContent, strlen(fileContent), encodeAsBase64 ? "Base64" : "binary");
    }

    ret = ATFile_Close(fileID, NULL, NULL);
    Calypso_Examples_Print("Close file", ret);


    ret = ATFile_Open(fileName, ATFile_OpenFlags_Read, ATFILE_FILE_MIN_SIZE, &fileID, &secureToken);
    Calypso_Examples_Print("Open file for reading", ret);

    char fileContentReadBack[32];
    uint16_t readBytes = 0;
    if (ret)
    {
        ret = ATFile_Read(fileID,
                          0,
                          encodeAsBase64 ? Calypso_DataFormat_Base64 : Calypso_DataFormat_Binary,
                          encodeAsBase64,
                          strlen(fileContent),
                          fileContentReadBack,
                          &readBytes);
        Calypso_Examples_Print("Read file", ret);
        if (ret)
        {
            printf("Read \"%s\" (length %d) in %s format\r\n", fileContentReadBack, readBytes, encodeAsBase64 ? "Base64" : "binary");
        }
    }

    ret = ATFile_Close(fileID, NULL, NULL);
    Calypso_Examples_Print("Close file", ret);

    /* Get list of files stored on Calypso module. Note that the file list
     * entries are provided in the form of individual events of type ATEvent_FileListEntry,
     * as the complete list of files might be too large to fit in the receive buffer.
     * In this example, the entries are processed in Calypso_File_Example_EventCallback(). */
    ret = ATFile_GetFileList();
    Calypso_Examples_Print("Get file list", ret);

    ret = ATFile_Delete(fileName, secureToken);
    Calypso_Examples_Print("Delete file", ret);

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
 * events (i.e. events from ATEvent_t). Some events might in fact be responses
 * to AT commands that are not included in ATEvent_t.
 */
void Calypso_File_Example_EventCallback(char* eventText)
{
    ATEvent_t event;
    ATEvent_ParseEventType(&eventText, &event);

    switch (event)
    {
    case ATEvent_FileListEntry:
    {
        ATFile_FileListEntry_t fileListEntry;
        if (ATEvent_ParseFileListEntryEvent(&eventText, &fileListEntry))
        {
            char propertiesStr[256] = {0};
            ATFile_PrintFileProperties(fileListEntry.properties, propertiesStr, sizeof(propertiesStr));
            printf("File list entry: "
                   "Name = \"%s\", "
                   "max. size = %lu, "
                   "properties = \"%s\", "
                   "blocks = %lu\r\n",
                   fileListEntry.fileName,
                   fileListEntry.maxFileSize,
                   propertiesStr,
                   fileListEntry.allocatedBlocks);
        }
        break;
    }

    case ATEvent_Startup:
    case ATEvent_WakeUp:
    case ATEvent_Ping:
    case ATEvent_SocketTxFailed:
    case ATEvent_SocketAsyncEvent:
    case ATEvent_SocketTCPConnect:
    case ATEvent_SocketTCPAccept:
    case ATEvent_SocketRcvd:
    case ATEvent_SocketRcvdFrom:
    case ATEvent_WlanP2PConnect:
    case ATEvent_WlanP2PDisconnect:
    case ATEvent_WlanP2PClientAdded:
    case ATEvent_WlanP2PClientRemoved:
    case ATEvent_WlanP2PDevFound:
    case ATEvent_WlanP2PRequest:
    case ATEvent_WlanP2PConnectFail:
    case ATEvent_Invalid:
    case ATEvent_GeneralResetRequest:
    case ATEvent_GeneralError:
    case ATEvent_WlanConnect:
    case ATEvent_WlanDisconnect:
    case ATEvent_WlanStaAdded:
    case ATEvent_WlanStaRemoved:
    case ATEvent_WlanProvisioningStatus:
    case ATEvent_WlanProvisioningProfileAdded:
    case ATEvent_NetappIP4Acquired:
    case ATEvent_NetappIP6Acquired:
    case ATEvent_NetappIPCollision:
    case ATEvent_NetappDHCPv4_leased:
    case ATEvent_NetappDHCPv4_released:
    case ATEvent_NetappIPv4Lost:
    case ATEvent_NetappDHCPIPv4AcquireTimeout:
    case ATEvent_NetappIPv6Lost:
    case ATEvent_MQTTOperation:
    case ATEvent_MQTTRecv:
    case ATEvent_MQTTDisconnect:
    case ATEvent_HTTPGet:
    case ATEvent_CustomGPIO:
    case ATEvent_CustomHTTPPost:
    case ATEvent_FatalErrorDeviceAbort:
    case ATEvent_FatalErrorDriverAbort:
    case ATEvent_FatalErrorSyncLost:
    case ATEvent_FatalErrorNoCmdAck:
    case ATEvent_FatalErrorCmdTimeout:
    default:
        break;
    }
}
