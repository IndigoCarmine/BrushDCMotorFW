/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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
#include "can.h"

/* USER CODE BEGIN 0 */
#include "string.h"
#include "config.h"
#include "motor_control.h"
#include "led.h"

extern MotorMode motor_mode;

void CAN_FILTER_Init(void){
	CAN_FilterTypeDef filter;
	filter.FilterIdHigh = 0;
	filter.FilterIdLow = 0;
	filter.FilterMaskIdHigh = 0;
	filter.FilterMaskIdLow = 0;
	filter.FilterScale = CAN_FILTERSCALE_32BIT;
	filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	filter.FilterBank = 0;
	filter.FilterMode = CAN_FILTERMODE_IDMASK;
	filter.SlaveStartFilterBank = 14;
	filter.FilterActivation = ENABLE;
	if (HAL_CAN_ConfigFilter(&hcan, &filter) != HAL_OK)
	{
		Error_Handler();
	}

}
/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 2;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_14TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_3TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */
  CAN_FILTER_Init();

	// 割り込み処�?の開�?
	if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_CAN_Start(&hcan);
  /* USER CODE END CAN_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */


CAN_TxHeaderTypeDef TxHeader ={
		.StdId = BASE_ID + 3,
		.DLC = 1
};
void Send_Status(uint8_t status){
	uint32_t TxMailbox;
    HAL_CAN_AddTxMessage(&hcan, &TxHeader, &status, &TxMailbox);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8];
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
    {
    	led_on(can);
    	switch(RxHeader.StdId){
    	case BASE_ID:
    	{
    		//set target
    		switch(motor_mode){
    		case Current:
    		{
    			memcpy(&currentPID.target,RxData,4);
    			break;
    		}
    		case Position:
    		{
    			memcpy(&positionPID.target,RxData,4);
    			break;
    		}
    		case Homing:
    		{
    			memcpy(&currentPID.target,RxData,4);
    			break;
    		}
    		case Stop:
    		default:
    			break;
    		}
    		break;
    	}
    	case BASE_ID + 1:
		{
			//mode setting
			ChangeMode(motor_mode);
			break;
		}
    	case BASE_ID + 2:
		{
			//parameter setting
			/*
			 * byte: PID parameter index
			 * float: parameter value
			 */
			switch(RxData[0]){
			case 0:{memcpy(&currentPID.PGain,RxData + 1,4);break;}
			case 1:{memcpy(&currentPID.IGain,RxData + 1,4);break;}
			case 2:{memcpy(&currentPID.DGain,RxData + 1,4);break;}
			case 3:{memcpy(&currentPID.max,  RxData + 1,4);break;}

			case 4:{memcpy(&positionPID.PGain,RxData + 1,4);break;}
			case 5:{memcpy(&positionPID.IGain,RxData + 1,4);break;}
			case 6:{memcpy(&positionPID.DGain,RxData + 1,4);break;}
			case 7:{memcpy(&positionPID.max,  RxData + 1,4);break;}
			default:break;
			}
			break;
		}
    	}

    }
}
/* USER CODE END 1 */
