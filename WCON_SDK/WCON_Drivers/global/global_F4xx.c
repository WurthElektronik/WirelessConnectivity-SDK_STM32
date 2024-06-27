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
 * @brief STM32F4xx platform driver.
 */

#include "global_F4xx.h"

#ifdef STM32F401xE

#include "global.h"

#ifdef WE_MICROSECOND_TICK
/**
 * @brief Number of data watchpoint trigger (DWT) ticks per microsecond (used for microsecond resolution delay/measurements).
 */
static uint32_t dwtTicksPerMicrosecond = 1;
#endif /* WE_MICROSECOND_TICK */

#if defined(WE_UART_DMA)
/**
 * @brief Priority for DMA interrupts (used for receiving data from radio module)
 */
#define WE_PRIORITY_DMA_RX 0

/**
 * @brief Size of DMA receive buffer.
 */
#define WE_DMA_RX_BUFFER_SIZE 512

static void WE_DMA_for_UART1_OnDataReceived(uint8_t *data, size_t size);
static void WE_DMA_for_UART1_Init();
static void WE_DMA_for_UART1_DeInit();
static void WE_UART1_CheckIfDmaDataAvailable();
static void WE_UART1_CheckIfDmaDataAvailableAsync();

static void WE_DMA_for_UART6_OnDataReceived(uint8_t *data, size_t size);
static void WE_DMA_for_UART6_Init();
static void WE_DMA_for_UART6_DeInit();
static void WE_UART6_CheckIfDmaDataAvailable();
static void WE_UART6_CheckIfDmaDataAvailableAsync();

#endif

static HAL_StatusTypeDef UartTransmitInternal(USART_TypeDef *hUart, const uint8_t *data, uint16_t size);

typedef struct
{
	/**
	 * @brief UART interface used for communicating with radio module.
	 */
	USART_TypeDef *uart;

	/**
	 * @brief Flow control setting used for uart.
	 */
	WE_FlowControl_t flowControl;

	/**
	 * @brief Pointer to the function to be called when one or several bytes have been received via UART
	 */
	WE_UART_HandleRxByte_t *rxByteHandlerP;

#if defined(WE_UART_DMA)
	/**
	 * @brief DMA used for receiving data from radio module (used only if DMA is enabled).
	 */
	DMA_TypeDef *dmaRx;

	/**
	 * @brief DMA stream used for receiving data from radio module (used only if DMA is enabled).
	 */
	uint32_t dmaRxStream;

	/**
	 * @brief Buffer used for dmaRx.
	 */
	uint8_t WE_dmaRxBuffer[WE_DMA_RX_BUFFER_SIZE];

	/**
	 * @brief Last read position in DMA receive buffer (used only if DMA is enabled).
	 * @see WE_UART1_CheckIfDmaDataAvailable()
	 */
	size_t dmaLastReadPos;

	bool triggered;
#endif

	/**
	 * @brief Last received byte
	 */
	uint8_t receivedByte;

} WE_UART_Internal_t;

WE_UART_Internal_t WE_UART1_Internal = {
		.uart = NULL,
		.flowControl = WE_FlowControl_NoFlowControl,
		.rxByteHandlerP = NULL,
#if defined(WE_UART_DMA)
		.dmaRx = NULL,
		.dmaRxStream = 0,
		.dmaLastReadPos = 0,
		.triggered = false,
#endif
		};

WE_UART_Internal_t WE_UART6_Internal = {
		.uart = NULL,
		.flowControl = WE_FlowControl_NoFlowControl,
		.rxByteHandlerP = NULL,
#if defined(WE_UART_DMA)
		.dmaRx = NULL,
		.dmaRxStream = 0,
		.dmaLastReadPos = 0,
		.triggered = false,
#endif
		};

void WE_SystemClock_Config()
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {
			0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {
			0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 84;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		WE_Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		WE_Error_Handler();
	}

#ifdef WE_MICROSECOND_TICK
    /* Enable microsecond tick counter using data watchpoint trigger (DWT) */

    /* Disable TRC */
    CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
    /* Enable TRC */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    /* Disable clock cycle counter */
    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
    /* Enable clock cycle counter */
    DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk;

    /* Reset the clock cycle counter value */
    DWT->CYCCNT = 0;

    /* 3 NO OPERATION instructions */
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");

    /* Check if clock cycle counter has started */
    if (DWT->CYCCNT == 0)
    {
        /* clock cycle counter not started */
        WE_Error_Handler();
    }

    dwtTicksPerMicrosecond = (HAL_RCC_GetHCLKFreq() / 1000000);
#endif
}

