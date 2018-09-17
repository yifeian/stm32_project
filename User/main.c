/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   用1.5.1版本库建的工程模板
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
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
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main ( void )
{	
	uint8_t *p_in = NULL;
	uint8_t *p_out = NULL;
	LED_GPIO_Config ();	          //初始化 LED
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
