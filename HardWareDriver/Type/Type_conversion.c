/******************** (C) COPYRIGHT 2015-2017 Xiluna Tech ************************
 * ���� 	:Xiluna Tech
 * �ļ��� :Type_conversion.c
 * ����   :��������ת��
 * ����   :http://xiluna.com/
 * ���ں� :XilunaTech
**********************************************************************************/
#include "Type_conversion.h"


/* ��������ʮ������ת�� */
/* floatNum�������ͱ���  */
/* byteArry���ֽ��4���ֽڵ��׵�ַ  */
void FloatToByte(float floatNum,unsigned char* byteArry)
{
	int i;
	char* pchar=(char*)&floatNum;
	for(i=0;i<sizeof(float);i++)
	{
		*byteArry=*pchar;
		pchar++;
		byteArry++;
	}
}

void arrycat(u8 *dst,u8 index,u8 *src,u8 len)
{
	u8 i=0;
	for(i=0;i<len;i++)
	{
		*(dst+index+i)=*(src+i);
	}
}


