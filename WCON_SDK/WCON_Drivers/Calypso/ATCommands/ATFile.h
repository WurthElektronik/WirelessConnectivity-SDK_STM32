/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK:
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
 * @file ATFile.h
 * @brief AT commands for file I/O functionality.
 */

#ifndef CALYPSO_AT_FILE_H_INCLUDED
#define CALYPSO_AT_FILE_H_INCLUDED
#include <Calypso/Calypso.h>
#include <global/ATCommands.h>
#include <stdbool.h>
#include <stdint.h>

#define ATFILE_FILENAME_MAX_LENGTH (uint8_t)180      /**< Maximum file name length */
#define ATFILE_FILE_MIN_SIZE (uint16_t)4096          /**< Minimum file size in bytes */
#define ATFILE_FILE_FAILSAFE_MIN_SIZE (uint16_t)8192 /**< Failsafe minimum file size in bytes */
#define ATFILE_FILE_MAX_CHUNK_SIZE                                                                                                                                                                                                                                                                                                                                                                             \
    (uint16_t)750 /**< Max. recommended data size to transfer in one go
                                                                         (using Calypso_ATFile_Write() and Calypso_ATFile_Read()). Is limited
                                                                         to 750 bytes because of issues when using Base64 encoding. */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Open flags for AT+fileOpen command.
 */
typedef enum Calypso_ATFile_OpenFlags_t
{
    Calypso_ATFile_OpenFlags_Create = 1 << 0,            /**< Create a new file (optionally combined with Calypso_ATFile_OpenFlags_Write or Calypso_ATFile_OpenFlags_Overwrite) */
    Calypso_ATFile_OpenFlags_Read = 1 << 1,              /**< Open file for reading (no bitmask) */
    Calypso_ATFile_OpenFlags_Write = 1 << 2,             /**< Open file for writing (optionally combined with Calypso_ATFile_OpenFlags_Create) */
    Calypso_ATFile_OpenFlags_Overwrite = 1 << 3,         /**< Opens an existing file (optionally combined with Calypso_ATFile_OpenFlags_Create) */
    Calypso_ATFile_OpenFlags_CreateFailsafe = 1 << 4,    /**< Fail safe */
    Calypso_ATFile_OpenFlags_CreateSecure = 1 << 5,      /**< Secure file */
    Calypso_ATFile_OpenFlags_CreateNoSignature = 1 << 6, /**< For secure files only */
    Calypso_ATFile_OpenFlags_CreateStaticToken = 1 << 7, /**< For secure files only */
    Calypso_ATFile_OpenFlags_CreateVendorToken = 1 << 8, /**< For secure files only */
    Calypso_ATFile_OpenFlags_CreatePublicWrite = 1 << 9, /**< For secure files only */
    Calypso_ATFile_OpenFlags_CreatePublicRead = 1 << 10, /**< For secure files only */
    Calypso_ATFile_OpenFlags_NumberOfValues = 11         /**< Number of flags in this enumeration */
} Calypso_ATFile_OpenFlags_t;

/**
 * @brief File property flags used in file list entries.
 *
 * @see Calypso_ATFile_FileListEntry_t, Calypso_ATFile_GetFileList()
 */
typedef enum Calypso_ATFile_FileProperties_t
{
    Calypso_ATFile_FileProperties_OpenWrite = 1 << 0,
    Calypso_ATFile_FileProperties_OpenRead = 1 << 1,
    Calypso_ATFile_FileProperties_MustCommit = 1 << 2,
    Calypso_ATFile_FileProperties_BundleFile = 1 << 3,
    Calypso_ATFile_FileProperties_PendingCommit = 1 << 4,
    Calypso_ATFile_FileProperties_PendingBundleCommit = 1 << 5,
    Calypso_ATFile_FileProperties_NotFailsafe = 1 << 6,
    Calypso_ATFile_FileProperties_NotValid = 1 << 7,
    Calypso_ATFile_FileProperties_SysFile = 1 << 8,
    Calypso_ATFile_FileProperties_Secure = 1 << 9,
    Calypso_ATFile_FileProperties_NoSignature = 1 << 10,
    Calypso_ATFile_FileProperties_PublicWrite = 1 << 11,
    Calypso_ATFile_FileProperties_PublicRead = 1 << 12,
    Calypso_ATFile_FileProperties_NumberOfValues = 13
} Calypso_ATFile_FileProperties_t;

/**
 * @brief File info as returned by Calypso_ATFile_GetInfo()
 */
typedef struct Calypso_ATFile_FileInfo_t
{
    uint32_t size;
    uint32_t allocatedSize;
    uint32_t storageSize;
    uint32_t writeCounter;
} Calypso_ATFile_FileInfo_t;

/**
 * @brief File list entry used with Calypso_ATFile_GetFileList() and Calypso_ATEvent_FileListEntry.
 */
