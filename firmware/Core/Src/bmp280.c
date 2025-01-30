#include "bmp280.h"
#include <math.h>

// Register addresses
#define BMP280_REG_ID          0xD0
#define BMP280_REG_RESET       0xE0
#define BMP280_REG_STATUS      0xF3
#define BMP280_REG_CTRL_MEAS   0xF4
#define BMP280_REG_CONFIG      0xF5
#define BMP280_REG_PRESS       0xF7
#define BMP280_REG_TEMP        0xFA
#define BMP280_REG_CALIB       0x88

#define BMP280_READ_BIT        0x80

typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
} bmp280_calib_t;

static struct {
    SPI_HandleTypeDef* hspi;
    bmp280_calib_t calib;
    int32_t t_fine;
} bmp280;

static void BMP280_WriteReg(uint8_t reg, uint8_t value) {
    HAL_GPIO_WritePin(BMP280_nCS_GPIO_Port, BMP280_nCS_Pin, GPIO_PIN_RESET);
    uint8_t data[2] = {reg & ~BMP280_READ_BIT, value};
    HAL_SPI_Transmit(bmp280.hspi, data, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(BMP280_nCS_GPIO_Port, BMP280_nCS_Pin, GPIO_PIN_SET);
}

static uint8_t BMP280_ReadReg(uint8_t reg) {
    uint8_t rx_data[2] = {0};
    uint8_t tx_data[2] = {reg | BMP280_READ_BIT, 0xFF};  // Use 0xFF as dummy byte to make it obvious
    HAL_StatusTypeDef status;

    // Make sure CS starts high
    HAL_GPIO_WritePin(BMP280_nCS_GPIO_Port, BMP280_nCS_Pin, GPIO_PIN_SET);
    HAL_Delay(1);

    // Assert CS
    HAL_GPIO_WritePin(BMP280_nCS_GPIO_Port, BMP280_nCS_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);

    // Do transfer
    status = HAL_SPI_TransmitReceive(bmp280.hspi, tx_data, rx_data, 2, HAL_MAX_DELAY);

    // Release CS
    HAL_GPIO_WritePin(BMP280_nCS_GPIO_Port, BMP280_nCS_Pin, GPIO_PIN_SET);

    // For debugging - check what's going on
    if(status != HAL_OK) {
        // Handle error or return 0
        return 0;
    }

    return rx_data[1];
}

static void BMP280_ReadRegs(uint8_t reg, uint8_t *buffer, uint8_t length) {
    HAL_GPIO_WritePin(BMP280_nCS_GPIO_Port, BMP280_nCS_Pin, GPIO_PIN_RESET);
    uint8_t tx_data = reg | BMP280_READ_BIT;
    HAL_SPI_Transmit(bmp280.hspi, &tx_data, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(bmp280.hspi, buffer, length, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(BMP280_nCS_GPIO_Port, BMP280_nCS_Pin, GPIO_PIN_SET);
}

void BMP280_Init(SPI_HandleTypeDef* hspi) {
    bmp280.hspi = hspi;

    // Check chip ID
    if (BMP280_ReadReg(BMP280_REG_ID) != 0x58) {
        Error_Handler();
    }

    // Reset device
    BMP280_WriteReg(BMP280_REG_RESET, 0xB6);
    HAL_Delay(10);

    // Read calibration data
    uint8_t calib_data[24];
    BMP280_ReadRegs(BMP280_REG_CALIB, calib_data, 24);

    bmp280.calib.dig_T1 = (calib_data[1] << 8) | calib_data[0];
    bmp280.calib.dig_T2 = (calib_data[3] << 8) | calib_data[2];
    bmp280.calib.dig_T3 = (calib_data[5] << 8) | calib_data[4];
    bmp280.calib.dig_P1 = (calib_data[7] << 8) | calib_data[6];
    bmp280.calib.dig_P2 = (calib_data[9] << 8) | calib_data[8];
    bmp280.calib.dig_P3 = (calib_data[11] << 8) | calib_data[10];
    bmp280.calib.dig_P4 = (calib_data[13] << 8) | calib_data[12];
    bmp280.calib.dig_P5 = (calib_data[15] << 8) | calib_data[14];
    bmp280.calib.dig_P6 = (calib_data[17] << 8) | calib_data[16];
    bmp280.calib.dig_P7 = (calib_data[19] << 8) | calib_data[18];
    bmp280.calib.dig_P8 = (calib_data[21] << 8) | calib_data[20];
    bmp280.calib.dig_P9 = (calib_data[23] << 8) | calib_data[22];

    // Configure sensor
    BMP280_WriteReg(BMP280_REG_CONFIG, 0x0C);  // IIR filter coefficient 2
    BMP280_WriteReg(BMP280_REG_CTRL_MEAS, 0x27);  // Normal mode, oversampling x1
}

float BMP280_ReadTemperature(void) {
    uint8_t data[3];
    BMP280_ReadRegs(BMP280_REG_TEMP, data, 3);

    int32_t adc_T = (data[0] << 16 | data[1] << 8 | data[2]) >> 4;

    int32_t var1 = ((((adc_T >> 3) - ((int32_t)bmp280.calib.dig_T1 << 1))) *
                    ((int32_t)bmp280.calib.dig_T2)) >> 11;

    int32_t var2 = (((((adc_T >> 4) - ((int32_t)bmp280.calib.dig_T1)) *
                      ((adc_T >> 4) - ((int32_t)bmp280.calib.dig_T1))) >> 12) *
                    ((int32_t)bmp280.calib.dig_T3)) >> 14;

    bmp280.t_fine = var1 + var2;
    float T = (bmp280.t_fine * 5 + 128) >> 8;
    return T / 100.0f;
}

float BMP280_ReadPressure(void) {
    uint8_t data[3];
    BMP280_ReadRegs(BMP280_REG_PRESS, data, 3);

    int32_t adc_P = (data[0] << 16 | data[1] << 8 | data[2]) >> 4;

    int64_t var1 = ((int64_t)bmp280.t_fine) - 128000;
    int64_t var2 = var1 * var1 * (int64_t)bmp280.calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)bmp280.calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)bmp280.calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)bmp280.calib.dig_P3) >> 8) +
           ((var1 * (int64_t)bmp280.calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)bmp280.calib.dig_P1) >> 33;

    if (var1 == 0) {
        return 0.0f;
    }

    int64_t p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)bmp280.calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)bmp280.calib.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)bmp280.calib.dig_P7) << 4);
    return (float)p / 256.0f;
}

float BMP280_ReadAltitude(float seaLevelPressure) {
    float pressure = BMP280_ReadPressure();
    return 44330.0f * (1.0f - pow(pressure / seaLevelPressure, 0.1903f));
}