bool WE_UART1_Transmit(const uint8_t *data, uint16_t length)
{
	return (HAL_OK == UartTransmitInternal(WE_UART1_Internal.uart, data, length));
}

bool WE_UART6_Transmit(const uint8_t *data, uint16_t length)
{
	return (HAL_OK == UartTransmitInternal(WE_UART6_Internal.uart, data, length));
}

bool WE_UART1_Init(uint32_t baudrate, WE_FlowControl_t flowControl, WE_Parity_t parity, WE_UART_HandleRxByte_t *rxByteHandlerP)
{
	WE_UART1_Internal.flowControl = flowControl;
	WE_UART1_Internal.rxByteHandlerP = rxByteHandlerP;

#if defined(WE_UART_DMA)
	/* Enable pendable service interrupt which is used for processing data received from radio module
	 * (interrupt is triggered via WE_UART1_CheckIfDmaDataAvailableAsync() when bytes have been received) */
	NVIC_SetPriority(PendSV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), WE_PRIORITY_RX_DATA_PROCESSING, 0));
	NVIC_EnableIRQ(PendSV_IRQn);
#endif

	/* USART1 clock enable */
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

	/* USART 1 GPIO initialization */
	LL_GPIO_InitTypeDef gpioInitStruct = {
			0 };

	/* USART1 GPIO Configuration
	 PA11   ------> USART1_CTS
	 PA12   ------> USART1_RTS
	 PB6    ------> USART1_TX
	 PB7    ------> USART1_RX
	 */
	gpioInitStruct.Pin = LL_GPIO_PIN_6;
	gpioInitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	gpioInitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	gpioInitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpioInitStruct.Pull = LL_GPIO_PULL_NO;
	gpioInitStruct.Alternate = LL_GPIO_AF_7;
	LL_GPIO_Init(GPIOB, &gpioInitStruct);

	gpioInitStruct.Pin = LL_GPIO_PIN_7;
	gpioInitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	gpioInitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	gpioInitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpioInitStruct.Pull = LL_GPIO_PULL_UP;
	gpioInitStruct.Alternate = LL_GPIO_AF_7;
	LL_GPIO_Init(GPIOB, &gpioInitStruct);

	/* Flow control pins */
	if (WE_FlowControl_NoFlowControl != WE_UART1_Internal.flowControl)
	{
		/* enable port clock */
		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

		if (WE_FlowControl_RTSOnly == WE_UART1_Internal.flowControl || WE_FlowControl_RTSAndCTS == WE_UART1_Internal.flowControl)
		{
			gpioInitStruct.Pin = LL_GPIO_PIN_12;
			gpioInitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
			gpioInitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
			gpioInitStruct.Pull = LL_GPIO_PULL_NO;
			gpioInitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
			gpioInitStruct.Alternate = LL_GPIO_AF_7;
			LL_GPIO_Init(GPIOA, &gpioInitStruct);
		}
		if (WE_FlowControl_CTSOnly == WE_UART1_Internal.flowControl || WE_FlowControl_RTSAndCTS == WE_UART1_Internal.flowControl)
		{
			gpioInitStruct.Pin = LL_GPIO_PIN_11;
			gpioInitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
			gpioInitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
			gpioInitStruct.Pull = LL_GPIO_PULL_NO;
			gpioInitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
			gpioInitStruct.Alternate = LL_GPIO_AF_7;
			LL_GPIO_Init(GPIOA, &gpioInitStruct);
		}
	}

	/* Initialize USART1 */

	WE_UART1_Internal.uart = USART1;

	LL_USART_InitTypeDef uartInitStruct = {
			0 };
	uartInitStruct.BaudRate = baudrate;
	/* Data width / word length is specified including parity bit - so if using
	 * a parity bit, 9 bit data width has to be set */
	if (parity == WE_Parity_None)
	{
		uartInitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	}
	else
	{
		uartInitStruct.DataWidth = LL_USART_DATAWIDTH_9B;
	}
	uartInitStruct.StopBits = LL_USART_STOPBITS_1;
	switch (parity)
	{
	case WE_Parity_Odd:
		uartInitStruct.Parity = LL_USART_PARITY_ODD;
		break;

	case WE_Parity_Even:
		uartInitStruct.Parity = LL_USART_PARITY_EVEN;
		break;

	case WE_Parity_None:
	default:
		uartInitStruct.Parity = LL_USART_PARITY_NONE;
		break;
	}
	uartInitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
	switch (WE_UART1_Internal.flowControl)
	{
	case WE_FlowControl_RTSOnly:
		uartInitStruct.HardwareFlowControl = LL_USART_HWCONTROL_RTS;
		break;

	case WE_FlowControl_CTSOnly:
		uartInitStruct.HardwareFlowControl = LL_USART_HWCONTROL_CTS;
		break;

	case WE_FlowControl_RTSAndCTS:
		uartInitStruct.HardwareFlowControl = LL_USART_HWCONTROL_RTS_CTS;
		break;

	case WE_FlowControl_NoFlowControl:
	default:
		uartInitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
		break;
	}

	uartInitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
	LL_USART_Init(WE_UART1_Internal.uart, &uartInitStruct);

	LL_USART_ConfigAsyncMode(WE_UART1_Internal.uart);
	LL_USART_Enable(WE_UART1_Internal.uart);

	/* USART1 interrupt Init */
	NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), WE_PRIORITY_UART_RX, 0));
	NVIC_EnableIRQ(USART1_IRQn);

