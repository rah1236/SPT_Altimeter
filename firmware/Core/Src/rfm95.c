/**
 * @file rfm95.c
 * @brief Driver implementation for RFM95W LoRa module
 */

#include "rfm95.h"
#include "main.h"
#include "stm32f1xx_hal.h"
#include <string.h>

// Static SPI handle
static SPI_HandleTypeDef *rfm95_spi;

// Private functions
static void rfm95_select(void) {
    HAL_GPIO_WritePin(SX1276_nCS_GPIO_Port, SX1276_nCS_Pin, GPIO_PIN_RESET);
}

static void rfm95_unselect(void) {
    HAL_GPIO_WritePin(SX1276_nCS_GPIO_Port, SX1276_nCS_Pin, GPIO_PIN_SET);
}

uint8_t rfm95_read_reg(uint8_t reg) {
    uint8_t data;
    rfm95_select();

    uint8_t addr = reg & 0x7F;  // Clear top bit to indicate read
    HAL_SPI_Transmit(rfm95_spi, &addr, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(rfm95_spi, &data, 1, HAL_MAX_DELAY);

    rfm95_unselect();
    return data;
}

void rfm95_write_reg(uint8_t reg, uint8_t data) {
    rfm95_select();

    uint8_t addr = reg | 0x80;  // Set top bit to indicate write
    HAL_SPI_Transmit(rfm95_spi, &addr, 1, HAL_MAX_DELAY);
    HAL_SPI_Transmit(rfm95_spi, &data, 1, HAL_MAX_DELAY);

    rfm95_unselect();
}

// DIO interrupt callbacks (to be implemented by user)
__weak void rfm95_dio0_callback(void) {}
__weak void rfm95_dio1_callback(void) {}
__weak void rfm95_dio2_callback(void) {}
__weak void rfm95_dio3_callback(void) {}

// GPIO interrupt handlers
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    switch(GPIO_Pin) {
        case SX1276_DIO0_Pin:
            rfm95_dio0_callback();
            break;
        case SX1276_DIO1_Pin:
            rfm95_dio1_callback();
            break;
        case SX1276_DIO2_Pin:
            rfm95_dio2_callback();
            break;
        case SX1276_DIO3_Pin:
            rfm95_dio3_callback();
            break;
        default:
            break;
    }
}

bool rfm95_init(const rfm95_config_t *config, SPI_HandleTypeDef *spi) {
    // Store SPI handle
    rfm95_spi = spi;

    // Module shares reset with MCU, so it's already reset at this point
    HAL_Delay(10); // Wait for module to be ready after reset

    // Read version register to check if module is responsive
    uint8_t version = rfm95_read_reg(RFM95_REG_VERSION);
    if (version != 0x12) {
        return false;
    }

    // Set sleep mode to access LoRa registers
    rfm95_sleep();

    // Set LoRa mode
    rfm95_write_reg(RFM95_REG_OP_MODE, 0x80);  // LoRa mode

    // Configure frequency
    uint32_t frf = ((uint64_t)config->frequency << 19) / 32000000;
    rfm95_write_reg(RFM95_REG_FR_MSB, (frf >> 16) & 0xFF);
    rfm95_write_reg(RFM95_REG_FR_MID, (frf >> 8) & 0xFF);
    rfm95_write_reg(RFM95_REG_FR_LSB, frf & 0xFF);

    // Configure PA
    if (config->power > 15) {
        // Enable PA_BOOST
        rfm95_write_reg(RFM95_REG_PA_CONFIG, RFM95_PA_BOOST | (config->power - 2));
    } else {
        rfm95_write_reg(RFM95_REG_PA_CONFIG, config->power);
    }

    // Set LNA gain
    rfm95_write_reg(RFM95_REG_LNA, config->lna_gain);

    // Configure modem - spreading factor, bandwidth, coding rate
    rfm95_write_reg(RFM95_REG_MODEM_CONFIG_1,
                    (config->bandwidth << 4) | (config->coding_rate << 1));
    rfm95_write_reg(RFM95_REG_MODEM_CONFIG_2,
                    (config->spreading_factor << 4));

    // Enable LNA boost
    rfm95_write_reg(RFM95_REG_MODEM_CONFIG_3, 0x04);

    // Configure DIO mapping for common usage
    // DIO0 = RxDone/TxDone
    // DIO1 = RxTimeout
    // DIO2 = FhssChangeChannel
    // DIO3 = ValidHeader
    rfm95_write_reg(RFM95_REG_DIO_MAPPING_1, 0x00);

    // Set to standby mode
    rfm95_standby();

    return true;
}


