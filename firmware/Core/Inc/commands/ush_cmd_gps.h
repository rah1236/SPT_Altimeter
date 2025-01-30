#ifndef USH_CMD_GPS_H
#define USH_CMD_GPS_H

#include "ush.h"

void cmd_gps_init(struct ush_object *ush);
void gps_monitor_service(struct ush_object *self);

#endif /* USH_CMD_GPS_H */
