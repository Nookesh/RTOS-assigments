/*......................................................................Client.................................................................*/
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <arpa/inet.h>

#include <pulse/simple.h>
#include <pulse/error.h>

#define PORT 6542

#define BUFSIZE 18000 

/* sampling rate is atleast twice of maximum freq of audio */
#define RATE 6800  

/* For recording */
static pa_simple *s = NULL; 

/* For playing */ 
static pa_simple *s1 = NULL; 

int Udp_Sckt = 0;
struct sockaddr_in serv_addr;
int addrlen = sizeof(serv_addr); 
int8_t Record_Send_Buffer[BUFSIZE];
int8_t Rcv_Buffer[BUFSIZE];
pthread_t SendT, RecvT;

/* Recording Spec*/
static const pa_sample_spec ss = { 
  	.format = PA_SAMPLE_S16LE,
  	.rate = RATE,
  	.channels = 2
    };


/* Playing Spec */
static const pa_sample_spec ss1 = {
  	.format = PA_SAMPLE_S16LE,
  	.rate = RATE,
  	.channels = 2 
    };


/* For recording */
void Create_Handle_For_Recording(void)
{
    int error;
  
    if (!(s = pa_simple_new(NULL, "client", PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error)))
    {
        printf("Error: adin_pulseaudio: pa_simple_new() failed: %s\n", pa_strerror(error));
    }
}

/* For playing */
void Create_Handle_For_Playing(void)
{
    int error;

    if (!(s1 = pa_simple_new(NULL, "client", PA_STREAM_PLAYBACK, NULL, "playback", &ss1, NULL, NULL, &error)))
    {
        printf(": pa_simple_new() failed: %s\n", pa_strerror(error));
    }
}

/* Free recording source */
void Free_Recording_Source(void)
{
    if (s != NULL) 
    {
        pa_simple_free(s);
        s = NULL;
    }
}

/* Free playing source */
void Free_Playing_Source(void)
{
    if (s1 != NULL) 
    {
        pa_simple_free(s1);
        s1 = NULL;
    }
}

/* Recording to a buffer */
void Record_Audio(int8_t *buf, int sampnum)
{
    int error;

    if (pa_simple_read(s, buf, BUFSIZE, &error) < 0) 
    {
        printf("Error: pa_simple_read() failed: %s\n", pa_strerror(error));
    }
}

/* Records audio and send those packets through socket */
void *SendThread()
{
    /* Signed size type */
    size_t r; 
   
    /* Create handle for recording */
    Create_Handle_For_Recording();
    while(r!=50000)
    {
	Record_Audio(Record_Send_Buffer, BUFSIZE);
        r=sendto(Udp_Sckt ,Record_Send_Buffer, BUFSIZE , 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        printf("number of bytes sent %ld\n",r); 
    }  
    /* Free the recording source */
    Free_Recording_Source(); 
}

/* Receive packets from sockets and play the received audio packets */
void * ReceiveThread()
{
    size_t r;
    int error;
   
    /* Create handle for playing */
    Create_Handle_For_Playing();
    while(r!=50000)
    {
        /* Receive packets from UDP socket */
        r = recvfrom(Udp_Sckt, Rcv_Buffer, BUFSIZE, 0, (struct sockaddr *)&serv_addr, &addrlen);
   
        if (pa_simple_write(s1, Rcv_Buffer, BUFSIZE, &error) < 0)
        {
            printf(": pa_simple_write() failed: %s\n", pa_strerror(error));
            break;
        }
        printf("number of bytes received %ld\n",r);
     }
     /* Free the playing source */
     Free_Playing_Source();
}

/* Specify IP address of server as command line arguments */
int main(int argc, char const *argv[])
{
      /* Creating socket with communication using UDP */
      if ((Udp_Sckt = socket(AF_INET, SOCK_DGRAM, 0)) < 0)  // check with SOCK_RAW (direct IP service)
      {
        printf("\nUdp Socket creation fail \n");
        return -1;
      }
  
      /* Clearing the Server address strcture */
      memset(&serv_addr, '0', sizeof(serv_addr));
  
      /* Specifying port and address of server */
      serv_addr.sin_family = AF_INET;
      serv_addr.sin_port = htons(PORT);
      if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
      {
          printf("Invalid address\n");
          close(Udp_Sckt);
          return 0;
      }

      /* Invoking "SendThread" function as a thread whose handle is "SendT" */
      pthread_create(&SendT,NULL,SendThread,NULL); 
     
      /* Invoking "ReceiveThread" function as a thread whose handle is "RecvT" */
      pthread_create(&RecvT,NULL,ReceiveThread,NULL);

      /* Wait until all threads are terminated */
      pthread_join(SendT,NULL);
      pthread_join(RecvT,NULL);       
     
      while(1);

      close(Udp_Sckt);
      Free_Recording_Source();
      Free_Playing_Source();
      return 0;    
}
