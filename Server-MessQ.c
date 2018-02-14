#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    struct msg_buffer 
    {
         long msg_type;
         char msg_text[100];
    }message;

    key_t key;
    int msgid,msgid1,len;
    long ClientId;
    int opernds[3],result,Inval_Op_fg=0;
    char op[10], operator;

    /* Geting a unique key value */
    if((key = ftok("Server-MessQ.c", '1')) == -1)
    {
         printf("Ftoken Error");
         return 0;
    }

    /* Creating a message queue */
    if(-1 == (msgid = msgget(key, (0644 | IPC_CREAT))))
    {
         printf("Msgget Error");
         return 0;
    }

    /* Server waits forever in order to service clients */
    while(1)
    { 
    	/* Receiving messages from clients over the message queue whose id is "msgid", copy
           the received data in to message structure and it receives messages from client in
           FIFO(whoever comes first has to recived)*/
    	msgrcv(msgid, &message, sizeof(message), 0, 0);

        /* Copy the type.This field helps server in differentaiting diff clients.Each client should
           use different type */
    	ClientId = message.msg_type;

    	printf("\nInput request from client-%ld is to compute %s\n",ClientId,message.msg_text); 

    	int i = 0,j = 0;
        Inval_Op_fg = 0;

    	/* Extracting 2-operands and operator */
    	for(i=0; i<3; i++)
    	{
       	     int k = 0;
             /* Extracting */
       	     while(('\0' != message.msg_text[j]) && (' ' != message.msg_text[j]))
             {
                  op[k++] = message.msg_text[j++];

       	     }

             /* Adding a null character at the end */
             op[k] = '\0';
             j++;
       
             if(i!=1)
             {    
                  opernds[i] = atoi(op);
             }
             else
             {
                  operator = op[0]; 
             }   
        }
   
        /* Calculating the results */
        switch(operator)
        {
             case '+':
             result = opernds[0]+opernds[2];    
             break;

             case '-':
             result = opernds[0]-opernds[2];         
             break;

             case '*':
             result = opernds[0]*opernds[2];
             break;

             default :
             Inval_Op_fg = 1;
             break;

        }
  
        if(1!=Inval_Op_fg) 
             /* Converting from int to string */
             gcvt(result, 10, message.msg_text);
        else
             strcpy(message.msg_text, "Inavlid-operation is received from client");

        /* Giving permissions so that only desired client can read it */
        message.msg_type = ClientId;//
  
        /* calculating the length of message text */
        len = strlen(message.msg_text) + 1;
   
        /* Sending calculated result to client */
        if(msgsnd(msgid, &message, len, 0) == -1) 
            printf("\nmsgsnd");
    }   

    /* Destroy the message queue */
    if (msgctl(msgid, IPC_RMID, NULL) == -1)
    {
       perror("msgctl");
       exit(1);
    }

    return 0;
}
