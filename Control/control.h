#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"

u8 Turn_Off(float angle, int voltage,int encoder_left,int encoder_right);
void Get_Angle(u8 way);
void Control(void);
int Pick_Up(float Acceleration,float Angle,int encoder_left,int encoder_right);
int Put_Down(float Angle,int encoder_left,int encoder_right);
void Get_Velocity_Form_Encoder(int encoder_left,int encoder_right);


#endif
