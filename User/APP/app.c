/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                             (c) Copyright 2013; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                       IAR Development Kits
*                                              on the
*
*                                    STM32F429II-SK KICKSTART KIT
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : YS
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
OS_MUTEX p_mutex;
OS_Q p_q;

uint8_t ucValue [ 2 ] = { 0x00, 0x00 };

/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;

static  OS_TCB   AppMutexTestTCB;

static  OS_TCB   AppMutexTestTCB1;

/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];

static  CPU_STK  AppMutexTestStk [ APP_MUTEX_TEST_STK_SIZE ];


static  CPU_STK  AppMutexTestStk1 [ APP_MUTEX_TEST_STK_SIZE1 ];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);

static  void  AppMutexTest   ( void * p_arg );
static  void  AppMutexTest1 ( void * p_arg );


static void my_callback(void *p_tmr, void *p_arg);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int  main (void)
{
    OS_ERR  err;


    OSInit(&err);                                                           //初始化 uC/OS-III

	  /* 创建起始任务 */
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                            //任务控制块地址
                 (CPU_CHAR   *)"App Task Start",                            //任务名称
                 (OS_TASK_PTR ) AppTaskStart,                               //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_START_PRIO,                        //任务的优先级
                 (CPU_STK    *)&AppTaskStartStk[0],                         //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,               //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,                    //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型

    OSStart(&err);                                                          //启动多任务管理（交由uC/OS-III控制）
		
		
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/





static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;
	OS_TMR      my_tmr;


   (void)p_arg;

    CPU_Init();                                                 //初始化 CPU 组件（时间戳、关中断时间测量和主机名）
                                                   //板级初始化

    cpu_clk_freq = BSP_CPU_ClkFreq();                           //获取 CPU 内核时钟频率（SysTick 工作时钟）
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //根据用户设定的时钟节拍频率计算 SysTick 定时器的计数值
    OS_CPU_SysTickInit(cnts);                                   //调用 SysTick 初始化函数，设置定时器计数值和启动定时器

    Mem_Init();                                                 //初始化内存管理组件（堆内存池和内存池表）

#if OS_CFG_STAT_TASK_EN > 0u                                    //如果使能（默认使能）了统计任务
    OSStatTaskCPUUsageInit(&err);                               //计算没有应用任务（只有空闲任务）运行时 CPU 的（最大）
#endif                                                          //容量（决定 OS_Stat_IdleCtrMax 的值，为后面计算 CPU 
                                                                //使用率使用）。
#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();                                //复位（清零）当前最大关中断时间
#endif
    BSP_Init();  
		/* 创建测试任务 */
    OSTaskCreate((OS_TCB     *)&AppMutexTestTCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Mutex Test",                             //任务名称
                 (OS_TASK_PTR ) AppMutexTest,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_MUTEX_TEST_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppMutexTestStk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_MUTEX_TEST_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_MUTEX_TEST_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型

			/* 创建测试任务 */
    OSTaskCreate((OS_TCB     *)&AppMutexTestTCB1,                             //任务控制块地址
                 (CPU_CHAR   *)"App Mutex Test1",                             //任务名称
                 (OS_TASK_PTR ) AppMutexTest1,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_MUTEX_TEST_PRIO1,                         //任务的优先级
                 (CPU_STK    *)&AppMutexTestStk1[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_MUTEX_TEST_STK_SIZE1 / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_MUTEX_TEST_STK_SIZE1,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
	//互斥信号量

	OSMutexCreate(&p_mutex,(char *)"test mutex",&err);
	OSQCreate(&p_q, (CPU_CHAR *)"queue test", 20, &err);
	//软件定时器
	OSTmrCreate(&my_tmr,"soft tmr",10,10,OS_OPT_TMR_PERIODIC,my_callback,(void *)"time over",&err);
	OSTmrStart(&my_tmr,&err);

	

	OSTaskDel ( &AppTaskStartTCB, &err );                     //删除起始任务本身，该任务不再运行
		
		
}


/*
*********************************************************************************************************
*                                          TEST TASK
*********************************************************************************************************
*/

static  void  AppMutexTest ( void * p_arg )
{

	OS_ERR   err;
	char *pMsg = NULL;
	OS_MSG_SIZE msg_size;
	(void)p_arg;
	
	while (DEF_TRUE) 
	{                                    //任务体，通常都写成一个死循环    
		OSMutexPend(&p_mutex, 0, OS_OPT_PEND_BLOCKING, 0, &err);
		ucValue[0] ++;
		OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
		ucValue[1] ++;
		pMsg = OSQPend(&p_q, 0, OS_OPT_PEND_BLOCKING, &msg_size, 0, &err);
		if(err == OS_ERR_NONE && pMsg != NULL)
		{
			//LOG("消息长度: %d字节, 内容: %s",msg_size,pMsg);
		}
		OSMutexPost(&p_mutex, OS_OPT_POST_NONE, &err);
	}
		
}


static  void  AppMutexTest1 ( void * p_arg )
{
	OS_ERR           err;

	
 	(void)p_arg;
	while (DEF_TRUE) {                                    //任务体，通常都写成一个死循环    
	
		OSMutexPend(&p_mutex, 0, OS_OPT_PEND_BLOCKING, 0, &err);
		if(err == OS_ERR_NONE)
		if(ucValue[0] == ucValue[1])
		{  
			//LOG("successful, ucValue0 is %d,ucValue1 is %d",ucValue[0],ucValue[1]);
			OSQPost(&p_q,(void *)"test for queue !",sizeof("test for queue !"),OS_OPT_POST_ALL|OS_OPT_POST_FIFO|OS_OPT_POST_NO_SCHED,&err);
		}
		else
		{
			LOG("failed, ucValue0 is %d,ucValue1 is %d",ucValue[0],ucValue[1]);
		}
		OSTimeDly(1000,OS_OPT_TIME_DLY, &err);
		OSMutexPost(&p_mutex, OS_OPT_POST_NONE, &err);
	}
		
		
}



static void my_callback(void *p_tmr, void *p_arg)
{
	
	LED4_TOGGLE;
}




