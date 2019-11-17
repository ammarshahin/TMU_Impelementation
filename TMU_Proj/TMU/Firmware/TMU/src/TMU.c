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

#include "Led.h"

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
static job_Struct Gv_TmuJobsBuffer[MAX_NO_OF_JOBS];
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
	EnmTMUError_t state = OK_t;
	uint32 TimerCount_Time;
	
	/* Initializing the Global variable counter to one and the Zero will indicate that there are no job */
	Gv_JobsCount = ZERO;
	Gv_TimerNumber = ConfigPtr->Timer_no;
	TimerCount_Time = MILLISECONSD_TO_MICROSECONSD_FACTOR * ConfigPtr->Resolution;
	/* init the timer used in the Tmu */
	switch(Gv_TimerNumber)
	{
		case TIMER_0:
			/* Init the Timer Used  */
			Timers_Init(&timer0_cfg_s);
			/* Set the default resolution  */
			Timers_SetCounter(TIMER_0,TimerCount_Time);
			/* Start the Timer Used  */
			Timers_Start(TIMER_0);
			break;
		case TIMER_1:
			/* Init the Timer Used  */
			Timers_Init(&timer1_cfg_s);
			/* Set the default resolution  */
			Timers_SetCounter(TIMER_1,TimerCount_Time);
			/* Start the Timer Used  */
			Timers_Start(TIMER_1);
			break;
		case TIMER_2:
			/* Init the Timer Used  */
			Timers_Init(&timer2_cfg_s);
			
			/* Set the default resolution  */
			Timers_SetCounter(TIMER_2, TimerCount_Time);
			
			/* Start the Timer Used  */
			Timers_Start(TIMER_2);
			break;
		default:
			state = NOT_OK_t;
			break;
	}
	
	return state;
}


/**
 * Function : TMU_Start_Timer
 * Description: this function is to create and start the job
 * @param delay_time: 
					i/p: The delay time that the job needs to wait in milliseconds
 * @param job_Type:
					i/p: The Type of the job >> [JOB_ONCE or JOB_PERIODIC]
 * @param Fun_Pointer: 
					i/p: Pointer to the function to be called at the end of the delay
 * @param Job_ID_Num: 
					o/p: Returns the ID Number with the pointer
 * @return EnmTMUError_t: the status of the function according to the Error handling Enum 
 */
EnmTMUError_t TMU_Start_Timer(uint32 delay_time, uint8 job_Type, v_ptrFun_v callBackFunctionPtr, uint8* Job_ID_Num)
{
	
	EnmTMUError_t state = OK_t;
	uint8 index;
	
	/* check if there is enough space in the Job Stack */
	if( Gv_JobsCount > MAX_NO_OF_JOBS )
	{
		state = NO_ENOUGH_SPACE_IN_STACK;
	} /* check if the delay is a valid delay */
	else if(delay_time > MAX_DELAY_TIME)
	{
		state = DELAY_IS_VERY_LARGE;
	}
	else if( job_Type != JOB_ONCE &&  job_Type != JOB_PERIODIC)
	{
		state = NOT_VALID_JOB_TYPE;
	}
	else
	{
		state = OK_t;
	}
	
	if(state == OK_t)
	{
		for(index = ONE; index < MAX_NO_OF_JOBS ; index++ )
		{
			if(Gv_TmuJobsBuffer[index].job_ID == FALSE)
			{
				Gv_TmuJobsBuffer[index].job_ID = index;
				Gv_TmuJobsBuffer[index].job_Type = job_Type;
				Gv_TmuJobsBuffer[index].job_Time = delay_time;
				Gv_TmuJobsBuffer[index].job_Funptr = callBackFunctionPtr;
				Gv_TmuJobsBuffer[index].job_Passed = ZERO;
				*Job_ID_Num = index;
				Gv_JobsCount++;
				break;
			}
			else
			{
				//Do nothing
			}
		}
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
	EnmTMUError_t state = OK_t;
	uint8 index;
	uint8 found_Flag = FALSE;
	
	
	for(index = ONE; index < MAX_NO_OF_JOBS ; index++ )
	{
		if(Gv_TmuJobsBuffer[index].job_ID == Job_ID_Num)
		{
			Gv_TmuJobsBuffer[index].job_ID = ZERO;
			Gv_TmuJobsBuffer[index].job_Type = FALSE;
			Gv_TmuJobsBuffer[index].job_Time = FALSE;
			Gv_TmuJobsBuffer[index].job_Passed = FALSE;
			Gv_TmuJobsBuffer[index].job_Funptr = NULL;
			Gv_JobsCount--;
			found_Flag = TRUE;
			break;
		}
		else
		{
			//Do nothing
		}

	}
	
	if (found_Flag == FALSE)
	{
		state = NO_JOBS_WITH_THAT_ID;
	}
	else
	{
		//Do nothing
	}
	
	return state;
}	



/**
 * Function : TMU_Dispatcher
 * Description: This function is to come periodically and check for the time passed for every job 
 * @return EnmTMUError_t: the status of the function according to the Error handling Enum 
 */
void TMU_Dispatcher(void) 
{
	uint8 index;
	if(Timer_Flag)
	{
		for(index = ONE; (index <= Gv_JobsCount) && (Gv_TmuJobsBuffer[index].job_ID != ZERO) ; index++ )
		{
				/* Update the Time Passed and check if the time come or not */
				Gv_TmuJobsBuffer[index].job_Passed += DEFAULT_RESELOTION;
			
				if ( Gv_TmuJobsBuffer[index].job_Passed == Gv_TmuJobsBuffer[index].job_Time )
				{
					Gv_TmuJobsBuffer[index].job_Funptr();
					Gv_TmuJobsBuffer[index].job_Passed = ZERO;
					if(Gv_TmuJobsBuffer[index].job_Type == JOB_ONCE)
					{
						TMU_Stop_Timer(Gv_TmuJobsBuffer[index].job_ID);
					}
					else
					{
						// Do Nothing 
					}
				}
				else
				{
					// Do Nothing 
				}
		}
		Timer_Flag = FALSE;
	}
}




/**
 * Function : TMU_DeInit
 * Description: this function is to DeInit the TMU >> it basicly returns the TMU Stack to it's default values
 *                      and stops the timer and returns it to it's default value
 * @return EnmTMUError_t: the status of the function according to the Error handling Enum 
 */
EnmTMUError_t TMU_DeInit(void) 
{
	EnmTMUError_t state = OK_t;
	uint8 index;
	
	/* Stop the Timer and reset it to the default state */
	Timers_Stop(Gv_TimerNumber);
	//Timers_DeInit(Gv_TimerNumber);
	
	for(index = ONE; index < MAX_NO_OF_JOBS ; index++ )
	{
		if(Gv_TmuJobsBuffer[index].job_ID != FALSE)
		{
			Gv_TmuJobsBuffer[index].job_ID = FALSE;
			Gv_TmuJobsBuffer[index].job_Type = FALSE;
			Gv_TmuJobsBuffer[index].job_Time = FALSE;
			Gv_TmuJobsBuffer[index].job_Passed = FALSE;
			Gv_TmuJobsBuffer[index].job_Funptr = NULL;
			Gv_JobsCount--;
		}
		else
		{
			//Do nothing
		}
	}
	
	/* return the Number of jobs back to One */
	Gv_JobsCount = ZERO;
	
	return state;
}