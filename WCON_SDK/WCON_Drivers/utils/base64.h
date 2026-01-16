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
 * @file base64.h
 * @brief base64 encoder/decoder header file.
 */

#ifndef BASE64_H_INCLUDED
#define BASE64_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Get Base64 decoded buffer size.
 *
 * This routine calculates the expected raw data buffer size
 * for the given Base64 buffer and buffer size.
 *
 * @param[in] inputData: Source buffer holding the base64 data
 * @param[in] inputLength: Length of base64 data without termination character
 * @param[out] outputLengthP: Pointer to calculated length of decoded data and the termination character
 *
 * @return True if successful, false otherwise.
 */
extern bool Base64_GetDecBufSize(uint8_t* inputData, uint32_t inputLength, uint32_t* outputLengthP);

/**
 * @brief Get Base64 encoded buffer size.
 *
 * This routine calculates the expected Base64 buffer size
 * for the given raw data size.
 *
 * @param[in] inputLength: Length of the raw data
 * @param[out] outputLengthP: Pointer to calculated length of encoded data and the termination character
 *
 * @return True if successful, false otherwise.
 */
extern bool Base64_GetEncBufSize(uint32_t inputLength, uint32_t* outputLengthP);

/**
 * @brief Decode Base64 data.
 *
 * This routine decodes the supplied data in Base64 format to raw data
 * and writes it to the output buffer (outputData, must be pre-allocated).
 * The size of the output buffer is also returned.
 *
 * @param[in] inputData: Source buffer (holding the Base64 data to be decoded)
 * @param[in] inputLength: Source buffer size
 * @param[out] outputData: Destination buffer (will contain the decoded data)
 * @param[in,out] outputLength: As input it is the size of the buffer allocated for outputData and it will
 * 				  be updated with the actual output length
 *
 * @return True if successful, false otherwise
 */
extern bool Base64_Decode(uint8_t* inputData, uint32_t inputLength, uint8_t* outputData, uint32_t* outputLength);

/**
 * @brief Encodes raw data to Base64 format.
 *
 * This routine encodes the supplied raw data to base64 format and writes it to the
 * output buffer (outputData, must be pre-allocated).
 * The size of the output buffer is also returned.
 *
 * @attention The data is returned as a null terminated string, the destination
 * buffer must have size @ref Base64_GetEncBufSize.
 *
 * @param[in] inputData: Source buffer holding the raw data
 * @param[in] inputLength: Source buffer size
 * @param[out] outputData: Destination buffer (will contain the Base64 encoded data)
 * @param[in,out] outputLength: As input it is the size of the buffer allocated for outputData and it will
 * 			   	  be updated with the actual output length
 *
 * @return True if successful, false otherwise
 */
extern bool Base64_Encode(uint8_t* inputData, uint32_t inputLength, uint8_t* outputData, uint32_t* outputLength);

#ifdef __cplusplus
}
#endif

#endif /* BASE64_H_INCLUDED */
