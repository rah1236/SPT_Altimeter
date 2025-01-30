#include "commands/ush_cmd_gps.h"
#include <string.h>
#include <stdio.h>
#include "sam_m8q.h"
#include "main.h"

extern UART_HandleTypeDef huart1;
extern I2C_HandleTypeDef hi2c1;

static bool gps_initialized = false;
static bool continuous_mode = false;

static void cmd_gps_init_callback(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[]) {
    if (argc != 2) {
        ush_print(self, "Usage: gps_init <uart|i2c>\r\n");
        return;
    }

    bool init_success = false;
    if (strcmp(argv[1], "uart") == 0) {
        init_success = sam_m8q_init_uart(&huart1);
        sam_m8q_set_debug(true);
    } else if (strcmp(argv[1], "i2c") == 0) {
        init_success = sam_m8q_init_i2c(&hi2c1);
    } else {
        ush_print(self, "Invalid interface. Use 'uart' or 'i2c'\r\n");
        return;
    }

    if (init_success) {
        gps_initialized = true;
        ush_print(self, "GPS initialized successfully\r\n");
    } else {
        ush_print(self, "GPS initialization failed\r\n");
    }
}




static void cmd_gps_status_callback(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[]) {
    if (!gps_initialized) {
        ush_print(self, "GPS not initialized. Use 'gps_init' first.\r\n");
        return;
    }



    sam_m8q_data_t gps_data;
    char buf[256];

    if (sam_m8q_get_data(&gps_data, false)) {
        // Convert floating point values to fixed-point integers
        int32_t lat_deg = (int32_t)gps_data.latitude;
        int32_t lat_min = (int32_t)((gps_data.latitude - lat_deg) * 60000.0f);  // 3 decimal places for minutes

        int32_t lon_deg = (int32_t)gps_data.longitude;
        int32_t lon_min = (int32_t)((gps_data.longitude - lon_deg) * 60000.0f);

        int32_t alt_meters = (int32_t)gps_data.altitude;
        int32_t alt_cm = (int32_t)((gps_data.altitude - alt_meters) * 100.0f);

        int32_t speed_whole = (int32_t)gps_data.speed;
        int32_t speed_frac = (int32_t)((gps_data.speed - speed_whole) * 10.0f);

        snprintf(buf, sizeof(buf),
            "GPS Status:\r\n"
            "Time: %02d:%02d:%02d\r\n"
            "Date: %02d/%02d/%04d\r\n"
            "Position: %d deg %d.%03d min %c, %d deg %d.%03d min %c\r\n"
            "Altitude: %d.%02d m\r\n"
            "Speed: %d.%d knots\r\n"
            "Satellites: %d\r\n"
            "Fix Valid: %s\r\n",
            gps_data.hours, gps_data.minutes, gps_data.seconds,
            gps_data.day, gps_data.month, gps_data.year,
            lat_deg, lat_min/1000, lat_min%1000, gps_data.lat_direction,
            lon_deg, lon_min/1000, lon_min%1000, gps_data.lon_direction,
            alt_meters, alt_cm,
            speed_whole, speed_frac,
            gps_data.satellites,
            gps_data.fix_valid ? "Yes" : "No");
    } else {
        snprintf(buf, sizeof(buf), "No new GPS data available\r\n");
    }

    ush_print(self, buf);
}

static void cmd_gps_sleep_callback(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[]) {
    if (!gps_initialized) {
        ush_print(self, "GPS not initialized. Use 'gps_init' first.\r\n");
        return;
    }

    if (sam_m8q_sleep()) {
        ush_print(self, "GPS entered sleep mode\r\n");
    } else {
        ush_print(self, "Failed to put GPS into sleep mode\r\n");
    }
}

static void cmd_gps_wake_callback(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[]) {
    if (!gps_initialized) {
        ush_print(self, "GPS not initialized. Use 'gps_init' first.\r\n");
        return;
    }

    if (sam_m8q_wake()) {
        ush_print(self, "GPS woken from sleep mode\r\n");
    } else {
        ush_print(self, "Failed to wake GPS from sleep mode\r\n");
    }
}

