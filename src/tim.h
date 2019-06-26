#ifndef __TIM_H
#define __TIM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stm32f1xx_hal.h>

extern TIM_HandleTypeDef tim1;

void MX_TIM1_Init();

#ifdef __cplusplus
}
#endif

#endif //__TIM_H
