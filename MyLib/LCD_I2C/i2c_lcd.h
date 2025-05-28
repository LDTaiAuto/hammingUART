#ifndef I2C_LCD_H
#define I2C_LCD_H

#include <stdint.h>

/**
 * @brief Includes the HAL driver present in the project
 */
#ifdef STM32F103xB
    #include "stm32f1xx_hal.h"
#elif defined(STM32C0xx)
    #include "stm32c0xx_hal.h"
#elif defined(STM32G4xx)
    #include "stm32g4xx_hal.h"
#else
    #error "Unsupported STM32 series. Please define the correct STM32 macro."
#endif

/**
 * @brief Structure to hold LCD instance information
 */
typedef struct {
    I2C_HandleTypeDef *hi2c;     // I2C handler for communication
    uint8_t address;            // I2C address of the LCD
} I2C_LCD_HandleTypeDef;

/**
 * @brief Initializes the LCD.
 * @param lcd: Pointer to the LCD handle
 */
void lcd_init(I2C_LCD_HandleTypeDef *lcd);

/**
 * @brief Sends a command to the LCD.
 * @param lcd: Pointer to the LCD handle
 * @param cmd: Command byte to send
 */
void lcd_send_cmd(I2C_LCD_HandleTypeDef *lcd, char cmd);

/**
 * @brief Sends data (character) to the LCD.
 * @param lcd: Pointer to the LCD handle
 * @param data: Data byte to send
 */
void lcd_send_data(I2C_LCD_HandleTypeDef *lcd, char data);

/**
 * @brief Sends a single character to the LCD.
 * @param lcd: Pointer to the LCD handle
 * @param ch: Character to send
 */
void lcd_putchar(I2C_LCD_HandleTypeDef *lcd, char ch);

/**
 * @brief Sends a string to the LCD.
 * @param lcd: Pointer to the LCD handle
 * @param str: Null-terminated string to send
 */
void lcd_puts(I2C_LCD_HandleTypeDef *lcd, char *str);

/**
 * @brief Moves the cursor to a specific position on the LCD.
 * @param lcd: Pointer to the LCD handle
 * @param col: Column number (0-15)
 * @param row: Row number (0 or 1)
 */
void lcd_gotoxy(I2C_LCD_HandleTypeDef *lcd, int col, int row);

/**
 * @brief Clears the LCD display.
 * @param lcd: Pointer to the LCD handle
 */
void lcd_clear(I2C_LCD_HandleTypeDef *lcd);
void lcd_put_int(I2C_LCD_HandleTypeDef *lcd, int num);
void lcd_put_bin(I2C_LCD_HandleTypeDef *lcd, uint8_t num) ;
void lcd_put_hex(I2C_LCD_HandleTypeDef *lcd, uint8_t num) ;
#endif /* I2C_LCD_H */
