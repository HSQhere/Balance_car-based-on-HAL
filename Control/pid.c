#include "pid.h"

uint8_t stop;

extern TIM_HandleTypeDef htim2,htim4;
extern float distance;
extern uint8_t Fore,Back,Left,Right;
#define SPEED_Y 30 //俯仰(前后)最大设定速度
#define SPEED_Z 150//偏航(左右)最大设定速度 
//直立环PD控制器
//输入：期望角度、真实角度、角速度
int Vertical(float Med,float Angle,float gyro_Y)
{
	int temp;
	temp=Vertical_Kp*(Angle-Med)+Vertical_Kd*gyro_Y;
	return temp;
}

//速度环PI控制器
//输入：期望速度、左编码器、右编码器
int Velocity(int Target,int encoder_L,int encoder_R)
{
	static int Err_LowOut_last,Encoder_S;
	static float a=0.7;
	int Err,Err_LowOut,temp;
	Velocity_Ki=Velocity_Kp/200;
	//1、计算偏差值
	Err=(encoder_L+encoder_R)-Target;
	//2、低通滤波
	Err_LowOut=(1-a)*Err+a*Err_LowOut_last;
	Err_LowOut_last=Err_LowOut;
	//3、积分
	Encoder_S+=Err_LowOut;
	//4、积分限幅(-20000~20000)
	Encoder_S=Encoder_S>20000?20000:(Encoder_S<(-20000)?(-20000):Encoder_S);
	if(stop==1)Encoder_S=0,stop=0;
	//5、速度环计算
	temp=Velocity_Kp*Err_LowOut+Velocity_Ki*Encoder_S;
	return temp;
}


//转向环PD控制器
//输入：角速度、角度值
int Turn(float gyro_Z,int Target_turn)
{
	int temp;
	temp=Turn_Kp*Target_turn+Turn_Kd*gyro_Z;
	return temp;
}
