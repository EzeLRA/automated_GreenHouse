/********************************************************************************************/
/*		Project		   :	Arduino Camera using OV7670   			                        */
/*		Filename	   :	thirdstream.c			                                        */
/*		Functionality  :    MJPEG Streamer            			                            */
/*		Author		   :	TBE                                        					    */
/********************************************************************************************/
/********************************************************************************************/
/*                          Includes	                                                    */
/********************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/poll.h>
#include <linux/if.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

/********************************************************************************************/
/*                          Defines	                                                        */
/********************************************************************************************/
#define MY_PORT				         8080
#define SOCKET_SEND_BUFF_LENGTH_TS	(2048*2)
#define CLIENT_COUNT_TS 		      1
#define BUFER_SIZE			           1024*1024
/********************************************************************************************/
/*                          Global Vars	                                                    */
/********************************************************************************************/
char HEAD_RESPONSE[] = 
{
  "HTTP/1.0 200 OK\r\n"
  "Content-Type: multipart/x-mixed-replace; "
  "boundary=--kl01bb9322kl01s7266tbe\r\n\r\n"
};    

const char SEPARATOR[] =
{
    "\r\n\r\n"
    "--kl01bb9322kl01s7266tbe\r\n"
};

long ts_fileSize_0=0,ts_fileSize_1=0,filesendsize=0,ts_buffer_filled=0,ts_user_connected=0,
    image_header_send=0,image_separator_send=0,image_data_send_0=0,image_data_send_1=0,
    ts_stream_start=0;
unsigned long fileSize=0;    
char *ts_filebuffer_0=NULL,*ts_filebuffer_1=NULL,*filebuffer_1=NULL;
signed char jpeg_cnt=1,new_image=0;
char rxbuffer_thirdstream[SOCKET_SEND_BUFF_LENGTH_TS];
unsigned char ts_timer=0;

int ts_sockfd;
long ts_txsize;
struct sockaddr_in ts_self;
int ts_client_thirdstream[CLIENT_COUNT_TS];
struct sockaddr_in ts_client_thirdstream_addr[CLIENT_COUNT_TS];
int ts_addrlen;
struct pollfd ts_socket_poll;

/********************************************************************************************/
/*                          Function Prototypes                                             */
/********************************************************************************************/
void sock_thirdstream_thread();
/********************************************************************************************/
/*                          Function Definition                                             */
/********************************************************************************************/

long send_head_response_OK(int socket_handler)
{ 
    return((long)send(socket_handler,(const void *)HEAD_RESPONSE, strlen(HEAD_RESPONSE), 0));  
}

long send_head_response_per_image(int socket_handler,int size)
{
    char tmpbuffer[1024]={0};  

    sprintf(tmpbuffer,"Content-Type: image/jpg\r\nContent-Length: %ld\r\n\r\n",size);

    return((long)send(socket_handler,(const void *)tmpbuffer, strlen(tmpbuffer), 0));  
}

long send_separator(int socket_handler)
{
    return((long)send(socket_handler,(const void *)SEPARATOR, strlen(SEPARATOR), 0));  
}

void file_operations(char *filename)
{
    FILE *fptr;
    int nRet;

    fptr=fopen(filename,"rb");
    if(fptr==NULL)
    {
        printf("File Open Error:%s\n",filename);
        return;
    }
    else
    {
        printf("File Opened %s\n",filename);
    } 

    if(fptr)
    {
        fileSize=fseek(fptr,0,SEEK_END);
    }
    else
    {
        printf("Invalid Seek:File Doesn't Exist\n");
    } 
    fileSize=ftell(fptr);
    fseek(fptr,0,SEEK_SET); 

    if(fptr)
    {
        //nRet=fread((buffer_filled==0)?filebuffer_1:filebuffer_2,1,fileSize,fptr);
        nRet=fread(filebuffer_1,1,fileSize,fptr); 
        /*if(buffer_filled==0)
        {
            buffer_filled=1;
        }
        else
        {
            buffer_filled=0;
        }*/
    }
    else
    {
        printf("Invalid Read:File Doesn't Exist\n");
    }
    fclose(fptr);

}

void data_buffer_init()
{
   /* if(ts_filebuffer_0==NULL)
    {
        ts_filebuffer_0=(char *)malloc(BUFER_SIZE);
    }*/
    if(filebuffer_1==NULL)
    {
        filebuffer_1=(char *)malloc(BUFER_SIZE);
    }
}

