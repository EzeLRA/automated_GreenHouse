/********************************************************************************************/
/*		Project		   :	Arduino Camera using OV7670   			                        */
/*		Filename	   :	Serial.c				                                        */
/*		Functionality  :    Serial Port Routines      			                            */
/*		Author		   :	TBE                                        					    */
/********************************************************************************************/

/********************************************************************************************/
/*                          Includes	                                                    */
/********************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <sys/mman.h>

#include "SystemDefines.h"
/********************************************************************************************/
/*                          Defines	                                                    */
/********************************************************************************************/
#define USBAT   0

#define QVGA_COLOUR     320*240*2
#define QQVGA_COLOUR    160*120*2

#define VGA_MONO        640*480
#define QVGA_MONO       320*240
#define QQVGA_MONO      160*120


/********************************************************************************************/
/*                          Extern Variable	                                            */
/********************************************************************************************/
extern int cnt;
/********************************************************************************************/
/*                          Global Variable	                                            */
/********************************************************************************************/ 
static int Cport[3],error;
static struct termios new_port_settings,old_port_settings[3];
static char comports[1][16]=
{
	"/dev/ttyUSB0"
};

static char SerialTxbuffer[UART_FIFO_SIZE];
static char SerialRxbuffer[UART_FIFO_SIZE];
static int SerialRxState;
static CIRCULAR_FIFO SerialTxFifo;
static CIRCULAR_FIFO SerialRxFifo;
unsigned char ImgBuffer[QQVGA_COLOUR];

