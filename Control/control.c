#include "control.h"

/**************************************************************************
Function: HAL_GPIO_EXTI_Callback
Input   : none
Output  : none
函数功能：5ms外部中断由MPU6050的INT引脚触发
         严格保证采样和数据处理的时间同步	
入口参数：无
返回  值：无				 
**************************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint16_t count;
	if(GPIO_Pin==GPIO_PIN_2)
	{
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)==GPIO_PIN_SET)
		{
			__HAL_TIM_SetCounter(&htim3,0);
			HAL_TIM_Base_Start(&htim3);
		}
		else
		{
			HAL_TIM_Base_Stop(&htim3);
			count=__HAL_TIM_GetCounter(&htim3);
			distance=count*0.017;
		}
	}
	if(GPIO_Pin==GPIO_PIN_5)
		Control();
}

/**************************************************************************
Function: Control function
Input   : none
Output  : none
函数功能：所有的控制代码都在这里面
入口参数：无
返回  值：无				 
**************************************************************************/
void Control(void)	//每隔10ms调用一次
{
	
	// int Balance_Pwm,Velocity_Pwm,Turn_Pwm;		  					//平衡环PWM变量，速度环PWM变量，转向环PWM变
	int PWM_out;
	int Vertical_out, Velocity_out, Turn_out, MOTO1, MOTO2; 
	static int Target_Speed = 0; 
	static int Target_turn = 0;//将这两个变量定义为静态变量，避免它是临时分配在栈上
	Voltage =1200;//假设有adc检测电压为12v
	
	//1、读取编码器和陀螺仪的数据
	Get_Angle(Way_Angle);                     					//更新姿态
	Encoder_Left=Read_Encoder(&htim2);//读取左轮编码器的值，前进为正，后退为负，前面的正负号与硬件有关，具体实际为准
	Encoder_Right=-Read_Encoder(&htim4);//读取右轮编码器的值，前进为正，后退为负，前面的正负号与硬件有关，具体实际为准
	Get_Velocity_Form_Encoder(Encoder_Left,Encoder_Right);//编码器读数转速度（mm/s）
	
	//遥控
	if((Fore==0)&&(Back==0))Target_Speed=0;//未接受到前进后退指令-->速度清零，稳在原地
	if(Fore==1)
	{
//		if(distance<50)
//			Target_Speed--;
//		else
//			Target_Speed++;
		Target_Speed++;
	}
	if(Back==1){Target_Speed--;}//
	Target_Speed=Target_Speed>SPEED_Y?SPEED_Y:(Target_Speed<-SPEED_Y?(-SPEED_Y):Target_Speed);//限幅
	
	/*左右*/
	if((Left==0)&&(Right==0))Target_turn=0;
	if(Left==1)Target_turn-=30;	//左转
	if(Right==1)Target_turn+=30;	//右转
	Target_turn=Target_turn>SPEED_Z?SPEED_Z:(Target_turn<-SPEED_Z?(-SPEED_Z):Target_turn);//限幅( (20*100) * 100   )
	
	/*转向约束*/
	if((Left==0)&&(Right==0))Turn_Kd=0.6;//若无左右转向指令，则开启转向约束
	else if((Left==1)||(Right==1))Turn_Kd=0;//若左右转向指令接收到，则去掉转向约束
	
	
	//2、将数据传入PID控制器，计算输出结果，即左右电机转速值
	Velocity_out=Velocity(Target_Speed,Encoder_Left,Encoder_Right);
	Vertical_out=Vertical(Velocity_out+Middle_angle,pitch,gyrox);
	Turn_out=Turn(gyroz,Target_turn);
	PWM_out=Vertical_out;
	MOTO1=PWM_out-Turn_out;
	MOTO2=PWM_out+Turn_out;
	Limit(&MOTO1,&MOTO2);
	if(Pick_Up(aacz,pitch,Encoder_Left,Encoder_Right))//检查是否小车被拿起
		Flag_Stop=1;	                           					//如果被拿起就关闭电机
	if(Put_Down(pitch,Encoder_Left,Encoder_Right))//检查是否小车被放下
		Flag_Stop=0;	                           					//如果被放下就启动电机
	if(Turn_Off(pitch,Voltage,Encoder_Left,Encoder_Right)==0)     					//如果不存在异常       					  
		Load(MOTO1,MOTO2);//赋值给PWM寄存器
	else 
		Load(0,0);
}

/**************************************************************************
Function: Get angle
Input   : way：The algorithm of getting angle 1：DMP  2：kalman  3：Complementary filtering
Output  : none
函数功能：获取角度	
入口参数：way：获取角度的算法 1：DMP  2：卡尔曼 3：互补滤波
返回  值：无
**************************************************************************/	
void Get_Angle(u8 way)
{ 
	Temperature=MPU_Get_Temperature();      //读取MPU6050内置温度传感器数据，近似表示主板温度。
	if(way==1)                           //DMP的读取在数据采集中断读取，严格遵循时序要求
	{	
		mpu_dmp_get_data(&pitch,&roll,&yaw);
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);
	}			
}

