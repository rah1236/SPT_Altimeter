// bmp280.h
#ifndef BMP280_H
#define BMP280_H

#include "main.h"
#include <stdbool.h>
#include "stm32f1xx_hal.h"

#define BMP280_nCS_Pin GPIO_PIN_4
#define BMP280_nCS_GPIO_Port GPIOA

void BMP280_Init(SPI_HandleTypeDef* hspi);
float BMP280_ReadTemperature(void);
float BMP280_ReadPressure(void);
float BMP280_ReadAltitude(float seaLevelPressure);

#endif // BMP280_H
