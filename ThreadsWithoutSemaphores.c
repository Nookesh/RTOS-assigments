/*.......................................................Threads execution without semaphores..................................................*/
#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<time.h>

/* Thread functions prototypes */
void *Thread1Proc();
void *Thread2Proc();
void *Thread3Proc();

/* These global variables are shared by all threads invoked in this program */
int Global_a, Global_b, Global_c;

int main()
{
    /* Assigning values to global varibles before invoking threads */
    Global_a = 1;
    Global_b = 2;
    Global_c = 3;
  
    /* Declaring handle variables in order to handle threads */
    pthread_t T1, T2, T3;

    /* Invoking "Thread1proc" function as a thread whose handle is "T1" */
    pthread_create(&T1,NULL,Thread1Proc,NULL);

    /* Invoking "Thread2proc" function as a thread whose handle is "T2" */
    pthread_create(&T2,NULL,Thread2Proc,NULL);

    /* Invoking "Thread3proc" function as a thread whose handle is "T3" */
    pthread_create(&T3,NULL,Thread3Proc,NULL);
    
    /* Wait until all threads are terminated. If we dont use these function calls, our program terminates
       as soon main function completes its execution */
    pthread_join(T1,NULL);
    pthread_join(T2,NULL);
    pthread_join(T3,NULL);

    return 0;
}


void *Thread1Proc()
{
    /* Declaring and initialising local variables */
    int i = 0,j = 2,k = 4;
   
    /* Assigning local variables of thread to shared global variables */
    Global_a = i;
    Global_b = j;
    Global_c = k;    

    printf("Executing Thread-1\n");
    printf("Address of local variables\ni ---> %p   ,j ---> %p    ,k ---> %p\n", &i, &j, &k);
    printf("Address of global variables\nGlobal_a  ---> %p   ,Global_b ---> %p    ,Global_c ---> %p\n", &Global_a, &Global_b, &Global_c);
    printf("Values of local variables are i=%d,  j=%d, k=%d\n",i,j,k);
    printf("Values of global variables are a=%d,  b=%d, c=%d\n\n",Global_a,Global_b,Global_c);

    /* Wait forever */
    while(1);
}


void *Thread2Proc()
{
    /* Declaring and initialising local variables */
    int i = 1,j = 3,k = 5;
   
    /* Assigning local variables of thread to shared global variables */
    Global_a = i;
    Global_b = j;
    Global_c = k;

    printf("Executing Thread-2\n");
    printf("Address of local variables\ni ---> %p   ,j ---> %p    ,k ---> %p\n", &i, &j, &k);
    printf("Address of global variables\nGlobal_a  ---> %p   ,Global_b ---> %p    ,Global_c ---> %p\n", &Global_a, &Global_b, &Global_c);
    printf("Values of local variables are i=%d,  j=%d, k=%d\n",i,j,k);
    printf("Values of global variables are a=%d,  b=%d, c=%d\n\n",Global_a,Global_b,Global_c);
 
    /* Wait forever */
    while(1);
}


void *Thread3Proc()
{
    /* Declaring and initialising local variables */
    int i = -4,j = -5,k = -6;

    /* Assigning local variables of thread to shared global variables */
    Global_a = i;
    Global_b = j;
    Global_c = k;

    printf("Executing Thread-3\n");
    printf("Address of local variables\ni ---> %p   ,j ---> %p    ,k ---> %p\n", &i, &j, &k);
    printf("Address of global variables\nGlobal_a  ---> %p   ,Global_b ---> %p    ,Global_c ---> %p\n", &Global_a, &Global_b, &Global_c);
    printf("Values of local variables are i=%d,  j=%d, k=%d\n",i,j,k);
    printf("Values of global variables are a=%d,  b=%d, c=%d\n\n",Global_a,Global_b,Global_c);

    /* Wait forever */
    while(1);
}

/*.......................................................Observations and conclusions..........................................................*/


/* As here,threads are implemented without any locking mechanism,
1. The output is not always the same(i.e different threads set the common global variables to its local variables
2. As global variables are shared by all threads, global variables address remains same in all threads 
3. As O.S schedules these threads in cocurrent way(in a time slice mannner), the global values set by other threads may be
   shown(printed) by some other threads 
4. As global variables are stored in heap, their addresses are lower than that of local variables.Local variables are stored
   in stack.So local variables have higher addresses when compared to global variabl addresses
5. As local variables are private to each thread, local variables of one thread cant be accessed by other threads.So addresses
   are different for different local variables of threads 
6. As 3 threads entered in to while(1) loop, they will wait forever.But main calls "pthead_join()" for all theads indicating
   main funtion terminates only when all threads are terminated
7. As 3 threads wait forever,implies main also waits forever, thereby program runs forever until we kill it */
