/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
/* lwIP includes. */
#define SYS_ARCH_GLOBALS

/* lwIP includes. */
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"
#include "includes.h"
#include "arch/sys_arch.h"


//����Ϣָ��Ϊ��ʱ,ָ��һ������pvNullPointer��ָ���ֵ.
//��UCOS�����OSQPost()�е�msg==NULL�᷵��һ��OS_ERR_POST_NULL
//����,����lwip�л����sys_mbox_post(mbox,NULL)����һ������Ϣ,����
//�ڱ������а�NULL���һ������ָ��0Xffffffff
const void * const pvNullPointer = (mem_ptr_t*)0xffffffff;
 
#define LWIP_ARCH_TICK_PER_MS       1



//  Creates an empty mailbox.
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
  OS_ERR       ucErr;
      
  OSQCreate(mbox,"LWIP quiue", size, &ucErr); 
  LWIP_ASSERT( "OSQCreate ", ucErr == OS_ERR_NONE );
  
  if( ucErr == OS_ERR_NONE){ 
    return 0; 
  }
  return -1;
}

/*-----------------------------------------------------------------------------------*/
/*
  Deallocates a mailbox. If there are messages still present in the
  mailbox when the mailbox is deallocated, it is an indication of a
  programming error in lwIP and the developer should be notified.
*/
void sys_mbox_free(sys_mbox_t *mbox)
{
	OS_ERR     ucErr;
	LWIP_ASSERT( "sys_mbox_free ", mbox != NULL );      
			
	OSQFlush(mbox,& ucErr);
	
	OSQDel(mbox, OS_OPT_DEL_ALWAYS, &ucErr);
	LWIP_ASSERT( "OSQDel ", ucErr == OS_ERR_NONE );
}

/*-----------------------------------------------------------------------------------*/
//   Posts the "msg" to the mailbox.
void sys_mbox_post(sys_mbox_t *mbox, void *data)
{
	OS_ERR     ucErr;
  CPU_INT08U  i=0; 
  if( data == NULL ) data = (void*)&pvNullPointer;
  /* try 10 times */
  while(i<10){
    OSQPost(mbox, data,0,OS_OPT_POST_ALL,&ucErr);
    if(ucErr == OS_ERR_NONE)
      break;
    i++;
    OSTimeDly(5,OS_OPT_TIME_DLY,&ucErr);
  }
  LWIP_ASSERT( "sys_mbox_post error!\n", i !=10 );  
}

/*-----------------------------------------------------------------------------------*/
//   Try to post the "msg" to the mailbox.
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
  OS_ERR     ucErr;
  if(msg == NULL ) msg = (void*)&pvNullPointer;  
  OSQPost(mbox, msg,0,OS_OPT_POST_ALL,&ucErr);    
  if(ucErr != OS_ERR_NONE){
    return ERR_MEM;
  }
  return ERR_OK;
}


//�ȴ������е���Ϣ
//*mbox:��Ϣ����
//*msg:��Ϣ
//timeout:��ʱʱ�䣬���timeoutΪ0�Ļ�,��һֱ�ȴ�
//����ֵ:��timeout��Ϊ0ʱ����ɹ��Ļ��ͷ��صȴ���ʱ�䣬
//		ʧ�ܵĻ��ͷ��س�ʱSYS_ARCH_TIMEOUT
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
  OS_ERR	ucErr;
  OS_MSG_SIZE   msg_size;
  CPU_TS        ucos_timeout;  
  CPU_TS        in_timeout = timeout/LWIP_ARCH_TICK_PER_MS;
  if(timeout && in_timeout == 0)
    in_timeout = 1;
  *msg  = OSQPend (mbox,in_timeout,OS_OPT_PEND_BLOCKING,&msg_size, 
                          &ucos_timeout,&ucErr);

  if ( ucErr == OS_ERR_TIMEOUT ) 
      ucos_timeout = SYS_ARCH_TIMEOUT;  
  return ucos_timeout; 
}

//���Ի�ȡ��Ϣ
//*mbox:��Ϣ����
//*msg:��Ϣ
//����ֵ:�ȴ���Ϣ���õ�ʱ��/SYS_ARCH_TIMEOUT
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
	return sys_arch_mbox_fetch(mbox,msg,1);//���Ի�ȡһ����Ϣ
}
//���һ����Ϣ�����Ƿ���Ч
//*mbox:��Ϣ����
//����ֵ:1,��Ч.
//      0,��Ч
/*----------------------------------------------------------------------------------*/
int sys_mbox_valid(sys_mbox_t *mbox)          
{      
  if(mbox->NamePtr)  
    return (strcmp(mbox->NamePtr,"?Q"))? 1:0;
  else
    return 0;
} 

//����һ����Ϣ����Ϊ��Ч
//*mbox:��Ϣ����
void sys_mbox_set_invalid(sys_mbox_t *mbox)   
{                                             
  if(sys_mbox_valid(mbox))
    sys_mbox_free(mbox);                  
}                                             

//����һ���ź���
//*sem:�������ź���
//count:�ź���ֵ
//����ֵ:ERR_OK,����OK
// 	     ERR_MEM,����ʧ��
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
	OS_ERR	ucErr;
  OSSemCreate (sem,"LWIP Sem",count,&ucErr);
  if(ucErr != OS_ERR_NONE ){
    LWIP_ASSERT("OSSemCreate ",ucErr == OS_ERR_NONE );
    return -1;    
  }
  return 0;
}

