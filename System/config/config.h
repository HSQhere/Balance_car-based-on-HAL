#ifndef __CONFIG_H
#define __CONFIG_H
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"

// =========================================================================
// 1. 数据类型简写定义 (原本 sys.h 或裸定义的类型移植到这里)
// =========================================================================
typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  
typedef const int16_t sc16;  
typedef const int8_t sc8;  

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;  
typedef __I int16_t vsc16; 
typedef __I int8_t vsc8;   

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  
typedef const uint16_t uc16;  
typedef const uint8_t uc8; 

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;  
typedef __I uint16_t vuc16; 
typedef __I uint8_t vuc8;  

// =========================================================================
// 2. 功能控制开关 (1开启, 0关闭)
// =========================================================================
//#define ENABLE_ULTRASONIC   1   // 是否使能超声波测距
//#define ENABLE_DEBUG_PRINT  1   // 是否开启串口printf调试

// =========================================================================
// 3. 物理参数与机械常量宏定义
// =========================================================================
#define PWM_MAX 7200
#define PWM_MIN -7200  // 电机占空比限幅（防止烧驱动或飞车）
// #define SAFE_DISTANCE     50.0f // 超声波避障的安全距离 (cm)
#define SPEED_Y 30 //俯仰(前后)最大设定速度
#define SPEED_Z 150//偏航(左右)最大设定速度 
#define AACZ_STOP 10000//向上提起加速度停止标志 平地16000
#define PICK_UP_ENCODER_STOP 70//向上提起编码器合速度停止标志
#define NO_LOAD_ENCODER_STOP 100//空载时编码器合速度停止标志


#define MPU6050_EXTI_Pin GPIO_PIN_5
#define MPU6050_EXTI_GPIO_Port GPIOB
#define MPU6050_EXTI_EXTI_IRQn EXTI9_5_IRQn

#define PI 3.14159265							//PI圆周率
#define Control_Frequency  100.0	//编码器读取频率,看dmp的采样频率
#define Diameter_67  67.0 				//轮子直径67mm 
#define EncoderMultiples   4.0 		//编码器倍频数 通常在 CubeMX 里配置为 Encoder Mode TI1 and TI2，即 4 倍频（值为 4）
#define Encoder_precision  11.0 	//编码器精度 11线
#define Reduction_Ratio  30.0			//减速比30
#define Perimeter  210.4867 			//周长，单位mm

// #define DIFFERENCE 100
#define INT PAin(12)   //PA12连接到MPU6050的中断引脚

// =========================================================================
// 4. 全局配置变量的外部声明 (extern)
// =========================================================================
extern u8 Way_Angle;
extern u8 Flag_Stop,Flag_Show;
extern float pitch,roll,yaw;
extern short gyrox,gyroy,gyroz;
extern short aacx,aacy,aacz;
extern int Encoder_Left,Encoder_Right; 
extern float Velocity_Left,Velocity_Right;
extern float Middle_angle;      
extern int Voltage;
extern float Vertical_Kp,Vertical_Kd;			
extern float Velocity_Kp,Velocity_Ki;		
extern float Turn_Kp,Turn_Kd;											

extern u8 PID_Send;

extern u8 Usart3_Receive_buf[1];        
extern u8 Usart3_Receive;  
extern uint8_t Fore,Back,Left,Right;
extern u8 Flag_front,Flag_back,Flag_Left,Flag_Right,Flag_velocity;

extern float distance;
extern int Temperature; 
extern char display_buf[20];

#endif
