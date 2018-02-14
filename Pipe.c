#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{ 
    /* For storing file desciptors of pipe */
    int Fd[2];
    pid_t pid;
    int status;

    if(-1 == pipe(Fd))
    {
        printf("\nCant create pipe");
        return 0;
    }

    if((pid = fork()) < 0)
    {
        printf("\nCant fork a child");
        return 0;  
    }

    /* Child-process */
    if(0 == pid)
    {
        /* Closing standard input file discriptor and making a copy of it and assigning it to Fd[0] */
        dup2(Fd[0], 0);

        /* Closing the writting file descriptor of pipe */
        close(Fd[1]);
  
        /* Command and arguemnts of word count in lines */
        const char* list[] = { "wc", "-l", 0};
      
        /* Exec system call replaces all executable statements after exec sys call with the program mentioned in list[] */
        execvp(list[0], list);
 
        /* If exec is failed,this should be printed */
        perror("execvp of ls failed");
    }

    /* Parent-process */
    else
    {
        /* Closing standard output file discriptor and making a copy of it and assigning it to Fd[1] */
        dup2(Fd[1], 1);
   
        /* Closing the reading file descriptor of pipe */
        close(Fd[1]);
  
        /* Command and arguemnts of list all in vertical format */
        const char* WordCount[] = { "ls", "-1", 0};

        /* Exec system call replaces all executable statements after exec sys call with the program mentioned in WordCount[] */
        execvp(WordCount[0], WordCount);

        /* If exec is failed,this should be printed */
        perror("execvp of wc failed");

        /* Close pipe file descriptors */
        close(Fd[1]);
        close(Fd[0]);
    
        /* Wait until child terminates */
        wait(0);        
     }
return 0;
}
