/*
 * Tmu.h
 *
 * Created: 11/16/2019 6:20:53 PM
 *  Author: Ammar Shahin
 */ 



#ifndef TMU_H_
#define TMU_H_


/************************************************************************/
/*                               INCLUDES                               */
/************************************************************************/
#include "Typedefs.h"
#include "Tmu_Lcfg.h"


/************************************************************************/
/*                               CONSTANTS                               */
/************************************************************************/
/* Job periodicity */
#define JOB_ONCE 0
#define JOB_PERIODIC 1

/* the Max Number Of Jobs that can be created */
#define MAX_NO_OF_JOBS 5

/* the Timers Defines Numbers */
#define TIMER_0  0
#define TIMER_1  1
#define TIMER_2  2

/* the default resolution in milliseconds */
#define DEFAULT_RESELOTION 1

/************************************************************************/
/*                               ENUMS                                  */
/************************************************************************/
typedef enum EnmTMUError_t{
	OK_t,
	NO_ENOUGH_SPACE_IN_STACK,
	DELAY_IS_VERY_LARGE,
	NO_JOBS_WITH_THAT_ID,
	NOT_VALID_JOB_TYPE,
	NOT_OK_t,
}EnmTMUError_t;

/************************************************************************/
/*                       STRUCTS AND UNIONS                              */
/************************************************************************/
typedef struct{
	uint8 job_ID;
	uint8 job_Type;
	uint32 job_Time; // in microSeconds
	uint32 job_Passed;
	v_ptrFun_v job_Funptr;
}StrTMU_Job_type;

/************************************************************************/
/*                       FUNCTION PROTOTYPES                           */
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
EnmTMUError_t TMU_Init(const TMU_ConfigType *ConfigPtr );

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
EnmTMUError_t TMU_Start_Timer(uint32 delay_time, uint8 job_Type, v_ptrFun_v callBackFunctionPtr, uint8* Job_ID_Num);

/**
 * Function : TMU_Stop_Timer
 * Description: this function is to Stop the job
 * @param Job_ID_Num: 
					i/p: the ID Number of the job
 * @return EnmTMUError_t: the status of the function according to the Error handling Enum 
 */
EnmTMUError_t TMU_Stop_Timer(uint8 Job_ID_Num);

/**
 * Function : TMU_Dispatcher
 * Description: This function is to come periodically and check for the time passed for every job 
 * @return EnmTMUError_t: the status of the function according to the Error handling Enum 
 */
void TMU_Dispatcher(void);


/**
 * Function : TMU_DeInit
 * Description: this function is to DeInit the TMU >> it basicly returns the TMU Stack to it's default values
 *                      and stops the timer and returns it to it's default value
 * @return EnmTMUError_t: the status of the function according to the Error handling Enum 
 */
EnmTMUError_t TMU_DeInit(void);

/**
 * Function : TMU_SetCallBkFunction
 * Description: This function is used to set the Call Back Function in the TMU
 * @return void
 */
void TMU_SetCallBkFunction(void);
#endif /* TMU_H_ */