#if defined(WE_UART_DMA)
	WE_DMA_for_UART1_Init();
#else
	// Legacy mode using USART interrupt
	LL_USART_EnableIT_RXNE(WE_UART1_Internal.uart);
#endif

	return true;
}

bool WE_UART1_DeInit()
{
	if (WE_UART1_Internal.uart == NULL)
	{
		return true;
	}

#if defined(WE_UART_DMA)
	WE_DMA_for_UART1_DeInit();
#else
	LL_USART_DisableIT_RXNE(WE_UART1_Internal.uart);
#endif

	WE_UART1_Internal.rxByteHandlerP = NULL;

	NVIC_DisableIRQ(USART1_IRQn);
	LL_USART_Disable(WE_UART1_Internal.uart);
	LL_USART_DeInit(WE_UART1_Internal.uart);
	LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_USART1);

	if (WE_FlowControl_NoFlowControl != WE_UART1_Internal.flowControl)
	{
		LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	}

	WE_UART1_Internal.uart = NULL;
	return true;
}

static HAL_StatusTypeDef UartTransmitInternal(USART_TypeDef *hUart, const uint8_t *data, uint16_t size)
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

/**
 * @brief Interrupt handler for data received from wireless module via UART.
 *
 * Handles the following interrupts, depending on whether DMA is enabled or not:
 * - If DMA is enabled: Idle line interrupt
 * - If DMA is disabled: Character received interrupt
 */
void USART1_IRQHandler()
{
#if defined(WE_UART_DMA)
	if (LL_USART_IsActiveFlag_IDLE(USART1))
	{
		/* USART idle line interrupt (used only if DMA is enabled) */

		LL_USART_ClearFlag_IDLE(USART1);
		WE_UART1_CheckIfDmaDataAvailableAsync();
	}
#else
    if (LL_USART_IsActiveFlag_RXNE(WE_UART1_Internal.uart))
    {
        /* Character has been received (used only if DMA is disabled) */

        /* RXNE flag will be cleared by reading of DR register */
    	WE_UART1_Internal.receivedByte = LL_USART_ReceiveData8(WE_UART1_Internal.uart);
    	(*WE_UART1_Internal.rxByteHandlerP)(&WE_UART1_Internal.receivedByte, 1);
    }
	if (LL_USART_IsActiveFlag_ORE(WE_UART1_Internal.uart)) {
        LL_USART_ClearFlag_ORE(WE_UART1_Internal.uart);
    }
#endif
}

#if defined(WE_UART_DMA)

/**
 * @brief Interrupt handler for data received from wireless module via DMA.
 *
 * Is only used if DMA is enabled.
 */
void DMA2_Stream2_IRQHandler()
{
	if (LL_DMA_IsEnabledIT_HT(WE_UART1_Internal.dmaRx, WE_UART1_Internal.dmaRxStream) && LL_DMA_IsActiveFlag_HT2(WE_UART1_Internal.dmaRx))
	{
		/* DMA receive half-transfer complete */
		LL_DMA_ClearFlag_HT2(WE_UART1_Internal.dmaRx);
		WE_UART1_CheckIfDmaDataAvailableAsync();
	}

	if (LL_DMA_IsEnabledIT_TC(WE_UART1_Internal.dmaRx, WE_UART1_Internal.dmaRxStream) && LL_DMA_IsActiveFlag_TC2(WE_UART1_Internal.dmaRx))
	{
		/* DMA receive transfer complete */
		LL_DMA_ClearFlag_TC2(WE_UART1_Internal.dmaRx);
		WE_UART1_CheckIfDmaDataAvailableAsync();
	}

	/* Other events can be implemented if required. */
}