unsigned long rx_cnt;
/********************************************************************************************/
/*                          Function Prototypes                                             */
/********************************************************************************************/
static int SerialOpenComport(int, int, const char *);
static int SerialPollComport(int);
static int SerialSendByte(int, unsigned char);
static int SerialSendBuf(int, unsigned char *, int);
static int SerialIsDCDEnabled(int);
static int SerialIsCTSEnabled(int);
static int SerialIsDSREnabled(int);
static void SerialCloseComport(int);
static void SerialCPuts(int, const char *);
static void SerialEnableDTR(int);
static void SerialDisableDTR(int);
static void SerialEnableRTS(int);
static void SerialDisableRTS(int);
static void SerialTransmit(void);
static void SerialReceive(void);
static void SerialRead(void);
static void SerialWrite(char *message);
static void SerialReadComport(int comport_number, unsigned char *buf, int size);
/********************************************************************************************/
/*                          Function Definition                                             */
/********************************************************************************************/
static int SerialOpenComport(int comport_number, int baudrate, const char *mode)
{
	int baudr,status;
	int cbits=CS8,cpar=0,ipar=IGNPAR,bstop=0;

	if((comport_number>2)||(comport_number<0))
	{
		printf("SERIAL:Illegal comport number\n");
		return(1);
	}

	switch(baudrate)
	{
		case    2400 : 	baudr = B2400;
			   	break;
		case    4800 : 	baudr = B4800;
			   	break;
		case    9600 : 	baudr = B9600;
			   	break;
		case   19200 : 	baudr = B19200;
			   	break;
		case   38400 : 	baudr = B38400;
			   	break;
		case   57600 : 	baudr = B57600;
			   	break;
		case  115200 : 	baudr = B115200;
			   	break;
		case  230400 : 	baudr = B230400;
			   	break;
		case  460800 : 	baudr = B460800;
			   	break;
		case  500000 : 	baudr = B500000;
			   	break;
		case  576000 : 	baudr = B576000;
			   	break;
		case  921600 : 	baudr = B921600;
			   	break;
		case 1000000 : 	baudr = B1000000;
			   	break;
		case 1152000 : 	baudr = B1152000;
			   	break;
		case 1500000 : 	baudr = B1500000;
			   	break;
		case 2000000 : 	baudr = B2000000;
			   	break;
		case 2500000 : 	baudr = B2500000;
			   	break;
		case 3000000 : 	baudr = B3000000;
			   	break;
		case 3500000 : 	baudr = B3500000;
			   	break;
		case 4000000 : 	baudr = B4000000;
			   	break;
		default      : 	printf("SERIAL:Invalid baudrate\n");
			   	return(1);
			   	break;
	}

	if(strlen(mode) != 3)
	{
		printf("SERIAL:Invalid mode \"%s\"\n", mode);
		return(1);
	}

	switch(mode[0])
	{
		case '8': 	cbits = CS8;
		      		break;
		case '7': 	cbits = CS7;
		      		break;
		case '6': 	cbits = CS6;
		      		break;
		case '5': 	cbits = CS5;
		      		break;
		default : 	printf("SERIAL:Invalid number of data-bits '%c'\n", mode[0]);
				return(1);
		      		break;
	}

	switch(mode[1])
	{
		case 'N':
		case 'n': 	cpar = 0;
		      		ipar = IGNPAR;
		      		break;
		case 'E':
		case 'e': 	cpar = PARENB;
		      		ipar = INPCK;
		      		break;
		case 'O':
		case 'o': 	cpar = (PARENB | PARODD);
		      		ipar = INPCK;
		      		break;
		default : 	printf("SERIAL:Invalid parity '%c'\n", mode[1]);
		      		return(1);
		      		break;
	}

	switch(mode[2])
	{
		case '1': 	bstop = 0;
		      		break;
		case '2': 	bstop = CSTOPB;
		      		break;
		default : 	printf("SERIAL:Invalid number of stop bits '%c'\n", mode[2]);
		      		return(1);
		      		break;
	}

	/*
	http://pubs.opengroup.org/onlinepubs/7908799/xsh/termios.h.html

	http://man7.org/linux/man-pages/man3/termios.3.html
	*/

	Cport[comport_number] = open(comports[comport_number], O_RDWR | O_NOCTTY | O_NDELAY);
	if(Cport[comport_number]==-1)
	{
		//perror("SERIAL:Unable to open comport\n");
		return(1);
	}

	fcntl(Cport[comport_number],F_SETFL,O_NONBLOCK);

	error = tcgetattr(Cport[comport_number], old_port_settings + comport_number);
	if(error==-1)
	{
		close(Cport[comport_number]);
		perror("SERIAL:Unable to read portsettings\n");
		return(1);
	}
	memset(&new_port_settings, 0, sizeof(new_port_settings));  /* clear the new struct */

	new_port_settings.c_cflag = cbits | cpar | bstop | CLOCAL | CREAD;
	new_port_settings.c_iflag = ipar;
	new_port_settings.c_oflag = 0;
	new_port_settings.c_lflag = 0;
	new_port_settings.c_cc[VMIN] = 0;      /* block untill n bytes are received */
	new_port_settings.c_cc[VTIME] = 0;     /* block untill a timer expires (n * 100 mSec.) */

	cfsetispeed(&new_port_settings, baudr);
	cfsetospeed(&new_port_settings, baudr);

	error = tcsetattr(Cport[comport_number], TCSANOW, &new_port_settings);
	if(error==-1)
	{
		close(Cport[comport_number]);
		perror("SERIAL:Unable to adjust portsettings\n");
		return(1);
	}

	return(0);
}
 
 
static int SerialPollComport(int comport_number)//, unsigned char *buf, int size)
{
	int n=0;

	ioctl(Cport[comport_number], TIOCINQ, &n);
	if (n > 0) 
	{
		return n;
	}

	return 0;
	
}
 
static void SerialReadComport(int comport_number, unsigned char *buf, int size)
{
	read(Cport[comport_number], buf, size);
}

static int SerialSendByte(int comport_number, unsigned char byte)
{
	int n;
	//printf("%c\n",byte);
	n = write(Cport[comport_number], &byte, 1);
	if(n<0)  
	{
		return(1);
	}

	return(0);
}
 
 
static int SerialSendBuf(int comport_number, unsigned char *buf, int size)
{
  	return(write(Cport[comport_number], buf, size));
}
 
 
static void SerialCloseComport(int comport_number)
{
	int status;

	if(ioctl(Cport[comport_number], TIOCMGET, &status) == -1)
	{
		perror("SERIAL:Unable to get portstatus\n");
	}

	status &= ~TIOCM_DTR;    /* turn off DTR */
	status &= ~TIOCM_RTS;    /* turn off RTS */

	if(ioctl(Cport[comport_number], TIOCMSET, &status) == -1)
	{
		perror("SERIAL:Unable to set portstatus\n");
	}

	tcsetattr(Cport[comport_number], TCSANOW, old_port_settings + comport_number);
	close(Cport[comport_number]);
}
 
