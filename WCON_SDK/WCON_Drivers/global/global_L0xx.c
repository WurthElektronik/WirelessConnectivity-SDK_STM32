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
 * @brief STM32L0xx platform driver.
 */

#include "global_L0xx.h"

#ifdef STM32L073xx

#include "global.h"

void WE_SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /* Configure the main internal regulator output voltage */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /* Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
    RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        WE_Error_Handler();
    }
    /* Initializes the CPU, AHB and APB buses clocks */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
    {
        WE_Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        WE_Error_Handler();
    }
}

void WE_UART_Init(uint32_t baudrate,
                  WE_FlowControl_t fc,
                  WE_Parity_t par,
                  bool dma)
{
    WE_uartFlowControl = fc;
    WE_uartParity = par;
    WE_dmaEnabled = dma;

    /* USART1 clock enable */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

    /* USART 1 GPIO initialization */
    LL_GPIO_InitTypeDef gpioInitStruct = {0};

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
    gpioInitStruct.Alternate = LL_GPIO_AF_0;
    LL_GPIO_Init(GPIOB, &gpioInitStruct);

    gpioInitStruct.Pin = LL_GPIO_PIN_7;
    gpioInitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    gpioInitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    gpioInitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpioInitStruct.Pull = LL_GPIO_PULL_UP;
    gpioInitStruct.Alternate = LL_GPIO_AF_0;
    LL_GPIO_Init(GPIOB, &gpioInitStruct);

    /* Flow control pins */
    if (WE_FlowControl_NoFlowControl != WE_uartFlowControl)
    {
        GPIO_TypeDef *flowControlPort = GPIOA;
        uint32_t rtsPin = LL_GPIO_PIN_12;
        uint32_t ctsPin = LL_GPIO_PIN_11;
        uint32_t flowControlAlternateFunction = LL_GPIO_AF_4;
        if (WE_FlowControl_RTSOnly == WE_uartFlowControl ||
                WE_FlowControl_RTSAndCTS == WE_uartFlowControl)
        {
            gpioInitStruct.Pin = rtsPin;
            gpioInitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
            gpioInitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
            gpioInitStruct.Pull = LL_GPIO_PULL_NO;
            gpioInitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
            gpioInitStruct.Alternate = flowControlAlternateFunction;
            LL_GPIO_Init(flowControlPort, &gpioInitStruct);
        }
        if (WE_FlowControl_CTSOnly == WE_uartFlowControl ||
                WE_FlowControl_RTSAndCTS == WE_uartFlowControl)
        {
            gpioInitStruct.Pin = ctsPin;
            gpioInitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
            gpioInitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
            gpioInitStruct.Pull = LL_GPIO_PULL_NO;
            gpioInitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
            gpioInitStruct.Alternate = flowControlAlternateFunction;
            LL_GPIO_Init(flowControlPort, &gpioInitStruct);
        }
    }


    /* Initialize USART1 */

    WE_uartWireless = USART1;

    LL_USART_InitTypeDef uart1InitStruct = {0};
    uart1InitStruct.BaudRate = baudrate;
    /* Data width / word length is specified including parity bit - so if using
     * a parity bit, 9 bit data width has to be set */
    if (WE_uartParity == WE_Parity_None)
    {
        uart1InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    }
    else
    {
        uart1InitStruct.DataWidth = LL_USART_DATAWIDTH_9B;
    }
    uart1InitStruct.StopBits = LL_USART_STOPBITS_1;
    switch (WE_uartParity)
    {
    case WE_Parity_Odd:
        uart1InitStruct.Parity = LL_USART_PARITY_ODD;
        break;

    case WE_Parity_Even:
        uart1InitStruct.Parity = LL_USART_PARITY_EVEN;
        break;

    case WE_Parity_None:
    default:
        uart1InitStruct.Parity = LL_USART_PARITY_NONE;
        break;
    }
    uart1InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    switch (WE_uartFlowControl)
    {
    case WE_FlowControl_RTSOnly:
        uart1InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_RTS;
        break;

    case WE_FlowControl_CTSOnly:
        uart1InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_CTS;
        break;

    case WE_FlowControl_RTSAndCTS:
        uart1InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_RTS_CTS;
        break;

    case WE_FlowControl_NoFlowControl:
    default:
        uart1InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
        break;
    }

    uart1InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(WE_uartWireless, &uart1InitStruct);

    LL_USART_ConfigAsyncMode(WE_uartWireless);
    LL_USART_Enable(WE_uartWireless);

    /* USART1 interrupt Init */
    NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), WE_PRIORITY_UART_RX, 0));
    NVIC_EnableIRQ(USART1_IRQn);

    if (WE_dmaEnabled)
    {
        WE_DMA_Init();
    }
    else
    {
        // Legacy mode using USART interrupt
        LL_USART_EnableIT_RXNE(WE_uartWireless);
    }
}

