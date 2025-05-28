/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
	#include "i2c_lcd.h"
	#include "stdio.h" 
	#include "stdlib.h"
	#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
	I2C_LCD_HandleTypeDef lcd1 ; 
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
		// khai bao bien
			char inStr[50] ; 
			char inBit[50] ; 
		// ma ran phim 4x4 
		char keyPad[4][4]= {
			{'1', '2', '3', 'A'},
			{'4', '5', '6', 'B'},
			{'7', '8', '9', 'C'},
			{'*', '0', '#', 'D'},
		}; 
		
		// khai bao hang va cot ;
		uint8_t rowsPin[4] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3} ;
		uint8_t colsPin[4] = {GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7} ; 
		
		// dinh nghia port hang va cot 
			GPIO_TypeDef* row_port = GPIOA;
			GPIO_TypeDef* col_port = GPIOA;
		
		// quet phim 
		char scanKeypad() {
    for (int row = 0; row < 4; row++) {
        // All set rows 
        for (int i = 0; i < 4; i++) {
            HAL_GPIO_WritePin(row_port, rowsPin[i], GPIO_PIN_SET);
        }

        //  Pull current row LOW
        HAL_GPIO_WritePin(row_port, rowsPin[row], GPIO_PIN_RESET);

        // Scan columns
        for (int col = 0; col < 4; col++) {
            if (HAL_GPIO_ReadPin(col_port, colsPin[col]) == GPIO_PIN_RESET) {
                HAL_Delay(20);  // Debounce delay
                if (HAL_GPIO_ReadPin(col_port, colsPin[col]) == GPIO_PIN_RESET) {
                    return keyPad[row][col];
                }
            }
        }
			}
    return '\0';
	}
		
		// hien thi cac chuc nang : RX- Truyen , RX - Nhan, EXIT - Thoat
		void showMenu(){
				lcd_clear(&lcd1);
				lcd_gotoxy(&lcd1 , 2, 0) ; 
				lcd_puts(&lcd1 , "Chose Option") ; 
				lcd_gotoxy(&lcd1 , 0, 1) ; 
				lcd_puts(&lcd1 , "1.TX") ; 
				lcd_gotoxy(&lcd1 , 5, 1) ;
				lcd_puts(&lcd1 , "2.RX") ;
				lcd_gotoxy(&lcd1 , 10, 1) ;
				lcd_puts(&lcd1 , "3.Exit") ; 
		}
		
		// su dung kieu du lieu Enum de luu trang thai nut nhan chon chuc nang 
		typedef enum{
			btnNone = 0 ,
			btnRX , 
			btnTX , 
			btnEXIT
		}btnOption;
		
		btnOption BTNOption ; 
		// ham luu trang thai nut nhan 
		void stateBtn(){
			if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET ){ 
				HAL_Delay(20) ; // chong doi phim 
				// nut nhan TX duoc nhan 
				if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET ){ 
						BTNOption = btnTX ; 
				}
				else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET){
						BTNOption = btnRX ; 
				}
				else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET )
						BTNOption = btnEXIT ; 
			}
		}
	
		// Ham nhap chuoi ki tu phim 4x4  
		void inString(char dStr[], int n) {
				uint8_t index = 0;
				char key;

				while (1) {
						key = scanKeypad();  

						if (key != '\0') { 
								if (key == '#') {
										dStr[index] = '\0';
										break;
								} 
								else if (key == '*') {
										if (index > 0) {
												index--;
												dStr[index] = '\0';

												lcd_gotoxy(&lcd1, index, 1);
												lcd_puts(&lcd1, " ");
												lcd_gotoxy(&lcd1, index,1) ;
										}
								} 
								else if (index < n - 1) {
										dStr[index++] = key;
										dStr[index] = '\0';  
										lcd_putchar(&lcd1, key);
								}
								HAL_Delay(200);  
						}
				}
		}
		
		// Ham ma hoa hamming 
		uint16_t hamEncode(uint8_t data){ 
				uint8_t d[8] ; 
				// d1-d8 : LSB - MSB 
				for ( int i = 0 ; i < 8 ; i++){
					d[i] = (data >> i) & 1 ; 
				}
			 
				// tinh bit kiem tra p1, p2, p4, p8
				uint8_t p1, p2, p4, p8 ; 
				p1 = d[0] ^ d[1] ^ d[3] ^ d[4] ^ d[6]	; 
				p2 = d[0] ^ d[2] ^ d[3] ^ d[5] ^ d[6] ; 
				p4 = d[1] ^ d[2] ^ d[3] ^ d[7] ; 
				p8 = d[4] ^ d[5] ^ d[6] ^ d[7] ; 
				
				// Chen cac bit vao dung theo kieu ma hoa hamming 
				uint16_t codeHam ; 
				codeHam |= (p1 << 0)	 		; 
				codeHam |= (p2 << 1) 			; 
				codeHam |= (d[0] << 2) 		; 
				codeHam |= (p4 << 3)  		; 
				codeHam |= (d[1] << 4)		;
				codeHam |= (d[2] << 5)  	;
				codeHam |= (d[3] << 6) 		; 
				codeHam |= (p8 << 7) 			;
				codeHam |= (d[4] << 8)		;
				codeHam |= (d[5] << 9)  	;
				codeHam |= (d[6] << 10)   ; 
				codeHam |= (d[7] << 11)   ; 
				
				return codeHam ; 
 		}
		
		// Ham giai ma hamming 
		uint8_t hamDecode( uint16_t code){
				uint8_t bits[13] ; 
				for( int i = 1 ; i <= 12 ; i++){
					bits[i] = (code >>(i-1)) & 1 ; 		//bits[1] tuong  ung p1 
				}
				
				uint8_t s1, s2, s4, s8 ; 
				s1 = bits[1] ^ bits[3] ^ bits[5] ^ bits[7] ^ bits[9] ^ bits[11] ; 
				s2 = bits[2] ^ bits[3] ^ bits[6] ^ bits[7] ^ bits[10] ^ bits[11]; 
				s4 = bits[4] ^ bits[5] ^ bits[6] ^ bits[7] ^ bits[12];
				s8 = bits[8] ^ bits[9] ^ bits[10] ^ bits[11] ^ bits[12]; 
				
				uint8_t errorPos  ; 
					errorPos = (s8 << 3) | (s4 << 2) | (s2  << 1 ) | (s1 << 0 ) ; 
				
				if (errorPos != 0 && errorPos <= 12){
						bits[errorPos] ^= 1 ; 
				}
				
				uint8_t data = 0  ; 
				
				data |= ( bits[3] << 0 ) ; 
				data |= ( bits[5] << 1 ) ; 
				data |= ( bits[6] << 2 ) ; 
				data |= ( bits[7] << 3 ) ; 
				data |= ( bits[9] << 4 ) ; 
				data |= ( bits[10] << 5 ) ; 
				data |= ( bits[11] << 6 ) ; 
				data |= ( bits[12] << 7 ) ; 
				
				return data ; 
		}
		
	uint16_t flipBit(uint16_t codeHam, int bitPos) {
    if (bitPos >= 0 && bitPos <= 15) {
        codeHam ^= (1 << bitPos);
    }
    return codeHam;
	}

		//Ham xu ly thoat chuong trinh
		void exitHandle(){
			showMenu(); 
		}
		// UART TX 
			void uartSendString(UART_HandleTypeDef *huart, char *str) {
				HAL_UART_Transmit(huart, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
		}
			
		
		void uartReceiveString(UART_HandleTypeDef *huart, char *buffer, uint16_t maxLength) {
    uint8_t byte;
    uint16_t index = 0;

    while (1) {
        // Nhận 1 byte
        HAL_UART_Receive(huart, &byte, 1, HAL_MAX_DELAY);

        // Nếu gặp ký tự kết thúc chuỗi
        if (byte == '\r' || byte == '\n') {
            break;
        }

        // Lưu vào buffer nếu chưa đầy
        if (index < maxLength - 1) {
            buffer[index++] = byte;
        } else {
            break; // tránh tràn bộ đệm
        }
    }

    buffer[index] = '\0'; // Kết thúc chuỗi bằng NULL
}

		// Ham xu ly truyen du lieu  
		void txHandle(){
		
			while(1){
				
			// Nhap du lieu can ma hoa hamming 
			char buffer_inStr[50] ;
			lcd_clear(&lcd1);  
			lcd_puts(&lcd1, "Input Data:") ;
			lcd_gotoxy(&lcd1, 0, 1);
			inString(inStr, sizeof(inStr));
			int inInt = atoi(inStr) ; 
			sprintf(buffer_inStr, "Input Data:%04X\r\n", inInt) ; 
			
			uartSendString(&huart1, buffer_inStr);
			HAL_Delay(100) ; 
			
			//Chon vi tri dao bit 
			char buffer_bitPos[3] ; 
			lcd_clear(&lcd1);
			lcd_puts(&lcd1, "Nhap bit:");     
			lcd_gotoxy(&lcd1, 0, 1); 
			HAL_Delay(500) ; 
			inString(inBit, sizeof(inBit)) ; 	
			int bitPos = atoi(inBit) ;  
			sprintf(buffer_bitPos,"Bit Pos: %d\r\n", bitPos); 
			
			uartSendString(&huart1, buffer_bitPos);
			HAL_Delay(100) ; 		
			
			// Ma hoa hamming 
			char buffer_hamEncode[50] ; 
			uint16_t hamData = hamEncode(inInt);
			sprintf(buffer_hamEncode, "Before: %04X\r\n", hamData);
			
			uartSendString(&huart1, buffer_hamEncode);
			HAL_Delay(100) ;
			
			
			// Ma hamming sau khi dao bit 
			char buffer_hamDataFliped[50] ; 
			uint16_t hamDataFliped = flipBit(hamData, bitPos) ;
			sprintf(buffer_hamDataFliped, "After: %04X \r\n", hamDataFliped); 
			
			uartSendString(&huart1, buffer_hamDataFliped); 
			HAL_Delay(100) ;
			
			// Giai ma hamming tra ve du lieu goc 
			char buffer_deData[50] ; 
			uint16_t hamdeData = hamDecode(hamDataFliped);
			sprintf(buffer_deData, "Inputed: %04X", hamdeData);
			
			uartSendString(&huart1, buffer_deData);
			HAL_Delay(100) ;
			
			
			lcd_clear(&lcd1);     
			lcd_gotoxy(&lcd1, 0, 0);         
			lcd_puts(&lcd1, buffer_hamEncode); 
    
			lcd_gotoxy(&lcd1, 0, 1);          
			lcd_puts(&lcd1, buffer_hamDataFliped);
			
			
			HAL_Delay(2000) ;
			
			lcd_clear(&lcd1); 
    
			lcd_gotoxy(&lcd1, 0, 0);         
			lcd_puts(&lcd1, buffer_deData); 
			
			HAL_Delay(2000) ; 
			
		}
	}
		
		// Ham xu ly nhan du lieu  
		void rxHandle(){
			lcd_clear(&lcd1);

			uartReceiveString(&huart1, rxBuffer, sizeof(rxBuffer));
			lcd_gotoxy(&lcd1, 0, 0);
			lcd_puts(&lcd1, rxBuffer);
			HAL_Delay(2000) ; 
}


 
		
		
		
		// chuyen den chuc nang tuong ung cua nut nhan 
		void btnHandleOption(){
			switch(BTNOption){
				case btnEXIT: 
					exitHandle(); 
					BTNOption = btnNone ; 
					break ; 
				case btnTX: 
					txHandle(); 
					break;
				case btnRX: 
					rxHandle(); 
					break ; 
				default:
					break ; 
			}
		}
	
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
		
		lcd1.hi2c = &hi2c1;
    lcd1.address = 0x4E   ;
    lcd_init(&lcd1); 
		
		showMenu(); 
		
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {		
		
				stateBtn();
				btnHandleOption(); 



		

//			uint16_t hamData = hamEncode(inInt); // Mã hóa Hamming

//			char buffer[10];
//			sprintf(buffer, "%04X", hamData); // Mã Hamming ban đầu (hex 4 chữ số)

//			uint16_t hamDataFliped = flipBit(hamData, bitPos) ;
//		
//			char buffer1[10];
//			sprintf(buffer1, "%04X", hamDataFliped); // Mã Hamming sau khi đảo
//			
//			uint8_t deData = hamDecode(hamDataFliped); 
//			char buffer2[10] ; 
//			sprintf(buffer2,"%04X", deData) ; 
//			
 
//			
//			lcd_clear(&lcd1); 
//			lcd_puts(&lcd1, "Before:");       // Hiển thị trước khi đảo
//			lcd_gotoxy(&lcd1, 8, 0);          // In ra ở cột 8 dòng 0
//			lcd_puts(&lcd1, buf);
//			lcd_gotoxy(&lcd1, 0, 1); 
//			lcd_puts(&lcd1, "Decode:");
//			lcd_gotoxy(&lcd1, 8, 1);  
//			lcd_puts(&lcd1, buffer2); 
//			HAL_Delay(10000);                 // Chờ 10 giây trước khi xóa

		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 PA2 PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA4 PA5 PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : btnRX_Pin btnTX_Pin btnEXIT_Pin */
  GPIO_InitStruct.Pin = btnRX_Pin|btnTX_Pin|btnEXIT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