void DMA2_Stream1_IRQHandler()
{
	if (LL_DMA_IsEnabledIT_HT(WE_UART6_Internal.dmaRx, WE_UART6_Internal.dmaRxStream) && LL_DMA_IsActiveFlag_HT1(WE_UART6_Internal.dmaRx))
	{
		/* DMA receive half-transfer complete */
		LL_DMA_ClearFlag_HT1(WE_UART6_Internal.dmaRx);
		WE_UART6_CheckIfDmaDataAvailableAsync();
	}

	if (LL_DMA_IsEnabledIT_TC(WE_UART6_Internal.dmaRx, WE_UART6_Internal.dmaRxStream) && LL_DMA_IsActiveFlag_TC1(WE_UART6_Internal.dmaRx))
	{
		/* DMA receive transfer complete */
		LL_DMA_ClearFlag_TC1(WE_UART6_Internal.dmaRx);
		WE_UART6_CheckIfDmaDataAvailableAsync();
	}

	/* Other events can be implemented if required. */
}

/**
 * @brief Pendable service interrupt handler.
 *
 * Is used for asynchronous processing of data received from radio module via DMA.
 * Calls WE_UART1_CheckIfDmaDataAvailable().
 *
 * @see WE_UART1_CheckIfDmaDataAvailableAsync()
 */
void PendSV_Handler()
{
	if (WE_UART1_Internal.triggered)
	{
		WE_UART1_Internal.triggered = false;
		WE_UART1_CheckIfDmaDataAvailable();
	}

	if (WE_UART6_Internal.triggered)
	{
		WE_UART6_Internal.triggered = false;
		WE_UART6_CheckIfDmaDataAvailable();
	}
}

static void WE_UART6_CheckIfDmaDataAvailableAsync()
{
	/* Trigger pendable service interrupt (PendSV) */
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
	WE_UART6_Internal.triggered = true;
}

static void WE_UART1_CheckIfDmaDataAvailableAsync()
{
	/* Trigger pendable service interrupt (PendSV) */
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
	WE_UART1_Internal.triggered = true;
}

static void WE_UART1_CheckIfDmaDataAvailable()
{
	/* Get current DMA write position (in ring buffer) */
	size_t pos = WE_DMA_RX_BUFFER_SIZE - LL_DMA_GetDataLength(WE_UART1_Internal.dmaRx, WE_UART1_Internal.dmaRxStream);

	/* Check if new data is available (i.e. write position has changed) */
	if (pos != WE_UART1_Internal.dmaLastReadPos)
	{
		if (pos > WE_UART1_Internal.dmaLastReadPos)
		{
			/* Data to be read from DMA ring buffer is continuous - between lastPos and pos */
			WE_DMA_for_UART1_OnDataReceived(&WE_UART1_Internal.WE_dmaRxBuffer[WE_UART1_Internal.dmaLastReadPos], pos - WE_UART1_Internal.dmaLastReadPos);
		}
		else /* pos < WE_UART1_Internal.dmaLastReadPos */
		{
			/* Ring buffer write position has reached the end of the buffer and restarted at the beginning.
			 * Data is split:
			 * - Bytes between lastPos and the end of the buffer
			 * - Bytes between start of the buffer and pos
			 */
			WE_DMA_for_UART1_OnDataReceived(&WE_UART1_Internal.WE_dmaRxBuffer[WE_UART1_Internal.dmaLastReadPos], WE_DMA_RX_BUFFER_SIZE - WE_UART1_Internal.dmaLastReadPos);
			if (pos > 0)
			{
				WE_DMA_for_UART1_OnDataReceived(&WE_UART1_Internal.WE_dmaRxBuffer[0], pos);
			}
		}

		/* Store DMA write position for next interrupt */
		WE_UART1_Internal.dmaLastReadPos = pos;
	}
}

/**
 * @brief Is called when one or more bytes have been received from UART via DMA.
 * @param data Received data
 * @param size Number of bytes received
 */
static void WE_DMA_for_UART1_OnDataReceived(uint8_t *data, size_t size)
{
	(*WE_UART1_Internal.rxByteHandlerP)(data, size);
}

