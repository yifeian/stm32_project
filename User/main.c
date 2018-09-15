/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ��1.5.1�汾�⽨�Ĺ���ģ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F429 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
	
#include "stm32f4xx.h"
#include "bsp_led.h"



static void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main ( void )
{	
	LED_GPIO_Config ();	          //��ʼ�� LED
 

	while ( 1 )
	{
		/*
		LED1_ON;			  // ��
		Delay ( 0x0FFFFFF );
		LED1_OFF;		    // ��

		LED2_ON;			  // ��
		Delay ( 0x0FFFFFF );
		LED2_OFF;		    // ��

		LED3_ON;			  // ��
		Delay ( 0x0FFFFFF );
		LED3_OFF;		    // ��
		*/
		LED4(1);
		Delay ( 0x0FFFFFF );
		LED4(0);
		Delay ( 0x0FFFFFF );
		
	}

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
