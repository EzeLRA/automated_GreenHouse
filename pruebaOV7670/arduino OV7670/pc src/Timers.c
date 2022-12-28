/********************************************************************************************/
/*		Project		   :	Arduino Camera using OV7670   			                        */
/*		Filename	   :	Timers.c				                                        */
/*		Functionality  :    Timer Utilities            			                            */
/*		Author		   :	TBE                                        					    */
/********************************************************************************************/

/********************************************************************************************/
/*                          Includes	                                                    */
/********************************************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h> 
#include <linux/watchdog.h>

#include "SystemDefines.h"
/********************************************************************************************/
/*                          MACROS		                                            */
/********************************************************************************************/
#define ONESEC_TIME_CNT	100

#define CLOCKID 	CLOCK_REALTIME
#define SIG 		SIGRTMIN

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
			} while (0)

/********************************************************************************************/
/*                          Extern Variable	                                            */
/********************************************************************************************/
extern unsigned char ts_timer;
/********************************************************************************************/
/*                          Global Variable	                                            */
/********************************************************************************************/
static timer_t timerid;
int ProcessDelay;

/********************************************************************************************/
/*                          Function Prototypes	                                            */
/********************************************************************************************/
static void ProcessTimerHandler(int sig, siginfo_t *si, void *uc);
static void ConfigureProcessTimer(void);
/********************************************************************************************/
/*                          Function Defines	                                            */
/********************************************************************************************/
static void ProcessTimerHandler(int sig, siginfo_t *si, void *uc)
{
	// To take care of timers required in some processes..
	static unsigned int  SecCnt=0;
	int i=0,j=0;

	sig=0;
	si=NULL;
	uc=NULL;

	if(SecCnt == 0)
	{
		//printf("Timer Alive\n");

		SecCnt = ONESEC_TIME_CNT;
	}
	else
	{
		SecCnt--;
        if(ts_timer)
        {
            ts_timer--;
        }

	}    
}

static void ConfigureProcessTimer(void)
{

	struct sigevent sev;
	struct itimerspec its;
	long long freq_nanosecs;
	//sigset_t mask;
	struct sigaction sa;


	/* Establish handler for timer signal */

	printf("TIMER:Establishing handler for signal %d\n", SIG);
	

	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = ProcessTimerHandler;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIG, &sa, NULL) == -1)
	{
		errExit("sigaction");
	}

	/* Create the timer */

	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIG;
	sev.sigev_value.sival_ptr = &timerid;
	if (timer_create(CLOCKID, &sev, &timerid) == -1)
	{
		errExit("timer_create");
	}

	printf("TIMER:ID is 0x%lx\n", (long) timerid);

	/* Start the timer */

	freq_nanosecs = 10000000;
	its.it_value.tv_sec = freq_nanosecs / 1000000000;
	its.it_value.tv_nsec = freq_nanosecs % 1000000000;
	its.it_interval.tv_sec = its.it_value.tv_sec;
	its.it_interval.tv_nsec = its.it_value.tv_nsec;

	if (timer_settime(timerid, 0, &its, NULL) == -1)
	{
		errExit("timer_settime");
	}
	
	
}

void ProcessTimerUnInit(void)
{
	timer_delete(timerid);
}

void ConfigureTimers(void)
{
	ConfigureProcessTimer();
}

