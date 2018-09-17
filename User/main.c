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
#include "bsp_SysTick.h"
#include "bsp_debug_usart.h"
#include "stdlib.h"
#include "bsp_sdram.h"   
#include "malloc.h"	
#include <stdio.h>

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main ( void )
{	
	uint8_t *p_in = NULL;
	uint8_t *p_out = NULL;
	LED_GPIO_Config ();	          //��ʼ�� LED
 	SysTick_Init();
	Debug_USART_Config();
	SDRAM_Init();
	Delay_ms(1000);
	LOG("MEM TEST START\r\n");
	p_in = mymalloc(0,5*1024);
	p_out = mymalloc(1,100*1024);
	if(p_in == NULL || p_out == NULL)
	{
		LOG("MALLOC fail \r\n");
	}
	else
	{
		snprintf((char*)p_in,80,"%s","this is a test for mem malloc SRAMIN");
		snprintf((char*)p_out,80,"%s","this is a test for mem malloc SRAMEX");
	}
	LOG("%s \r\n",p_in);
	LOG("%s \r\n",p_out);
	LOG("SRAMIN USED:%d%%\r\n", my_mem_perused(SRAMIN));
	LOG("SRAMEX USED:%d%%\r\n", my_mem_perused(SRAMEX));

	myfree(SRAMIN,p_in);
	myfree(SRAMEX,p_out);

		LOG("release successful\r\n");
		LOG("SRAMIN USED:%d%%\r\n", my_mem_perused(SRAMIN));
		LOG("SRAMEX USED:%d%%\r\n", my_mem_perused(SRAMEX));

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
