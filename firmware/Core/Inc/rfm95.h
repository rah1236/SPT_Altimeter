/**
 * @file rfm95.h
 * @brief Driver for RFM95W LoRa module
 */
#ifndef RFM95_H
#define RFM95_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx_hal.h"

// Define fixed packet length
#define FIXED_PACKET_LENGTH 32

// RFM95 register addresses
#define RFM95_REG_FIFO                 0x00
#define RFM95_REG_OP_MODE             0x01
#define RFM95_REG_FR_MSB              0x06
#define RFM95_REG_FR_MID              0x07
#define RFM95_REG_FR_LSB              0x08
#define RFM95_REG_PA_CONFIG           0x09
#define RFM95_REG_PA_RAMP             0x0A
#define RFM95_REG_OCP                 0x0B
#define RFM95_REG_LNA                 0x0C
#define RFM95_REG_FIFO_ADDR_PTR      0x0D
#define RFM95_REG_FIFO_TX_BASE       0x0E
#define RFM95_REG_FIFO_RX_BASE       0x0F
#define RFM95_REG_IRQ_FLAGS          0x12
#define RFM95_REG_MODEM_CONFIG_1     0x1D
#define RFM95_REG_MODEM_CONFIG_2     0x1E
#define RFM95_REG_PAYLOAD_LENGTH     0x22
#define RFM95_REG_MODEM_CONFIG_3     0x26
#define RFM95_REG_DIO_MAPPING_1      0x40
#define RFM95_REG_VERSION            0x42
#define RFM95_REG_RSSI_VALUE		 0x22
#define RFM95_REG_PKT_SNR_VALUE      0x23

// RFM95 modes
#define RFM95_MODE_SLEEP             0x00
#define RFM95_MODE_STDBY             0x01
#define RFM95_MODE_TX                0x03
#define RFM95_MODE_RX_CONTINUOUS     0x05
#define RFM95_MODE_RX_SINGLE         0x06

// RFM95 PA config
#define RFM95_PA_BOOST               0x80

// IRQ masks
#define RFM95_IRQ_TX_DONE_MASK           0x08
#define RFM95_IRQ_PAYLOAD_CRC_ERROR_MASK 0x20
#define RFM95_IRQ_RX_DONE_MASK           0x40

typedef struct {
    // Add configuration parameters as needed
    int8_t power;
    uint32_t frequency;
    uint8_t lna_gain;
    uint8_t bandwidth;
    uint8_t coding_rate;
    uint8_t spreading_factor;
} rfm95_config_t;

/**
 * @brief Initialize the RFM95W module
 * @param config Pointer to configuration structure
 * @param spi Pointer to SPI handle to use
 * @return true if successful, false otherwise
 */
bool rfm95_init(const rfm95_config_t *config, SPI_HandleTypeDef *spi);

/**
 * @brief Send data using LoRa modulation
 * @param data Pointer to data buffer
 * @param len Length of data
 * @return true if successful, false otherwise
 */
bool rfm95_send(const uint8_t *data, uint8_t len);

/**
 * @brief Receive data using LoRa modulation
 * @param data Pointer to data buffer
 * @param len Pointer to length of data
 * @return true if successful, false otherwise
 */
bool rfm95_receive(uint8_t *data, uint8_t *len, uint32_t timeout_ms);
/**
 * @brief Set the module to sleep mode
 */
void rfm95_sleep(void);

/**
 * @brief Set the module to standby mode
 */
void rfm95_standby(void);

/**
 * @brief Read a register from the RFM95
 * @param reg Register address
 * @return Register value
 */
uint8_t rfm95_read_reg(uint8_t reg);

/**
 * @brief Write a register on the RFM95
 * @param reg Register address
 * @param data Data to write
 */
void rfm95_write_reg(uint8_t reg, uint8_t data);

int16_t rfm95_get_rssi(void);

float rfm95_get_snr(void);

void rfm95_clear_flags(void);
void rfm95_clear_flags(void);
#endif /* RFM95_H */
