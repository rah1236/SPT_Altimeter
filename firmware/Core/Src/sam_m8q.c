#include "sam_m8q.h"
#include <string.h>
#include <stdlib.h>
#include "shell.h"

#define SAM_M8Q_I2C_ADDR    0x42
#define NMEA_BUFFER_SIZE    128
#define MAX_FIELD_LENGTH    16

// Static variables
static UART_HandleTypeDef *gps_uart;
static I2C_HandleTypeDef *gps_i2c;
static bool using_uart = true;

// Buffer for incoming NMEA data
static char nmea_buffer[NMEA_BUFFER_SIZE];
static uint16_t nmea_index = 0;
static bool sentence_ready = false;

// Latest parsed GPS data
static sam_m8q_data_t current_data = {0};
static bool new_data_available = false;

// Static function declarations
static bool parse_nmea_sentence(const char *sentence);
static float parse_coordinate(const char *coord_str);
static uint8_t nmea_checksum(const char *sentence);

static bool debug_enabled = false;
void sam_m8q_set_debug(bool enable) {
    debug_enabled = enable;
}

bool sam_m8q_init_uart(UART_HandleTypeDef *huart) {
    gps_uart = huart;
    using_uart = true;

    // Enable UART receive interrupt
    HAL_UART_Receive_IT(gps_uart, (uint8_t*)&nmea_buffer[nmea_index], 1);

    return true;
}

bool sam_m8q_init_i2c(I2C_HandleTypeDef *hi2c) {
    gps_i2c = hi2c;
    using_uart = false;

    // TODO: Add I2C initialization if needed

    return true;
}

void sam_m8q_uart_rx_callback(uint8_t data) {
    if (debug_enabled) {
        // Optional: print each received character for debugging
        shell_printf("%c", data);
    }

    if (nmea_index < NMEA_BUFFER_SIZE - 1) {
        nmea_buffer[nmea_index++] = data;

        // Check for end of sentence
        if (data == '\n') {
            nmea_buffer[nmea_index] = '\0';

            if (debug_enabled) {
                shell_printf("Received sentence: %s", nmea_buffer);
            }

            sentence_ready = true;
            nmea_index = 0;

            // Broaden sentence type checking
            if (strncmp(nmea_buffer, "$GP", 3) == 0 ||
                strncmp(nmea_buffer, "$GN", 3) == 0 ||
                strncmp(nmea_buffer, "$GL", 3) == 0) {
                if (parse_nmea_sentence(nmea_buffer)) {
                    new_data_available = true;
                }
            }
        }
    } else {
        nmea_index = 0;  // Buffer overflow protection
    }

    // Re-enable UART receive interrupt
    HAL_UART_Receive_IT(gps_uart, (uint8_t*)&nmea_buffer[nmea_index], 1);
}

bool sam_m8q_get_data(sam_m8q_data_t *data, bool force_copy) {
    if (!new_data_available && !force_copy) {
        return false;
    }

    memcpy(data, &current_data, sizeof(sam_m8q_data_t));
    new_data_available = false;
    return true;
}

bool sam_m8q_sleep(void) {
    const char *sleep_cmd = "$PMTK161,0*28\r\n";

    if (using_uart) {
        return (HAL_UART_Transmit(gps_uart, (uint8_t*)sleep_cmd, strlen(sleep_cmd), 100) == HAL_OK);
    } else {
        // TODO: Implement I2C sleep command
        return false;
    }
}

bool sam_m8q_wake(void) {
    const char *wake_cmd = "$PMTK167*00\r\n";

    if (using_uart) {
        return (HAL_UART_Transmit(gps_uart, (uint8_t*)wake_cmd, strlen(wake_cmd), 100) == HAL_OK);
    } else {
        // TODO: Implement I2C wake command
        return false;
    }
}

bool sam_m8q_data_ready(void) {
    return new_data_available;
}

static float parse_coordinate(const char *coord_str) {
    if (strlen(coord_str) < 4) {
        return 0.0f;
    }

    char deg_str[4] = {0};
    const char *min_str;

    if (strlen(coord_str) > 7) {  // Longitude
        strncpy(deg_str, coord_str, 3);
        min_str = coord_str + 3;
    } else {  // Latitude
        strncpy(deg_str, coord_str, 2);
        min_str = coord_str + 2;
    }

    float degrees = atof(deg_str);
    float minutes = atof(min_str);

    return degrees + (minutes / 60.0f);
}

static bool parse_nmea_sentence(const char *sentence) {
    char field[MAX_FIELD_LENGTH];
    int field_index = 0;
    int char_index = 0;
    int sentence_index = 0;

    // Skip the sentence identifier (e.g., "$GPRMC,")
    while (sentence[sentence_index] != ',' && sentence[sentence_index] != '\0') {
        sentence_index++;
    }
    if (sentence[sentence_index] == '\0') return false;
    sentence_index++;

    // Check if this is an RMC sentence
    if (strstr(sentence, "RMC") == NULL) {
        return false;
    }

    // Parse each field
    while (sentence[sentence_index] != '\0' && sentence[sentence_index] != '*') {
        if (sentence[sentence_index] == ',') {
            field[char_index] = '\0';

            // Process field based on its position
            switch (field_index) {
                case 0:  // Time
                    if (strlen(field) >= 6) {
                        current_data.hours = (field[0] - '0') * 10 + (field[1] - '0');
                        current_data.minutes = (field[2] - '0') * 10 + (field[3] - '0');
                        current_data.seconds = (field[4] - '0') * 10 + (field[5] - '0');
                    }
                    break;

                case 1:  // Status
                    current_data.fix_valid = (field[0] == 'A');
                    break;

                case 2:  // Latitude
                    current_data.latitude = parse_coordinate(field);
                    break;

                case 3:  // N/S indicator
                    current_data.lat_direction = field[0];
                    if (current_data.lat_direction == 'S') {
                        current_data.latitude = -current_data.latitude;
                    }
                    break;

                case 4:  // Longitude
                    current_data.longitude = parse_coordinate(field);
                    break;

                case 5:  // E/W indicator
                    current_data.lon_direction = field[0];
                    if (current_data.lon_direction == 'W') {
                        current_data.longitude = -current_data.longitude;
                    }
                    break;

                case 6:  // Speed (knots)
                    current_data.speed = atof(field);
                    break;

                case 8:  // Date
                    if (strlen(field) >= 6) {
                        current_data.day = (field[0] - '0') * 10 + (field[1] - '0');
                        current_data.month = (field[2] - '0') * 10 + (field[3] - '0');
                        current_data.year = 2000 + (field[4] - '0') * 10 + (field[5] - '0');
                    }
                    break;
            }

            field_index++;
            char_index = 0;
        } else {
            if (char_index < MAX_FIELD_LENGTH - 1) {
                field[char_index++] = sentence[sentence_index];
            }
        }
        sentence_index++;
    }

    return true;
}
