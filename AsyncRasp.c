#include<wiringPi.h>
#include<stdio.h>

int main()
{
    int Prev_Data = 0, Curr_Data = 0;

    /* Using the physical pin numbers nomenclature */
    wiringPiSetupPhys();
    /* MSB(bit-3) */
    pinMode(40, INPUT);  
    /* bit-2 */
    pinMode(38, INPUT);
    /* bit-1 */
    pinMode(36, INPUT);
    /* LSB(bit-0) */
    pinMode(32, INPUT);  
    
    while(1)
    {  
        Curr_Data = 0;
         
        /* Reading the data from GPIO pins */
        /* Extracting MSB(bit-3) */
        Curr_Data = digitalRead(40);
        Curr_Data = Curr_Data << 1;
        /* Extracting bit-2 */
        Curr_Data = (Curr_Data | digitalRead(38));
        Curr_Data = Curr_Data << 1;
        /* Extracting bit-1 */
        Curr_Data = (Curr_Data | digitalRead(36));
        /* Extracting LSB(bit-0) */
        Curr_Data = Curr_Data << 1;
        Curr_Data = (Curr_Data | digitalRead(32));

        /* Checking if there is any change in the state of pins. If there is a change in sate, output
           to display */
        if(Curr_Data != Prev_Data).
        {
             printf("%d\n", Curr_Data);
             /* Assign new value to previous value */
             Prev_Data = Curr_Data;   
        }
        delay(500);
    }
    return 0;
}

/*..............................................................NOTES........................................................................../

Mapping:-

GPIO pin-6 of aurdino is connected to pin- of raspberry-32 ----> LSB(bit-0)
GPIO pin-7 of aurdino is connected to pin- of raspberry-36 ----> bit-1
GPIO pin-8 of aurdino is connected to pin- of raspberry-38 ----> bit-2
GPIO pin-9 of aurdino is connected to pin- of raspberry-40 ----> MSB(bit-3)

Whenever there is any change in state of anyone of the pins of gpio pins,the received new data is printed */
