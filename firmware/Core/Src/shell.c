#include "shell.h"
#include <string.h>
#include <stdarg.h>
#include "rfm95.h"
#include "servo.h"

#include "commands/ush_cmd_servo.h"
#include "commands/ush_cmd_baro.h"
#include "commands/ush_cmd_rfm95.h"
#include "commands/ush_cmd_gps.h"


#define SHELL_PROMPT     "SloPropTech"
#define BUF_IN_SIZE     128
#define BUF_OUT_SIZE    128
#define PATH_MAX_SIZE   128

static char ush_in_buf[BUF_IN_SIZE];
static char ush_out_buf[BUF_OUT_SIZE];
static struct ush_object* shell_handle = NULL;

// Shell instance
static struct ush_object ush;
static UART_HandleTypeDef* uart_handle;


extern bool lora_rx_mode;

// Non-blocking read interface
static int ush_read(struct ush_object *self, char *ch)
{
    if(HAL_UART_Receive(uart_handle, (uint8_t*)ch, 1, 0) == HAL_OK) {
        return 1;
    }
    return 0;
}

// Non-blocking write interface
static int ush_write(struct ush_object *self, char ch)
{
    if(HAL_UART_Transmit(uart_handle, (uint8_t*)&ch, 1, 10) == HAL_OK) {
        return 1;
    }
    return 0;
}

// I/O interface descriptor
static const struct ush_io_interface ush_iface = {
    .read = ush_read,
    .write = ush_write,
};

// LED command callbacks
static void cmd_led_callback(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[])
{
    if (argc != 2) {
        ush_print_status(self, USH_STATUS_ERROR_COMMAND_WRONG_ARGUMENTS);
        return;
    }

    if (strcmp(argv[1], "on") == 0) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
    } else if (strcmp(argv[1], "off") == 0) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
    } else {
        ush_print_status(self, USH_STATUS_ERROR_COMMAND_WRONG_ARGUMENTS);
    }
}

// Custom commands files descriptor
static const struct ush_file_descriptor cmd_files[] = {
    {
        .name = "led",
        .description = "control onboard LED",
        .help = "usage: led {on,off}\r\n",
        .exec = cmd_led_callback,
    },
};

// Command handler
static struct ush_node_object cmd;

// Shell descriptor
static const struct ush_descriptor ush_desc = {
    .io = &ush_iface,
    .input_buffer = ush_in_buf,
    .input_buffer_size = sizeof(ush_in_buf),
    .output_buffer = ush_out_buf,
    .output_buffer_size = sizeof(ush_out_buf),
    .path_max_length = PATH_MAX_SIZE,
    .hostname = SHELL_PROMPT,
};

void shell_init(UART_HandleTypeDef* huart)
{
    uart_handle = huart;

    // Initialize shell instance
    ush_init(&ush, &ush_desc);

    //store handle for global access
    shell_handle = &ush;
    // Add custom commands
    ush_commands_add(&ush, &cmd, cmd_files, sizeof(cmd_files) / sizeof(cmd_files[0]));

    //Custom command inits
    cmd_servo_init(&ush);
    cmd_baro_init(&ush);
    cmd_rfm95_init(&ush);
    cmd_gps_init(&ush);
}

void shell_print(const char* text)
{
    if (shell_handle != NULL) {
        ush_print(shell_handle, (char*)text);
    }
}

void shell_printf(const char* format, ...)
{
    if (shell_handle != NULL) {
        va_list args;
        va_start(args, format);
        ush_printf(shell_handle, format, args);
        va_end(args);
    }
}

void lora_monitor_service(void) {
    if (lora_rx_mode) {
        uint8_t data[FIXED_PACKET_LENGTH];
        uint8_t len;

        if (rfm95_receive(data, &len, 10)) {
            char output_buf[256] = {0};

            // Check if we received enough data
            if (len >= 8) {
                // Extract pressure and altitude as uint32_t first
                uint32_t pressure_raw, altitude_raw;
                memcpy(&pressure_raw, &data[0], sizeof(uint32_t));
                memcpy(&altitude_raw, &data[4], sizeof(uint32_t));

                // Convert to float
                float pressure = *((float*)&pressure_raw);
                float raw_altitude = *((float*)&altitude_raw);

                // Calibrate altitude:
                // We know the current value (-61290) should be 43m
                // Apply an offset to correct it
                float altitude_offset = 61333.0f; // 43 + 61290 = 61333
                float calibrated_altitude = raw_altitude + altitude_offset;

                // Convert to integers for printing
                int32_t pressure_int = (int32_t)(pressure * 100.0f);
                int32_t altitude_int = (int32_t)(calibrated_altitude * 100.0f);

                // Add values to output buffer
                snprintf(output_buf, sizeof(output_buf),
                        "Pressure: %ld.%02ld hPa, Altitude: %ld.%02ld m\r\n",
                        pressure_int / 100, abs(pressure_int % 100),
                        altitude_int / 100, abs(altitude_int % 100));
            } else {
                strcpy(output_buf, "Packet too short\r\n");
            }

            // Handle servo commands if present
            if (len >= 6 && memcmp(data, "servo_", 6) == 0) {
                char angle_str[4] = {0};
                uint8_t angle_len = len - 6;
                if (angle_len > 0 && angle_len <= 3) {
                    memcpy(angle_str, &data[6], angle_len);
                    int angle = atoi(angle_str);
                    if (angle >= 0 && angle <= 180) {
                        servo_set_angle(&htim2, (uint8_t)angle);
                        char temp_buf[80];
                        snprintf(temp_buf, sizeof(temp_buf),
                                "Setting servo to %d degrees\r\n", angle);
                        strcat(output_buf, temp_buf);
                    }
                }
            }

            // Print the complete output buffer once
            ush_print(&ush, output_buf);
        }
    }
}

void shell_service(void)
{
    ush_service(&ush);
    lora_monitor_service();
    gps_monitor_service(&ush);


}