/**************************************************************************
Function: Encoder reading is converted to speed (mm/s)
Input   : none
Output  : none
函数功能：编码器读数转换为速度（mm/s）
入口参数：无
返回  值：无
**************************************************************************/
void Get_Velocity_Form_Encoder(int encoder_left,int encoder_right)
{ 	
	float Rotation_Speed_L,Rotation_Speed_R;						//电机转速  转速=编码器读数（5ms每次）*读取频率/倍频数/减速比/编码器精度
	Rotation_Speed_L = encoder_left*Control_Frequency/EncoderMultiples/Reduction_Ratio/Encoder_precision;
	Velocity_Left = Rotation_Speed_L*PI*Diameter_67;		//求出编码器速度=转速*周长
	Rotation_Speed_R = encoder_right*Control_Frequency/EncoderMultiples/Reduction_Ratio/Encoder_precision;
	Velocity_Right = Rotation_Speed_R*PI*Diameter_67;		//求出编码器速度=转速*周长
}

/**************************************************************************
Function: If abnormal, turn off the motor
Input   : angle：Car inclination；voltage：Voltage
Output  : 1：abnormal；0：normal
函数功能：异常关闭电机		
入口参数：angle：小车倾角；voltage：电压
返回  值：1：异常  0：正常
**************************************************************************/	
u8 Turn_Off(float angle, int voltage,int encoder_left,int encoder_right)
{
	u8 temp;
	if(angle<-30||angle>30||1==Flag_Stop||voltage<1000)//电池电压低于10V关闭电机
	{	                                                 //倾角大于40度关闭电机
		temp=1;                                          //Flag_Stop置1，即单击控制关闭电机
	}
	else if(myabs(encoder_left+encoder_right)>NO_LOAD_ENCODER_STOP)   //空载高速关闭
		temp=1;
	else
		temp=0;
	return temp;			
}

/**************************************************************************
Function: Check whether the car is picked up
Input   : Acceleration：Z-axis acceleration；Angle：The angle of balance；encoder_left：Left encoder count；encoder_right：Right encoder count
Output  : 1：picked up  0：No action
函数功能：检测小车是否被拿起
入口参数：Acceleration：z轴加速度；Angle：平衡的角度；encoder_left：左编码器计数；encoder_right：右编码器计数
返回  值：1:小车被拿起  0：小车未被拿起
**************************************************************************/
int Pick_Up(float Acceleration,float Angle,int encoder_left,int encoder_right)
{ 		   
	 static u16 flag = 0,count0,count1,count2;
	 if(flag==0)                                                      //第一步
	 {
			if(myabs(encoder_left)+myabs(encoder_right)<30)               //条件1，小车接近静止
			count0++;
			else 
			count0=0;		
			if(count0>10)				
			flag=1,count0=0; 
	 } 
	 if(flag==1)                                                      //进入第二步
	 {
			if(++count1>200)       count1=0,flag=0;                       //超时不再等待2000ms，返回第一步
			if(Acceleration>AACZ_STOP&&(Angle>(-20+Middle_angle))&&(Angle<(20+Middle_angle)))   //条件2，小车是在0度附近被拿起
			flag=2; 
	 } 
	 if(flag==2)                                                       //第三步
	 {
		  if(++count2>100)       count2=0,flag=0;                        //超时不再等待1000ms
	    if(myabs(encoder_left+encoder_right)>PICK_UP_ENCODER_STOP)     //条件3，小车的轮胎因为正反馈达到最大的转速   
      {
				flag=0;                                                                                     
				return 1;                                                    //检测到小车被拿起
			}
	 }
	return 0;
}
/**************************************************************************
Function: Check whether the car is lowered
Input   : The angle of balance；Left encoder count；Right encoder count
Output  : 1：put down  0：No action
函数功能：检测小车是否被放下
入口参数：平衡角度；左编码器读数；右编码器读数
返回  值：1：小车放下   0：小车未放下
**************************************************************************/
int Put_Down(float Angle,int encoder_left,int encoder_right)
{ 		   
	 static u16 flag,count;	 
	 if(Flag_Stop==0)                     //防止误检      
			return 0;	                 
	 if(flag==0)                                               
	 {
			if(Angle>(-10+Middle_angle)&&Angle<(10+Middle_angle)&&encoder_left==0&&encoder_right==0) //条件1，小车是在0度附近的
			flag=1; 
	 } 
	 if(flag==1)                                               
	 {
		  if(++count>50)                     //超时不再等待 500ms
		  {
				count=0;flag=0;
		  }
	    if(encoder_left>3&&encoder_right>3&&encoder_left<40&&encoder_right<40) //条件2，小车的轮胎在未上电的时候被人为转动  
      {
				flag=0;
				flag=0;
				return 1;                         //检测到小车被放下
			}
	 }
	return 0;
}
