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
 * @brief Contains code related to redirecting WE_DEBUG_PRINT() output to UART for debug reasons.
 */

#include <global/debug.h>

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <global/global.h>
#if defined(STM32L073xx)
#include "global_L0xx.h"
#elif defined(STM32F401xE)
#include "global_F4xx.h"
#endif

#if defined(WE_DEBUG) || defined(WE_DEBUG_INIT)

/**
 * @brief Debug ring buffer size.
 */
#define WE_DEBUG_BUFFER_SIZE 2048

/**
 * @brief UART used for debug output.
 */
static USART_TypeDef* uartDebug = NULL;

/**
 * @brief Ring buffer for debug output (output is transmitted asynchronously).
 */
static uint8_t debugBuffer[WE_DEBUG_BUFFER_SIZE];

/**
 * @brief Current write position in ring buffer used for debug output
 * (next character to be queued).
 */
static uint16_t debugBufferWritePos = 0;

/**
 * @brief Current read position in ring buffer used for debug output
 * (next character to be transferred).
 */
static uint16_t debugBufferReadPos = 0;

static bool transferRunning = false;

/**
 * @brief Initializes UART2 and connects this interface to WE_DEBUG_PRINT().
 *
 * Note that for redirection of WE_DEBUG_PRINT() output to work, there must not be other
 * definitions of system file functions such as _write() (e.g. it might be necessary
 * to exclude the STM32CubeIDE-generated syscalls.c from compilation).
 *
 * Also note that it is not safe to call WE_DEBUG_PRINT() from different contexts (e.g.
 * inside main and inside ISRs) - there is no guarantee, that the debug output is
 * forwarded correctly in all circumstances.
 *
 * There are two preprocessor defines controlling debug behavior:
 * - WE_DEBUG: Initialize debug UART and enable printing of debug messages in drivers.
 * - WE_DEBUG_INIT: Initialize debug UART but disable printing of debug messages in drivers
 *   (adds support for debugging using printf in user/example code, but drivers don't print
 *   diagnostic/debug info).
 */
void WE_Debug_Init()
{
    uartDebug = USART2;

#if defined(STM32L073xx)

    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
    PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        WE_Error_Handler();
    }

    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    uint32_t alternateFunction = LL_GPIO_AF_4;

#elif defined(STM32F401xE)
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    uint32_t alternateFunction = LL_GPIO_AF_7;

#elif defined(STM32L432xx)
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    uint32_t alternateFunction = LL_GPIO_AF_7;
#endif

    /* USART2 GPIO Configuration
	 PA2     ------> USART2_TX
	 PA3     ------> USART2_RX
	 */

    LL_GPIO_InitTypeDef gpioInitStruct = {0};
    gpioInitStruct.Pin = LL_GPIO_PIN_2;
    gpioInitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    gpioInitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    gpioInitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpioInitStruct.Pull = LL_GPIO_PULL_NO;
    gpioInitStruct.Alternate = alternateFunction;
    LL_GPIO_Init(GPIOA, &gpioInitStruct);

    gpioInitStruct.Pin = LL_GPIO_PIN_3;
    gpioInitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    gpioInitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    gpioInitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpioInitStruct.Pull = LL_GPIO_PULL_NO;
    gpioInitStruct.Alternate = alternateFunction;
    LL_GPIO_Init(GPIOA, &gpioInitStruct);

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

    LL_USART_InitTypeDef uartInitStruct = {0};
    uartInitStruct.BaudRate = 115200;
    uartInitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    uartInitStruct.StopBits = LL_USART_STOPBITS_1;
    uartInitStruct.Parity = LL_USART_PARITY_NONE;
    uartInitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    uartInitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    uartInitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(uartDebug, &uartInitStruct);

    LL_USART_ConfigAsyncMode(uartDebug);
    LL_USART_Enable(uartDebug);

    /* Enable transfer complete interrupt */
    LL_USART_EnableIT_TC(uartDebug);
    NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), WE_PRIORITY_UART_DEBUG, 0));
    NVIC_EnableIRQ(USART2_IRQn);

    /* No I/O buffering for STDOUT stream - transmit characters as soon as they are printed */
    setvbuf(stdout, NULL, _IONBF, 0);
}

/**
 * @brief Wait until any debug output pending to be written to UART has been transferred.
 */
void WE_Debug_Flush()
{
    while (transferRunning)
    {
    }
}

void USART2_IRQHandler(void)
{
    if (LL_USART_IsEnabledIT_TC(USART2) && LL_USART_IsActiveFlag_TC(USART2))
    {
        /* Character transfer complete */

        /* Clear flag */
        LL_USART_ClearFlag_TC(USART2);

        if (debugBufferReadPos != debugBufferWritePos)
        {
            /* Ring buffer read and write positions differ -> transfer is active */

            /* Move to next position in ring buffer */
            debugBufferReadPos++;
            if (debugBufferReadPos >= WE_DEBUG_BUFFER_SIZE)
            {
                debugBufferReadPos = 0;
            }

            transferRunning = debugBufferReadPos != debugBufferWritePos;
            if (transferRunning)
            {
                /* More characters to be transferred -> transmit next character */
                LL_USART_TransmitData8(USART2, debugBuffer[debugBufferReadPos]);
            }
        }
    }
}

int _write(int fd, char* ptr, int len)
{
    /* Note that it is not safe to call this function from different contexts! */

    if (fd == STDOUT_FILENO || fd == STDERR_FILENO)
    {
        /* Rudimentary check for concurrent access to this function */
        static bool lock = false;
        if (lock != false)
        {
            errno = EIO;
            return -1;
        }
        lock = true;

        uint16_t readPos = debugBufferReadPos;
        uint16_t writePos = debugBufferWritePos;

        /* Remaining space in ring buffer */
        uint16_t spaceRemaining = readPos > writePos ? readPos - writePos - 1 : WE_DEBUG_BUFFER_SIZE - writePos + readPos - 1;

        if (len > spaceRemaining)
        {
            /* Data to be written doesn't fit into ring buffer - limit to spaceRemaining */
            len = spaceRemaining;
        }

        /* Store data in ring buffer */
        int bytesWritten = 0;
        if (writePos > readPos)
        {
            /* Store data between writePos and end of buffer */

            int chunkSize = len;
            if (chunkSize > WE_DEBUG_BUFFER_SIZE - writePos)
            {
                chunkSize = WE_DEBUG_BUFFER_SIZE - writePos;
            }
            memcpy(debugBuffer + writePos, ptr, chunkSize);
            len -= chunkSize;
            writePos = (writePos + chunkSize) % WE_DEBUG_BUFFER_SIZE;
            bytesWritten += chunkSize;
        }

        if (len > 0)
        {
            /* Store remaining data between start of buffer and read pos */

            memcpy(debugBuffer + writePos, ptr + bytesWritten, len);
            writePos = (writePos + len) % WE_DEBUG_BUFFER_SIZE;
            bytesWritten += len;
        }

        debugBufferWritePos = writePos;

        /* Start transfer if not already running */
        if (!transferRunning && LL_USART_IsActiveFlag_TXE(uartDebug))
        {
            transferRunning = true;
            LL_USART_TransmitData8(uartDebug, *(debugBuffer + debugBufferReadPos));
        }

        lock = false;

        return bytesWritten;
    }

    errno = EBADF;
    return -1;
}
#endif // WE_DEBUG
