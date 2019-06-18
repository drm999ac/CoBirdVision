/******************** (C) COPYRIGHT 2015-2017 Xiluna Tech *********************
 * ���� 	:Xiluna Tech
 * �ļ��� :SimpleDigitalFiltering.c
 * ����   :���˲�����
 * ����   :http://xiluna.com/
 * ���ں� :XilunaTech
******************************************************************************/
#include "SimpleDigitalFiltering.h"
/*
 *  ����ƽ���˲����������ͣ�
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
 *  ��λֵ�˲������Σ�
 *  length ���Ȳ��˹��󣬷�����ʱ����󣬱�����ȫ��ʹ��7��ֵ�����˲�����
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


/*�޷�����*/
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
������:	float LPButterworth(float curr_input,Butter_BufferData *Buffer,Butter_Parameter *Parameter)
˵��:	���ٶȼƵ�ͨ�˲���
���:	float curr_input ��ǰ������ٶȼ�,�˲����������˲�������
����:	��
��ע:	2��Butterworth��ͨ�˲���
*************************************************/
float LPButterworth(float curr_input,Butter_BufferData *Buffer,Butter_Parameter *Parameter)
{
  /* ���ٶȼ�Butterworth�˲� */
  /* ��ȡ����x(n) */
  Buffer->Input_Butter[2]=curr_input;
  /* Butterworth�˲� */
  Buffer->Output_Butter[2]=
    Parameter->b[0] * Buffer->Input_Butter[2]
      +Parameter->b[1] * Buffer->Input_Butter[1]
        +Parameter->b[2] * Buffer->Input_Butter[0]
          -Parameter->a[1] * Buffer->Output_Butter[1]
            -Parameter->a[2] * Buffer->Output_Butter[0];
  /* x(n) ���б��� */
  Buffer->Input_Butter[0]=Buffer->Input_Butter[1];
  Buffer->Input_Butter[1]=Buffer->Input_Butter[2];
  /* y(n) ���б��� */
  Buffer->Output_Butter[0]=Buffer->Output_Butter[1];
  Buffer->Output_Butter[1]=Buffer->Output_Butter[2];
  return Buffer->Output_Butter[2];
}
