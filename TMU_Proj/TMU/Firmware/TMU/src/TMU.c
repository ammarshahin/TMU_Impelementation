/*
 * Tmu.c
 *
 * Created: 11/16/2019 6:20:53 PM
 *  Author: Ammar Shahin
 */ 


/************************************************************************/
/*                               INCLUDES                               */
/************************************************************************/
#include "Typedefs.h"
#include "Tmu.h"
#include "Tmu_Lcfg.h"
#include "Timers_Lcfg.h"
#include "Timers.h"

/************************************************************************/
/*                               LOCAL MACROS                           */
/************************************************************************/
#define ZERO 0
#define ONE 1
/* the max delay time by timer 1 assuming 8 mhz freq and 1024 Prescaler in milliseconds*/
#define MAX_DELAY_TIME 8000000



/************************************************************************/
/*                     LOCAL FUNCTIONS PROTOTYPES                       */
/************************************************************************/



/************************************************************************/
/*                       GLOBAL STATIC VARIABLES                        */
/************************************************************************/
static job_Struct Gv_TmuJobsBuffer[MAX_NO_OF_JOBS]
static uint8 Gv_JobsCount;
static uint8 Gv_TimerNumber;


/************************************************************************/
/*                       GLOBAL EXTERN VARIABLES                        */
/************************************************************************/



/************************************************************************/
/*                       LOCAL FUNCTIONS IMPLEMENTATION                 */
/************************************************************************/



/************************************************************************/
/*                           APIs IMPLEMENTATION                        */
/************************************************************************/
/**
 * Function : Tmu_Init
 * Description: this function is to initialize the Timer management unit 
 * @param ConfigPtr pointer to the Configuration Structure which include:
 * 								1- Timer_no: the Channel of the Timer(Timer0,Timer1,Timer2)
 *								2- the Resolution of the Tmu which is the smallest delay that can be done in milliseconds
 * 								3- the Max Number Of Jobs that can be created 
 * @return Status: of the initialization according to the Error handling macros 
 */
EnmTMUError_t TMU_Init(const TMU_ConfigType *ConfigPtr ) 
{
	EnmTMUError_t state = OK;
	uint32 TimerCount_Time;
	
	/* Initializing the Global variable counter to one and the Zero will indicate that there are no job */
	Gv_JobsCount = ONE;
	Gv_TimerNumber = ConfigPtr->Timer_no;
	/* init the timer used in the Tmu */
	switch(Gv_TimerNumber)
	{ #define  
		case TIMER_0:
			/* Init the Timer Used  */
			Timers_Init(&timer0_cfg_s);
			/* Set the default resolution  */
			TimerCount_Time = MILLISECONSDS_TO_MICROSECONSDS_FACTOR * ConfigPtr->Resolution;
			Timers_SetCounter(TIMER_0,TimerCount_Time);
			/* Start the Timer Used  */
			Timers_Start(TIMER_0);
			break;
		case TIMER_1:
			/* Init the Timer Used  */
			Timers_Init(&timer1_cfg_s);
			/* Set the default resolution  */
			TimerCount_Time = MILLISECONSDS_TO_MICROSECONSDS_FACTOR * ConfigPtr->Resolution;
			Timers_SetCounter(TIMER_1,TimerCount_Time);
			/* Start the Timer Used  */
			Timers_Start(TIMER_1);
			break;
		case TIMER_2:
			/* Init the Timer Used  */
			Timers_Init(&timer2_cfg_s);
			
			/* Set the default resolution  */
			TimerCount_Time = MILLISECONSDS_TO_MICROSECONSDS_FACTOR * ConfigPtr->Resolution;
			Timers_SetCounter(TIMER_2, TimerCount_Time);
			
			/* Start the Timer Used  */
			Timers_Start(TIMER_2);
			break;
		default:
			state = NOT_OK;
			break;
	}
	
	return state;
}


/**
 * Function : TMU_Start_Timer
 * Description: this function is to create and start the job
 * @param delay_time: 
					i/p: The delay time that the job needs to wait in milliseconds
 * @param Fun_Pointer: 
					i/p: Pointer to the function to be called at the end of the delay
 * @param Job_ID_Num: 
					o/p: Returns the ID Number with the pointer
 * @return EnmTMUError_t: the status of the function according to the Error handling Enum 
 */
EnmTMUError_t TMU_Start_Timer(uint32 delay_time,v_ptrFun_v callBackFunctionPtr,uint8* Job_ID_Num)
{
	
	EnmTMUError_t state = OK;
	
	/* test if there is enough space in the Job Stack */
	if( Gv_JobsCount > MAX_NO_OF_JOBS )
	{
		state = NO_ENOUGH_SPACE_IN_STACK;
	}
	else if(delay_time > MAX_DELAY_TIME)
	{
		state = DELAY_IS_VERY_LARGE;
	}
	else
	{
		Gv_TmuJobsBuffer[Gv_JobsCount].job_ID = Gv_JobsCount;
		Gv_TmuJobsBuffer[Gv_JobsCount].job_time = delay_time;
		Gv_TmuJobsBuffer[Gv_JobsCount].job_Funptr = callBackFunctionPtr;
		*Job_ID_Num = Gv_JobsCount;
		Gv_JobsCount++;
	}
	return state;
}



/**
 * Function : TMU_Stop_Timer
 * Description: this function is to Stop the job
 * @param Job_ID_Num: 
					i/p: the ID Number of the job
 * @return EnmTMUError_t: the status of the function according to the Error handling Enum 
 */
EnmTMUError_t TMU_Stop_Timer(uint8 Job_ID_Num)
{
	EnmTMUError_t state = OK;
	uint8 index = ZERO;
	uint8 found_Flag = FALSE;
	
	/* Check for possible errors */
	while(index < Gv_JobsCount)
	{
		if( Job_ID_Num == Gv_TmuJobsBuffer[index].job_ID )
		{
			found_Flag = TRUE;
			break;
		}
		else
		{
			index++;
		}
	}
	
	if (found_Flag == FALSE)
	{
		state = NO_JOBS_WITH_THAT_ID;
	}
	else
	{
		Gv_TmuJobsBuffer[index].job_ID     = 0;
		Gv_TmuJobsBuffer[index].job_time   = 0;
		Gv_TmuJobsBuffer[index].job_Funptr = NULL;
	}
	
	return state;
}	





/**
 * Function : TMU_DeInit
 * Description: this function is to DeInit the TMU >> it basicly returns the TMU Stack to it's default values
 *                      and stops the timer and returns it to it's default value
 * @return EnmTMUError_t: the status of the function according to the Error handling Enum 
 */
EnmTMUError_t TMU_DeInit(void) 
{
	EnmTMUError_t state = OK;
	uint8 index;
	
	/* Stop the Timer and reset it to the default state */
	Timers_Stop(Gv_TimerNumber);
	Timers_DeInit(Gv_TimerNumber);
	
	for(index = 1 ; index < Gv_JobsCount ; index++)
	{
		Gv_TmuJobsBuffer[index].job_ID     = 0;
		Gv_TmuJobsBuffer[index].job_time   = 0;
		Gv_TmuJobsBuffer[index].job_Funptr = NULL;
	}
	/* return the Number of jobs back to One */
	Gv_JobsCount = ONE;
	
	return OK;
}