/*
Constant  Description
TIOCM_LE        DSR (data set ready/line enable)
TIOCM_DTR       DTR (data terminal ready)
TIOCM_RTS       RTS (request to send)
TIOCM_ST        Secondary TXD (transmit)
TIOCM_SR        Secondary RXD (receive)
TIOCM_CTS       CTS (clear to send)
TIOCM_CAR       DCD (data carrier detect)
TIOCM_CD        see TIOCM_CAR
TIOCM_RNG       RNG (ring)
TIOCM_RI        see TIOCM_RNG
TIOCM_DSR       DSR (data set ready)
 
http://man7.org/linux/man-pages/man4/tty_ioctl.4.html
*/ 
static int SerialIsDCDEnabled(int comport_number)
{
	int status;

	ioctl(Cport[comport_number], TIOCMGET, &status);

	if(status&TIOCM_CAR) 
	{
		return(1);
	}
	else 
	{
		return(0);
	}
}
 
static int SerialIsCTSEnabled(int comport_number)
{
	int status;

	ioctl(Cport[comport_number], TIOCMGET, &status);

	if(status&TIOCM_CTS) 
	{
		return(1);
	}
	else 
	{
		return(0);
	}
}
 
static int SerialIsDSREnabled(int comport_number)
{
	int status;

	ioctl(Cport[comport_number], TIOCMGET, &status);

	if(status&TIOCM_DSR) 
	{
		return(1);
	}
	else 
	{
		return(0);
	}
}
 
static void SerialEnableDTR(int comport_number)
{
	int status;

	if(ioctl(Cport[comport_number], TIOCMGET, &status) == -1)
	{
		perror("SERIAL:Unable to get portstatus\n");
	}

	status |= TIOCM_DTR;    /* turn on DTR */

	if(ioctl(Cport[comport_number], TIOCMSET, &status) == -1)
	{
		perror("SERIAL:unable to set portstatus\n");
	}
}
 
static void SerialDisableDTR(int comport_number)
{
	int status;

	if(ioctl(Cport[comport_number], TIOCMGET, &status) == -1)
	{
		perror("SERIAL:Unable to get portstatus\n");
	}

	status &= ~TIOCM_DTR;    /* turn off DTR */

	if(ioctl(Cport[comport_number], TIOCMSET, &status) == -1)
	{
		perror("SERIAL:unable to set portstatus\n");
	}
}
 
static void SerialEnableRTS(int comport_number)
{
	int status;

	if(ioctl(Cport[comport_number], TIOCMGET, &status) == -1)
	{
		perror("SERIAL:Unable to get portstatus\n");
	}

	status |= TIOCM_RTS;    /* turn on RTS */

	if(ioctl(Cport[comport_number], TIOCMSET, &status) == -1)
	{
		perror("SERIAL:unable to set portstatus\n");
	}
}
 
static void SerialDisableRTS(int comport_number)
{
	int status;

	if(ioctl(Cport[comport_number], TIOCMGET, &status) == -1)
	{
		perror("SERIAL:Unable to get portstatus\n");
	}

	status &= ~TIOCM_RTS;    /* turn off RTS */

	if(ioctl(Cport[comport_number], TIOCMSET, &status) == -1)
	{
		perror("SERIAL:unable to set portstatus\n");

	}
}
 
 
 
static void SerialCPuts(int comport_number, const char *text)  /* sends a string to serial port */
{
	while(*text != 0)   
	{
		SerialSendByte(comport_number, *(text++));
	}
}

static void SerialWrite(char *message)
{
	if((SerialTxFifo.fifo_depth - SerialTxFifo.filled_length) > UART_TX_BUF_SIZE)		
	{			
		FifoWrite(&SerialTxFifo,(unsigned char *)message,strlen(message));
		//printf("write ok\n");
	}
	else
	{
		printf("SERIAL:Error no space in Serial Tx fifo\n");

		
	}
}

static void SerialTransmit(void)
{
	char se_buff[UART_TX_BUF_SIZE];
	unsigned short len;
	int i;

	if(SerialTxFifo.filled_length)
	{	      
		len = FifoRead(&SerialTxFifo,(unsigned char *)se_buff,UART_TX_BUF_SIZE);
		SerialSendBuf(USBAT,se_buff,len);
		//printf("tx ok\n");
	}
}