//�ȴ�һ���ź���
//*sem:Ҫ�ȴ����ź���
//timeout:��ʱʱ��
//����ֵ:��timeout��Ϊ0ʱ����ɹ��Ļ��ͷ��صȴ���ʱ�䣬
//		ʧ�ܵĻ��ͷ��س�ʱSYS_ARCH_TIMEOUT
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  OS_ERR	ucErr;
  CPU_TS        ucos_timeout;
  CPU_TS        in_timeout = timeout/LWIP_ARCH_TICK_PER_MS;
  if(timeout && in_timeout == 0)
    in_timeout = 1;  
  OSSemPend (sem,in_timeout,OS_OPT_PEND_BLOCKING,&ucos_timeout,&ucErr);
    /*  only when timeout! */
  if(ucErr == OS_ERR_TIMEOUT)
      ucos_timeout = SYS_ARCH_TIMEOUT;	
  return ucos_timeout;
}

//����һ���ź���
//sem:�ź���ָ��
void sys_sem_signal(sys_sem_t *sem)
{
	OS_ERR	ucErr;  
  OSSemPost(sem,OS_OPT_POST_ALL,&ucErr);
  LWIP_ASSERT("OSSemPost ",ucErr == OS_ERR_NONE ); 
}

//�ͷŲ�ɾ��һ���ź���
//sem:�ź���ָ��
void sys_sem_free(sys_sem_t *sem)
{
  OS_ERR     ucErr;
	OSSemDel(sem, OS_OPT_DEL_ALWAYS, &ucErr );
	LWIP_ASSERT( "OSSemDel ", ucErr == OS_ERR_NONE );
}

//��ѯһ���ź�����״̬,��Ч����Ч
//sem:�ź���ָ��
//����ֵ:1,��Ч.
//      0,��Ч
int sys_sem_valid(sys_sem_t *sem)                                               
{
  if(sem->NamePtr)
    return (strcmp(sem->NamePtr,"?SEM"))? 1:0;
  else
    return 0;                                    
}

//����һ���ź�����Ч
//sem:�ź���ָ��
void sys_sem_set_invalid(sys_sem_t *sem)                                        
{                                                                               
  if(sys_sem_valid(sem))
    sys_sem_free(sem);                                                       
} 

//arch��ʼ��
void sys_init(void)
{

}

extern CPU_STK * TCPIP_THREAD_TASK_STK;//TCP IP�ں������ջ,��lwip_comm��������
//����һ���½���
//*name:��������
//thred:����������
//*arg:�����������Ĳ���
//stacksize:��������Ķ�ջ��С
//prio:������������ȼ�
OS_TCB        LwIP_task_TCB;


sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
	OS_ERR      err;
	if(strcmp(name,TCPIP_THREAD_NAME)==0)//����TCP IP�ں�����
	{
		
		CPU_SR_ALLOC();
		CPU_CRITICAL_ENTER();  //�����ٽ��� 
			OSTaskCreate(&LwIP_task_TCB,
							 (CPU_CHAR  *)name,
							 (OS_TASK_PTR)thread, 
							 (void      *)0,
							 (OS_PRIO    )prio,
							 (CPU_STK   *)&TCPIP_THREAD_TASK_STK[0],
							 (CPU_STK_SIZE)stacksize/10,
							 (CPU_STK_SIZE)stacksize,
							 (OS_MSG_QTY )0,
							 (OS_TICK    )0,
							 (void      *)0,
							 (OS_OPT     )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
							 (OS_ERR    *)&err);    
		CPU_CRITICAL_EXIT();  //�˳��ٽ���
	} 
	return 0;
} 

//��ȡϵͳʱ��,LWIP1.4.1���ӵĺ���
//����ֵ:��ǰϵͳʱ��(��λ:����)
u32_t sys_now(void)
{
	OS_TICK os_tick_ctr;
	CPU_SR_ALLOC();
	CPU_CRITICAL_ENTER();
	os_tick_ctr = OSTickCtr;
	CPU_CRITICAL_EXIT();
	return os_tick_ctr;
}
/*
  This optional function does a "fast" critical region protection and returns
  the previous protection level. This function is only called during very short
  critical regions. An embedded system which supports ISR-based drivers might
  want to implement this function by disabling interrupts. Task-based systems
  might want to implement this by using a mutex or disabling tasking. This
  function should support recursive calls from the same task or interrupt. In
  other words, sys_arch_protect() could be called while already protected. In
  that case the return value indicates that it is already protected.

  sys_arch_protect() is only required if your port is supporting an operating
  system.
*/
sys_prot_t sys_arch_protect(void)
{
	CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();
	return 1;
}

/*
  This optional function does a "fast" set of critical region protection to the
  value specified by pval. See the documentation for sys_arch_protect() for
  more information. This function is only required if your port is supporting
  an operating system.
*/
void sys_arch_unprotect(sys_prot_t pval)
{
	CPU_SR_ALLOC();

	LWIP_UNUSED_ARG(pval);
	CPU_CRITICAL_EXIT();
}











