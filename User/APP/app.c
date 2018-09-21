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


    OSInit(&err);                                                           //��ʼ�� uC/OS-III

	  /* ������ʼ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                            //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Start",                            //��������
                 (OS_TASK_PTR ) AppTaskStart,                               //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_START_PRIO,                        //��������ȼ�
                 (CPU_STK    *)&AppTaskStartStk[0],                         //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,               //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,                    //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������

    OSStart(&err);                                                          //�����������������uC/OS-III���ƣ�
		
		
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

    CPU_Init();                                                 //��ʼ�� CPU �����ʱ��������ж�ʱ���������������
                                                   //�弶��ʼ��

    cpu_clk_freq = BSP_CPU_ClkFreq();                           //��ȡ CPU �ں�ʱ��Ƶ�ʣ�SysTick ����ʱ�ӣ�
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //�����û��趨��ʱ�ӽ���Ƶ�ʼ��� SysTick ��ʱ���ļ���ֵ
    OS_CPU_SysTickInit(cnts);                                   //���� SysTick ��ʼ�����������ö�ʱ������ֵ��������ʱ��

    Mem_Init();                                                 //��ʼ���ڴ������������ڴ�غ��ڴ�ر�

#if OS_CFG_STAT_TASK_EN > 0u                                    //���ʹ�ܣ�Ĭ��ʹ�ܣ���ͳ������
    OSStatTaskCPUUsageInit(&err);                               //����û��Ӧ������ֻ�п�����������ʱ CPU �ģ����
#endif                                                          //���������� OS_Stat_IdleCtrMax ��ֵ��Ϊ������� CPU 
                                                                //ʹ����ʹ�ã���
#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();                                //��λ�����㣩��ǰ�����ж�ʱ��
#endif
    BSP_Init();  
		/* ������������ */
    OSTaskCreate((OS_TCB     *)&AppMutexTestTCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Mutex Test",                             //��������
                 (OS_TASK_PTR ) AppMutexTest,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_MUTEX_TEST_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppMutexTestStk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_MUTEX_TEST_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_MUTEX_TEST_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������

			/* ������������ */
    OSTaskCreate((OS_TCB     *)&AppMutexTestTCB1,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Mutex Test1",                             //��������
                 (OS_TASK_PTR ) AppMutexTest1,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_MUTEX_TEST_PRIO1,                         //��������ȼ�
                 (CPU_STK    *)&AppMutexTestStk1[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_MUTEX_TEST_STK_SIZE1 / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_MUTEX_TEST_STK_SIZE1,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
	//�����ź���

	OSMutexCreate(&p_mutex,(char *)"test mutex",&err);
	OSQCreate(&p_q, (CPU_CHAR *)"queue test", 20, &err);
	//�����ʱ��
	OSTmrCreate(&my_tmr,"soft tmr",10,10,OS_OPT_TMR_PERIODIC,my_callback,(void *)"time over",&err);
	OSTmrStart(&my_tmr,&err);

	

	OSTaskDel ( &AppTaskStartTCB, &err );                     //ɾ����ʼ������������������
		
		
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
	{                                    //�����壬ͨ����д��һ����ѭ��    
		OSMutexPend(&p_mutex, 0, OS_OPT_PEND_BLOCKING, 0, &err);
		ucValue[0] ++;
		OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
		ucValue[1] ++;
		pMsg = OSQPend(&p_q, 0, OS_OPT_PEND_BLOCKING, &msg_size, 0, &err);
		if(err == OS_ERR_NONE && pMsg != NULL)
		{
			//LOG("��Ϣ����: %d�ֽ�, ����: %s",msg_size,pMsg);
		}
		OSMutexPost(&p_mutex, OS_OPT_POST_NONE, &err);
	}
		
}


static  void  AppMutexTest1 ( void * p_arg )
{
	OS_ERR           err;

	
 	(void)p_arg;
	while (DEF_TRUE) {                                    //�����壬ͨ����д��һ����ѭ��    
	
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




