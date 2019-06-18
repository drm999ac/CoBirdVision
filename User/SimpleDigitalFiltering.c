/******************** (C) COPYRIGHT 2015-2017 Xiluna Tech *********************
 * 作者 	:Xiluna Tech
 * 文件名 :SimpleDigitalFiltering.c
 * 描述   :简单滤波处理
 * 官网   :http://xiluna.com/
 * 公众号 :XilunaTech
******************************************************************************/
#include "SimpleDigitalFiltering.h"
/*
 *  滑动平均滤波（浮点类型）
 */
float Average_Filter(float FilterData,uint16_t Length,float *FilterArray){
    float AverageValue = 0;
    int i;
    for(i=0;i<Length-1;i++)
    {
        FilterArray[i]=FilterArray[i+1];
    }
    FilterArray[Length - 1] = FilterData;
    for(i=0;i<Length;i++)
    {
        AverageValue += FilterArray[i];
    }
    AverageValue /= Length;
    return AverageValue;
}
/*
 *  中位值滤波（整形）
 *  length 长度不宜过大，否者延时会过大，本例程全部使用7个值进行滤波处理
 */
float Median_Filter(int data,int measureNum,int *Filterdata){
  unsigned int i = 0;
	unsigned int j = 0;
	int temp;
	unsigned int MAX_error_targe = 0;
	int MAX_error1;
	Filterdata[measureNum-1] = data;
	for(i=0;i<measureNum-1;i++)
	{
	 Filterdata[i]=Filterdata[i+1];
	}
	for(i = 0 ; i < measureNum-1 ; i++)
	{
			for(j = 0 ; j < measureNum-1-i; j++)
			{
					if(Filterdata[j] > Filterdata[j+1] )
					{
							temp = Filterdata[j];
							Filterdata[j] =  Filterdata[j+1];
							Filterdata[j+1] = temp;
					}
			}
	}
	MAX_error1 = Filterdata[1] - Filterdata[0];
	for(i = 1 ; i < measureNum-1 ; i++)
	{
			if(MAX_error1 < Filterdata[i+1] - Filterdata[i] )
			{
					MAX_error1 =  Filterdata[i+1] - Filterdata[i];
					MAX_error_targe = i; 
			}
	}
	float Average_data=0;
	if(MAX_error_targe+1 > (measureNum+1)/2)
	{
			for(i = 0 ; i <= MAX_error_targe ; i++)
			{
					Average_data += Filterdata[i];
			}
			Average_data /= (MAX_error_targe+1);
	}
	else
	{
			for(i = MAX_error_targe + 1 ; i < measureNum ; i++)
			{
					Average_data += Filterdata[i];
			}
			Average_data /= (measureNum - MAX_error_targe -1);
	}
	return Average_data;
}


/*限幅函数*/
float Limits_data(float LimitsData,float LimitsUp,float LimitsDown)
{
	if(LimitsData > LimitsUp)
	{
		LimitsData = LimitsUp;
	}
	if(LimitsData < LimitsDown)
	{
		LimitsData = LimitsDown;
	}
	return LimitsData;
}


Butter_Parameter US100_Parameter;
Butter_BufferData  US100_filter_buf[3];

void Set_Cutoff_Frequency(float sample_frequent, float cutoff_frequent,Butter_Parameter *LPF)
{
  float fr = sample_frequent / cutoff_frequent;
  float ohm = tanf(3.141592653589793f / fr);
  float c = 1.0f + 2.0f * cosf(3.141592653589793f / 4.0f) * ohm + ohm * ohm;
  if (cutoff_frequent <= 0.0f) {
    // no filtering
    return;
  }
  LPF->b[0] = ohm * ohm / c;
  LPF->b[1] = 2.0f * LPF->b[0];
  LPF->b[2] = LPF->b[0];
  LPF->a[0]=1.0f;
  LPF->a[1] = 2.0f * (ohm * ohm - 1.0f) / c;
  LPF->a[2] = (1.0f - 2.0f * cosf(3.141592653589793f / 4.0f) * ohm + ohm * ohm) / c;
}


/*************************************************
函数名:	float LPButterworth(float curr_input,Butter_BufferData *Buffer,Butter_Parameter *Parameter)
说明:	加速度计低通滤波器
入口:	float curr_input 当前输入加速度计,滤波器参数，滤波器缓存
出口:	无
备注:	2阶Butterworth低通滤波器
*************************************************/
float LPButterworth(float curr_input,Butter_BufferData *Buffer,Butter_Parameter *Parameter)
{
  /* 加速度计Butterworth滤波 */
  /* 获取最新x(n) */
  Buffer->Input_Butter[2]=curr_input;
  /* Butterworth滤波 */
  Buffer->Output_Butter[2]=
    Parameter->b[0] * Buffer->Input_Butter[2]
      +Parameter->b[1] * Buffer->Input_Butter[1]
        +Parameter->b[2] * Buffer->Input_Butter[0]
          -Parameter->a[1] * Buffer->Output_Butter[1]
            -Parameter->a[2] * Buffer->Output_Butter[0];
  /* x(n) 序列保存 */
  Buffer->Input_Butter[0]=Buffer->Input_Butter[1];
  Buffer->Input_Butter[1]=Buffer->Input_Butter[2];
  /* y(n) 序列保存 */
  Buffer->Output_Butter[0]=Buffer->Output_Butter[1];
  Buffer->Output_Butter[1]=Buffer->Output_Butter[2];
  return Buffer->Output_Butter[2];
}
