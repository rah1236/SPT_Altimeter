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
            // Print received data info
            char print_buf[256];
            snprintf(print_buf, sizeof(print_buf), "RX (%d bytes): ", len);

            char hex_buf[4];
            for (int i = 0; i < len; i++) {
                snprintf(hex_buf, sizeof(hex_buf), "%02X ", data[i]);
                strcat(print_buf, hex_buf);
            }

            char rssi_snr_buf[64];
            int16_t snr = (int16_t)(rfm95_get_snr() * 100);
            snprintf(rssi_snr_buf, sizeof(rssi_snr_buf),
                    "\r\nRSSI:%ddBm SNR:%d.%02ddB",
                    rfm95_get_rssi(),
                    snr / 100,
                    abs(snr % 100));
            strcat(print_buf, rssi_snr_buf);

            ush_print(&ush, print_buf);

            // Check if data starts with "servo_"
            if (len >= 6 && memcmp(data, "servo_", 6) == 0) {
                // Convert remaining data to angle value
                char angle_str[4] = {0}; // Max 3 digits + null terminator
                uint8_t angle_len = len - 6;
                if (angle_len > 0 && angle_len <= 3) {
                    memcpy(angle_str, &data[6], angle_len);
                    int angle = atoi(angle_str);

                    // Check if angle is within valid range (0-180)
                    if (angle >= 0 && angle <= 180) {
                        servo_set_angle(&htim2, (uint8_t)angle);

                        // Print confirmation message
                        char confirm_buf[32];
                        snprintf(confirm_buf, sizeof(confirm_buf),
                                "\r\nSetting servo to %d degrees", angle);
                        ush_print(&ush, confirm_buf);
                    }
                }
            }
        }
    }
}

void shell_service(void)
{
    ush_service(&ush);
    lora_monitor_service();
    gps_monitor_service(&ush);


}
