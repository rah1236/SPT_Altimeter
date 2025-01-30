#include "commands/ush_cmd_rfm95.h"
#include <string.h>
#include <stdio.h>
#include "rfm95.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1;

static void hex_to_str(uint8_t value, char* str) {
    const char hex_chars[] = "0123456789ABCDEF";
    str[0] = '0';
    str[1] = 'x';
    str[2] = hex_chars[(value >> 4) & 0xF];
    str[3] = hex_chars[value & 0xF];
    str[4] = '\0';
}

// Helper function to print register value
static void print_reg_value(struct ush_object *self, const char* reg_name, uint8_t reg_addr) {
    uint8_t value = rfm95_read_reg(reg_addr);
    char buf[64];
    snprintf(buf, sizeof(buf), "%s: 0x%02X\r\n", reg_name, value);
    ush_print(self, buf);
}

static void cmd_rfm95_status_callback(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[]) {
    if (argc != 1) {
        ush_print_status(self, USH_STATUS_ERROR_COMMAND_WRONG_ARGUMENTS);
        return;
    }

    // Use a single large buffer for all output
    char output[512] = "";  // Start with empty string
    char hex[8];

    // Version check
    uint8_t version = rfm95_read_reg(RFM95_REG_VERSION);
    strcat(output, "Version: ");
    hex_to_str(version, hex);
    strcat(output, hex);
    strcat(output, " (should be 0x12)\r\n");

    // Operation Mode
    uint8_t op_mode = rfm95_read_reg(RFM95_REG_OP_MODE);
    strcat(output, "Op Mode: ");
    hex_to_str(op_mode, hex);
    strcat(output, hex);
    strcat(output, "\r\n");

    // Current Mode interpretation
    const char* mode_str;
    switch(op_mode & 0x07) {
        case 0x00: mode_str = "SLEEP"; break;
        case 0x01: mode_str = "STDBY"; break;
        case 0x02: mode_str = "FSTX"; break;
        case 0x03: mode_str = "TX"; break;
        case 0x04: mode_str = "FSRX"; break;
        case 0x05: mode_str = "RXCONTINUOUS"; break;
        case 0x06: mode_str = "RXSINGLE"; break;
        case 0x07: mode_str = "CAD"; break;
        default: mode_str = "UNKNOWN"; break;
    }
    strcat(output, "Current Mode: ");
    strcat(output, mode_str);
    strcat(output, "\r\n");

    // LoRa Mode status
    strcat(output, "LoRa Mode: ");
    strcat(output, (op_mode & 0x80) ? "Yes" : "No");
    strcat(output, "\r\n");

    // Modem configuration
    uint8_t modem_conf1 = rfm95_read_reg(RFM95_REG_MODEM_CONFIG_1);
    strcat(output, "Modem Config 1: ");
    hex_to_str(modem_conf1, hex);
    strcat(output, hex);
    strcat(output, "\r\n");

    uint8_t modem_conf2 = rfm95_read_reg(RFM95_REG_MODEM_CONFIG_2);
    strcat(output, "Modem Config 2: ");
    hex_to_str(modem_conf2, hex);
    strcat(output, hex);
    strcat(output, "\r\n");

    // IRQ Flags
    uint8_t irq_flags = rfm95_read_reg(RFM95_REG_IRQ_FLAGS);
    strcat(output, "IRQ Flags: ");
    hex_to_str(irq_flags, hex);
    strcat(output, hex);
    strcat(output, "\r\n");

    // Only add active flags
    if(irq_flags) {
        if(irq_flags & 0x80) strcat(output, "  RxTimeout\r\n");
        if(irq_flags & 0x40) strcat(output, "  RxDone\r\n");
        if(irq_flags & 0x20) strcat(output, "  PayloadCrcError\r\n");
        if(irq_flags & 0x10) strcat(output, "  ValidHeader\r\n");
        if(irq_flags & 0x08) strcat(output, "  TxDone\r\n");
        if(irq_flags & 0x04) strcat(output, "  CadDone\r\n");
        if(irq_flags & 0x02) strcat(output, "  FhssChangeChannel\r\n");
        if(irq_flags & 0x01) strcat(output, "  CadDetected\r\n");
    }

    // PA settings
    uint8_t pa_config = rfm95_read_reg(RFM95_REG_PA_CONFIG);
    strcat(output, "PA Config: ");
    hex_to_str(pa_config, hex);
    strcat(output, hex);
    strcat(output, "\r\n");

    // Send all output at once
    ush_print(self, output);
}

//static void cmd_rfm95_init_callback(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[]) {
//    if (argc != 1) {
//        ush_print_status(self, USH_STATUS_ERROR_COMMAND_WRONG_ARGUMENTS);
//        return;
//    }
//
//    rfm95_config_t config = {
//        .frequency = 868000000,  // 868 MHz
//        .power = 17,            // 17 dBm
//        .lna_gain = 0x01,      // Max LNA gain
//        .bandwidth = 7,        // 125 kHz
//        .coding_rate = 1,      // 4/5
//        .spreading_factor = 7  // SF7
//    };
//
//    if (!rfm95_init(&config, &hspi1)) {
//        ush_print(self, "Failed to initialize RFM95\r\n");
//        return;
//    }
//
//    ush_print(self, "RFM95 initialized successfully\r\n");
//}

