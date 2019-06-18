#ifndef __SIMPLEDIGITALFILTERING_H
#define __SIMPLEDIGITALFILTERING_H

#include "stm32f10x.h"
#include <math.h>

typedef struct
{
 float Input_Butter[3];
 float Output_Butter[3];
}Butter_BufferData;


typedef struct
{
  float a[3];
  float b[3];
}Butter_Parameter;

float Average_Filter(float FilterData,uint16_t Length,float *FilterArray);

float Median_Filter(int data,int measureNum,int *Filterdata);

float Limits_data(float LimitsData,float LimitsUp,float LimitsDown);

void Set_Cutoff_Frequency(float sample_frequent, float cutoff_frequent,Butter_Parameter *LPF);

float LPButterworth(float curr_input,Butter_BufferData *Buffer,Butter_Parameter *Parameter);

#endif 

