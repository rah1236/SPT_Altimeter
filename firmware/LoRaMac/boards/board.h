#ifndef __BOARD_H__
#define __BOARD_H__

#include <stdbool.h>
#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"

// GPIO pin structure
typedef struct
{
    GPIO_TypeDef*       port;
    uint16_t           pin;
} Gpio_t;

// SPI structure
typedef struct
{
    SPI_HandleTypeDef* hspi;
    Gpio_t            nss;
} Spi_t;

// Define your SX1276 pins here - modify these according to your wiring
#define RADIO_RESET_PORT                          GPIOA
#define RADIO_RESET_PIN                           GPIO_PIN_12  // Change as needed

#define RADIO_MOSI_PORT                           GPIOA
#define RADIO_MOSI_PIN                            GPIO_PIN_7  // SPI1 MOSI pin

#define RADIO_MISO_PORT                           GPIOA
#define RADIO_MISO_PIN                            GPIO_PIN_6  // SPI1 MISO pin

#define RADIO_SCLK_PORT                           GPIOA
#define RADIO_SCLK_PIN                            GPIO_PIN_5  // SPI1 SCK pin

#define RADIO_NSS_PORT                            GPIOA
#define RADIO_NSS_PIN                             GPIO_PIN_8  // SPI1 NSS pin

// DIO pins
#define RADIO_DIO_0_PORT                          GPIOB
#define RADIO_DIO_0_PIN                           GPIO_PIN_1  // Change as needed

#define RADIO_DIO_1_PORT                          GPIOB
#define RADIO_DIO_1_PIN                           GPIO_PIN_0  // Change as needed

#define RADIO_DIO_2_PORT                          GPIOA
#define RADIO_DIO_2_PIN                           GPIO_PIN_11  // Change as needed
// Add more DIO pins if needed

// SPI definition
#define RADIO_SPI                                 SPI1        // Change if using different SPI

// Board functions
void BoardInitMcu(void);
void BoardInitPeriph(void);
void BoardDeInitMcu(void);
uint32_t BoardGetRandomSeed(void);
void BoardGetUniqueId(uint8_t *id);

// GPIO Functions
void GpioInit(Gpio_t *obj, GPIO_TypeDef* port, uint16_t pin, uint32_t mode, uint32_t pull);
void GpioWrite(Gpio_t *obj, uint32_t value);
uint32_t GpioRead(Gpio_t *obj);

// SPI Functions
void SpiInit(Spi_t *obj, SPI_HandleTypeDef* hspi, Gpio_t *nss);
uint16_t SpiInOut(Spi_t *obj, uint16_t outData);

#endif // __BOARD_H__
