#ifndef _MOTOR_H
#define _MOTOR_H

#include "stm32f1xx_hal.h"
#include "sys.h"

extern TIM_HandleTypeDef htim1;

void Load(int moto1,int moto2);
void Limit(int *motoA,int *motoB);
#endif
