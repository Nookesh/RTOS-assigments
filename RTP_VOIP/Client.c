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
#include <sys/time.h>	

#include <pulse/simple.h>
#include <pulse/error.h>

#define BUFSIZE 18000
#define PORT 6542 
#define RATE 5000  

/* RTP header */
struct RTP_Header 
{
	uint8_t Ver:2;
	uint8_t Padding:1;
	uint8_t Extension:1;
	uint8_t CSRC_Count:4;
	uint8_t Marker:1;
	uint8_t Payload_type:7;
	uint16_t Seq_Num;
	uint32_t Time_Stamp;
	uint32_t SSRC;
        uint8_t buf[BUFSIZE];
}RTP_Tx_Packet, RTP_Rx_Packet;

uint16_t Sender_SeqNum = 0;
uint16_t Rcvr_SeqNum = 0; 
/* For recording */
static pa_simple *s = NULL; 
/* For playing */ 
static pa_simple *s1 = NULL; 
int Udp_Sckt = 0;
struct sockaddr_in serv_addr;
int addrlen = sizeof(serv_addr);
pthread_t SendT, RecvT;

/* Recording */
static const pa_sample_spec ss = { 
  	.format = PA_SAMPLE_S16LE,
  	.rate = RATE,
  	.channels = 2 
    };


/* Playing */
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
 
    /* Write the recorded samples to buffer using pulse audio API */
    if (pa_simple_read(s, buf, BUFSIZE, &error) < 0) 
    {
        printf("Error: pa_simple_read() failed: %s\n", pa_strerror(error));
    }
}


void *SendThread()
{
    /* Signed size type */
    size_t r; 
    
    /* Create handle for recording */
    Create_Handle_For_Recording();
    while(r!=50000)
    {  
	Record_Audio(RTP_Tx_Packet.buf, BUFSIZE);   
        if(65536 == Sender_SeqNum)
        {
            Sender_SeqNum = 0;
        }
        RTP_Tx_Packet.Seq_Num = Sender_SeqNum++;
        
        /* Extract system time in secinds to update the time stamp field */
        struct timeval tv;
        gettimeofday(&tv, NULL); 
        RTP_Tx_Packet.Time_Stamp = tv.tv_sec;

        r=sendto(Udp_Sckt ,&RTP_Tx_Packet, sizeof(RTP_Tx_Packet) , 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        printf("Seq_num sent is %d,  Number of bytes sent %ld\n",Sender_SeqNum, r); 
    } 
    
    /* Free recoding source */
    Free_Recording_Source();  
}

void * ReceiveThread()
{
    size_t r;
    int error;
    Create_Handle_For_Playing();
    while(r!=50000)
    {
        r = recvfrom(Udp_Sckt, &RTP_Rx_Packet, sizeof(RTP_Rx_Packet), 0, (struct sockaddr *)&serv_addr, &addrlen);
        printf("number of bytes received %ld...     ",r);
        printf("Seq_Num recvd is %d\n",RTP_Rx_Packet.Seq_Num);
  
        /* If receied sequence number is greater than or equal to expected sequence number */
        if(RTP_Rx_Packet.Seq_Num >= Rcvr_SeqNum)
        {
             Rcvr_SeqNum = RTP_Rx_Packet.Seq_Num;
             if (pa_simple_write(s1, RTP_Rx_Packet.buf, BUFSIZE , &error) < 0)
             {
                 printf(": pa_simple_write() failed: %s\n", pa_strerror(error));
                 break;
             }
            
        } 
     }
     Free_Playing_Source();
}

/* Specify IP address of server as command line arguments */
int main(int argc, char const *argv[])
{
      /* Initialise all the RTP header structure members */
      RTP_Tx_Packet.Ver = 2;
      RTP_Tx_Packet.Padding = 0;
      RTP_Tx_Packet.Extension = 0;
      RTP_Tx_Packet.CSRC_Count = 1;
      RTP_Tx_Packet.Marker = 0;
      RTP_Tx_Packet.Payload_type = 2;
      RTP_Tx_Packet.SSRC = 1;

      /* Creating socket with communication using UDP */
      if ((Udp_Sckt = socket(AF_INET, SOCK_DGRAM, 0)) < 0)  
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
 
      printf("Welcome to VOIP Call\nConnecting..............\n");
     
      while(1);

      close(Udp_Sckt);
      Free_Recording_Source();
      Free_Playing_Source();
      return 0;    
}
