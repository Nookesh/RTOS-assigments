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
#define RATE 5000
struct rtp_header 
{
	uint8_t v:2;
	uint8_t p:1;
	uint8_t x:1;
	uint8_t cc:4;
	uint8_t m:1;
	uint8_t pt:7;
	uint16_t seqno;
	uint32_t ts;
	uint32_t ssrc;
	uint8_t buf[BUFSIZE]; 
}rtp_packet;

struct rtp_header sender;
struct rtp_header receiver;
uint16_t seqcount = 0;
uint16_t Rcvr_SeqNum = 0; 

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

pthread_t SendT, RecvT;

void Create_Handle_For_Playing(void)
{
    int error;

    if (!(s = pa_simple_new(NULL, "Server", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) 
    {
        printf(": pa_simple_new() failed: %s\n", pa_strerror(error));
    }
}

void Create_Handle_For_Recording(void)
{
    int error;    

    if (!(s1 = pa_simple_new(NULL, "Server", PA_STREAM_RECORD, NULL, "record", &ss1, NULL, NULL, &error))) 
    {
        printf("Error: adin_pulseaudio: pa_simple_new() failed: %s\n", pa_strerror(error));
    }
}

void Free_Playing_Source(void)
{
    if (s != NULL)
    {
        pa_simple_free(s);
        s = NULL;
    }
}
void Free_Recording_Source(void)
{
    if (s1 != NULL)
    {
        pa_simple_free(s1);
        s1 = NULL;
    }
}

void Record_Audio (uint8_t *buf, int sampnum)
{
    int error;

    if (pa_simple_read(s1, buf, BUFSIZE, &error) < 0)
    {
        printf("Error: pa_simple_read() failed: %s\n", pa_strerror(error));
    }
}

void * SendThread()
{
    size_t r;
    Create_Handle_For_Recording();
    while(r!=50000)
    {
        Record_Audio(sender.buf, BUFSIZE);

	struct timeval tv;
	time_t curtime;
	gettimeofday(&tv, NULL); 
	sender.ts=tv.tv_sec;
	
	sender.seqno = seqcount++;
	if (seqcount == 65536)
	{
	    seqcount = 0;
	}	    		
        printf("Sequence number: %d,  ",sender.seqno); 
        r=sendto(srvr_sockt, &sender, sizeof(sender), 0, (struct sockaddr *)&address, sizeof(address));
        printf("number of bytes sent: %ld\n",r);
    } 
    Free_Recording_Source();  
}

void * ReceiveThread()
{
    size_t r;
    int error;
    Create_Handle_For_Playing();
    while(r!=50000)
    {
        r = recvfrom(srvr_sockt, &receiver, sizeof(receiver),0,(struct sockaddr *)&address, &addrlen);
        printf("number of bytes: %ld, ",r);
        printf("Sequence number received: %d\n",receiver.seqno);

        /* If receied sequence number is greater than or equal to expected sequence number */   
        if(receiver.seqno >= Rcvr_SeqNum)
        {
            Rcvr_SeqNum = receiver.seqno;   
            if (pa_simple_write(s, receiver.buf, BUFSIZE, &error) < 0) 
            {
                printf(": pa_simple_write() failed: %s\n", pa_strerror(error));
                break;
            }
            
	    
        }
    }
    Free_Playing_Source();
}


int main()
{
    sender.v = 2;
    sender.p = 0;
    sender.x = 0;
    sender.cc = 1;
    sender.m = 0;
    sender.pt =	2;
    sender.ssrc = 1;	

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
   
    recvfrom(srvr_sockt, &receiver, sizeof(receiver),0,(struct sockaddr *)&address, &addrlen);
    pthread_create(&RecvT,NULL,ReceiveThread,NULL);
    pthread_create(&SendT,NULL,SendThread,NULL);
    pthread_join(RecvT,NULL);
    pthread_join(SendT,NULL);   
    
    while(1);
    close(srvr_sockt);
    Free_Recording_Source();
    Free_Playing_Source();
    return 0;
}
