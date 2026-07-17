#include "encoder.h"

/**************************************************************************
函数功能：单位时间读取编码器计数
入口参数：定时器
返回  值：单位时间积累脉冲值
**************************************************************************/
int Read_Encoder(TIM_HandleTypeDef *htim)
{
    int ticks;
    ticks = (short)__HAL_TIM_GET_COUNTER(htim); // 1. 读当前攒了多少个脉冲
    __HAL_TIM_SET_COUNTER(htim, 0);// 2. 清零，重新攒脉冲
    return ticks;
}
