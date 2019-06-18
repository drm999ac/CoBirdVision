#include "stm32f10x.h"
#include "delay.h"
#include "Time_2.h"
#include "DataPara.h"
#include "USART1.h"
#include "USART2.h"
#include "USART3.h"
#include "SimpleDigitalFiltering.h"

SensorData Sensor_Info;
unsigned char Mode_Select = 1;

extern Butter_Parameter US100_Parameter;
extern Butter_BufferData  US100_filter_buf[3];

int main(void)
{
	delay_init(72);																					//延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  														
  TIM2_Configuration();                                   //TIM2 定时配置 
	delay_ms(100);
	USART1_Config(9600);																		//超声波串口
	delay_ms(100);
	USART2_Config(115200);                                  //飞控接口
	delay_ms(100);
	USART3_Config(115200);                      						//Raspberry接口
	delay_ms(100);
	Set_Cutoff_Frequency(60, 10,&US100_Parameter);          //US100巴特沃斯参数初始化


	while(1)
	{
		if(RaspberryLen == 4){
			if(RaspberryBuff[0]==0X55 && RaspberryBuff[1]==0XAA 
			&& RaspberryBuff[2]==0XAA && RaspberryBuff[3]==0X5A){
				Mode_Select = PassMode;
				RaspberryLen = 0;
			}
			else if(RaspberryBuff[0]==0X55 && RaspberryBuff[1]==0XAA 
			&& RaspberryBuff[2]==0XA5 && RaspberryBuff[3]==0XAA){
				Mode_Select = IntegrationMode;
				RaspberryLen = 0;
			}
		}
		else
		{
			if(Mode_Select == PassMode){
				if(RaspberryFinish == 1){
					if(RaspberryLen != 0){
						memcpy(Usart2SendBuff,RaspberryBuff,RaspberryLen);
						Usart2SendData_DMA(Usart2SendBuff,RaspberryLen);
					}
					RaspberryFinish = 0;
				}
						
//			if(US100_Raspberry == 1 && PcFinish != 1){
//					memcpy(Usart3SendBuff,US100buf,2);
//					Usart3SendData_DMA(Usart3SendBuff,2);
//			}
				
			}else if(Mode_Select == IntegrationMode  && SendToFlightFreqflag == 1){
					SendToFlight();
					SendToFlightFreqflag = 0;
			}
			else if(Mode_Select == GeneralMode && SendToFlightFreqflag == 1)
			{
				SendHeightToFlight();
				SendToFlightFreqflag = 0;
			}
		}	
				
		
		if(PcFinish == 1){
			memcpy(Usart3SendBuff,PcBuff,PcLen);
			Usart3SendData_DMA(Usart3SendBuff,PcLen);
			PcFinish = 0;
		}
		if(RecUltraFreqflag == 1){
			//rec ultra data
			RecUltraData();
		  RecUltraFreqflag = 0;
		}		
//		if(RaspberryFreqflag>=20){
//			RaspberryFreqflag = 100;
//			for(int i = 0;i < 9;i ++){
//				Sensor_Info.RaspberryArray[i] = 0;
//			}
//		}		
		
	}
}



//------------------End of File----------------------------
