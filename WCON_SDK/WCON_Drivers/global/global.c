/**
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
 * COPYRIGHT (c) 2021 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 **/

#include "global.h"
#include <string.h>


#ifdef __cplusplus
extern "C" {
#endif

#ifndef _global_defined
#define _global_defined

/**************************************
 *             Variables              *
 **************************************/
UART_HandleTypeDef huart1;

#define UART_BUFFERSIZE 256
static uint8_t RX_buffer[UART_BUFFERSIZE];
static uint16_t RX_buffer_count = 0;
static FlowControl_t flow_control = No_flow_control;

/**************************************
 *         Global functions           *
 **************************************/

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PB3     ------> USART1_RTS
    PB4     ------> USART1_CTS
    PB6     ------> USART1_TX
    PB7     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF0_USART1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* flow control pins */
    if(No_flow_control != flow_control)
    {
    	switch(flow_control)
    	{
			case RTS_Only:
			{
				GPIO_InitStruct.Pin = GPIO_PIN_3;
			}
			break;
			case CTS_Only:
			{
				GPIO_InitStruct.Pin = GPIO_PIN_4;
			}
			break;
			case RTS_and_CTS:
			{
				GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4;
			}
			break;
			default:
			{
				GPIO_InitStruct.Pin = 0;
			}
			break;
    	}
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_USART1;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PB3     ------> USART1_RTS
    PB4     ------> USART1_CTS
    PB6     ------> USART1_TX
    PB7     ------> USART1_RX
    */

    switch(flow_control)
	{
		case RTS_Only:
		{
			HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3);
		}
		break;
		case CTS_Only:
		{
			HAL_GPIO_DeInit(GPIOB, GPIO_PIN_4);
		}
		break;
		case RTS_and_CTS:
		{
			HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_4);
		}
		break;
		default:
		{
		}
		break;
	}
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);


    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  }
}


/**
  * @brief  Tx Transfer completed callback
  * @param  UartHandle: UART handle.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{

}

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  UART_HandleRxByte(RX_buffer[RX_buffer_count]);

  if(RX_buffer_count++ >= UART_BUFFERSIZE)
  {
	RX_buffer_count = 0;
  }

  if (HAL_UART_Receive_IT(&huart1, &RX_buffer[RX_buffer_count], 1 /* byte */) != HAL_OK)
  {
	  Error_Handler();
  }
}

void UART_Init(uint32_t baudrate, FlowControl_t fc)
{
	flow_control = fc;

	huart1.Instance = USART1;
	huart1.Init.BaudRate = baudrate;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	switch(flow_control)
	{
		case RTS_Only:
		{
			huart1.Init.HwFlowCtl = UART_HWCONTROL_RTS;
		}
		break;
		case CTS_Only:
		{
			huart1.Init.HwFlowCtl = UART_HWCONTROL_CTS;
		}
		break;
		case RTS_and_CTS:
		{
			huart1.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
		}
		break;
		case No_flow_control:
		default:
		{
			huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		}
		break;
	}

	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_UART_Receive_IT(&huart1, &RX_buffer[RX_buffer_count], 1);
}

void UART_DeInit()
{
	HAL_UART_AbortReceive_IT(&huart1);
	HAL_UART_DeInit(&huart1);
}

void UART_Transmit(uint8_t *dataP, uint16_t length)
{
	HAL_UART_Transmit_IT(&huart1, dataP, length);
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_8;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
}

bool DeinitPin(Pin_t pin)
{
	HAL_GPIO_DeInit(pin.port, pin.pin);
    return true;
}

bool SetPin(Pin_t pin, SetPin_Out_t out)
{
	switch(out)
	{
		case SetPin_Out_High:
			{
				HAL_GPIO_WritePin(pin.port, pin.pin, GPIO_PIN_SET);
			}
		break;
		case SetPin_Out_Low:
			{
				HAL_GPIO_WritePin(pin.port, pin.pin, GPIO_PIN_RESET);
			}
		break;
		default:
			{
				return false;
			}
	}

    return true;
}

SetPin_Out_t GetPinLevel(Pin_t pin)
{
    switch(HAL_GPIO_ReadPin(pin.port,pin.pin))
    {
		case GPIO_PIN_RESET:
		{
			return SetPin_Out_Low;
		}
		break;
		case GPIO_PIN_SET:
		{
			return SetPin_Out_High;
		}
		break;
		default:
		{
			return SetPin_Out_Low;
		}
		break;
    }
}

void delay(uint16_t sleepFor)
{
    HAL_Delay((uint32_t)sleepFor);
}

/*
 *Request the 3 byte driver version
 *
 *output:
 * -version: pointer to the 3 byte driver version
 *
 *return true if request succeeded
 *       false otherwise
 */
bool GetDriverVersion(uint8_t* version)
{
    uint8_t help[3] = WIRELESS_CONNECTIVITY_SDK_VERSION;
    memcpy(version,help,3);
    return true;
}

#endif // _global_defined
#ifdef __cplusplus
}
#endif
