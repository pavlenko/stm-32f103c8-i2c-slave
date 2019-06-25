#include "main.h"

#include "stm32f1xx_it.h"

#include "i2c.h"
#include "led.h"

/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void) {
    /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

    /* USER CODE END NonMaskableInt_IRQn 0 */
    /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

    /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void) {
    while (1) {}
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void) {
    while (1) {}
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void) {
    while (1) {}
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void) {
    while (1) {}
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void) {}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void) {}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void) {}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void) {
    HAL_IncTick();
}

/**
* @brief This function handles I2C1 event interrupt.
*/
void I2C2_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&i2c2);
}

/**
* @brief This function handles I2C1 error interrupt.
*/
void I2C2_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&i2c2);
}
