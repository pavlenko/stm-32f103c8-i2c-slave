#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

/* Definition for I2Cx's NVIC */
/* WARNING: for stm32f103c8t6 single handler used for both I2C1 and I2C2 */
#define I2Cx_EV_IRQHandler I2C1_EV_IRQHandler
#define I2Cx_ER_IRQHandler I2C1_ER_IRQHandler

void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

#ifdef __cplusplus
}
#endif

#endif //__MAIN_H
