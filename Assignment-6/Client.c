#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
  
int ClientSckt = 0, loop_break=0;
char message[1024] = {0};
char buf[1024] = {0};

struct sockaddr_in ServAddr;

/* Declaring handle variables in order to handle threads */
pthread_t SendT, RecvT;

/* Thread functions prototypes */
void *SendThread();
void *ReceiveThread();

/* Specify IP address of server as command line arguments */
int main(int argc, char const *argv[])
{    
    /* Creating socket with communication using TCP */
    if ((ClientSckt = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\nSocket creat error\n");
        return 0;
    }
  
    /* Clearing the Server address strcture */
    memset(&ServAddr, '0', sizeof(ServAddr));
  
    /* Specifying port and address of server */
    ServAddr.sin_family = AF_INET;
    ServAddr.sin_port = htons(6998);      
    
    if(inet_pton(AF_INET, argv[1], &ServAddr.sin_addr)<=0) 
    {
        printf("Invalid address\n");
        close(ClientSckt);
        return 0;
    }
    /* Connect to client */
    if (connect(ClientSckt, (struct sockaddr *)&ServAddr, sizeof(ServAddr)) < 0)
    {
        printf("Connect Failed \n");
        close(ClientSckt);
        return 0;
    }

    /* Invoking "SendThread" function as a thread whose handle is "SendT" */
    pthread_create(&SendT,NULL,SendThread,NULL);

    /* Invoking "ReceiveThread" function as a thread whose handle is "RecvT" */
    pthread_create(&RecvT,NULL,ReceiveThread,NULL);

    /* Wait until all threads are terminated */
    //pthread_join(SendT,NULL);
    //pthread_join(RecvT,NULL);
    
/*..........................................................Chat with server...............................................................*/    
   while(!loop_break)
   {

   }

   /* Send END to client to indicate end of session */
    char dummy[] = "END\0";

    strcpy(message, dummy);
    write(ClientSckt ,message, strlen(message)+1, 0 );
    printf("Session ended with server\n");

    /* Close the socket */
    close(ClientSckt);

    return 0;
}

void *SendThread()
{
    while(1)
    {
        printf("Enter the data to be send to server\n");
        gets(message);
        write(ClientSckt, message, strlen(message)+1, 0 );
        memset(message, '0', sizeof(message)); 
    }
}
void *ReceiveThread()
{
    while(1)
    {
       read(ClientSckt, buf, 1024);
       if(!(strcmp(buf,"END")==0))
       {
          printf("Server: %s\n",buf);
          /* Clearing the receive buffer */
          memset(buf, '0', sizeof(buf));
       }
       else
       {
          loop_break = 1;
          break;
       }
    }
}
/************************************************************IMPORTANT.************************************************************************/

//give 127.0.0.1 as commad line argument to indicate local coomunciation
//By sending "END" the chat will be ended with server
