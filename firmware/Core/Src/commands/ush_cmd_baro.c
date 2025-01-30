#include "commands/ush_cmd_baro.h"
#include <string.h>
#include <stdio.h>
#include "bmp280.h"


// Helper function to convert integer to string
static void int_to_str(int32_t value, char* str) {
    // Handle negative numbers
    if (value < 0) {
        *str++ = '-';
        value = -value;
    }

    // Find length by dividing repeatedly by 10
    int32_t temp = value;
    int len = 0;
    do {
        len++;
        temp /= 10;
    } while (temp > 0);

    // Add null terminator
    str[len] = '\0';

    // Fill in digits from right to left
    while (len > 0) {
        str[--len] = '0' + (value % 10);
        value /= 10;
    }
}

static void cmd_read_pressure_callback(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[])
{
    if (argc != 1) {
        ush_print_status(self, USH_STATUS_ERROR_COMMAND_WRONG_ARGUMENTS);
        return;
    }

    float pressure = BMP280_ReadPressure();
    int32_t pressure_whole = (int32_t)(pressure / 100);
    int32_t pressure_decimal = (int32_t)(pressure) % 100;

    char whole[16];
    char decimal[8];
    int_to_str(pressure_whole, whole);
    int_to_str(pressure_decimal, decimal);

    char buf[64];
    strcpy(buf, "Pressure: ");
    strcat(buf, whole);
    strcat(buf, ".");
    // Ensure 2 digits for decimal part
    if (pressure_decimal < 10) {
        strcat(buf, "0");
    }
    strcat(buf, decimal);
    strcat(buf, " hPa\r\n");

    ush_print(self, buf);
}
static void cmd_read_altitude_callback(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[])
{
    if (argc != 1) {
        ush_print_status(self, USH_STATUS_ERROR_COMMAND_WRONG_ARGUMENTS);
        return;
    }

    float altitude = BMP280_ReadAltitude(101325.0f);
    int32_t altitude_whole = (int32_t)altitude;
    int32_t altitude_decimal = (int32_t)(altitude * 10) % 10;

    char whole[16];
    char decimal[8];
    int_to_str(altitude_whole, whole);
    int_to_str(altitude_decimal, decimal);

    char buf[64];
    strcpy(buf, "Altitude: ");
    strcat(buf, whole);
    strcat(buf, ".");
    strcat(buf, decimal);
    strcat(buf, " m\r\n");

    ush_print(self, buf);
}

static const struct ush_file_descriptor cmd_baro_files[] = {
    {
        .name = "pressure",
        .description = "read pressure",
        .help = "usage: pressure\r\n",
        .exec = cmd_read_pressure_callback,
    },
    {
        .name = "altitude",
        .description = "read altitude",
        .help = "usage: altitude\r\n",
        .exec = cmd_read_altitude_callback,
    }
};

static struct ush_node_object cmd_node_baro;

void cmd_baro_init(struct ush_object *ush)
{
    ush_commands_add(ush, &cmd_node_baro, cmd_baro_files,
                    sizeof(cmd_baro_files) / sizeof(cmd_baro_files[0]));
}
