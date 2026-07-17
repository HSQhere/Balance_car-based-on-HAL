#include "usart3.h"

// 定义这些以避免在不使用 MicroLIB 时进入半主机模式导致卡死
#if 1 
struct __FILE { 
    int handle; 
}; 
FILE __stdout;       
void _sys_exit(int x) { 
    x = x; 
} 
#endif
/**
 * @brief  重定向 fputc (Keil 下 printf 核心)
 * @note   全部改用 HAL 库标准发送 API，安全、稳定、不直接操作寄存器
 */
int fputc(int ch, FILE *f)
{
    if (Flag_Show == 0)
    {
        //使用 HAL 库的阻塞发送：向串口3发送 1 字节数据，超时时间设为 100ms
        HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 100);
    }
    else if (Flag_Show == 1)
    {
        //向串口1发送 1 字节数据
        //HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 100);
    }
    return ch;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle) //接收回调函数
{
	
	if(UartHandle->Instance == USART3)
	{
    static	int uart_receive=0;//蓝牙接收相关变量
		static u8 Flag_PID,i,j,Receive[50];
		static float Data;
  	uart_receive=Usart3_Receive_buf[0]; 
		Usart3_Receive=uart_receive;
		if(uart_receive==0x59)  Flag_velocity=2;  //收到 'Y'：切换为低速挡（默认）
		if(uart_receive==0x58)  Flag_velocity=1;  //收到 'X'：切换为高速挡
		
	  if(uart_receive>10)  //默认使用 标准 ASCII 码手柄（数据 > 10）
    {			
			if(uart_receive==0x5A)	    Fore=0,Back=0,Left=0,Right=0;// 'Z' 刹车
			else if(uart_receive==0x41)	Fore=1,Back=0,Left=0,Right=0;// 'A' 前进
			else if(uart_receive==0x45)	Fore=0,Back=1,Left=0,Right=0;// 'E' 后退
			else if(uart_receive==0x42||uart_receive==0x43||uart_receive==0x44)	
																	Fore=0,Back=0,Left=0,Right=1;// 'B'/'C'/'D' 右转
			else if(uart_receive==0x46||uart_receive==0x47||uart_receive==0x48)	    
																	Fore=0,Back=0,Left=1,Right=0;// 'F'/'G'/'H' 左转
			else Fore=0,Back=0,Left=0,Right=0;//刹车
  	}
		if(uart_receive<10)     //发送二进制数值 备用app为：MiniBalanceV1.0  MiniBalance APP 发送的是 0~8 的控制代码 其HEX都小于10
		{			
//			Flag_velocity=1;//切换至高速档
			if(uart_receive==0x00)	Fore=0,Back=0,Left=0,Right=0;//刹车
			else if(uart_receive==0x01)	Fore=1,Back=0,Left=0,Right=0;//前
			else if(uart_receive==0x05)	Fore=0,Back=1,Left=0,Right=0;//后
			else if(uart_receive==0x02||uart_receive==0x03||uart_receive==0x04)	
														Fore=0,Back=0,Left=0,Right=1;  //左
			else if(uart_receive==0x06||uart_receive==0x07||uart_receive==0x08)	    //右
														Fore=0,Back=0,Left=1,Right=0;
			else Fore=0,Back=0,Left=0,Right=0;//刹车
  	}	
		
		if(Usart3_Receive==0x7B) Flag_PID=1;   //APP参数指令起始位 { (0x7B) 开始
		if(Usart3_Receive==0x7D) Flag_PID=2;   //APP参数指令停止位 } (0x7D) 结束

		 if(Flag_PID==1)  //采集数据
		 {
				Receive[i]=Usart3_Receive;
				i++;
		 }
		 if(Flag_PID==2)  //分析数据
		 {
			  if(Receive[3]==0x50) 				 PID_Send=1; // 如果第4个字符是 'P'，代表 APP 请求单片机把当前的 PID 参数“上传”给手机显示。
			  else if(Receive[1]!=0x23) // 如果不是 '#'
				{								
					for(j=i;j>=4;j--)//把 ASCII 字符数组转换为物理浮点数
					{
						Data+=(Receive[j-1]-48)*pow(10,i-j);// -48 是将字符 '0'~'9' 转为数字 0~9
					}
					switch(Receive[1])// 根据包头（Receive[1]）判断要把这个数值赋给哪一个 PID 变量
					{
						//接收到的 Data 除以 1000.0f，还原浮点数
						case 0x30:  Vertical_Kp = Data; break;  // '0' -> 直立环 Kp
						case 0x31:  Vertical_Kd = Data; break;  // '1' -> 直立环 Kd
						case 0x32:  Velocity_Kp = Data / 1000.0f; break;  // '2' -> 速度环 Kp
						case 0x33:  Velocity_Ki = Data / 1000.0f; break;  // '3' -> 速度环 Ki 
						case 0x34:  Turn_Kp     = Data; break;  // '4' -> 转向环 Kp
						case 0x35:  Turn_Kd     = Data / 1000.0f; break;  // '5' -> 转向环 Kd
						case 0x36:  break; //预留
						case 0x37:  break; //预留
						case 0x38:  break; //预留
					}
				}				 
			    Flag_PID=0;
					i=0;
					j=0;
					Data=0;
					memset(Receive, 0, sizeof(u8)*50);//数组清零

		 } 
		HAL_UART_Receive_IT(&huart3,Usart3_Receive_buf,sizeof(Usart3_Receive_buf));//串口3回调函数执行完毕之后，需要再次开启接收中断等待下一次接收中断的发生
	}
	
}

