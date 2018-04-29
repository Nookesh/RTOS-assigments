/*......................................................................Server.................................................................*/
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#define PORT 6542  
#define BUFSIZE 18000 
#define RATE 6800  

/* Recording */
static const pa_sample_spec ss1 = {
  	.format = PA_SAMPLE_S16LE,
  	.rate = RATE,
  	.channels = 2 
    };


/* Playing */
static const pa_sample_spec ss = {
  	.format = PA_SAMPLE_S16LE,
  	.rate = RATE,
  	.channels = 2 
  };

static pa_simple *s = NULL;
static pa_simple *s1 = NULL;
static int srvr_sockt;
static struct sockaddr_in address;
int addrlen = sizeof(address);
static int8_t Rcv_buffer[BUFSIZE];
static  int8_t Send_Buffer[BUFSIZE];

pthread_t SendT, RecvT;

/* Create handle for playing */
void Create_Handle_For_Playing(void)
{
    int error;

    if (!(s = pa_simple_new(NULL, "Server", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) 
    {
        printf(": pa_simple_new() failed: %s\n", pa_strerror(error));
    }
}

/* Create handle for recording */
void Create_Handle_For_Recording(void)
{
    int error;    

    if (!(s1 = pa_simple_new(NULL, "Server", PA_STREAM_RECORD, NULL, "record", &ss1, NULL, NULL, &error))) 
    {
        printf("Error: adin_pulseaudio: pa_simple_new() failed: %s\n", pa_strerror(error));
    }
}

/* Free playing source */
void Free_Playing_Source(void)
{
    if (s != NULL)
    {
        pa_simple_free(s);
        s = NULL;
    }
}

/* Free recording source */
void Free_Recording_Source(void)
{
    if (s1 != NULL)
    {
        pa_simple_free(s1);
        s1 = NULL;
    }
}

/* Record audio packets to a buffer using pulse API */
void Record_Audio (int8_t *buf, int sampnum)
{
    int error;

    /* Record using pulse audio API- "pa_simple_read" */
    if (pa_simple_read(s1, buf, BUFSIZE, &error) < 0)
    {
        printf("Error: pa_simple_read() failed: %s\n", pa_strerror(error));
    }
}


/* Records audio and send those packets through socket */
void * SendThread()
{
    size_t r;

    /* Create handle for recording */
    Create_Handle_For_Recording();
    while(r!=50000)
    {
        Record_Audio(Send_Buffer, BUFSIZE); 	  
        /* Send packets using UDP socket */ 
        r=sendto(srvr_sockt, Send_Buffer, BUFSIZE, 0, (struct sockaddr *)&address, sizeof(address));
        printf("number of bytes sent %ld\n",r);
    }   
    /* Free the recording source */
    Free_Recording_Source();
}

void * ReceiveThread()
{
    size_t r;
    int error;
    
    /* Create handle for playing */
    Create_Handle_For_Playing();
    while(r!=50000)
    {  
        /* Receive packets through UDP packets */
        r = recvfrom(srvr_sockt, Rcv_buffer, BUFSIZE,0,(struct sockaddr *)&address, &addrlen);  
        
        /* Play the audio using pulse audio API */        
        if (pa_simple_write(s, Rcv_buffer, BUFSIZE, &error) < 0) 
        {
            printf(": pa_simple_write() failed: %s\n", pa_strerror(error));
            break;
        }
        printf("number of bytes received %ld\n",r);
    }
    /* Free the recording source */
    Free_Playing_Source();
   
}


int main()
{
    /* Creating socket with communication using UDP */
    if ((srvr_sockt = socket(AF_INET, SOCK_DGRAM, 0)) == 0) // raw
    {
        printf("Socket creation failed\n");
        return 0;
    }
       
    /* Specifying welcome port */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
      
    /* Binding to the socket created above */
    if (bind(srvr_sockt, (struct sockaddr *)&address, sizeof(address))<0)
    {
        printf("Socket bind failed\n");
        close(srvr_sockt);
        return 0;
    }
   
    recvfrom(srvr_sockt, Rcv_buffer, BUFSIZE,0,(struct sockaddr *)&address, &addrlen);
    /* Invoking "ReceiveThread" function as a thread whose handle is "RecvT" */
    pthread_create(&RecvT,NULL,ReceiveThread,NULL);

    /* Invoking "SendThread" function as a thread whose handle is "SendT" */
    pthread_create(&SendT,NULL,SendThread,NULL);

    /* Wait until all threads are terminated */
    pthread_join(RecvT,NULL);
    pthread_join(SendT,NULL);   
    
    while(1);
    close(srvr_sockt);
    finish:
    Free_Playing_Source();
    return 0;
}