static void WE_DMA_for_UART1_Init()
{
	/* USART1 DMA Init */
	WE_UART1_Internal.dmaRx = DMA2;
	WE_UART1_Internal.dmaRxStream = LL_DMA_STREAM_2;
	WE_UART1_Internal.dmaLastReadPos = 0;

	/* DMA controller clock enable */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);

	/* DMA interrupt init */
	/* DMA2_Stream2_IRQn interrupt configuration */
	NVIC_SetPriority(DMA2_Stream2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), WE_PRIORITY_DMA_RX, 0));
	NVIC_EnableIRQ(DMA2_Stream2_IRQn);

	LL_DMA_SetChannelSelection(WE_UART1_Internal.dmaRx, WE_UART1_Internal.dmaRxStream, LL_DMA_CHANNEL_4);
	LL_DMA_SetDataTransferDirection(WE_UART1_Internal.dmaRx, WE_UART1_Internal.dmaRxStream, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
	LL_DMA_SetStreamPriorityLevel(WE_UART1_Internal.dmaRx, WE_UART1_Internal.dmaRxStream, LL_DMA_PRIORITY_LOW);
	LL_DMA_SetMode(WE_UART1_Internal.dmaRx, WE_UART1_Internal.dmaRxStream, LL_DMA_MODE_CIRCULAR);
	LL_DMA_SetPeriphIncMode(WE_UART1_Internal.dmaRx, WE_UART1_Internal.dmaRxStream, LL_DMA_PERIPH_NOINCREMENT);
	LL_DMA_SetMemoryIncMode(WE_UART1_Internal.dmaRx, WE_UART1_Internal.dmaRxStream, LL_DMA_MEMORY_INCREMENT);
	LL_DMA_SetPeriphSize(WE_UART1_Internal.dmaRx, WE_UART1_Internal.dmaRxStream, LL_DMA_PDATAALIGN_BYTE);
	LL_DMA_SetMemorySize(WE_UART1_Internal.dmaRx, WE_UART1_Internal.dmaRxStream, LL_DMA_MDATAALIGN_BYTE);
	LL_DMA_DisableFifoMode(WE_UART1_Internal.dmaRx, WE_UART1_Internal.dmaRxStream);

	LL_DMA_SetPeriphAddress(WE_UART1_Internal.dmaRx, WE_UART1_Internal.dmaRxStream, (uint32_t) &USART1->DR);
	LL_DMA_SetMemoryAddress(WE_UART1_Internal.dmaRx, WE_UART1_Internal.dmaRxStream, (uint32_t) WE_UART1_Internal.WE_dmaRxBuffer);
	LL_DMA_SetDataLength(WE_UART1_Internal.dmaRx, WE_UART1_Internal.dmaRxStream, WE_DMA_RX_BUFFER_SIZE);

	/* Enable HT & TC interrupts */
	LL_DMA_EnableIT_HT(WE_UART1_Internal.dmaRx, WE_UART1_Internal.dmaRxStream);
	LL_DMA_EnableIT_TC(WE_UART1_Internal.dmaRx, WE_UART1_Internal.dmaRxStream);

	/* Enable DMA */
	LL_USART_EnableDMAReq_RX(WE_UART1_Internal.uart);
	LL_USART_EnableIT_IDLE(WE_UART1_Internal.uart);
	LL_DMA_EnableStream(WE_UART1_Internal.dmaRx, WE_UART1_Internal.dmaRxStream);
}

static void WE_DMA_for_UART1_DeInit()
{
	if (WE_UART1_Internal.dmaRx == NULL)
	{
		return;
	}

	LL_DMA_DisableStream(WE_UART1_Internal.dmaRx, WE_UART1_Internal.dmaRxStream);
	LL_USART_DisableIT_IDLE(WE_UART1_Internal.uart);
	LL_USART_DisableDMAReq_RX(WE_UART1_Internal.uart);
	LL_DMA_DisableIT_TC(WE_UART1_Internal.dmaRx, WE_UART1_Internal.dmaRxStream);
	LL_DMA_DisableIT_HT(WE_UART1_Internal.dmaRx, WE_UART1_Internal.dmaRxStream);
	NVIC_DisableIRQ(DMA2_Stream2_IRQn);
	LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_DMA2);

	WE_UART1_Internal.dmaRxStream = 0;
	WE_UART1_Internal.dmaRx = NULL;
	WE_UART1_Internal.dmaLastReadPos = 0;
}

#endif /* defined(WE_UART_DMA) */

