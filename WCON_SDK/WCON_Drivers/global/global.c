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
 * @brief Contains global function definitions for the Wireless Connectivity SDK for STM32.
 */

#include "global.h"

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

HAL_StatusTypeDef UartTransmitInternal(USART_TypeDef *hUart, const uint8_t *data, uint16_t size);
void OnDmaDataReceived(uint8_t* data, size_t size);

/*              Variables              */

USART_TypeDef *WE_uartWireless = NULL;
DMA_TypeDef *WE_dmaWirelessRx = NULL;
uint32_t WE_dmaWirelessRxStream = 0;
WE_FlowControl_t WE_uartFlowControl = WE_FlowControl_NoFlowControl;
WE_Parity_t WE_uartParity = WE_Parity_None;
bool WE_dmaEnabled = false;
uint8_t WE_dmaRxBuffer[WE_DMA_RX_BUFFER_SIZE];
size_t WE_dmaLastReadPos = 0;

#define NUM_GPIO_PORTS 4
static GPIO_TypeDef *gpioPorts[NUM_GPIO_PORTS] = {GPIOA, GPIOB, GPIOC, GPIOH};


/*              Functions              */

void WE_Platform_Init(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    WE_SystemClock_Config();

    /* Enable pendable service interrupt which is used for processing data received from radio module
     * (interrupt is triggered via WE_CheckIfDmaDataAvailableAsync() when bytes have been received) */
    NVIC_SetPriority(PendSV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), WE_PRIORITY_RX_DATA_PROCESSING, 0));
    NVIC_EnableIRQ(PendSV_IRQn);
}

HAL_StatusTypeDef UartTransmitInternal(USART_TypeDef *hUart, const uint8_t *data, uint16_t size)
{
    for (; size > 0; --size, ++data)
    {
        LL_USART_TransmitData8(hUart, *data);
        while (!LL_USART_IsActiveFlag_TXE(hUart))
        {
        }
    }
    while (!LL_USART_IsActiveFlag_TC(hUart))
    {
    }
    return HAL_OK;
}

void WE_UART_Transmit(const uint8_t *data, uint16_t length)
{
    UartTransmitInternal(WE_uartWireless, data, length);
}

/**
 * @brief Is called when one or more bytes have been received from UART via DMA.
 * @param data Received data
 * @param size Number of bytes received
 */
void OnDmaDataReceived(uint8_t* data, size_t size)
{
    for (; size > 0; size--, data++)
    {
        WE_UART_HandleRxByte(*data);
    }
}

void WE_CheckIfDmaDataAvailable(void)
{
    /* Get current DMA write position (in ring buffer) */
    size_t pos = WE_DMA_RX_BUFFER_SIZE - LL_DMA_GetDataLength(WE_dmaWirelessRx, WE_dmaWirelessRxStream);

    /* Check if new data is available (i.e. write position has changed) */
    if (pos != WE_dmaLastReadPos)
    {
        if (pos > WE_dmaLastReadPos)
        {
            /* Data to be read from DMA ring buffer is continuous - between lastPos and pos */
            OnDmaDataReceived(&WE_dmaRxBuffer[WE_dmaLastReadPos], pos - WE_dmaLastReadPos);
        }
        else
        {
            /* Ring buffer write position has reached the end of the buffer and restarted at the beginning.
             * Data is split:
             * - Bytes between lastPos and the end of the buffer
             * - Bytes between start of the buffer and pos
             */
            OnDmaDataReceived(&WE_dmaRxBuffer[WE_dmaLastReadPos], WE_DMA_RX_BUFFER_SIZE - WE_dmaLastReadPos);
            if (pos > 0)
            {
                OnDmaDataReceived(&WE_dmaRxBuffer[0], pos);
            }
        }

        /* Store DMA write position for next interrupt */
        WE_dmaLastReadPos = pos;
    }
}

void WE_CheckIfDmaDataAvailableAsync()
{
    /* Trigger pendable service interrupt (PendSV) */
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

/**
 * @brief Pendable service interrupt handler.
 *
 * Is used for asynchronous processing of data received from radio module via DMA.
 * Calls WE_CheckIfDmaDataAvailable().
 *
 * @see WE_CheckIfDmaDataAvailableAsync()
 */
void PendSV_Handler(void)
{
    WE_CheckIfDmaDataAvailable();
}

/**
 * @brief Interrupt handler for data received from wireless module via UART.
 *
 * Handles the following interrupts, depending on whether DMA is enabled or not:
 * - If DMA is enabled: Idle line interrupt
 * - If DMA is disabled: Character received interrupt
 */
void USART1_IRQHandler(void)
{
    if (LL_USART_IsEnabledIT_IDLE(USART1) && LL_USART_IsActiveFlag_IDLE(USART1))
    {
        /* USART idle line interrupt (used only if DMA is enabled) */

        LL_USART_ClearFlag_IDLE(USART1);
        WE_CheckIfDmaDataAvailableAsync();
    }

    if (LL_USART_IsActiveFlag_RXNE(WE_uartWireless) && LL_USART_IsEnabledIT_RXNE(WE_uartWireless))
    {
        /* Character has been received (used only if DMA is disabled) */

        /* RXNE flag will be cleared by reading of DR register */
        WE_UART_HandleRxByte(LL_USART_ReceiveData8(WE_uartWireless));
    }
}

void WE_Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
}