bool rfm95_send(const uint8_t *data, uint8_t len) {
    uint8_t packet[FIXED_PACKET_LENGTH] = {0}; // Initialize with zeros for padding

    // Copy data, truncating if too long or padding with zeros if too short
    uint8_t copy_len = (len > FIXED_PACKET_LENGTH) ? FIXED_PACKET_LENGTH : len;
    memcpy(packet, data, copy_len);

    rfm95_write_reg(RFM95_REG_FIFO_TX_BASE, 0);
    rfm95_write_reg(RFM95_REG_FIFO_ADDR_PTR, 0);

    rfm95_select();
    uint8_t addr = RFM95_REG_FIFO | 0x80;
    HAL_SPI_Transmit(rfm95_spi, &addr, 1, HAL_MAX_DELAY);
    HAL_SPI_Transmit(rfm95_spi, packet, FIXED_PACKET_LENGTH, HAL_MAX_DELAY);
    rfm95_unselect();

    // Always use fixed length
    rfm95_write_reg(RFM95_REG_PAYLOAD_LENGTH, FIXED_PACKET_LENGTH);
    rfm95_write_reg(RFM95_REG_DIO_MAPPING_1, 0x40);

    rfm95_write_reg(RFM95_REG_OP_MODE, RFM95_MODE_TX);

    while ((rfm95_read_reg(RFM95_REG_IRQ_FLAGS) & RFM95_IRQ_TX_DONE_MASK) == 0) {
        HAL_Delay(1);
    }

    rfm95_write_reg(RFM95_REG_IRQ_FLAGS, 0xFF);
    return true;
}

bool rfm95_receive(uint8_t *data, uint8_t *len, uint32_t timeout_ms) {
    // Check if we need to start receiving
    uint8_t op_mode = rfm95_read_reg(RFM95_REG_OP_MODE);
    if ((op_mode & 0x07) != RFM95_MODE_RX_SINGLE) {
        rfm95_write_reg(RFM95_REG_DIO_MAPPING_1, 0x00);
        rfm95_write_reg(RFM95_REG_FIFO_RX_BASE, 0);
        rfm95_write_reg(RFM95_REG_FIFO_ADDR_PTR, 0);
        rfm95_write_reg(RFM95_REG_OP_MODE, RFM95_MODE_RX_SINGLE);
    }

    uint32_t start = HAL_GetTick();
    while ((HAL_GetTick() - start) < timeout_ms) {
        if (rfm95_read_reg(RFM95_REG_IRQ_FLAGS) & RFM95_IRQ_RX_DONE_MASK) {
            uint8_t current_addr = rfm95_read_reg(RFM95_REG_FIFO_RX_BASE);
            rfm95_write_reg(RFM95_REG_FIFO_ADDR_PTR, current_addr);

            rfm95_select();
            uint8_t addr = RFM95_REG_FIFO;
            HAL_SPI_Transmit(rfm95_spi, &addr, 1, HAL_MAX_DELAY);
            HAL_SPI_Receive(rfm95_spi, data, FIXED_PACKET_LENGTH, HAL_MAX_DELAY);
            rfm95_unselect();

            // Find actual message length by looking for zero padding
            *len = FIXED_PACKET_LENGTH;
            for (int i = 0; i < FIXED_PACKET_LENGTH; i++) {
                if (data[i] == 0) {
                    *len = i;
                    break;
                }
            }

            rfm95_write_reg(RFM95_REG_IRQ_FLAGS, 0xFF);  // Clear all flags

            // Restart RX mode
            rfm95_write_reg(RFM95_REG_FIFO_RX_BASE, 0);
            rfm95_write_reg(RFM95_REG_FIFO_ADDR_PTR, 0);
            rfm95_write_reg(RFM95_REG_OP_MODE, RFM95_MODE_RX_SINGLE);

            return true;
        }
        HAL_Delay(1);
    }
    return false;
}

void rfm95_sleep(void) {
    rfm95_write_reg(RFM95_REG_OP_MODE, RFM95_MODE_SLEEP);
}

void rfm95_standby(void) {
    rfm95_write_reg(RFM95_REG_OP_MODE, RFM95_MODE_STDBY);
}

int16_t rfm95_get_rssi(void) {
    return -137 + rfm95_read_reg(RFM95_REG_RSSI_VALUE);
}

float rfm95_get_snr(void) {
    return ((int8_t)rfm95_read_reg(RFM95_REG_PKT_SNR_VALUE)) * 0.25;
}

void rfm95_clear_flags(void){
	   rfm95_write_reg(RFM95_REG_IRQ_FLAGS, 0xFF);
}

