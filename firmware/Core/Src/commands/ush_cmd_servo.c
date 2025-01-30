#include "commands/ush_cmd_servo.h"
#include <string.h>
#include "servo.h"
// Private callback functions - only used in this file


typedef enum {
    SERVO_1 = 0,
    SERVO_2 = 1,
    SERVO_COUNT
} servo_channel_t;

static void cmd_set_servo_callback(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[])
{
    if (argc != 3) {
        ush_print_status(self, USH_STATUS_ERROR_COMMAND_WRONG_ARGUMENTS);
        return;
    }

    int channel = atoi(argv[1]);
    int angle = atoi(argv[2]);

    if (channel < 0 || channel >= SERVO_COUNT) {
        ush_print(self, "Error: Invalid channel\r\n");
        return;
    }

    if (angle < 0 || angle > 180) {
        ush_print(self, "Error: Angle must be 0-180\r\n");
        return;
    }

    TIM_HandleTypeDef* timers[] = {&htim2, &htim3};
    servo_set_angle(timers[channel], angle);

    ush_print_status(self, USH_STATUS_OK);
}




//static void cmd_read_pressure_callback(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[])
//{
//    if (argc != 1) {
//        ush_print_status(self, USH_STATUS_ERROR_COMMAND_WRONG_ARGUMENTS);
//        return;
//    }
//
//    // Example implementation
//    ush_print(self, "Pressure: 1013.25hPa\r\n");
//}

// Private command list - only used in this file
static const struct ush_file_descriptor cmd_servo_files[] = {
    {
        .name = "servo",
        .description = "set servo angle",
        .help = "usage: servo {channel 0|1} {angle 0:180}\r\n",
        .exec = cmd_set_servo_callback,
    },
//    {
//        .name = "altitude",
//        .description = "read altitude",
//        .help = "usage: altitude\r\n",
//        .exec = cmd_read_altitude_callback,
//    }
};

// Node object for this module
static struct ush_node_object cmd_node_servo;

// Public init function
void cmd_servo_init(struct ush_object *ush)
{
    ush_commands_add(ush, &cmd_node_servo, cmd_servo_files,
                    sizeof(cmd_servo_files) / sizeof(cmd_servo_files[0]));
}