long thirdstream_tx(int socket_handler)
{  
    char tmpbuffer[1024];
    long sendRet=0; 
    FILE *fptr;

    if(ts_timer==0)
    {
        if(ts_user_connected)
        {
            if((fptr=fopen("tmp.jpg","r"))!=NULL)
            {
                sendRet=send_head_response_OK(socket_handler);
                ts_user_connected=0;
                new_image=1;
                //usleep(5000);
                ts_timer=2;
            }
            else
            {
                printf("file null retrun\n");
                return 0;
            }
        }
        else
        {
            if(new_image==1)
            {
                image_header_send=1;
                new_image=0;
                sendRet=send_separator(socket_handler);      
            }
            else
            {
                if(image_header_send==1)
                {
                    //sprintf(tmpbuffer,"/tmp/httpd/cgi-bin/image%03d.jpg",jpeg_cnt);
                    if((fptr=fopen("tmp.jpg","r"))!=NULL)
                    {
                        file_operations("tmp.jpg"); 
                        system("rm -f tmp.jpg");
                        //sendRet=send_head_response_per_image(socket_handler,(ts_buffer_filled==0)?ts_fileSize_1:ts_fileSize_0);	
                        sendRet=send_head_response_per_image(socket_handler,fileSize);
                        image_header_send=0;
                        //if(ts_buffer_filled==0)
                        //{
                            image_data_send_1=1;
                        //}
                        //else
                        //{
                        //    image_data_send_0=1;	
                        //}
                        printf("image header sent %d\n",sendRet);
                        //usleep(5000);
                        ts_timer=2;
                    }
                    else
                    {
                        return 0;
                    }
                }
                else
                {
                   /* if(image_data_send_0==1)
                    {
                        if((ts_fileSize_0 - filesendsize)>1024)
                        {
                            sendRet=send(socket_handler,(const void *)(ts_filebuffer_0+filesendsize),1024,0);
                            filesendsize+=1024;
                            //usleep(5000);
                            ts_timer=2;
                        }
                        else
                        {
                            sendRet=send(socket_handler,(const void *)(ts_filebuffer_0+filesendsize),(ts_fileSize_0-filesendsize),0);
                            filesendsize=0;
                            ts_fileSize_0=0;
                            image_separator_send=1;
                            image_data_send_0=0;
                            //printf("image data sent:END\n");
                            //usleep(5000);
                            ts_timer=2;
                        }	  
                    }
                    else*/
                    //{
                        if(image_data_send_1==1)
                        {
                            if((fileSize - filesendsize)>1024)
                            {
                                sendRet=send((long)socket_handler,(const void *)(filebuffer_1+filesendsize),1024,0);
                                filesendsize+=1024;
                                //usleep(5000);
                                printf("image data sent:pkt %d\n",sendRet);
                                ts_timer=2;
                            }
                            else
                            {
                                sendRet=send(socket_handler,(const void *)(filebuffer_1+filesendsize),(fileSize-filesendsize),0);
                                filesendsize=0;
                                fileSize=0;
                                image_separator_send=1;
                                image_data_send_1=0;
                                printf("image data sent:END\n");
                                //usleep(5000);
                                ts_timer=2;
                            }	  
                        }	  
                        else
                        {
                            if(image_separator_send==1)
                            {
                                //sendRet=send_separator(socket_handler);
                                new_image=1;
                                image_separator_send=0;
                                //jpeg_cnt++;
                                //if(jpeg_cnt>5)
                                //{
                                //jpeg_cnt=1;
                                //}
                                printf("image separator sent\n");
                                //usleep(5000);
                                ts_timer=2;
                            }
                            else
                            {
                                return 0;
                            }	  
                        }
                    //}
                }
            }
        }
    }
    return sendRet;
}

//-----------------------------------------------------------------------------------------------------------------
void server_thirdstream_rx(int ts_client_thirdstream)
{
    recv(ts_client_thirdstream, rxbuffer_thirdstream, 1, 0);
    if(rxbuffer_thirdstream[0] == 'q')
    {
        close(ts_client_thirdstream);
        ts_client_thirdstream = -1;
    }
    return;
}




//-----------------------------------------------------------------------------------------------------------------

