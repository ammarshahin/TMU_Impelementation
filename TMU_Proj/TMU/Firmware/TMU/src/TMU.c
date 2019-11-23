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
#include "Timers_Lcfg.h"
#include "Timers.h"
#include "Tmu.h"
#include "Tmu_Lcfg.h"

/************************************************************************/
/*                               LOCAL MACROS                           */
/************************************************************************/
#define INITIAL_VALUE_ZERO 0
#define INITIAL_VALUE_ONE 1

/* the max delay time by timer 1 assuming 8 mhz freq and 1024 Prescaler in milliseconds*/
#define MAX_DELAY_TIME 8000000

/************************************************************************/
/*                       GLOBAL STATIC VARIABLES                        */
/************************************************************************/
static StrTMU_Job_type gStrTMU_JobsBuffer[MAX_NO_OF_JOBS];
static uint8 gJobsCount;
static uint8 gTimerNumber;
static uint8 gTimer_Flag;

/************************************************************************/
/*                           APIs IMPLEMENTATION                        */
/************************************************************************/
/**
 * Function : Tmu_Init
 * Description: this function is to initialize the Timer management unit 
 * @param ConfigPtr pointer to the Configuration Structure which include:
 * 								1- Timer_no: the Channel of the Timer(Timer0,Timer1,Timer2)
 *								2- the Resolution of the Tmu which is the smallest delay that can be dINITIAL_VALUE_ONE in milliseconds
 * 								3- the Max Number Of Jobs that can be created 
 * @return Status: of the initialization according to the Error handling macros 
 */
EnmTMUError_t TMU_Init(const TMU_ConfigType *ConfigPtr ) 
{
	EnmTMUError_t state = OK_t;
	uint32 TimerCount_Time;
	uint8 index;
	/* Initializing the Global variable counter to INITIAL_VALUE_ONE and the INITIAL_VALUE_ZERO will indicate that there are no job */
	gJobsCount = INITIAL_VALUE_ZERO;
	gTimerNumber = ConfigPtr->Timer_no;
	gTimer_Flag = INITIAL_VALUE_ZERO;
	
	/* Set the Resolution of the Timer ISR */
	TimerCount_Time = MILLISECONSD_TO_MICROSECONSD_FACTOR * ConfigPtr->Resolution;
	
	
	/* Initialize the Buffer used in the TMU */
	for(index = INITIAL_VALUE_ZERO; index < MAX_NO_OF_JOBS ; index++ )
	{
			gStrTMU_JobsBuffer[index].job_ID = INITIAL_VALUE_ZERO;
			gStrTMU_JobsBuffer[index].job_Time = INITIAL_VALUE_ZERO;
			gStrTMU_JobsBuffer[index].job_Funptr = NULL;
			gStrTMU_JobsBuffer[index].job_Passed = INITIAL_VALUE_ZERO;
	}
	
	/* Set the timer Call Back Value */
	Timers_SetCallBack(TMU_SetCallBkFunction);
	
	/* Initialize the Timer used in the TMU */
	switch(gTimerNumber)
	{
		case TIMER_0:
			/* Initialize the Timer Used  */
			Timers_Init(&timer0_cfg_s);
			
			/* Set the default resolution  */
			Timers_SetCounter(TIMER_0,TimerCount_Time);
			
			/* Start the Timer Used  */
			Timers_Start(TIMER_0);
			break;
		case TIMER_1:
			/* Initialize the Timer Used  */
			Timers_Init(&timer1_cfg_s);
			
			/* Set the default resolution  */
			Timers_SetCounter(TIMER_1,TimerCount_Time);
			
			/* Start the Timer Used  */
			Timers_Start(TIMER_1);
			break;
		case TIMER_2:
			/* Initialize the Timer Used  */
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
	if( gJobsCount > MAX_NO_OF_JOBS )
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
		for(index = INITIAL_VALUE_ONE; index < MAX_NO_OF_JOBS ; index++ )
		{
			if(gStrTMU_JobsBuffer[index].job_ID == FALSE)
			{
				gStrTMU_JobsBuffer[index].job_ID = index;
				gStrTMU_JobsBuffer[index].job_Type = job_Type;
				gStrTMU_JobsBuffer[index].job_Time = delay_time;
				gStrTMU_JobsBuffer[index].job_Funptr = callBackFunctionPtr;
				gStrTMU_JobsBuffer[index].job_Passed = INITIAL_VALUE_ZERO;
				*Job_ID_Num = index;
				gJobsCount++;
				break;
			}
			else
			{
				//Do nothing
			}
		}
	}
	else
	{
		/* Do Nothing */
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
	
	
	for(index = INITIAL_VALUE_ONE; index < MAX_NO_OF_JOBS ; index++ )
	{
		if(gStrTMU_JobsBuffer[index].job_ID == Job_ID_Num)
		{
			gStrTMU_JobsBuffer[index].job_ID = INITIAL_VALUE_ZERO;
			gStrTMU_JobsBuffer[index].job_Type = FALSE;
			gStrTMU_JobsBuffer[index].job_Time = FALSE;
			gStrTMU_JobsBuffer[index].job_Passed = FALSE;
			gStrTMU_JobsBuffer[index].job_Funptr = NULL;
			gJobsCount--;
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
	if(gTimer_Flag)
	{
		for(index = INITIAL_VALUE_ONE; (index <= gJobsCount) && (gStrTMU_JobsBuffer[index].job_ID != INITIAL_VALUE_ZERO) ; index++ )
		{
				/* Update the Time Passed and check if the time come or not */
				gStrTMU_JobsBuffer[index].job_Passed += DEFAULT_RESELOTION;
			
				if ( gStrTMU_JobsBuffer[index].job_Passed == gStrTMU_JobsBuffer[index].job_Time )
				{
					gStrTMU_JobsBuffer[index].job_Funptr();
					gStrTMU_JobsBuffer[index].job_Passed = INITIAL_VALUE_ZERO;
					if(gStrTMU_JobsBuffer[index].job_Type == JOB_ONCE)
					{
						TMU_Stop_Timer(gStrTMU_JobsBuffer[index].job_ID);
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
		gTimer_Flag = FALSE;
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
	Timers_Stop(gTimerNumber);
	//Timers_DeInit(gTimerNumber);
	
	for(index = INITIAL_VALUE_ONE; index < MAX_NO_OF_JOBS ; index++ )
	{
		if(gStrTMU_JobsBuffer[index].job_ID != FALSE)
		{
			gStrTMU_JobsBuffer[index].job_ID = FALSE;
			gStrTMU_JobsBuffer[index].job_Type = FALSE;
			gStrTMU_JobsBuffer[index].job_Time = FALSE;
			gStrTMU_JobsBuffer[index].job_Passed = FALSE;
			gStrTMU_JobsBuffer[index].job_Funptr = NULL;
			gJobsCount--;
		}
		else
		{
			//Do nothing
		}
	}
	
	/* return the Number of jobs back to INITIAL_VALUE_ONE */
	gJobsCount = INITIAL_VALUE_ZERO;
	
	return state;
}

/**
 * Function : TMU_SetCallBkFunction
 * Description: This function is used to set the Call Back Function in the TMU
 * @return void
 */
void TMU_SetCallBkFunction(void)
{
	gTimer_Flag = TRUE;
}