void WE_UART_DeInit()
{
    if (WE_uartWireless == NULL)
    {
        return;
    }

    if (WE_dmaEnabled)
    {
        WE_DMA_DeInit();
    }
    else
    {
        LL_USART_DisableIT_RXNE(WE_uartWireless);
    }

    NVIC_DisableIRQ(USART1_IRQn);
    LL_USART_Disable(WE_uartWireless);
    LL_USART_DeInit(WE_uartWireless);
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_USART1);
    LL_IOP_GRP1_DisableClock(LL_IOP_GRP1_PERIPH_GPIOB);
}

void WE_DMA_Init()
{
    /* USART1 DMA Init */
    WE_dmaWirelessRx = DMA1;
    WE_dmaWirelessRxStream = LL_DMA_CHANNEL_3;

    /* DMA controller clock enable */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    /* DMA interrupt init */
    /* DMA1_Channel2_3_IRQn interrupt configuration */
    NVIC_SetPriority(DMA1_Channel2_3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), WE_PRIORITY_DMA_RX, 0));
    NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

    LL_DMA_SetPeriphRequest(WE_dmaWirelessRx, WE_dmaWirelessRxStream, LL_DMA_REQUEST_3);
    LL_DMA_SetDataTransferDirection(WE_dmaWirelessRx, WE_dmaWirelessRxStream, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_DMA_SetChannelPriorityLevel(WE_dmaWirelessRx, WE_dmaWirelessRxStream, LL_DMA_PRIORITY_LOW);
    LL_DMA_SetMode(WE_dmaWirelessRx, WE_dmaWirelessRxStream, LL_DMA_MODE_CIRCULAR);
    LL_DMA_SetPeriphIncMode(WE_dmaWirelessRx, WE_dmaWirelessRxStream, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(WE_dmaWirelessRx, WE_dmaWirelessRxStream, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize(WE_dmaWirelessRx, WE_dmaWirelessRxStream, LL_DMA_PDATAALIGN_BYTE);
    LL_DMA_SetMemorySize(WE_dmaWirelessRx, WE_dmaWirelessRxStream, LL_DMA_MDATAALIGN_BYTE);

    LL_DMA_SetPeriphAddress(WE_dmaWirelessRx, WE_dmaWirelessRxStream, (uint32_t) &USART1->RDR);
    LL_DMA_SetMemoryAddress(WE_dmaWirelessRx, WE_dmaWirelessRxStream, (uint32_t) WE_dmaRxBuffer);
    LL_DMA_SetDataLength(WE_dmaWirelessRx, WE_dmaWirelessRxStream, WE_DMA_RX_BUFFER_SIZE);

    /* Enable HT & TC interrupts */
    LL_DMA_EnableIT_HT(WE_dmaWirelessRx, WE_dmaWirelessRxStream);
    LL_DMA_EnableIT_TC(WE_dmaWirelessRx, WE_dmaWirelessRxStream);

    /* Enable DMA */
    LL_USART_EnableDMAReq_RX(WE_uartWireless);
    LL_USART_EnableIT_IDLE(WE_uartWireless);
    LL_DMA_EnableChannel(WE_dmaWirelessRx, WE_dmaWirelessRxStream);
}

void WE_DMA_DeInit()
{
    if (WE_dmaWirelessRx == NULL)
    {
        return;
    }

    LL_DMA_DisableChannel(WE_dmaWirelessRx, WE_dmaWirelessRxStream);
    LL_USART_DisableIT_IDLE(WE_uartWireless);
    LL_USART_DisableDMAReq_RX(WE_uartWireless);
    LL_DMA_DisableIT_TC(WE_dmaWirelessRx, WE_dmaWirelessRxStream);
    LL_DMA_DisableIT_HT(WE_dmaWirelessRx, WE_dmaWirelessRxStream);
    NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);
    LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    WE_dmaWirelessRxStream = 0;
    WE_dmaWirelessRx = NULL;
}

/**
 * @brief Interrupt handler for data received from wireless module via DMA.
 *
 * Is only used if DMA is enabled.
 */
void DMA1_Channel2_3_IRQHandler(void)
{
    if (LL_DMA_IsEnabledIT_HT(WE_dmaWirelessRx, WE_dmaWirelessRxStream) &&
            LL_DMA_IsActiveFlag_HT3(WE_dmaWirelessRx))
    {
        /* DMA receive half-transfer complete */

        LL_DMA_ClearFlag_HT3(WE_dmaWirelessRx);
        WE_CheckIfDmaDataAvailableAsync();
    }

    if (LL_DMA_IsEnabledIT_TC(WE_dmaWirelessRx, WE_dmaWirelessRxStream) &&
            LL_DMA_IsActiveFlag_TC3(WE_dmaWirelessRx))
    {
        /* DMA receive transfer complete */

        LL_DMA_ClearFlag_TC3(WE_dmaWirelessRx);
        WE_CheckIfDmaDataAvailableAsync();
    }
}

#endif /* STM32L073xx */
