#ifndef SHELL_H
#define SHELL_H

#include "inc/microshell.h"
#include "main.h"
#include "stm32f1xx_hal.h"

void shell_init(UART_HandleTypeDef* huart);
void shell_service(void);
void shell_print(const char* text);
void shell_printf(const char* format, ...);

#endif /* SHELL_H */
