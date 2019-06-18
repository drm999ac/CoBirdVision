#include "USART1.h"

uint8_t Usart1SendBuff[USART1SENDBUFF_SIZE];
uint8_t Usart1RecBuff[USART1RECBUFF_SIZE];
uint8_t Usart1_Dma_SendFlag = 1;

void USART1_Config(unsigned int baudrate)
{
    USART_InitTypeDef USART_InitStruct;  
    GPIO_InitTypeDef GPIO_InitStructure;  
    NVIC_InitTypeDef NVIC_InitStructure;  
    DMA_InitTypeDef DMA_InitStructure;  
    //open clk  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);      
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
  
    /* Configure USART Tx and Rx as alternate function push-pull */  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);  
  
    // Enable the USARTx Interrupt   
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);  
  
    USART_InitStruct.USART_BaudRate = baudrate;  
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;  
    USART_InitStruct.USART_StopBits = USART_StopBits_1;  
    USART_InitStruct.USART_Parity = USART_Parity_No ;  
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    USART_Init(USART1, &USART_InitStruct);        
  
    DMA_DeInit(DMA1_Channel5);  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR); 
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Usart1RecBuff;  
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;       
    DMA_InitStructure.DMA_BufferSize = USART1RECBUFF_SIZE;        
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;         
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;      
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;               
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; 
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;             
    DMA_Init(DMA1_Channel5,&DMA_InitStructure);  
    DMA_Cmd(DMA1_Channel5,ENABLE); 
    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);        
  
    /* DMA1 Channel4 (triggered by USART1 Tx event) Config */  
    DMA_DeInit(DMA1_Channel4);   
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);  
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Usart1SendBuff;  
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
    DMA_InitStructure.DMA_BufferSize = USART1SENDBUFF_SIZE;  
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);  
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);  
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TE, ENABLE);  
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);  
    DMA_Cmd(DMA1_Channel4, DISABLE);  
  
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =3;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);  
  
    USART_Cmd(USART1, ENABLE);  
    USART_ClearFlag(USART1,USART_FLAG_TC);  
}

extern Butter_Parameter US100_Parameter;
extern Butter_BufferData  US100_filter_buf[3];
uint16_t ReceiveHeight;
u8 US100buf[2];
void USART1_IRQHandler(void)  
{  
	static int HeightData[5];
	uint16_t len = 0;
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)   
	{  
		USART1->SR;   
		USART1->DR;
		DMA_Cmd(DMA1_Channel5,DISABLE);  
		len = USART1RECBUFF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5);
		memcpy(US100buf,Usart1RecBuff,len);		
		ReceiveHeight = ((((uint16_t)US100buf[0])<<8) + US100buf[1]);
		if(ReceiveHeight<=3000 && ReceiveHeight>0.0f)
		Sensor_Info.US100_Zaxis = Median_Filter(ReceiveHeight,5,HeightData)/1000;	
//		Sensor_Info.US100_Zaxis=LPButterworth(ReceiveHeight,&US100_filter_buf[0],&US100_Parameter);
//		Sensor_Info.US100_Zaxis = Median_Filter(LPButterworth(ReceiveHeight,&US100_filter_buf[0],&US100_Parameter),5,HeightData)/1000;	
//		Sensor_Info.US100_Zaxis = (float)ReceiveHeight / 1000.0f;
		DMA_SetCurrDataCounter(DMA1_Channel5,USART1RECBUFF_SIZE);   
		DMA_Cmd(DMA1_Channel5,ENABLE); 
	}   
}  

void DMA1_Channel4_IRQHandler(void)  
{  
  DMA_Cmd(DMA1_Channel4, DISABLE);
  USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);  
  DMA1_Channel4->CNDTR = USART1SENDBUFF_SIZE;  
  DMA_ClearITPendingBit(DMA1_IT_GL4);
  Usart1_Dma_SendFlag = 1; 
}  

void Usart1SendData_DMA(uint8_t *pdata, uint16_t Length)  
{  
    while(Usart1_Dma_SendFlag == 0);  
    Usart1_Dma_SendFlag = 0;  
    DMA1_Channel4->CMAR = (uint32_t)pdata;  
    DMA1_Channel4->CNDTR = Length;  
    DMA_Cmd(DMA1_Channel4, ENABLE);  
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);  
} 


void RecUltraData(void){
	Usart1SendBuff[0] = 0x55;
	Usart1SendData_DMA(Usart1SendBuff,USART1SENDBUFF_SIZE);
}




