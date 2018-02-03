void setup() 
{
     /* ...................Initializing digital pins of aurdino as outputs................ */

     /* Least significant bit */
     pinMode(6, OUTPUT);
     pinMode(7, OUTPUT);
     pinMode(8, OUTPUT);

     /* Most significant bit */
     pinMode(9, OUTPUT);
   
     /* Control strobe signal to be send to be made high before sending data */
     pinMode(10, OUTPUT);
}

#define BIT0    1
#define BIT1    2
#define BIT2    4
#define BIT3    8

int val, SendData = 1;

/* Loop function runs over and over again forever */
void loop() 
{
     /* Control signal to be send should be made high before sending data */
     digitalWrite(10, 1); 

     /* Overrolls to zero(i.e "SendData" varies from 0 to 15 ). This is data we are sending to 
        raspberry pi through the data pins (6, 7, 8, 9) */
     if(SendData == 16)
        SendData = 0;


     /* Extracting LSB(bit-0) */
     val = (SendData & BIT0);
     /* Sending extracted bit value through GPIO pin-6 */
     if(val  ==BIT0)
        digitalWrite(6, 1); 
     else
        digitalWrite(6, 0);


    
     /* Extracting (bit-1) */
     val = (SendData & BIT1);
     /* Sending extracted bit value through GPIO pin-7 */
     if(val == BIT1)
        digitalWrite(7, 1); 
     else
        digitalWrite(7, 0); 



     /* Extracting (bit-2) */
     val = (SendData & BIT2);
     /* Sending extracted bit value through GPIO pin-8 */
     if(val == BIT2)
        digitalWrite(8, 1); 
     else
        digitalWrite(8, 0); 


     
     /* Extracting (bit-3) */
     val = (SendData & BIT3);
     /* Sending extracted bit value through GPIO pin-9 */
     if(val == BIT3)
        digitalWrite(9, 1); 
     else
        digitalWrite(9, 0); 


     /* Incrementing SendData value */
     SendData++;
    
     /* Control strobe signal be made low after sending data */
     digitalWrite(10, 0); 

     /* Delay of 1-second to allow raspberrypi to read the data which has sent through gpio pins */
     delay(1000);  
  }