bool WE_UART6_Init(uint32_t baudrate, WE_FlowControl_t flowControl, WE_Parity_t parity, WE_UART_HandleRxByte_t *rxByteHandlerP)
{
	WE_UART6_Internal.flowControl = flowControl;
	WE_UART6_Internal.rxByteHandlerP = rxByteHandlerP;

#if defined(WE_UART_DMA)
	/* Enable pendable service interrupt which is used for processing data received from radio module
	 * (interrupt is triggered via WE_UART1_CheckIfDmaDataAvailableAsync() when bytes have been received) */
	NVIC_SetPriority(PendSV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), WE_PRIORITY_RX_DATA_PROCESSING, 0));
	NVIC_EnableIRQ(PendSV_IRQn);
#endif

	/* USART6 clock enable */
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART6);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);

	/* USART 1 GPIO initialization */
	LL_GPIO_InitTypeDef gpioInitStruct = {
			0 };

	/* USART6 GPIO Configuration
	 PC6    ------> USART6_TX
	 PC7    ------> USART6_RX
	 */
	gpioInitStruct.Pin = LL_GPIO_PIN_6;
	gpioInitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	gpioInitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	gpioInitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpioInitStruct.Pull = LL_GPIO_PULL_NO;
	gpioInitStruct.Alternate = LL_GPIO_AF_8;
	LL_GPIO_Init(GPIOC, &gpioInitStruct);

	gpioInitStruct.Pin = LL_GPIO_PIN_7;
	gpioInitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	gpioInitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	gpioInitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpioInitStruct.Pull = LL_GPIO_PULL_UP;
	gpioInitStruct.Alternate = LL_GPIO_AF_8;
	LL_GPIO_Init(GPIOC, &gpioInitStruct);

	/* Flow control pins */
	if (WE_FlowControl_NoFlowControl != WE_UART6_Internal.flowControl)
	{
		/* UART6 does not support flow control */
		return false;
	}

	/* Initialize USART6 */

	WE_UART6_Internal.uart = USART6;

	LL_USART_InitTypeDef uartInitStruct = {
			0 };
	uartInitStruct.BaudRate = baudrate;
	/* Data width / word length is specified including parity bit - so if using
	 * a parity bit, 9 bit data width has to be set */
	if (parity == WE_Parity_None)
	{
		uartInitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	}
	else
	{
		uartInitStruct.DataWidth = LL_USART_DATAWIDTH_9B;
	}
	uartInitStruct.StopBits = LL_USART_STOPBITS_1;
	switch (parity)
	{
	case WE_Parity_Odd:
		uartInitStruct.Parity = LL_USART_PARITY_ODD;
		break;

	case WE_Parity_Even:
		uartInitStruct.Parity = LL_USART_PARITY_EVEN;
		break;

	case WE_Parity_None:
	default:
		uartInitStruct.Parity = LL_USART_PARITY_NONE;
		break;
	}
	uartInitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
	switch (WE_UART6_Internal.flowControl)
	{
	case WE_FlowControl_RTSOnly:
		uartInitStruct.HardwareFlowControl = LL_USART_HWCONTROL_RTS;
		break;

	case WE_FlowControl_CTSOnly:
		uartInitStruct.HardwareFlowControl = LL_USART_HWCONTROL_CTS;
		break;

	case WE_FlowControl_RTSAndCTS:
		uartInitStruct.HardwareFlowControl = LL_USART_HWCONTROL_RTS_CTS;
		break;

	case WE_FlowControl_NoFlowControl:
	default:
		uartInitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
		break;
	}

	uartInitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
	LL_USART_Init(WE_UART6_Internal.uart, &uartInitStruct);

	LL_USART_ConfigAsyncMode(WE_UART6_Internal.uart);
	LL_USART_Enable(WE_UART6_Internal.uart);

	/* USART6 interrupt Init */
	NVIC_SetPriority(USART6_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), WE_PRIORITY_UART_RX, 0));
	NVIC_EnableIRQ(USART6_IRQn);

#if defined(WE_UART_DMA)
	WE_DMA_for_UART6_Init();
#else
	// Legacy mode using USART interrupt
	LL_USART_EnableIT_RXNE(WE_UART6_Internal.uart);
#endif

	return true;
}

