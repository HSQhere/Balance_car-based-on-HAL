#include "show.h"

/**************************************************************************
Function: OLED display
Input   : none
Output  : none
函数功能：OLED显示
入口参数：无
返回  值：无
**************************************************************************/
void oled_show(void)
{
		sprintf(display_buf,"Encoder_L:%d   ",Encoder_Left);
		OLED_ShowString(0,0,display_buf,16);
		sprintf(display_buf,"Encoder_R:%d   ",Encoder_Right);
		OLED_ShowString(0,2,display_buf,16);	
//		sprintf(display_buf,"roll:%.1f   ",roll); 
//		OLED_ShowString(0,0,display_buf,16);
//		sprintf(display_buf,"yaw:%.1f   ",yaw); 
//		OLED_ShowString(0,2,display_buf,16);	
		sprintf(display_buf,"pitch:%.1f   ",-pitch); 
		OLED_ShowString(0,4,display_buf,16);
		GET_Distance();
		sprintf(display_buf,"distance:%.1f  ",distance);
		OLED_ShowString(0,6,display_buf,12);
		//sprintf(display_buf,"Temperature:%d  ",Temperature);
		//OLED_ShowString(1,display_buf,12);
}

/**************************************************************************
Function: Send data to APP
Input   : none
Output  : none
函数功能：向APP发送数据
入口参数：无
返回  值：无
**************************************************************************/
void APP_Show(void)
{    
  static u8 flag;
	int Encoder_Left_Show,Encoder_Right_Show,Voltage_Show;
	Voltage_Show=(Voltage-1000)*2/3;		if(Voltage_Show<0)Voltage_Show=0;if(Voltage_Show>100) Voltage_Show=100;   //对电压数据进行处理
	Encoder_Right_Show=Velocity_Right*1.1; if(Encoder_Right_Show<0) Encoder_Right_Show=-Encoder_Right_Show;			  //对编码器数据就行数据处理便于图形化
	Encoder_Left_Show=Velocity_Left*1.1;  if(Encoder_Left_Show<0) Encoder_Left_Show=-Encoder_Left_Show;
	flag=!flag;
	if(PID_Send==1)			// 发送PID参数，在APP调参界面显示
	{
			// 对不同量级的参数，采用不同的放大倍数
			printf("{C%d:%d:%d:%d:%d:%d:%d:%d:%d}$",
						 (int)(Vertical_Kp),  // 直立 Kp
						 (int)(Vertical_Kd),  // 直立 Kd
						 (int)(Velocity_Kp * 1000.0f),  // 速度 Kp
						 (int)(Velocity_Ki * 1000.0f),  // 速度 Ki
						 (int)(Turn_Kp),  // 转向 Kp
						 (int)(Turn_Kd * 1000.0f),  // 转向 Kd
						 0, 0, 0); // 预留位
						 
			PID_Send=0;	
	}
   else	if(flag==0)		// 发送电池电压，速度，角度等参数，在APP首页显示
		printf("{A%d:%d:%d:%d}$",(int)Encoder_Left_Show,(int)Encoder_Right_Show,(int)Voltage_Show,-(int)pitch); //打印到APP上面
	 else								//发送小车姿态角，在波形界面显示
	  printf("{B%d:%d:%d}$",(int)pitch,(int)roll,(int)yaw); //x，y，z轴角度 在APP上面显示波形
																													//可按格式自行增加显示波形，最多可显示五个
}
