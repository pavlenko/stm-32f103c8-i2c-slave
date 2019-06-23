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

        /* I2C1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(I2C2_EV_IRQn);
        HAL_NVIC_DisableIRQ(I2C2_ER_IRQn);
    }
}

//TODO state machines for rx & tx
void restartComm(void) {
    HAL_I2C_Slave_Receive_IT(&hi2c1, (uint8_t *)&trans_type, 1);
    i2c_state = RX_TRANS_TYPE;
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c){
    if ((hi2c->Instance == I2C1) && (HAL_I2C_GetState(hi2c) == HAL_I2C_STATE_READY)) {
        switch (i2c_state) {
            case RX_TRANS_TYPE:
                if ((trans_type == MASTER_REQ_READ) || (trans_type == MASTER_REQ_WRITE)) {
                    HAL_I2C_Slave_Receive_IT(hi2c, (uint8_t *)&reg_index, 1);
                    i2c_state = RX_TRANS_ADDR;
                }
                else  {
                    restartComm();
                }
                break;
            case RX_TRANS_ADDR:
                if (reg_index < MASTER_REQ_READ) {
                    p = mem_data;
                    p += reg_index;

                    switch (trans_type) {
                        case MASTER_REQ_READ:
                            re = (int32_t)(*p * 65536);
                            HAL_I2C_Slave_Transmit_IT(hi2c, (uint8_t *)&re, 4);
                            i2c_state = TX_TRANS_DATA;
                            break;
                        case MASTER_REQ_WRITE:
                            HAL_I2C_Slave_Receive_IT(hi2c, (uint8_t *)&re, 4);
                            i2c_state = RX_TRANS_DATA;
                            break;
                    }
                }
                else  {
                    restartComm();
                }
                break;
            case RX_TRANS_DATA:
                *p = re / 65536.0;
                restartComm();
                break;
            default:
                restartComm();
                break;
        }
    }
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c){
    if ((hi2c->Instance == I2C1) && (HAL_I2C_GetState(hi2c) == HAL_I2C_STATE_READY)) {
        switch (i2c_state) {
            case TX_TRANS_DATA:
                restartComm();
                break;
            default:
                restartComm();
                break;
        }
    }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    //while(1);
    restartComm();

}