bool WE_UART6_DeInit()
{
	if (WE_UART6_Internal.uart == NULL)
	{
		return true;
	}

#if defined(WE_UART_DMA)
	WE_DMA_for_UART6_DeInit();
#else
	LL_USART_DisableIT_RXNE(WE_UART6_Internal.uart);
#endif

	WE_UART6_Internal.rxByteHandlerP = NULL;

	NVIC_DisableIRQ(USART6_IRQn);
	LL_USART_Disable(WE_UART6_Internal.uart);
	LL_USART_DeInit(WE_UART6_Internal.uart);
	LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_USART6);

	if (WE_FlowControl_NoFlowControl != WE_UART6_Internal.flowControl)
	{
		/* flow control for UART6 not implemented */
	}

	WE_UART6_Internal.uart = NULL;
	return true;
}

/**
 * @brief Interrupt handler for data received from wireless module via UART.
 *
 * Handles the following interrupts, depending on whether DMA is enabled or not:
 * - If DMA is enabled: Idle line interrupt
 * - If DMA is disabled: Character received interrupt
 */
void USART6_IRQHandler()
{
#if defined(WE_UART_DMA)
	if (LL_USART_IsActiveFlag_IDLE(USART6))
	{
		/* USART idle line interrupt (used only if DMA is enabled) */

		LL_USART_ClearFlag_IDLE(USART6);
		WE_UART6_CheckIfDmaDataAvailableAsync();
	}
#else
    if (LL_USART_IsActiveFlag_RXNE(WE_UART6_Internal.uart))
    {
        /* Character has been received (used only if DMA is disabled) */

        /* RXNE flag will be cleared by reading of DR register */
    	WE_UART6_Internal.receivedByte = LL_USART_ReceiveData8(WE_UART6_Internal.uart);
    	(*WE_UART6_Internal.rxByteHandlerP)(&WE_UART6_Internal.receivedByte, 1);
    }
	if (LL_USART_IsActiveFlag_ORE(WE_UART6_Internal.uart)) {
        LL_USART_ClearFlag_ORE(WE_UART6_Internal.uart);
    }
#endif
}

#if defined(WE_UART_DMA)
static void WE_UART6_CheckIfDmaDataAvailable()
{
	/* Get current DMA write position (in ring buffer) */
	size_t pos = WE_DMA_RX_BUFFER_SIZE - LL_DMA_GetDataLength(WE_UART6_Internal.dmaRx, WE_UART6_Internal.dmaRxStream);

	/* Check if new data is available (i.e. write position has changed) */
	if (pos != WE_UART6_Internal.dmaLastReadPos)
	{
		if (pos > WE_UART6_Internal.dmaLastReadPos)
		{
			/* Data to be read from DMA ring buffer is continuous - between lastPos and pos */
			WE_DMA_for_UART6_OnDataReceived(&WE_UART6_Internal.WE_dmaRxBuffer[WE_UART6_Internal.dmaLastReadPos], pos - WE_UART6_Internal.dmaLastReadPos);
		}
		else /* pos < WE_UART6_Internal.dmaLastReadPos */
		{
			/* Ring buffer write position has reached the end of the buffer and restarted at the beginning.
			 * Data is split:
			 * - Bytes between lastPos and the end of the buffer
			 * - Bytes between start of the buffer and pos
			 */
			WE_DMA_for_UART6_OnDataReceived(&WE_UART6_Internal.WE_dmaRxBuffer[WE_UART6_Internal.dmaLastReadPos], WE_DMA_RX_BUFFER_SIZE - WE_UART6_Internal.dmaLastReadPos);
			if (pos > 0)
			{
				WE_DMA_for_UART6_OnDataReceived(&WE_UART6_Internal.WE_dmaRxBuffer[0], pos);
			}
		}

		/* Store DMA write position for next interrupt */
		WE_UART6_Internal.dmaLastReadPos = pos;
	}
}

/**
 * @brief Is called when one or more bytes have been received from UART via DMA.
 * @param data Received data
 * @param size Number of bytes received
 */
static void WE_DMA_for_UART6_OnDataReceived(uint8_t *data, size_t size)
{
	for (; size > 0; size--, data++)
	{
		(*WE_UART6_Internal.rxByteHandlerP)(data, size);
	}
}

