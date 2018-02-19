#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
  
/* Specify IP address of server as command line arguments */
int main(int argc, char const *argv[])
{
    int ClientSckt = 0;
    struct sockaddr_in ServAddr;
    char message[1024] = {0};
    char buf[1024] = {0};
    
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
    ServAddr.sin_port = htons(8080);      
    
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
    
/*..........................................................Chat with server...............................................................*/    
    while(!(strcmp(buf,"END")==0))
    {
         printf("Enter the data to be send to server\n");
         gets(message);
         write(ClientSckt, message, strlen(message)+1, 0 );
         memset(message, '0', sizeof(message)); 
         read(ClientSckt, buf, 1024);
         printf("Server: %s\n",buf);         
    }
    /* Send END to client to indicate end of session */
    char dummy[] = "END\0";
    strcpy(message, dummy);
    write(ClientSckt ,message, strlen(message)+1, 0 );

    /* Close the socket */
    close(ClientSckt);
}

/************************************************************IMPORTANT.************************************************************************/

//give 127.0.0.1 as commad line argument to indicate local coomunciation
