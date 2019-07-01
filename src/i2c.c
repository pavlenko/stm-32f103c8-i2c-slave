#include "i2c.h"

#include <stdio.h>
#include <string.h>

#include "led.h"

I2C_HandleTypeDef i2c1;
I2C_HandleTypeDef i2c2;

enum {
    I2C_STATUS_READY    = 0,
    I2C_STATUS_LISTEN   = 1,
    I2C_STATUS_BUSY     = 2,
    I2C_STATUS_COMPLETE = 3,
} I2C_status_t;

uint8_t I2C2_status;
uint8_t I2C2_rxBufferData[I2C_RX_BUFFER_MAX];
uint8_t I2C2_rxBufferSize;
uint8_t I2C2_txBufferData[I2C_RX_BUFFER_MAX];
uint8_t I2C2_txBufferSize;

uint8_t I2C2_responseIndex;
const char* I2C2_responseValue[] = {
        "STM32F103xx",
        "1.2.3"
};

uint8_t *I2C2_responseData;
uint8_t I2C2_responseSize;

void MX_I2C1_Init()
{
    i2c1.Instance             = I2C1;
    i2c1.Init.ClockSpeed      = 400000;
    i2c1.Init.DutyCycle       = I2C_DUTYCYCLE_2;
    i2c1.Init.OwnAddress1     = I2C2_OWN_ADDRESS_1;
    i2c1.Init.OwnAddress2     = I2C2_OWN_ADDRESS_2;
    i2c1.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    i2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    i2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    i2c1.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&i2c1) != HAL_OK) {
        Error_Handler();
    }
}

void MX_I2C2_Init()
{
    i2c2.Instance             = I2C2;
    i2c2.Init.ClockSpeed      = 400000;
    i2c2.Init.DutyCycle       = I2C_DUTYCYCLE_2;
    i2c2.Init.OwnAddress1     = I2C2_OWN_ADDRESS_1;
    i2c2.Init.OwnAddress2     = I2C2_OWN_ADDRESS_2;
    i2c2.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    i2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    i2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    i2c2.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&i2c2) != HAL_OK) {
        Error_Handler();
    }
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2c)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    if (i2c->Instance == I2C1) {
        /* GPIOB clock enable */
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /* I2C1 clock enable */
        __HAL_RCC_I2C2_CLK_ENABLE();

        /**
         * I2C1 GPIO Configuration
         * PB6 ------> I2C1_SCL
         * PB7 ------> I2C1_SDA
         */
        GPIO_InitStruct.Pin   = GPIO_PIN_6|GPIO_PIN_7;
        GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* I2C1 interrupt Init */
        HAL_NVIC_SetPriority(I2C1_EV_IRQn, 1, 1);
        HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);

        HAL_NVIC_SetPriority(I2C1_ER_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
    }

    if (i2c->Instance == I2C2) {
        /* GPIOB clock enable */
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /* I2C2 clock enable */
        __HAL_RCC_I2C2_CLK_ENABLE();

        /**
         * I2C2 GPIO Configuration
         * PB10 ------> I2C2_SCL
         * PB11 ------> I2C2_SDA
         */
        GPIO_InitStruct.Pin   = GPIO_PIN_10|GPIO_PIN_11;
        GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* I2C2 interrupt Init */
        HAL_NVIC_SetPriority(I2C2_EV_IRQn, 1, 1);
        HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);

        HAL_NVIC_SetPriority(I2C2_ER_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);
    }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2c)
{
    if (i2c->Instance == I2C1) {
        /* I2C2 clock disable */
        __HAL_RCC_I2C1_CLK_DISABLE();

        /**
         * I2C1 GPIO Configuration
         * PB6 ------> I2C1_SCL
         * PB7 ------> I2C1_SDA
         */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);

        /* I2C1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
        HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
    }

    if (i2c->Instance == I2C2) {
        /* I2C2 clock disable */
        __HAL_RCC_I2C2_CLK_DISABLE();

        /**
         * I2C2 GPIO Configuration
         * PB10 ------> I2C2_SCL
         * PB11 ------> I2C2_SDA
         */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

        /* I2C2 interrupt Deinit */
        HAL_NVIC_DisableIRQ(I2C2_EV_IRQn);
        HAL_NVIC_DisableIRQ(I2C2_ER_IRQn);
    }
}

void MX_I2C2_Dispatch()
{
    if (I2C2_status == I2C_STATUS_READY) {
        if (HAL_I2C_EnableListen_IT(&i2c2) != HAL_OK) {
            Error_Handler();
        }

        I2C2_status = I2C_STATUS_LISTEN;
    }

    if (I2C2_status == I2C_STATUS_COMPLETE) {
        I2C2_rxBufferSize = 0;
        I2C2_txBufferSize = 0;

        I2C2_status = I2C_STATUS_READY;

        LED(LED_OFF);
    }
}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *i2c, uint8_t direction, uint16_t address)
{
    if (i2c->Instance == I2C2) {
        if (address == i2c->Init.OwnAddress1) {
            I2C2_status = I2C_STATUS_BUSY;

            // First of all, check the transfer direction to call the correct Slave Interface
            if (direction == I2C_DIRECTION_TRANSMIT) {
                if (HAL_I2C_Slave_Sequential_Receive_IT(i2c, &I2C2_rxBufferData[I2C2_rxBufferSize], 1, I2C_FIRST_FRAME) != HAL_OK) {
                    Error_Handler();
                }

                I2C2_rxBufferSize++;
            } else {
                I2C2_responseData = (uint8_t*) (I2C2_responseValue[I2C2_responseIndex]);
                I2C2_responseSize = strlen((char *) (I2C2_responseValue[I2C2_responseIndex]));

                if (HAL_I2C_Slave_Sequential_Transmit_IT(i2c, I2C2_responseData, I2C2_responseSize, I2C_LAST_FRAME) != HAL_OK) {
                    Error_Handler();
                }
            }
        }
    }
}

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *i2c)
{
    if (i2c->Instance == I2C2) {
        I2C2_status = I2C_STATUS_COMPLETE;
    }
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *i2c)
{
    if (i2c->Instance == I2C2) {
        LED(LED_ON);

        if (I2C2_rxBufferData[0] == 0x00) {
            I2C2_responseIndex = 0;
        } else if (I2C2_rxBufferData[0] == 0x01) {
            I2C2_responseIndex = 1;
        } else {
            if (HAL_I2C_Slave_Sequential_Receive_IT(i2c, &I2C2_rxBufferData[I2C2_rxBufferSize], 1, I2C_FIRST_FRAME) != HAL_OK) {
                Error_Handler();
            }

            I2C2_rxBufferSize++;
        }
    }
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *i2c)
{
    if (i2c->Instance == I2C2) {
        LED(LED_OFF);
    }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *i2c)
{
    if (i2c->Instance == I2C2) {
        if (HAL_I2C_GetError(i2c) != HAL_I2C_ERROR_AF) {
            Error_Handler();
        }

        I2C2_status = I2C_STATUS_COMPLETE;
    }
}