bool WE_InitPins(WE_Pin_t pins[], uint8_t numPins)
{
    /* Sort pins by port */
    uint32_t inputPinsPerPort[NUM_GPIO_PORTS] = {0};
    uint32_t outputPinsPerPort[NUM_GPIO_PORTS] = {0};
    for (uint8_t i = 0; i < numPins; i++)
    {
        if (0 == pins[i].pin || NULL == pins[i].port)
        {
            /* Unused */
            continue;
        }

        if (WE_Pin_Type_Output == pins[i].type)
        {
            /* Configure GPIO pin output level */
            HAL_GPIO_WritePin(pins[i].port, pins[i].pin, GPIO_PIN_RESET);
        }

        for (uint8_t j = 0; j < NUM_GPIO_PORTS; j++)
        {
            if (gpioPorts[j] == pins[i].port)
            {
                if (pins[i].type == WE_Pin_Type_Output)
                {
                    outputPinsPerPort[j] |= pins[i].pin;
                }
                else
                {
                    inputPinsPerPort[j] |= pins[i].pin;
                }
                break;
            }
        }
    }

    for (uint8_t i = 0; i < NUM_GPIO_PORTS; i++)
    {
        if (0 == outputPinsPerPort[i] && 0 == inputPinsPerPort)
        {
            /* Unused */
            continue;
        }

        /* GPIO Ports Clock Enable */
        if (GPIOA == gpioPorts[i])
        {
            __HAL_RCC_GPIOA_CLK_ENABLE();
        }
        else if (GPIOB == gpioPorts[i])
        {
            __HAL_RCC_GPIOB_CLK_ENABLE();
        }
        else if (GPIOC == gpioPorts[i])
        {
            __HAL_RCC_GPIOC_CLK_ENABLE();
        }
        else if (GPIOH == gpioPorts[i])
        {
            __HAL_RCC_GPIOH_CLK_ENABLE();
        }

        if (0 != outputPinsPerPort[i])
        {
            GPIO_InitTypeDef GPIO_InitStruct = {0};
            GPIO_InitStruct.Pin = outputPinsPerPort[i];
            GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
            HAL_GPIO_Init(gpioPorts[i], &GPIO_InitStruct);
        }
        if (0 != inputPinsPerPort[i])
        {
            GPIO_InitTypeDef GPIO_InitStruct = {0};
            GPIO_InitStruct.Pin = inputPinsPerPort[i];
            GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
            HAL_GPIO_Init(gpioPorts[i], &GPIO_InitStruct);
        }
    }

    return true;
}

bool WE_DeinitPin(WE_Pin_t pin)
{
	HAL_GPIO_DeInit(pin.port, pin.pin);
    return true;
}

bool WE_SetPin(WE_Pin_t pin, WE_Pin_Level_t out)
{
    if (0 == pin.pin || NULL == pin.port || pin.type != WE_Pin_Type_Output)
    {
        return false;
    }

	switch (out)
	{
    case WE_Pin_Level_High:
        HAL_GPIO_WritePin(pin.port, pin.pin, GPIO_PIN_SET);
        break;

    case WE_Pin_Level_Low:
        HAL_GPIO_WritePin(pin.port, pin.pin, GPIO_PIN_RESET);
        break;

    default:
        return false;
	}

    return true;
}

WE_Pin_Level_t WE_GetPinLevel(WE_Pin_t pin)
{
    switch (HAL_GPIO_ReadPin(pin.port, pin.pin))
    {
    case GPIO_PIN_RESET:
        return WE_Pin_Level_Low;

    case GPIO_PIN_SET:
        return WE_Pin_Level_High;

    default:
        return WE_Pin_Level_Low;
    }
}

void WE_Delay(uint16_t sleepForMs)
{
    if (sleepForMs > 0)
    {
        HAL_Delay((uint32_t) sleepForMs);
    }
}

uint32_t WE_GetTick()
{
    return HAL_GetTick();
}

#ifndef WE_MICROSECOND_TICK
void WE_DelayMicroseconds(uint32_t sleepForUsec)
{
    /* Microsecond tick is disabled: round to ms */
    WE_Delay(((sleepForUsec + 500) / 1000));
}

uint32_t WE_GetTickMicroseconds()
{
    /* Microsecond tick is disabled: return ms tick * 1000 */
    return WE_GetTick() * 1000;
}
#endif /* WE_MICROSECOND_TICK */

bool WE_GetDriverVersion(uint8_t* version)
{
    uint8_t help[3] = WE_WIRELESS_CONNECTIVITY_SDK_VERSION;
    memcpy(version, help, 3);
    return true;
}

#ifdef __cplusplus
}
#endif
