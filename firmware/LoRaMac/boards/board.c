#include "board.h"

void BoardInitMcu(void)
{
    // MCU is already initialized by CubeMX
}

void BoardInitPeriph(void)
{
    // Initialize peripherals if needed
}

void BoardDeInitMcu(void)
{
    // De-initialization code
}

uint32_t BoardGetRandomSeed(void)
{
    return HAL_GetTick(); // Simple implementation
}

void BoardGetUniqueId(uint8_t *id)
{
    // Read STM32 unique ID
    uint32_t *uniqueId = (uint32_t *)0x1FFFF7E8;  // STM32F1 unique ID address
    id[0] = (uniqueId[0] >> 24) & 0xFF;
    id[1] = (uniqueId[0] >> 16) & 0xFF;
    id[2] = (uniqueId[0] >> 8) & 0xFF;
    id[3] = uniqueId[0] & 0xFF;
}

void GpioInit(Gpio_t *obj, GPIO_TypeDef* port, uint16_t pin, uint32_t mode, uint32_t pull)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    obj->port = port;
    obj->pin = pin;

    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Pull = pull;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

void GpioWrite(Gpio_t *obj, uint32_t value)
{
    HAL_GPIO_WritePin(obj->port, obj->pin, (GPIO_PinState)value);
}

uint32_t GpioRead(Gpio_t *obj)
{
    return HAL_GPIO_ReadPin(obj->port, obj->pin);
}

void SpiInit(Spi_t *obj, SPI_HandleTypeDef* hspi, Gpio_t *nss)
{
    obj->hspi = hspi;
    obj->nss = *nss;

    GpioWrite(&obj->nss, 1); // Initialize NSS high
}

uint16_t SpiInOut(Spi_t *obj, uint16_t outData)
{
    uint16_t inData = 0;

    GpioWrite(&obj->nss, 0); // NSS low

    HAL_SPI_TransmitReceive(obj->hspi, (uint8_t*)&outData, (uint8_t*)&inData, 1, HAL_MAX_DELAY);

    GpioWrite(&obj->nss, 1); // NSS high

    return inData;
}
