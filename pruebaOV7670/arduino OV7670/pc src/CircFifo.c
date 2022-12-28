/********************************************************************************************/
/*		Project		   :	Arduino Camera using OV7670   			                        */
/*		Filename	   :	CircFifo.c					                                    */
/*		Functionality  :    Circular Fifo operations 			                            */
/*		Author		   :	TBE                                        					    */
/********************************************************************************************/

/********************************************************************************************/
/*                          Includes                                                        */
/********************************************************************************************/
#include <string.h>
#include "SystemDefines.h"
/********************************************************************************************/
/*                          Function Prototypes                                             */
/********************************************************************************************/

/********************************************************************************************/
/*                          Function Defines                                                */
/********************************************************************************************/

void FifoInit(CIRCULAR_FIFO *fifo, unsigned long depth, unsigned char *data)
{
	// resets read and write ptr length = 0 and data ptr to home;
	fifo->filled_length = 0;
	fifo->writeptr = 0;
	fifo->readptr = 0;
	fifo->fifo_depth = depth;
	fifo->fifo = data;
}
void FifoFlush(CIRCULAR_FIFO *fifo)
{
	// puts read ptr to write ptr, length = 0 and no change to data ptr;
	fifo->filled_length = 0;
	fifo->readptr = fifo->writeptr;
}

unsigned long FifoRewind(CIRCULAR_FIFO *fifo, unsigned long length)// if ok returns (rewind length) else (0)
{
	// rewinds read ptr by length and adjust the fifo filled_length;
	unsigned long ret = 0;
//	if((fifo->fifo_depth - fifo->filled_length) <= length)
//		return 0;
	ret = length;
	fifo->filled_length += length;
	if(fifo->readptr > length)
	{
		fifo->readptr -= length;
	}
	else
	{
		length -= fifo->readptr;
		fifo->readptr = fifo->fifo_depth - length;
	}
	return ret;
}

unsigned long FifoWrite(CIRCULAR_FIFO *fifo, unsigned char *data, unsigned long len)
{
	// returns no of bytes writteen in fifo...
	unsigned long /*i,*/ j, templen;
	//unsigned long *tempdata;
	if(!len)
	{
		return 0;
	}
	if(len > (fifo->fifo_depth - fifo->filled_length))
	{
		len = fifo->fifo_depth - fifo->filled_length;				
	}	
	templen = 0;
	j = fifo->writeptr;
	if(len > (fifo->fifo_depth - j))
	{
		templen = fifo->fifo_depth - j;
		memcpy(&fifo->fifo[j],data,templen);
		j+= templen;
		data+=templen;
		fifo->filled_length += (j - fifo->writeptr);
		j = fifo->writeptr = 0;
	}
	templen = len - templen;
	if(templen)
	{
		memcpy(&fifo->fifo[j],data,templen);
		j+= templen;
		data+=templen;
	}
	fifo->filled_length += (j - fifo->writeptr);
	fifo->writeptr = j;
	return len;
}

unsigned long FifoRead(CIRCULAR_FIFO *fifo, unsigned char * data, unsigned long len)
{
	// returns no of bytes read from fifo...
	unsigned long /*i,*/ j, templen;
	if(!len)
		return 0;
	if(len > fifo->filled_length)
		len = fifo->filled_length;
	templen = 0;
	j = fifo->readptr;
	if(len > (fifo->fifo_depth - j))
	{
		templen = fifo->fifo_depth - j;
		memcpy(data,&fifo->fifo[j],templen);
		data+=templen;
		j+=templen;
		fifo->filled_length -= (j - fifo->readptr);
		j = fifo->readptr = 0;
	}
	templen = len - templen;
	if(templen)
	{
		memcpy(data,&fifo->fifo[j],templen);
		j+=templen;
	}
	fifo->filled_length -= (j - fifo->readptr);
	fifo->readptr = j;
	return len;
}
