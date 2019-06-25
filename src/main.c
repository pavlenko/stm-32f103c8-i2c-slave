#include "main.h"

#include "i2c.h"
#include "led.h"

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config();

void I2C2_slaveRXCallback(uint8_t *data, uint8_t size)
{
    __NOP();
    (void) data;
    (void) size;
}

void I2C2_slaveTXCallback(void)
{
    __NOP();
    MX_I2C_slaveTransmit(&i2c2_api, (uint8_t *) "ABC", 3);
}

int main() {
    HAL_Init();

    SystemClock_Config();

    MX_LED_Init();
    MX_I2C2_Init();

    if (HAL_I2C_EnableListen_IT(&i2c2) != HAL_OK) {
        /* Transfer error in reception process */
        Error_Handler();
    }

    MX_I2C_attachSlaveRXEvent(&i2c2_api, I2C2_slaveRXCallback);
    MX_I2C_attachSlaveTXEvent(&i2c2_api, I2C2_slaveTXCallback);

    while (1) {
        /*LED(LED_ON);
        HAL_Delay(100);
        LED(LED_OFF);*/
        HAL_Delay(100);
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

#define RX_BUFFER_SIZE 2

uint8_t aRxBuffer[RX_BUFFER_SIZE];

/**
  * @brief  Slave Address Match callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  TransferDirection: Master request Transfer Direction (Write/Read), value of @ref I2C_XferOptions_definition
  * @param  AddrMatchCode: Address Match Code
  * @retval None
  */
void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
    /* A new communication with a Master is initiated */
    /* Turn LED2 On: A Communication is initiated */
    if (I2C2_OWN_ADDRESS_1 == AddrMatchCode) {
        LED(LED_ON);

        if (HAL_I2C_Slave_Sequential_Receive_IT(hi2c, (uint8_t *) aRxBuffer, RX_BUFFER_SIZE, I2C_FIRST_FRAME) != HAL_OK) {
            /* Transfer error in reception process */
            Error_Handler();
            //LED(LED_OFF);
            return;
        }
    }

    (void) hi2c;
}

/**
  * @brief  Listen Complete callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
    /* Turn LED2 off: Communication is completed */
    //LED(LED_ON);
    (void) hi2c;
}

/**
  * @brief  I2C error callbacks.
  * @param  I2cHandle: I2C handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *i2c)
{
    /** Error_Handler() function is called when error occurs.
    * 1- When Slave don't acknowledge it's address, Master restarts communication.
    * 2- When Master don't acknowledge the last data transferred, Slave don't care in this example.
    */
    //LED(LED_ON);
    if (HAL_I2C_GetError(i2c) != HAL_I2C_ERROR_AF) {
        Error_Handler();
        return;
    }

    HAL_I2C_DisableListen_IT(i2c);

    // Re-start listen
    if (HAL_I2C_EnableListen_IT(i2c) != HAL_OK) {
        /* Transfer error in reception process */
        LED(LED_OFF);
        Error_Handler();
        return;
    }

    LED(LED_OFF);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
    LED(LED_ON);
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    while(1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
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