typedef struct Calypso_ATFile_FileListEntry_t
{
    char fileName[ATFILE_FILENAME_MAX_LENGTH]; /**< File name */
    uint32_t maxFileSize;                      /**< Max. size of file */
    uint32_t properties;                       /**< File properties (see Calypso_ATFile_FileProperties_t)  */
    uint32_t allocatedBlocks;                  /**< Allocated blocks */
} Calypso_ATFile_FileListEntry_t;

/**
 * @brief Opens a file (using the AT+FileOpen command).
 *
 * @param[in] fileName: Name of file to be opened
 * @param[in] options: Option flags (see Calypso_ATFile_OpenFlags_t)
 * @param[in] fileSize: Maximum size of the file. Will be allocated on creation.
 * @param[out] fileID: ID of the opened file. Can be used for further file functions.
 * @param[out] secureToken: Secure token of the opened file.
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATFile_Open(const char* fileName, uint32_t options, uint16_t fileSize, uint32_t* fileID, uint32_t* secureToken);

/**
 * @brief Closes a file (using the AT+FileClose command).
 *
 * @param[in] fileID: File ID as returned by Calypso_ATFile_Open
 * @param[in] certFileName: Full path to certificate (optional). Can be NULL if not used.
 * @param[in] signature: SHA1 signature (optional). Can be NULL if not used.
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATFile_Close(uint32_t fileID, char* certFileName, char* signature);

/**
 * @brief Deletes a file (using the AT+FileDel command).
 *
 * @param[in] fileName: Name of the file to delete
 * @param[in] secureToken: Secure token returned by Calypso_ATFile_Open() (optional)
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATFile_Delete(const char* fileName, uint32_t secureToken);

/**
 * @brief Reads a file's contents (using the AT+FileRead command).
 *
 * The file has to be opened using Calypso_ATFile_Open() prior to reading!
 *
 * @param[in] fileID: ID of file to read as returned by Calypso_ATFile_Open()
 * @param[in] offset: Offset for the read operation
 * @param[in] format: Format of the output data. When using Calypso_DataFormat_Base64,
 *                   you can set decodeBase64 to true to automatically decode the received data.
 * @param[in] decodeBase64: Enables decoding of received Base64 data
 * @param[in] bytesToRead: Number of bytes to read. Note that the returned data is a
 *                        null terminated string, so the output buffer must have a
 *                        minimum size of bytesToRead+1.
 * @param[out] bytesRead: Number of bytes which have been read
 * @param[out] data: File contents which has been read
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATFile_Read(uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format, bool decodeBase64, uint16_t bytesToRead, char* data, uint16_t* bytesRead);

/**
 * @brief Writes a file's contents (using the AT+FileWrite command).
 *
 * The file has to be opened using Calypso_ATFile_Open() prior to writing!
 *
 * @param[in] fileID: ID of file to write as returned by Calypso_ATFile_Open()
 * @param[in] offset: Offset for the write operation
 * @param[in] format: Format of the data to be written. Note that when using Calypso_DataFormat_Base64,
 *                   you either need to provide Base64 encoded data or set encodeAsBase64 to true.
 * @param[in] encodeAsBase64: Encode the data in Base64 format before sending it to the Calypso module
 * @param[in] bytesToWrite: Number of bytes to write
 * @param[in] data: Data to be written
 * @param[out] bytesWritten: Number of bytes which have been written
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATFile_Write(uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format, bool encodeAsBase64, uint16_t bytesToWrite, const char* data, uint16_t* bytesWritten);

/**
 * @brief Returns information on a file.
 *
 * @param[in] fileName: Name of file
 * @param[in] secureToken: Secure token
 * @param[out] fileInfo: The returned file information
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATFile_GetInfo(const char* fileName, uint32_t secureToken, Calypso_ATFile_FileInfo_t* fileInfo);

/**
 * @brief Retrieves a list of the files stored in the Calypso module.
 *
 * The file list entries are made available in the form of events of type
 * Calypso_ATEvent_FileListEntry as soon as they are received (the complete list of files
 * may be too large to be returned by this function as a whole). The events are
 * triggered before this function returns.
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATFile_GetFileList();

/**
 * @brief Parses the values of a file list entry string.
 *
 * @param[in,out] pInArguments: String containing arguments of the AT command
 * @param[out] fileListEntry: The parsed file list entry data
 *
 * @return True if parsed successfully, false otherwise
 */
extern bool Calypso_ATFile_ParseFileListEntry(char** pInArguments, Calypso_ATFile_FileListEntry_t* fileListEntry);

/**
 * @brief Prints file property flags to string.
 *
 * @param[in] properties: File properties (see Calypso_ATFile_FileProperties_t)
 * @param[out] pOutStr: Output string
 * @param[in] maxLength: Max. length of output string
 *
 * @return True if successful, false otherwise
 */
extern bool Calypso_ATFile_PrintFileProperties(uint32_t properties, char* pOutStr, size_t maxLength);

#ifdef __cplusplus
}
#endif

#endif /* CALYPSO_AT_FILE_H_INCLUDED */
