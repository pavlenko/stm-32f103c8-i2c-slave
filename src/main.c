#include "main.h"

#include "i2c.h"
#include "led.h"
#include "tim.h"

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config();

enum {
    COMMAND_NONE  = 0,
    COMMAND_R_REG = 1,
    COMMAND_W_REG = 2,
};

uint8_t command;
uint16_t micros = 0;

void I2C2_slaveRXCallback(uint8_t *data, uint8_t size)
{
    if (*data == 0x00) {
        command = COMMAND_R_REG;
        //TODO other make in tx callback
        uint8_t response[2];

        response[0] = (uint8_t) (micros >> 8u);
        response[1] = (uint8_t) (micros & 0xFFu);

        MX_I2C_slaveTransmit(&i2c2_api, response, 2);
    }

    if (*data == 0x20) {
        command = COMMAND_W_REG;
        micros  = (*(data + 1) << 8) | *(data + 2);
        command = COMMAND_NONE;
    }

    //__NOP();
    //(void) data;
    //(void) size;
}

void I2C2_slaveTXCallback(void)
{
    /*if (command == COMMAND_R_REG) {
        uint8_t response[2];

        response[0] = (uint8_t) (micros >> 8u);
        response[1] = (uint8_t) (micros & 0xFFu);

        MX_I2C_slaveTransmit(&i2c2_api, response, 2);

        command = COMMAND_NONE;
    }*/
    //__NOP();
    uint8_t test[] = "ABC";
    MX_I2C_slaveTransmit(&i2c2_api, test, 3);
}

int main() {
    HAL_Init();

    SystemClock_Config();

    MX_LED_Init();
    MX_I2C2_Init();
    MX_TIM1_Init();

    if (HAL_I2C_EnableListen_IT(&i2c2) != HAL_OK) {
        /* Transfer error in reception process */
        Error_Handler();
    }

    MX_I2C_attachSlaveRXEvent(&i2c2_api, I2C2_slaveRXCallback);
    MX_I2C_attachSlaveTXEvent(&i2c2_api, I2C2_slaveTXCallback);

    while (1) {
        LED(LED_ON);
        HAL_Delay(100);
        LED(LED_OFF);
        HAL_Delay(900);
    }
}

void SystemClock_Config() {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Initializes the CPU clock source
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL9;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    // Initializes the CPU, AHB and APB buses clocks
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
    /* User can add his own implementation to report the HAL error return state */
    while (1) {
        // Predefined blink to inform about error
        LED(LED_ON);
        HAL_Delay(100);

        LED(LED_OFF);
        HAL_Delay(100);

        LED(LED_ON);
        HAL_Delay(100);

        LED(LED_OFF);
        HAL_Delay(100);

        LED(LED_ON);
        HAL_Delay(100);

        LED(LED_OFF);
        HAL_Delay(1500);
    }
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif
