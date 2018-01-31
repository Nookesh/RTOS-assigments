#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

/* Function Prototype*/
void PrintVariable(void);

/* Global varaibles */
int Global_a, Global_b, Global_c;

int main()
{
   FILE *Pid_fpr;
   /* Local variables of main */
   int a, b, c;
   
   /* Declaring a process ID for child */
   pid_t child_pid;
  
   /* Printing local and global variable addresses in main */
   printf("Address of local variables\na ---> %u   ,b ---> %u    ,c ---> %u\n", &a, &b, &c);
   printf("Address of global variables\nGlobal_a  ---> %u   ,Global_b ---> %u    ,Global_c ---> %u\n\n", &Global_a, &Global_b, &Global_c);
   
   /* Forking a child-1 process */
   child_pid = fork();

   /* If forking a child process is success */
   if(child_pid >= 0)
   {
       /* If it is child process */
       if(0 == child_pid) // first child
       {
           PrintVariable();
           while(1);
       }
       /* It is for parent process */
       else
       {
            /* Opening a file to write child PIDs */
            Pid_fpr = fopen("ProcessPID.txt", "w");
         
            if(0 == Pid_fpr)
                 printf("Cant open a file");
            else
            {
                 /* Write child PID value to the file */
                 fprintf(Pid_fpr, "%d\n",child_pid);
            }

            /* Forking a child-2 process */
            child_pid = fork();
            
            /* If forking a child process is success */
            if(child_pid >= 0)
            {
                 if(0 == child_pid) // 2nd child
                 {
                     PrintVariable();
                     while(1);
                 }
                 else
                 {
                      /* Write child PID value to the file */
                      fprintf(Pid_fpr, "%d\n",child_pid);                      
                      
                      /* Forking a child-3 process */
                      child_pid = fork();
      
                      /* If forking a child process is success */
                      if(child_pid >= 0)
                      {
                          if(0 == child_pid) // 3rd child
                          {
                              PrintVariable();
                              while(1);
                          }
                          else
                          {
                              /* Write child PID value to the file */
                              fprintf(Pid_fpr, "%d\n",child_pid);

                              fprintf(Pid_fpr, "%d\n",getpid());

                              /* Close file */
                              fclose(Pid_fpr);

                              while(1);
                          }
                       }
                       else
                       printf("Fork is unsuccessful");
                 }
            }
            else
               printf("Fork is unsuccessful");        
       }
   }
   else
      printf("Fork is unsuccessful");
 
return 0;
}

void PrintVariable(void)
{
    int j, k, l;
    srand(time(NULL));
    j = rand()%100;
    k = rand()%100;
    l = rand()%100;
    printf("Address of local variables\nj ---> %u   ,k ---> %u    ,l ---> %u\n", &j, &k, &l);
    printf("Values of j, k, l are %d,  %d,  %d\n",j,k,l);
    printf("Address of global variables\nGlobal_a  ---> %u   ,Global_b ---> %u    ,Global_c ---> %u\n\n", &Global_a, &Global_b, &Global_c);

}


/*.......................................................Observations and conclusions..........................................................*/

/*
  1. Here address printed by "printf" are virtual addresses, they are not real addresses. We(Programmer) can see only
     virual addresses, we cant see real physical addresses.
  2. Memory Management Unit(MMU) maps logical(virtual) addresses to physical addresses by adding offset value. Each process has a
     unique offset value.Thereby each process maps to different physical memory.
  3. Two processes(i.e parent and child) can have same virtual memory, but have different physical memory.
  4. Therefore using printf function we are getting same addresses for different process.
  5. When a new child process is forked by a parent process, then (virtual)all kinds of data pointers, memory addresses,
     variables will be duplicated in a seperate physical memory.So, all pointers from old process remain valid in new process
     and point to new process objects. 
  6. All processes enter in to wait forever state,So we are passing PIDs of all processes in to txt file.Then we are running
     a bash script to read PIDs from this txt file and kill those processes using PIDs. 
 

/*.......................................................Links and references..................................................................*/

    // https://stackoverflow.com/questions/13286931/does-fork-create-a-duplicate-instance-of-all-the-variables-and-object-created
    // https://stackoverflow.com/questions/4594329/difference-between-the-address-space-of-parent-process-and-its-child-process-in?rq=1
    // https://techdifferences.com/difference-between-logical-and-physical-address.html
    // https://www.differencebtw.com/difference-between-logical-and-physical-address-in-operating-system/
    // https://www.quora.com/Is-the-address-of-variables-printed-in-a-C-program-Physical-address-or-a-Logical-address */

  

