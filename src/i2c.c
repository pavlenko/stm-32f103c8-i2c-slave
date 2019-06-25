#include "i2c.h"

I2C_HandleTypeDef i2c2;

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

    if (i2c->Instance == I2C2) {
        /* GPIOA clock enable */
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* I2C2 clock enable */
        __HAL_RCC_I2C2_CLK_ENABLE();

        /**
         * I2C2 GPIO Configuration
         * PB10 ------> I2C2_SCL
         * PB11 ------> I2C2_SDA
         */
        GPIO_InitStruct.Pin   = GPIO_PIN_10|GPIO_PIN_11;
        GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull  = GPIO_PULLUP;
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

void MX_I2C_attachSlaveRXEvent(I2C_HandleTypeDef *i2c, void (*callback) (uint8_t *data, uint8_t size))
{
    if ((NULL == i2c) || (NULL == callback)) {
        return;
    }

    if (i2c->Instance == I2C2) {
        MX_I2C2_slaveRXCallback = callback;
    }

    HAL_I2C_EnableListen_IT(i2c);
}

void MX_I2C_attachSlaveTXEvent(I2C_HandleTypeDef *i2c, void (*callback) (void))
{
    if ((NULL == i2c) || (NULL == callback)) {
        return;
    }

    if (i2c->Instance == I2C2) {
        MX_I2C2_slaveTXCallback = callback;
    }

    HAL_I2C_EnableListen_IT(i2c);
}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *i2c, uint8_t direction, uint16_t address)
{
    if (i2c->Instance != I2C2) {
        return;
    }

    if (address == i2c->Init.OwnAddress1) {
        if (direction == I2C_DIRECTION_RECEIVE) {
            obj->slaveMode = SLAVE_MODE_TRANSMIT;

            if (MX_I2C2_slaveTXCallback != NULL) {
                MX_I2C2_slaveTXCallback();
            }

            HAL_I2C_Slave_Sequential_Transmit_IT(
                    i2c,
                    (uint8_t *) obj->i2cTxRxBuffer,
                    obj->i2cTxRxBufferSize,
                    I2C_LAST_FRAME
            );
        } else {
            obj->slaveRxNbData = 0;
            obj->slaveMode = SLAVE_MODE_RECEIVE;

            HAL_I2C_Slave_Sequential_Receive_IT(
                    i2c,
                    (uint8_t *) &(obj->i2cTxRxBuffer[obj->slaveRxNbData]),
                    1,
                    I2C_NEXT_FRAME
            );
        }
    }
}

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *i2c)
{
    if (i2c->Instance != I2C2) {
        return;
    }

    i2c_t *obj = get_i2c_obj(hi2c);

    if ((MX_I2C2_slaveRXCallback != NULL) && (obj->slaveMode == SLAVE_MODE_RECEIVE)) {
        if (obj->slaveRxNbData != 0) {
            MX_I2C2_slaveRXCallback((uint8_t *) obj->i2cTxRxBuffer, obj->slaveRxNbData);
        }
    }

    obj->slaveMode = SLAVE_MODE_LISTEN;
    obj->slaveRxNbData = 0;

    HAL_I2C_EnableListen_IT(i2c);
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (i2c->Instance != I2C2) {
        return;
    }

    i2c_t *obj = get_i2c_obj(hi2c);

    /* One more byte was received, store it then prepare next */
    if(obj->slaveRxNbData < I2C_TXRX_BUFFER_SIZE) {
        obj->slaveRxNbData++;
    } else {
        core_debug("ERROR: I2C Slave RX overflow\n");
    }

    /* Restart interrupt mode for next Byte */
    if (obj->slaveMode == SLAVE_MODE_RECEIVE) {
        HAL_I2C_Slave_Sequential_Receive_IT(
                hi2c,
                (uint8_t *) &(obj->i2cTxRxBuffer[obj->slaveRxNbData]),
                1,
                I2C_NEXT_FRAME
        );
    }
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *i2c)
{
    if (i2c->Instance != I2C2) {
        return;
    }

    i2c_t *obj = get_i2c_obj(hi2c);
    /* Reset transmit buffer size */
    obj->i2cTxRxBufferSize = 0;
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *i2c)
{
    if (i2c->Instance != I2C2) {
        return;
    }

    HAL_I2C_EnableListen_IT(i2c);
}

void I2C2_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&i2c2);
}

void I2C2_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&i2c2);
}