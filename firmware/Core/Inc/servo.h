/*
 * servo.h
 *
 *  Created on: Dec 30, 2024
 *      Author: raheel
 */

#ifndef INC_SERVO_H_
#define INC_SERVO_H_

#include "stm32f1xx_hal.h"

#define PRESCALER 159  // 16MHz/160 = 100kHz
#define PERIOD 1999    // 100kHz/50Hz = 2000 counts

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

void MX_TIM2_Init(void);
void MX_TIM3_Init(void);
void init_servo(void);
void servo_set_angle(TIM_HandleTypeDef *htim, uint8_t angle);

#endif /* INC_SERVO_H_ */
