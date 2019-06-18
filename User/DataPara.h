#ifndef __DATAPARA_H
#define __DATAPARA_H

#include "stm32f10x.h"

#define USART1SENDBUFF_SIZE 64
#define USART1RECBUFF_SIZE 64
#define USART2SENDBUFF_SIZE 64
#define USART2RECBUFF_SIZE 64
#define USART3SENDBUFF_SIZE 64
#define USART3RECBUFF_SIZE 64

typedef struct{
  float US100_Zaxis;
	u8 RaspberryArray[9];
}SensorData;

typedef enum{
	GeneralMode = 1,
	PassMode = 2,
	IntegrationMode =3
}ModeSelect;

typedef union{
	float fvalue;
	unsigned char cv[4];
}float_union;

extern u8 PcBuff[64];
extern uint16_t PcLen;
extern unsigned char PcFinish;

extern u8 RaspberryBuff[64];
extern uint16_t RaspberryLen;
extern unsigned char RaspberryFinish;
extern unsigned char US100_Raspberry;
extern u8 US100buf[2];

extern unsigned char Mode_Select;

#endif

