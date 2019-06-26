#include "i2c.h"

I2C_HandleTypeDef i2c1;
I2C_HandleTypeDef i2c2;
I2C_API_t i2c2_api;

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

    i2c2_api.handle = &i2c2;
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
        __HAL_RCC_GPIOB_CLK_ENABLE();//TODO <-- maybe bug be here, fix GPIOA -> GPIOB

        /* I2C2 clock enable */
        __HAL_RCC_I2C2_CLK_ENABLE();

        /**
         * I2C2 GPIO Configuration
         * PB10 ------> I2C2_SCL
         * PB11 ------> I2C2_SDA
         */
        GPIO_InitStruct.Pin   = GPIO_PIN_10|GPIO_PIN_11;
        GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD;
        //GPIO_InitStruct.Pull  = GPIO_PULLUP;//TODO try to check with no pull
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

void MX_I2C_attachSlaveRXEvent(I2C_API_t *i2c, void (*callback) (uint8_t *, uint8_t))
{
    if ((NULL == i2c) || (NULL == callback)) {
        return;
    }

    i2c->slaveRXCallback = callback;

    HAL_I2C_EnableListen_IT(i2c->handle);
}

void MX_I2C_attachSlaveTXEvent(I2C_API_t *i2c, void (*callback) (void))
{
    if ((NULL == i2c) || (NULL == callback)) {
        return;
    }

    i2c->slaveTXCallback = callback;

    HAL_I2C_EnableListen_IT(i2c->handle);
}

HAL_StatusTypeDef MX_I2C_slaveTransmit(I2C_API_t *i2c, uint8_t *data, uint8_t size)
{
    uint8_t i = 0;

    // Protection to not override the TxBuffer
    if (size > I2C_TX_BUFFER_MAX) {
        return HAL_ERROR;
    }

    // Check the communication status
    for (i = 0; i < size; i++) {
        i2c->txBufferData[i] = *(data + i);
    }

    i2c->txBufferSize = size;

    return HAL_OK;
}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *i2c, uint8_t direction, uint16_t address)
{
    if (i2c->Instance == I2C2) {
        if (address == i2c->Init.OwnAddress1) {
            if (direction == I2C_DIRECTION_RECEIVE) {
                i2c2_api.slaveMode = I2C_SLAVE_MODE_TRANSMIT;

                if (i2c2_api.slaveTXCallback != NULL) {
                    i2c2_api.slaveTXCallback();
                }

                HAL_I2C_Slave_Sequential_Transmit_IT(
                        i2c,
                        (uint8_t *) i2c2_api.txBufferData,
                        i2c2_api.txBufferSize,
                        I2C_LAST_FRAME
                );
            } else {
                i2c2_api.slaveMode    = I2C_SLAVE_MODE_RECEIVE;
                i2c2_api.rxBufferSize = 0;

                HAL_I2C_Slave_Sequential_Receive_IT(
                        i2c,
                        (uint8_t *) &(i2c2_api.rxBufferData[i2c2_api.rxBufferSize]),
                        1,
                        I2C_NEXT_FRAME
                );
            }
        }
    }
}

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *i2c)
{
    if (i2c->Instance == I2C2) {
        if ((i2c2_api.slaveRXCallback != NULL) && (i2c2_api.slaveMode == I2C_SLAVE_MODE_RECEIVE)) {
            if (i2c2_api.rxBufferSize != 0) {
                i2c2_api.slaveRXCallback((uint8_t *) i2c2_api.rxBufferData, i2c2_api.rxBufferSize);
            }
        }

        i2c2_api.slaveMode    = I2C_SLAVE_MODE_LISTEN;
        i2c2_api.rxBufferSize = 0;

        HAL_I2C_EnableListen_IT(i2c);
    }
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *i2c)
{
    if (i2c->Instance == I2C2) {
        /* One more byte was received, store it then prepare next */
        if (i2c2_api.rxBufferSize < I2C_RX_BUFFER_MAX) {
            i2c2_api.rxBufferSize++;
        } else {
            Error_Handler(); // ERROR: I2C Slave RX overflow
        }

        /* Restart interrupt mode for next Byte */
        if (i2c2_api.slaveMode == I2C_SLAVE_MODE_RECEIVE) {
            HAL_I2C_Slave_Sequential_Receive_IT(
                    i2c,
                    (uint8_t *) &(i2c2_api.rxBufferData[i2c2_api.rxBufferSize]),
                    1,
                    I2C_NEXT_FRAME
            );
        }
    }
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *i2c)
{
    if (i2c->Instance == I2C2) {
        /* Reset transmit buffer size */
        i2c2_api.txBufferSize = 0;
    }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *i2c)
{
    if (i2c->Instance == I2C2) {
        HAL_I2C_EnableListen_IT(i2c);
    }
}