/*.......................................................Threads execution semaphores..........................................................*/
#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<time.h>
#include<semaphore.h>

/* Thread functions prototypes */
void *Thread1Proc();
void *Thread2Proc();
void *Thread3Proc();

/* Declaring a variable to implement semaphore mechanism */
sem_t lock;

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
    
    /* Wait until all threads are terminated */
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
    printf("Address of local variables\ni ---> %p   ,j ---> %p    ,k ---> %p\n",&i, &j, &k);
    printf("Address of global variables\nGlobal_a  ---> %p   ,Global_b ---> %p    ,Global_c ---> %p\n",&Global_a, &Global_b, &Global_c);
    printf("Values of local variables are i=%d,  j=%d, k=%d\n",i,j,k);
    printf("Values of global variables are a=%d,  b=%d, c=%d\n\n",Global_a,Global_b,Global_c);

    /* This function call will increment semaphore variable "lock" value by 1. So, that it 
       allows any waiting thread(if present in waiting queue) to be unlocked and use the 
       critical(shared) data */
    sem_post(&lock);

    /* Wait forever */
    while(1);
}


void *Thread2Proc()
{
    /* Declaring and initialising local variables */
    int i = 1,j = 3,k = 5;
   
    /* This function waits until "lock" value is greater than 0.
       If value is <=0,it implies some other thread is using the critical section. So, the statements 
       after this function call wont be executed.
       If value is >0,it implies no other thread is using critical data.So it decrements lock by 
       1(i.e "lock" is made 0,so that other threads cant use critical data) and executes critical 
       section statements present after this function call */
    sem_wait(&lock);

    /* Assigning local variables of thread to shared global variables */
    Global_a = i;
    Global_b = j;
    Global_c = k;

    printf("Executing Thread-2\n");
    printf("Address of local variables\ni ---> %p   ,j ---> %p    ,k ---> %p\n", &i, &j, &k);
    printf("Address of global variables\nGlobal_a  ---> %p   ,Global_b ---> %p    ,Global_c ---> %p\n", &Global_a, &Global_b, &Global_c);
    printf("Values of local variables are i=%d,  j=%d, k=%d\n",i,j,k);
    printf("Values of global variables are a=%d,  b=%d, c=%d\n\n",Global_a,Global_b,Global_c);
 
    /* This function call will increment semaphore variable "lock" value by 1. So, that it 
       allows any waiting threads(if present in waiting queue) to be unlocked and use the 
       critical(shared) data */
    sem_post(&lock);

    /* Wait forever */
    while(1);
}


void *Thread3Proc()
{
    /* Declaring and initialising local variables */
    int i = -4,j = -5,k = -6;

    /* This function waits until "lock" value is greater than 0.
       If value is <=0,it implies some other thread is using the critical section. So, the statements 
       after this function call wont be executed.
       If value is >0,it implies no other thread is using critical data.So it decrements lock by 
       1(i.e "lock" is made 0,so that other threads cant use critical data) and executes critical 
       section statements present after this function call */
    sem_wait(&lock);

    /* Assigning local variables of thread to shared global variables */
    Global_a = i;
    Global_b = j;
    Global_c = k;

    printf("Executing Thread-3\n");
    printf("Address of local variables\ni ---> %p   ,j ---> %p    ,k ---> %p\n", &i, &j, &k);
    printf("Address of global variables\nGlobal_a  ---> %p   ,Global_b ---> %p    ,Global_c ---> %p\n", &Global_a, &Global_b, &Global_c);
    printf("Values of local variables are i=%d,  j=%d, k=%d\n",i,j,k);
    printf("Values of global variables are a=%d,  b=%d, c=%d\n\n",Global_a,Global_b,Global_c);

    /* This function call will increment semaphore variable "lock" value by 1. So, that it 
       allows any waiting threads(if present in waiting queue) to be unlocked and use the 
       critical(shared) data */
    sem_post(&lock);
 
    /* Wait forever */
    while(1);
}


/*.......................................................Observations and conclusions..........................................................*/


/* As here,threads are implemented using semaphores,
1. Only one thread can access the shared common data.i.e when one thread is using shared(critical data), other threads remain 
   in wait(Locked) state.
2. As soon as a thread finishes executing its critical code section,it unlocks the semaphore.i.e it allows any one of the other
   threads to use the critcal(common shared varibales) section.
3. Thereby, using semaphores we can regulate the control flow between threads. 
4. As global variables are shared by all threads, global variables address remains same in all threads 
5. As global variables are stored in heap, their addresses are lower than that of local variables.Local variables are stored
   in stack.So local variables have higher addresses when compared to global variabl addresses
6. As local variables are private to each thread, local variables of one thread cant be accessed by other threads.So addresses
   are different for different local variables of threads 
7. As 3 threads entered in to while(1) loop, they will wait forever.But main calls "pthead_join()" for all theads indicating
   main funtion terminates only when all threads are terminated
8. As 3 threads wait forever,implies main also waits forever, thereby program runs forever until we kill it */


/*.......................................................Links and references..........................................................*/
/*
 1. http://pages.cs.wisc.edu/~remzi/Classes/537/Fall2008/Notes/threads-semaphores.txt
 */
