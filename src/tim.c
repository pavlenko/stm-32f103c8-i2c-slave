#include "tim.h"

#include <main.h>

TIM_HandleTypeDef tim1;

void MX_TIM1_Init()
{
    TIM_OC_InitTypeDef sConfigOC;
    TIM_MasterConfigTypeDef sMasterConfig;

    tim1.Instance           = TIM1;
    tim1.Init.Prescaler     = (uint32_t) (SystemCoreClock / 1000000) - 1;
    tim1.Init.CounterMode   = TIM_COUNTERMODE_UP;
    tim1.Init.Period        = 20000;
    tim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    if (HAL_TIM_PWM_Init(&tim1) != HAL_OK) {
        Error_Handler();
    }

    sConfigOC.OCMode     = TIM_OCMODE_PWM1;
    sConfigOC.Pulse      = 1500;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    HAL_TIM_PWM_ConfigChannel(&tim1, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(&tim1, &sConfigOC, TIM_CHANNEL_2);
    HAL_TIM_PWM_ConfigChannel(&tim1, &sConfigOC, TIM_CHANNEL_3);
    HAL_TIM_PWM_ConfigChannel(&tim1, &sConfigOC, TIM_CHANNEL_4);

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_ENABLE;

    HAL_TIMEx_MasterConfigSynchronization(&tim1, &sMasterConfig);

    HAL_TIM_PWM_Start(&tim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&tim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&tim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&tim1, TIM_CHANNEL_4);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* timPWM)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    if (timPWM->Instance == TIM1) {
        /* GPIOA clock enable */
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* Peripheral clock enable */
        __HAL_RCC_TIM1_CLK_ENABLE();

        /**
         * TIM1 GPIO Configuration
         * PA8  --> TIM1_CH1
         * PA9  --> TIM1_CH2
         * PA10 --> TIM1_CH3
         * PA11 --> TIM1_CH4
         */
        GPIO_InitStruct.Pin   = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
        GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* timPWM)
{
    if (timPWM->Instance == TIM1) {
        /* Peripheral clock disable */
        __HAL_RCC_TIM1_CLK_DISABLE();

        /**
         * TIM1 GPIO Configuration
         * PA8  --> TIM1_CH1
         * PA9  --> TIM1_CH2
         * PA10 --> TIM1_CH3
         * PA11 --> TIM1_CH4
         */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11);
    }
}