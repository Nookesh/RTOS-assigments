/*......................................................................Server.................................................................*/
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

int main()
{
    int Serv_Socket, Clint_socket;
    struct sockaddr_in ServAddress;
    int addrlen = sizeof(ServAddress);
    char Recvbuf[1000] = {0};
    char message[1000] = {0};
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
    ServAddress.sin_port = htons(8080);//
      
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
    
    /*..........................................................Chat with client...............................................................*/
    read(Clint_socket ,Recvbuf, 1000);
    printf("Client: %s\n",Recvbuf);

    /* Chat until receives a END from client */
    while(!(strcmp(Recvbuf,"END")==0))
    { 
        memset(Recvbuf, '0', sizeof(Recvbuf)); 
        printf("Enter text to be sent to the client\n");
        gets(message);//
        write(Clint_socket ,message, strlen(message)+1, 0 );
        memset(message, '0', sizeof(message)); 
        read(Clint_socket ,Recvbuf, 1000);
        printf("Client:%s\n",Recvbuf);
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
