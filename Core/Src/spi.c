/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "spi.h"

/* USER CODE BEGIN 0 */
#include "assert.h"
#include "led.h"
#define SPI_TIMEOUT 1000
/* USER CODE END 0 */

SPI_HandleTypeDef hspi1;

/* SPI1 init function */
void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */
  HAL_GPIO_WritePin(nSCS_GPIO_Port, nSCS_Pin, GPIO_PIN_SET);
  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */
  __HAL_SPI_ENABLE(&hspi1);
  /* USER CODE END SPI1_Init 2 */

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
uint16_t SPI_Read(uint8_t address){
	//address size assert
	assert(!(address & 0b11000000));
	uint8_t TxData[2];
	TxData[1] = address | 0b01000000;//MSB bit high for read

	uint8_t RxData[2];
	HAL_GPIO_WritePin(nSCS_GPIO_Port, nSCS_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)TxData, (uint8_t*)RxData, 1, SPI_TIMEOUT);
	HAL_Delay(1);
	HAL_GPIO_WritePin(nSCS_GPIO_Port, nSCS_Pin, GPIO_PIN_SET);
	if(status!=HAL_OK)led_on(state2);
	return (((uint16_t)RxData[1]) << 8) + RxData[0];

}
uint16_t SPI_Write(uint8_t address,uint8_t data){
	//address size assert
	assert(!(address & 0b11000000));

	uint8_t TxData[2];
	TxData[1] = address | 0b00000000;
	TxData[0] = data;
	uint8_t RxData[2];

	HAL_GPIO_WritePin(nSCS_GPIO_Port, nSCS_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)TxData, (uint8_t*)RxData, 1, SPI_TIMEOUT);
	HAL_Delay(1);
	HAL_GPIO_WritePin(nSCS_GPIO_Port, nSCS_Pin, GPIO_PIN_SET);
	if(status!=HAL_OK){
		led_on(state2);
	}
	return (((uint16_t)RxData[1]) << 8) + RxData[0];
}
/* USER CODE END 1 */
