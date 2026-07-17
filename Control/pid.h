#ifndef __PID_H__
#define __PID_H__

#include "stm32f1xx_hal.h"
#include "sys.h"

int Vertical(float Med,float Angle,float gyro_Y);
int Velocity(int Target,int encoder_L,int encoder_R);
int Turn(float gyro_Z,int Target_turn);

#endif