static void WE_DMA_for_UART6_Init() //USART6_RX DMA2, stream 1, channel 5
{
	/* USART6 DMA Init */
	WE_UART6_Internal.dmaRx = DMA2;
	WE_UART6_Internal.dmaRxStream = LL_DMA_STREAM_1;
	WE_UART6_Internal.dmaLastReadPos = 0;

	/* DMA controller clock enable */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);

	/* DMA interrupt init */
	/* DMA2_Stream1_IRQn interrupt configuration */
	NVIC_SetPriority(DMA2_Stream1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), WE_PRIORITY_DMA_RX, 0));
	NVIC_EnableIRQ(DMA2_Stream1_IRQn);

	LL_DMA_SetChannelSelection(WE_UART6_Internal.dmaRx, WE_UART6_Internal.dmaRxStream, LL_DMA_CHANNEL_5);
	LL_DMA_SetDataTransferDirection(WE_UART6_Internal.dmaRx, WE_UART6_Internal.dmaRxStream, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
	LL_DMA_SetStreamPriorityLevel(WE_UART6_Internal.dmaRx, WE_UART6_Internal.dmaRxStream, LL_DMA_PRIORITY_LOW);
	LL_DMA_SetMode(WE_UART6_Internal.dmaRx, WE_UART6_Internal.dmaRxStream, LL_DMA_MODE_CIRCULAR);
	LL_DMA_SetPeriphIncMode(WE_UART6_Internal.dmaRx, WE_UART6_Internal.dmaRxStream, LL_DMA_PERIPH_NOINCREMENT);
	LL_DMA_SetMemoryIncMode(WE_UART6_Internal.dmaRx, WE_UART6_Internal.dmaRxStream, LL_DMA_MEMORY_INCREMENT);
	LL_DMA_SetPeriphSize(WE_UART6_Internal.dmaRx, WE_UART6_Internal.dmaRxStream, LL_DMA_PDATAALIGN_BYTE);
	LL_DMA_SetMemorySize(WE_UART6_Internal.dmaRx, WE_UART6_Internal.dmaRxStream, LL_DMA_MDATAALIGN_BYTE);
	LL_DMA_DisableFifoMode(WE_UART6_Internal.dmaRx, WE_UART6_Internal.dmaRxStream);

	LL_DMA_SetPeriphAddress(WE_UART6_Internal.dmaRx, WE_UART6_Internal.dmaRxStream, (uint32_t) &USART6->DR);
	LL_DMA_SetMemoryAddress(WE_UART6_Internal.dmaRx, WE_UART6_Internal.dmaRxStream, (uint32_t) WE_UART6_Internal.WE_dmaRxBuffer);
	LL_DMA_SetDataLength(WE_UART6_Internal.dmaRx, WE_UART6_Internal.dmaRxStream, WE_DMA_RX_BUFFER_SIZE);

	/* Enable HT & TC interrupts */
	LL_DMA_EnableIT_HT(WE_UART6_Internal.dmaRx, WE_UART6_Internal.dmaRxStream);
	LL_DMA_EnableIT_TC(WE_UART6_Internal.dmaRx, WE_UART6_Internal.dmaRxStream);

	/* Enable DMA */
	LL_USART_EnableDMAReq_RX(WE_UART6_Internal.uart);
	LL_USART_EnableIT_IDLE(WE_UART6_Internal.uart);
	LL_DMA_EnableStream(WE_UART6_Internal.dmaRx, WE_UART6_Internal.dmaRxStream);
}

static void WE_DMA_for_UART6_DeInit()
{
	if (WE_UART6_Internal.dmaRx == NULL)
	{
		return;
	}

	LL_DMA_DisableStream(WE_UART6_Internal.dmaRx, WE_UART6_Internal.dmaRxStream);
	LL_USART_DisableIT_IDLE(WE_UART6_Internal.uart);
	LL_USART_DisableDMAReq_RX(WE_UART6_Internal.uart);
	LL_DMA_DisableIT_TC(WE_UART6_Internal.dmaRx, WE_UART6_Internal.dmaRxStream);
	LL_DMA_DisableIT_HT(WE_UART6_Internal.dmaRx, WE_UART6_Internal.dmaRxStream);
	NVIC_DisableIRQ(DMA2_Stream1_IRQn);
	LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_DMA2);

	WE_UART6_Internal.dmaRxStream = 0;
	WE_UART6_Internal.dmaRx = NULL;
	WE_UART6_Internal.dmaLastReadPos = 0;
}

#endif /* defined(WE_UART_DMA) */

#ifdef WE_MICROSECOND_TICK
void WE_DelayMicroseconds(uint32_t sleepForUsec)
{
    uint32_t initialTicks = DWT->CYCCNT;
    sleepForUsec *= dwtTicksPerMicrosecond;
    while ((DWT->CYCCNT - initialTicks) < sleepForUsec - dwtTicksPerMicrosecond);
}

uint32_t WE_GetTickMicroseconds()
{
    return DWT->CYCCNT / dwtTicksPerMicrosecond;
}
#endif /* WE_MICROSECOND_TICK */

#endif /* STM32F401xE */