bool lora_rx_mode = false;

void cmd_lora_rx(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[]) {
    if (argc != 2) {
        ush_print(self, "Usage: lora_rx <on|off>");
        return;
    }

    if (strcmp(argv[1], "on") == 0) {
        // Initialize RX mode
        rfm95_write_reg(RFM95_REG_DIO_MAPPING_1, 0x00);
        rfm95_write_reg(RFM95_REG_FIFO_RX_BASE, 0);
        rfm95_write_reg(RFM95_REG_FIFO_ADDR_PTR, 0);
        rfm95_write_reg(RFM95_REG_OP_MODE, RFM95_MODE_RX_SINGLE);

        lora_rx_mode = true;
        ush_print(self, "LoRa receive mode enabled");
    }
    else if (strcmp(argv[1], "off") == 0) {
        // Clean exit from RX mode
        rfm95_clear_flags();  // Clear any pending interrupts
        rfm95_standby();      // Return to standby mode

        lora_rx_mode = false;
        ush_print(self, "LoRa receive mode disabled");
    }
    else {
        ush_print(self, "Invalid argument. Use 'on' or 'off'");
    }
}

static void cmd_rfm95_send_callback(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[]) {
    if (argc != 2) {
        ush_print(self, "Usage: send <message>\r\n");
        return;
    }

    const char *message = argv[1];
    uint8_t len = strlen(message);
    char buf[256];
    int offset = 0;

    if (len > FIXED_PACKET_LENGTH) {
        offset += snprintf(buf + offset, sizeof(buf) - offset,
            "Warning: Message too long - will be truncated to %d bytes\r\n",
            FIXED_PACKET_LENGTH);
    }

    offset += snprintf(buf + offset, sizeof(buf) - offset, "Sending %d bytes: %s\r\n",
        len > FIXED_PACKET_LENGTH ? FIXED_PACKET_LENGTH : len, message);

    if (!rfm95_send((uint8_t*)message, len)) {
        offset += snprintf(buf + offset, sizeof(buf) - offset, "Failed to send message\r\n");
    } else {
        offset += snprintf(buf + offset, sizeof(buf) - offset,
            "Message sent successfully (RSSI: %ddBm)\r\n", rfm95_get_rssi());
    }

    ush_print(self, buf);
}



static void cmd_rfm95_receive_callback(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[]) {
   if (argc != 1) {
       ush_print_status(self, USH_STATUS_ERROR_COMMAND_WRONG_ARGUMENTS);
       return;
   }

   uint8_t rx_data[64];
   uint8_t rx_len = FIXED_PACKET_LENGTH;
   uint32_t start = HAL_GetTick();
   char buf[512] = {0};
   int offset = 0;

   while (HAL_GetTick() - start < 5000) {
       if (rfm95_receive(rx_data, &rx_len, 500)) {
           offset += snprintf(buf + offset, sizeof(buf) - offset,
               "Received %d bytes (RSSI: %ddBm)\r\nHEX: ",
               rx_len, rfm95_get_rssi());

           for (int i = 0; i < rx_len; i++) {
               offset += snprintf(buf + offset, sizeof(buf) - offset, "%02X ", rx_data[i]);
           }

           rx_data[rx_len] = 0;
           offset += snprintf(buf + offset, sizeof(buf) - offset, "\r\nASCII: %s\r\n", rx_data);
           ush_print(self, buf);
           return;
       }
   }

   rfm95_standby();
   snprintf(buf, sizeof(buf), "Timeout - no data received (RSSI: %ddBm)\r\n", rfm95_get_rssi());
   ush_print(self, buf);
}

static const struct ush_file_descriptor cmd_rfm95_files[] = {
    {
        .name = "status",
        .description = "read RFM95 status",
        .help = "usage: status\r\n",
        .exec = cmd_rfm95_status_callback,
    },
//    {
//        .name = "init",
//        .description = "initialize RFM95",
//        .help = "usage: init\r\n",
//        .exec = cmd_rfm95_init_callback,
//    },
    {
        .name = "send",
        .description = "send a message",
        .help = "usage: send <message>\r\n",
        .exec = cmd_rfm95_send_callback,
    },
    {
        .name = "receive",
        .description = "wait for received message",
        .help = "usage: receive\r\n",
        .exec = cmd_rfm95_receive_callback,
    },
    {
        .name = "lora_rx",
        .description = "Toggle LoRa receive mode",
        .help = "Usage: lora_rx <on|off>",
        .exec = cmd_lora_rx
    },
};

static struct ush_node_object cmd_node_rfm95;

void cmd_rfm95_init(struct ush_object *ush) {
    ush_commands_add(ush, &cmd_node_rfm95, cmd_rfm95_files,
                    sizeof(cmd_rfm95_files) / sizeof(cmd_rfm95_files[0]));
}
