/********************************************************************************************/
/*		Project		:	HAT Network to Serial Interface			    */
/*		Filename	:	SystemDefines.h					    */
/*		Functionality	:	System Header File				    */
/*		Author		:	Maheen Rasheed					    */
/*		Mediatronix Pvt Ltd,Pappanamcode,Industrial Estate,Trivandrum.		    */
/********************************************************************************************/

/********************************************************************************************/
/*                          Includes	                                                    */
/********************************************************************************************/
#include <sys/types.h>
/********************************************************************************************/
/*                          Macro	                                                    */
/********************************************************************************************/

#define TCPIP_FIFO_SIZE 	(128*1024) 
#define TCPIP_TX_BUF_SIZE	(2*1024)
#define TCPIP_RX_BUF_SIZE	(2*1024)
#define TCPIP_SEND_BUFF_SIZE	(4*1024)
#define TCPIP_KEEPALIVETIMEOUT	10//45

#define UART_FIFO_SIZE		(1024*1024)
#define UART_TX_BUF_SIZE	2*1024
#define UART_RX_BUFF_SIZE	2*1024

#define FACTORY_RESET_SWITCH	48
#define MODEM_RESET_PIN			49

//Events definition

#define	IPV6_ADDRESS_CHANGE		0xC0
#define	IPV4_ADDRESS_CHANGE		0xC1
#define	FACTORY_DEFAULTS		0xC2
#define	NETWORK_ERROR_GNR		0xC3
#define	NETWORK_ERROR_8NR		0xC4
#define	NETWORK_OK				0xC5
#define	MODEM_INITIALIZING		0xC6
#define	BAD_CUSBPIDSWITCH_VAL	0xC7
#define	BAD_CLANMODE_VAL		0xC8
#define	BAD_NETWORK_REGN_STAT	0xC9
#define	BAD_NETWORK_REGN_MODE	0xCA
#define	CEER_ERROR_REPORTED		0xCB
#define	CEER_NET_ERR_GNR		0xCC
#define	MARGINAL_SIGNAL_POWER	0xCD
#define	MODEM_AT_CMD_NORESPONSE	0xCE
#define FAIL_TO_OPEN_USBAT_PORT 0xCF







/*
 * Timer Utilities
 */
#define	INIT_TIMER	struct timeval timer_start, timer_end;double timer_spent;
#define	EXT_INIT_TIMER	extern struct timeval timer_start, timer_end;
#define START_TIMER	gettimeofday(&timer_start, NULL);
#define STOP_TIMER	gettimeofday(&timer_end, NULL);
#define	PRINT_TIME	timer_spent = timer_end.tv_sec - timer_start.tv_sec + (timer_end.tv_usec - timer_start.tv_usec) / 1000000.0;\
			printf("Time spent: %.6f\n", timer_spent);

/*
 * File Operation Utilities
 */

#define LOGTOFILE(filename,buff,size)	\
					testfptr=fopen(filename,"w");	\
					fwrite(buff,1,size,testfptr);	\
					fflush(testfptr);            	\
					fsync(fileno(testfptr));	\
					fclose(testfptr);					
					
//FILE *testfptr;			
#define APPENDTOFILE(filename,buff,size)     				\
					testfptr=fopen(filename,"a");	\
					fwrite(buff,1,size,testfptr);	\
					fflush(testfptr);            	\
					fsync(fileno(testfptr));	\
					fclose(testfptr);	

						
#define LOGEVENT(filename,buff,size)				\
					eventtestfptr=fopen(filename,"w");	\
					fwrite(buff,1,size,eventtestfptr);	\
					fflush(eventtestfptr);            	\
					fsync(fileno(eventtestfptr));	\
					fclose(eventtestfptr);					
			
#define APPENDEVENT(filename,buff,size)     				\
					eventtestfptr=fopen(filename,"a");	\
					fwrite(buff,1,size,eventtestfptr);	\
					fflush(eventtestfptr);            	\
					fsync(fileno(eventtestfptr));	\
					fclose(eventtestfptr);	

					#define DEBUG_START	if(1)	\
			{

#define DEBUG_END	}

#define DEBUGCPY(stringbuffer)	strcpy(DebugTempBuffer,stringbuffer);					
#define DEBUGCAT(stringbuffer)	strcat(DebugTempBuffer,stringbuffer);
#define DEBUGPRINT(...) 		DebugTempBufferPointer=(char*)MakeMessage(__VA_ARGS__);	\
								sprintf(DebugTempBuffer,(char*)DebugTempBufferPointer);\
								if((DebugPrintFifo.fifo_depth - DebugPrintFifo.filled_length) > strlen(DebugTempBuffer))\
								{\
									FifoWrite(&DebugPrintFifo,DebugTempBuffer,strlen(DebugTempBuffer));	\
									free(DebugTempBufferPointer);\
									DebugTempBufferPointer=NULL;\
								}\
								DEBUGCPY("\r\n")										\
								if((DebugPrintFifo.fifo_depth - DebugPrintFifo.filled_length) > strlen(DebugTempBuffer))\
								{\
									FifoWrite(&DebugPrintFifo,DebugTempBuffer,strlen(DebugTempBuffer));\
								}
					
/*
 *System Utilities
 */					
					
#define KILL_ALL_PROCESS		system("pkill ColibriHAT");
/*lsof -i tcp:3000

This should list pids for applications using port 3000. 
Once you have the pid you can kill the process with kill command

kill -9 <PID> #where <PID> is the process id returned by lsof*/
#define KILL_COLIBRI			system("pkill -9 ColibriHAT");


/*
 *Debug Utilities
 */
/********************************************************************************************/
/*                         System Defines		                                    */
/********************************************************************************************/
typedef struct CircFifo
{
	unsigned long fifo_depth;	// fifo depth (constant)
	unsigned long reserved0;	// to pad
	unsigned char *fifo;		// address of the fifo (constant)
	unsigned long reserved1;	// to pad	
	unsigned long readptr;
	unsigned long writeptr;
	unsigned long filled_length;
}CIRCULAR_FIFO;

/********************************************************************************************/
/*                          Extern Function Prototypes                                      */
/********************************************************************************************/
extern void ConfigureGPIOs(void);
extern void GPIOWrite(int number,int val);
extern int GpioRead(int gpionumber);
extern void ConfigureSerialPort(void);
extern void SerialProcess(void);
extern void FifoInit(CIRCULAR_FIFO *fifo, unsigned long depth, unsigned char *data);
extern void FifoFlush(CIRCULAR_FIFO *fifo);
extern unsigned long FifoRewind(CIRCULAR_FIFO *fifo, unsigned long length);
extern unsigned long FifoWrite(CIRCULAR_FIFO *fifo, unsigned char *data, unsigned long len);
extern unsigned long FifoRead(CIRCULAR_FIFO *fifo, unsigned char * data, unsigned long len);
extern void ConfigureTimers(void);
extern void ConfigureSignalHandlers(void);
extern void ResetModem(void);
extern void EventLog(char *buffer);
extern void RTCRead(void);
extern void ConfigureDebugSharedMemory(void);
extern void ConfigureDebug(void);
extern void DebugProcess(void);
extern char *MakeMessage(const char *fmt, ...);
extern void DebugFileWrite(void);

extern void ConfigureUART2(void);
extern void UART2Write(char *buffer);
extern void UART2Transmit(void);
extern void UART2ToSARM(unsigned char EventByte);
extern void UART2Process(void);

extern void convertyuvtoBMP();

extern void sock_thirdstream_thread();
extern void sock_thirdstream_process();