void sock_thirdstream_thread()
{
    int i, j;

    ts_addrlen=sizeof(ts_client_thirdstream_addr);
    
    data_buffer_init();
    
    /*---Create streaming socket---*/
    if ( (ts_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0 )
    {
      perror("Socket");
      exit(errno);
    }

    fcntl(ts_sockfd,F_SETFL,O_NONBLOCK);
    j = SOCKET_SEND_BUFF_LENGTH_TS*2;
    i = setsockopt(ts_sockfd, SOL_SOCKET, SO_SNDBUF, &j, sizeof(int));
    j = 1;
    i = setsockopt(ts_sockfd, SOL_SOCKET, SO_REUSEADDR, &j, sizeof(int));
    printf("Socket created\n");
    
    /*---Initialize address/port structure---*/
    bzero(&ts_self, sizeof(ts_self));
    //memset(txbuffer_thirdstream, 'f', SOCKET_SEND_BUFF_LENGTH_TS);
    ts_self.sin_family = AF_INET;
    ts_self.sin_port = htons(MY_PORT);
    ts_self.sin_addr.s_addr = INADDR_ANY;
	/*---Assign a port number to the socket---*/
    if ( bind(ts_sockfd, (struct sockaddr*)&ts_self, sizeof(ts_self)) != 0 )
    {
      perror("socket--bind");
      exit(errno);
    }

	/*---Make it a "listening socket"---*/
    if ( listen(ts_sockfd, CLIENT_COUNT_TS+1) != 0 )
    {
      perror("socket--listen");
      exit(errno);
    }
    printf("listening on port %d\n",MY_PORT);
    for(i = 0; i < CLIENT_COUNT_TS; i++)
    {
      ts_client_thirdstream[i] = -1;
    }
    i = 0;// socket count
}   
    //fifo_init(&fifo_var[i], FIFO_SIZE_TS, data_fifo_thirdstream[i]);
void sock_thirdstream_process()
{
    int i=0, j=0;


    /*---accept a connection (creating a data pipe)---*/
    if (ts_client_thirdstream[i] == -1)
    {
        ts_client_thirdstream[i] = accept(ts_sockfd, (struct sockaddr*)&ts_client_thirdstream_addr[i], &ts_addrlen);
        if (ts_client_thirdstream[i] >= 0)
        {
            printf("%s:%d connected on sock %d\n", inet_ntoa(ts_client_thirdstream_addr[i].sin_addr), ntohs(ts_client_thirdstream_addr[i].sin_port),i);
            ts_user_connected=1;
            ts_stream_start=1;
            //jpeg_cnt=1;
        }
        else
        {
            if (errno != EAGAIN)
            {
                perror("Error in accept \n");
                exit(errno);
            }
        }
    }
    else
    {
        ts_socket_poll.fd = ts_client_thirdstream[i];
        ts_socket_poll.events = POLLIN|POLLOUT|POLLHUP|POLLNVAL;
        /* POLLIN      0x0001     There is data to read */
        /* POLLPRI     0x0002     There is urgent data to read */
        /* POLLOUT     0x0004     Writing now will not block */
        /* POLLERR     0x0008     Error condition */
        /* POLLHUP     0x0010     Hung up */
        /* POLLNVAL    0x0020     Invalid request: fd not open */
        j = poll(&ts_socket_poll, 1, 0);//int poll(struct pollfd *ufds, unsigned int nfds, int timeout);
        if(j > 0)
        {
            if((ts_socket_poll.revents & POLLHUP) || (ts_socket_poll.revents & POLLNVAL))
            {
                /*---Close data connection---*/
                printf("hup sock %d disconnected \n",i);
                close(ts_client_thirdstream[i]);
                ts_client_thirdstream[i] = -1;
                ts_stream_start=0;
                //jpeg_cnt=-1;
            }
            else if(ts_socket_poll.revents & POLLERR)
            {
                /*---Close data connection---*/
                printf("err sock %d disconnected \n",i);
                close(ts_client_thirdstream[i]);
                ts_client_thirdstream[i] = -1;
                //jpeg_cnt=-1;
                ts_stream_start=0;
            }
            else if(ts_socket_poll.revents & POLLOUT)
            {// writing to socket wont block data to be sent

                ts_txsize = thirdstream_tx(ts_client_thirdstream[i]);//server_ts_client_thirdstream_tx(ts_client_thirdstream[i],i);

                if(ts_txsize < 0)
                {
                    /*---Close data connection---*/
                    printf("-ve sock %d disconnected \n",i);
                    close(ts_client_thirdstream[i]);
                    ts_client_thirdstream[i] = -1;
                    //jpeg_cnt=-1;
                    ts_stream_start=0;
                }
                if(ts_socket_poll.revents & POLLIN)
                {// there is data pending on socket so read it..
                    //server_thirdstream_rx(ts_client_thirdstream[i]);
                    //printf("server rx\n");
                }
            }
        }
    }
    i++;
    if(i >= CLIENT_COUNT_TS)
    {
        i = 0;
    }

}