static void cmd_gps_monitor_callback(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[]) {
    if (!gps_initialized) {
        ush_print(self, "GPS not initialized. Use 'gps_init' first.\r\n");
        return;
    }

    if (argc != 2) {
        ush_print(self, "Usage: gps_monitor <on|off>\r\n");
        return;
    }

    if (strcmp(argv[1], "on") == 0) {
        continuous_mode = true;
        ush_print(self, "GPS monitoring enabled. Use 'gps_monitor off' to stop.\r\n");
    } else if (strcmp(argv[1], "off") == 0) {
        continuous_mode = false;
        ush_print(self, "GPS monitoring disabled.\r\n");
    } else {
        ush_print(self, "Invalid argument. Use 'on' or 'off'\r\n");
    }
}

// This function should be called periodically from the main loop
void gps_monitor_service(struct ush_object *self) {
    if (continuous_mode && sam_m8q_data_ready()) {
        cmd_gps_status_callback(self, NULL, 1, NULL);
    }
}

static void cmd_gps_raw_callback(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[]) {
    sam_m8q_data_t gps_data;
    sam_m8q_get_data(&gps_data, true);  // Force copy regardless of new data flag

        char buf[256];


            // Convert floating point values to fixed-point integers
            int32_t lat_deg = (int32_t)gps_data.latitude;
            int32_t lat_min = (int32_t)((gps_data.latitude - lat_deg) * 60000.0f);  // 3 decimal places for minutes

            int32_t lon_deg = (int32_t)gps_data.longitude;
            int32_t lon_min = (int32_t)((gps_data.longitude - lon_deg) * 60000.0f);

            int32_t alt_meters = (int32_t)gps_data.altitude;
            int32_t alt_cm = (int32_t)((gps_data.altitude - alt_meters) * 100.0f);

            int32_t speed_whole = (int32_t)gps_data.speed;
            int32_t speed_frac = (int32_t)((gps_data.speed - speed_whole) * 10.0f);

            snprintf(buf, sizeof(buf),
                "GPS Status:\r\n"
                "Time: %02d:%02d:%02d\r\n"
                "Date: %02d/%02d/%04d\r\n"
                "Position: %d deg %d.%03d min %c, %d deg %d.%03d min %c\r\n"
                "Altitude: %d.%02d m\r\n"
                "Speed: %d.%d knots\r\n"
                "Satellites: %d\r\n"
                "Fix Valid: %s\r\n",
                gps_data.hours, gps_data.minutes, gps_data.seconds,
                gps_data.day, gps_data.month, gps_data.year,
                lat_deg, lat_min/1000, lat_min%1000, gps_data.lat_direction,
                lon_deg, lon_min/1000, lon_min%1000, gps_data.lon_direction,
                alt_meters, alt_cm,
                speed_whole, speed_frac,
                gps_data.satellites,
                gps_data.fix_valid ? "Yes" : "No");


        ush_print(self, buf);
}

static const struct ush_file_descriptor cmd_gps_files[] = {
    {
        .name = "gps_init",
        .description = "Initialize GPS module",
        .help = "Usage: gps_init <uart|i2c>\r\n",
        .exec = cmd_gps_init_callback,
    },
    {
        .name = "gps_status",
        .description = "Show current GPS data",
        .help = "Usage: gps_status\r\n",
        .exec = cmd_gps_status_callback,
    },
    {
        .name = "gps_sleep",
        .description = "Put GPS in sleep mode",
        .help = "Usage: gps_sleep\r\n",
        .exec = cmd_gps_sleep_callback,
    },
    {
        .name = "gps_wake",
        .description = "Wake GPS from sleep mode",
        .help = "Usage: gps_wake\r\n",
        .exec = cmd_gps_wake_callback,
    },
    {
        .name = "gps_monitor",
        .description = "Enable/disable continuous GPS monitoring",
        .help = "Usage: gps_monitor <on|off>\r\n",
        .exec = cmd_gps_monitor_callback,
    },
	{
        .name = "gps_raw",
        .description = "Show raw GPS data buffer",
        .help = "Usage: gps_raw\r\n",
        .exec = cmd_gps_raw_callback,
    },
};


static struct ush_node_object cmd_node_gps;

void cmd_gps_init(struct ush_object *ush) {
    ush_commands_add(ush, &cmd_node_gps, cmd_gps_files,
                    sizeof(cmd_gps_files) / sizeof(cmd_gps_files[0]));
}
