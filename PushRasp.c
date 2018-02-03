#include<wiringPi.h>
#include<stdio.h>


/* This routine is called when a rising edge is detected on the 37-pin */
void RcvData(void)
{
    int data = 0;
    /* Reading the data from GPIO pins */

    /* Extracting MSB(bit-3) */
    data = digitalRead(40);
    data = data << 1;
    /* Extracting bit-2 */
    data = (data | digitalRead(38));
    /* Extracting bit-1 */
    data = data << 1;
    data = (data | digitalRead(36));
    /* Extracting LSB(bit-0) */
    data = data << 1;
    data = (data | digitalRead(32));

    /* Printing the received data to output */
    printf("%d\n", data);
}

int main()
{
    /* Using the physical pin numbers nomenclature */
    wiringPiSetupPhys(); 

    /* Configuring pins as input to read data */

    /* MSB(bit-3) */
    pinMode(40, INPUT);
    /* bit-2 */
    pinMode(38, INPUT);
    /* bit-1 */
    pinMode(36, INPUT);
    /* (LSB)bit-2 */
    pinMode(32, INPUT); 

    /* Control probe signal to indicate the upcoming data */
    pinMode(37, INPUT);  
    
    /* Enabes an interrupt when a rising edge is detected on 37-pin and calls the function "RcvData". */
    wiringPiISR(37, INT_EDGE_RISING, RcvData); 
    
    /* Wait forever */
    while(1);
    return 0;
}


/*..............................................................NOTES........................................................................../

Mapping:-

GPIO pin-6 of aurdino is connected to pin- of raspberry-32 ----> LSB(bit-0)
GPIO pin-7 of aurdino is connected to pin- of raspberry-36 ----> bit-1
GPIO pin-8 of aurdino is connected to pin- of raspberry-38 ----> bit-2
GPIO pin-9 of aurdino is connected to pin- of raspberry-40 ----> MSB(bit-3)
GPIO pin-10 of aurdino is connected to pin- of raspberry-37 ----> Control pin


Whenever a control signal(37pin) of raspberry pi changes its state from logic-0 to logic-1(on rising edge),
an interrupt is fired  and the ISR is void RcvData(void).This function reads the value of pins and outputs to display */
