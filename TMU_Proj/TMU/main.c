/*
 * TMU.c
 *
 * Created: 11/16/2019 5:36:09 PM
 * Author : AVE-LAP-44
 */ 

/************************************************************************/
/*                               INCLUDES                               */
/************************************************************************/
#include "Tmu.h"
#include "Tmu_Lcfg.h"
#include "Led.h"
#include "MCU_Frequency.h"

/************************************************************************/
/*                        Functions Prototypes                          */
/************************************************************************/
void led0_Tog(void);
void led1_Tog(void);
void led2_Tog(void);
void led3_Tog(void);

/************************************************************************/
/*                      Functions Implementations                       */
/************************************************************************/
int main(void)
{
	uint8 job_ID0,job_ID1,job_ID2,job_ID3;
	Led_Init(LED0);
	Led_Init(LED1);
	Led_Init(LED2);
	Led_Init(LED3);
	TMU_Init(&tmu_cfg_s);
    TMU_Start_Timer(10,JOB_PERIODIC,&led0_Tog,&job_ID0);
	TMU_Start_Timer(20,JOB_PERIODIC,&led1_Tog,&job_ID1);
	TMU_Start_Timer(300,JOB_PERIODIC,&led2_Tog,&job_ID2);
	TMU_Start_Timer(4000,JOB_PERIODIC,&led3_Tog,&job_ID3);
    while (TRUE) 
    {
		TMU_Dispatcher();
    }
}

void led0_Tog(void)
{
	Led_Tog(LED0);
}

void led1_Tog(void)
{
	Led_Tog(LED1);
}

void led2_Tog(void)
{
	Led_Tog(LED2);
}

void led3_Tog(void)
{
	Led_Tog(LED3);
}