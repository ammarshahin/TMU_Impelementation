/*
 * Tmu_Lcfg.h
 *
 * Created: 11/16/2019 6:22:21 PM
 *  Author: Ammar Shahin
 */ 


#ifndef TMU_LCFG_H_
#define TMU_LCFG_H_

/************************************************************************/
/*                               INCLUDES                               */
/************************************************************************/
#include "Typedefs.h"
#include "Tmu.h"

/************************************************************************/
/*                        Constants and Macros                          */
/************************************************************************/
/* the Timers Defines Numbers */
#define TIMER_0  0
#define TIMER_1  1
#define TIMER_2  2

/* the Max Number Of Jobs that can be created */
#define MAX_NO_OF_JOBS 100


/************************************************************************/
/*                       STRUCTS AND UNIONS                              */
/************************************************************************/
/************** Post Build Structure **************/

typedef struct TMU_ConfigType{
	uint8 Timer_no;
	uint8 Resolution;
	uint8 MaxNoOfJobs;
}TMU_ConfigType;

/************************************************************************/
/*                       Extern variables                               */
/************************************************************************/
extern TMU_ConfigType tmu_cfg_s;

#endif /* TMU_LCFG_H_ */