static void SerialReceive(void)
{
	char se_buff[UART_RX_BUFF_SIZE];
	unsigned short len;
	int i;

	if((SerialRxFifo.fifo_depth - SerialRxFifo.filled_length) > UART_RX_BUFF_SIZE)
	{
		len=SerialPollComport(USBAT);		
		if(len > 0)
		{
			if(len>=UART_RX_BUFF_SIZE)			
			{
				SerialReadComport(USBAT,se_buff,UART_RX_BUFF_SIZE);	
			}
			else
			{
				SerialReadComport(USBAT,se_buff,len);
			}
			//DisplayBuffer((unsigned char *)se_buff, len);
			i = FifoWrite(&SerialRxFifo,(unsigned char *)se_buff, len);
			if(i != len)
			{
				printf("SERIAL:Error no space in fifo\n" );

			}
		}
	} 
}

void ConfigureSerialPort(void)
{
	int  bdrate=2000000;    	
	char mode[]={'8','N','1',0}; 
	int i=0,j=0,k=0;
	unsigned short calc_cs=0;
	char tmpbuffer[128];
	
	printf("SERIAL:Checking USBAT Port\n");
    SerialRxState=-1;
	if(SerialOpenComport(USBAT, bdrate, mode))
	{
		printf("SERIAL:Can not open comport\n");	

	}
    else
    {
        
        FifoInit(&SerialTxFifo,UART_FIFO_SIZE,SerialTxbuffer);
        FifoInit(&SerialRxFifo,UART_FIFO_SIZE,SerialRxbuffer);
    }
}


static void SerialRead(void)
{
	FILE *testfptr;
	
    unsigned char so_buff[UART_RX_BUFF_SIZE];
	int nRet;

	int i=0,j=0;
	//char tempbuffer[1024];
	//char tmpbuffer[1024];

    if(SerialRxState==-1)
    {
        if(SerialRxFifo.filled_length)
        {
            //printf("SERIAL:Fifo Data available.%d\n",SerialRxFifo.filled_length);

            FifoRead(&SerialRxFifo,(unsigned char *)(so_buff),1);   
            printf("SERIAL:next byte Received %x\n",so_buff[0]);
            if(so_buff[0]=='*')
            {
                printf("SERIAL:byte Received %x\n",so_buff[0]);
                FifoRead(&SerialRxFifo,(unsigned char *)(so_buff),1);
                
                if(so_buff[0]=='R')
                {
                    printf("SERIAL:byte Received %x\n",so_buff[0]);
                    FifoRead(&SerialRxFifo,(unsigned char *)(so_buff),1);
                    if(so_buff[0]=='D')
                    {
                        printf("SERIAL:byte Received %x\n",so_buff[0]);
                        FifoRead(&SerialRxFifo,(unsigned char *)(so_buff),1);
                        if(so_buff[0]=='Y')
                        {
                            printf("SERIAL:byte Received %x\n",so_buff[0]);
                            FifoRead(&SerialRxFifo,(unsigned char *)(so_buff),1);
                            if(so_buff[0]=='*')
                            {
                                printf("SERIAL:byte Received %x\n",so_buff[0]);
                                FifoRead(&SerialRxFifo,(unsigned char *)(so_buff),1);
                                SerialRxState=0;
                                rx_cnt=0;
                                cnt=so_buff[0];
                                printf("SERIAL:Header Received imgcnt=%d\n",so_buff[0]);
                            }
                        }
                    }
                }
            }            
        }
    }
    else
    {
        if(SerialRxFifo.filled_length)
        {
            if(((QQVGA_COLOUR)-rx_cnt)>UART_RX_BUFF_SIZE)
            {
                nRet=FifoRead(&SerialRxFifo,(unsigned char *)(ImgBuffer+rx_cnt),UART_RX_BUFF_SIZE);
                //printf("SERIAL:UART_RX_BUFF_SIZE %d\n",rx_cnt);
            }
            else
            {
                nRet=FifoRead(&SerialRxFifo,(unsigned char *)(ImgBuffer+rx_cnt),(QQVGA_COLOUR)-rx_cnt);
               // printf("SERIAL:(QQVGA)-rx_cnt %d %d %d\n",QQVGA,nRet,rx_cnt);
            }
            
            rx_cnt+=nRet;
            if(rx_cnt>=(QQVGA_COLOUR))
            {
                printf("SERIAL:Image Received\n");
                LOGTOFILE("img1",ImgBuffer,QQVGA_COLOUR);
                rx_cnt=0;
                convertyuvtoBMP();
             
                SerialRxState=-1;
            }
        }
        
    }
}

void SerialProcess(void)
{
	SerialTransmit();
	SerialReceive();
	SerialRead();
}
 
 
 
 
