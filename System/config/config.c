#include "config.h"

// =========================================================================
// 全局变量定义并赋初值 —— 调试时直接在这里修改参数
// =========================================================================
u8 Way_Angle=1;                             //获取角度的算法，1：四元数  2：卡尔曼  3：互补滤波 
u8 Flag_Stop=0,Flag_Show=0;                 //电机停止标志位和显示标志位  默认停止 显示打开
float pitch,roll,yaw;//roll滚转,pitch俯仰,yaw偏航
short gyrox,gyroy,gyroz;
short	aacx,aacy,aacz;
int Encoder_Left,Encoder_Right;             					//左右编码器的脉冲计数	
float Velocity_Left,Velocity_Right;	//车轮速度(mm/s)
//机械中值：把车扶正读到的角度填在这里
float Middle_angle=3;//平衡时角度值偏移量（机械中值）
int Voltage;                   //电池电压采样相关的变量
//PID参数
float Vertical_Kp=180,Vertical_Kd=2;			//直立环 数量级（Kp：0~1000、Kd：0~10）
float Velocity_Kp=0.6,Velocity_Ki=0.003;		//速度环 数量级（Kp：0~1）
float Turn_Kp=10,Turn_Kd=0.6;											//转向环

u8 PID_Send; 					                    	//调参相关变量
u8 Usart3_Receive_buf[1];          //串口3接收中断数据存放的缓冲区
u8 Usart3_Receive;                 //从串口3读取的数据
uint8_t Fore,Back,Left,Right;// 遥控指令初始化
u8 Flag_front,Flag_back,Flag_Left,Flag_Right,Flag_velocity=2; //蓝牙遥控相关的变量

float distance;//超声波测距
int Temperature;                            //温度变量
char display_buf[20];

