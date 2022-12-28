/********************************************************************************************/
/*		Project		   :	Arduino Camera using OV7670   			                        */
/*		Filename	   :	Main.c					                                        */
/*		Functionality  :    Main File               			                            */
/*		Author		   :	TBE                                        					    */
/********************************************************************************************/

/********************************************************************************************/
/*                          Includes	                                                    */
/********************************************************************************************/
#include <stdio.h>
#include <sys/types.h> 
#include <string.h>
#include <stdlib.h>
#include <signal.h>


#include "SystemDefines.h"

/********************************************************************************************/
/*                          Function Definition                                             */
/********************************************************************************************/

int main(int argc, char **argv) 
{
	char buffer[256];
    
	ConfigureTimers();	
	ConfigureSerialPort();
    sock_thirdstream_thread();
	while(1)
	{
		SerialProcess();
        sock_thirdstream_process();
	}        

    return 0; 
}


