/*......................................................................Server.................................................................*/
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

/* Thread functions prototypes */
void *SendThread();
void *ReceiveThread();

int Clint_socket, loop_break=0;
char Recvbuf[1000] = {0};
char message[1000] = {0};

struct sockaddr_in ServAddress;

/* Declaring handle variables in order to handle threads */
pthread_t SendT, RecvT;

int main()
{
    int Serv_Socket;
    
    int addrlen = sizeof(ServAddress);
    
    int Sockopt = 1;    
      
    /* Creating socket with communication using TCP */
    if ((Serv_Socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        printf("Socket creation failed\n");
        return 0;
    }
   
    /* Socket opt */    
    if (setsockopt(Serv_Socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &Sockopt, sizeof(Sockopt)))
    {
        printf("Setsockopt failed\n");
        close(Serv_Socket);
        return 0;
    }
   
    /* Specifying welcome port */
    ServAddress.sin_family = AF_INET;
    ServAddress.sin_addr.s_addr = INADDR_ANY;
    ServAddress.sin_port = htons(6998);
      
    /* Binding to the socket created above */
    if (bind(Serv_Socket, (struct sockaddr *)&ServAddress, sizeof(ServAddress))<0)
    {
        printf("Bind failed\n");
        close(Serv_Socket);
        return 0;
    }
    
    /* Server listening for client requests and allowing at 10 requests in queue */
    if (listen(Serv_Socket, 10) < 0)
    {
        printf("Listen failed\n");
        close(Serv_Socket);
        return 0;
    }
 
    /* Accept the requests from clients */
    if ((Clint_socket = accept(Serv_Socket, (struct sockaddr *)&ServAddress, (socklen_t*)&addrlen))<0)
    {
        printf("Accept failed\n");
        close(Serv_Socket);
        return 0;
    }

    /* Invoking "SendThread" function as a thread whose handle is "SendT" */
    pthread_create(&SendT,NULL,SendThread,NULL);

    /* Invoking "ReceiveThread" function as a thread whose handle is "RecvT" */
    pthread_create(&RecvT,NULL,ReceiveThread,NULL);

    /* Wait until all threads are terminated */
    // pthread_join(SendT,NULL);
    // pthread_join(RecvT,NULL);
    
    /*..........................................................Chat with client...............................................................*/
    while(!loop_break)
    { 

    }

    /* Send END to Server to indicate end of session */
    char dummy[] = "END\0";
    strcpy(message, dummy);
    write(Clint_socket, message, strlen(message)+1, 0 );

    printf("Session ended with client\n");
   
    /* Close both the sockets */
    close(Serv_Socket);
    close(Clint_socket);
    
    return 0;    
}


void *SendThread()
{
    while(1)
    {
        printf("Enter text to be sent to the client\n");
        gets(message);//
        write(Clint_socket ,message, strlen(message)+1, 0 );
        /* Clearing the message buffer */
        memset(message, '0', sizeof(message)); 
    }
}


void *ReceiveThread()
{
   while(1)
   {
        read(Clint_socket ,Recvbuf, 1000);
        if(!(strcmp(Recvbuf,"END")==0))
        {
            printf("Client:%s\n",Recvbuf);
            /* Clearing the receive buffer */
            memset(Recvbuf, '0', sizeof(Recvbuf)); 
        }
        else
        {
          loop_break = 1;
          break;
        }
   }
}

/************************************************************IMPORTANT.************************************************************************/
//By sending "END" the chat will be ended with client
