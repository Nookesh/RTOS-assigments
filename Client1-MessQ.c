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
    int msgid,len;

    /* Geting a unique key value */
    if((key = ftok("Client1-MessQ.c", '1')) == -1)
    {
         printf("\nFtoken Error");
         return 0;
    }

    /* Creating a message queue using and giving read write permissions */
    if(-1 == (msgid = msgget(key, (0644 | IPC_CREAT))))
    {
         printf("\nMsgget Error");
         return 0;
    }
   
    /* This client uses 1 as type */
    message.msg_type = 1;

    printf("\nEnter the expression you want to compute(+ or - or *)\n eg:a * b ");
    gets(message.msg_text);
  
    len = strlen(message.msg_text) + 1;

    /* Sending input data to server */
    if (msgsnd(msgid, &message, len, 0) == -1)
         printf("\nmsgsnd"); 

    /* Receiving result from client of type-1 only */
    msgrcv(msgid, &message, sizeof(message), 1, 0);
    printf("\nResult obtained from server is %s",message.msg_text); 

    return 0;
}
