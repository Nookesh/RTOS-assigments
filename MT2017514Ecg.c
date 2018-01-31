#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main()
{
    float RawValues[3];
    float Filteredvalues[3];
    int SampleCount=0;

    /* Cut off frequecny */
    float fc = 30;

    /* Sampling Frequency */
    float fs = 1000;

    /* Tan((fc*3.14)/fs) */
    float Theta=tan((float)(fc*3.14)/fs);

    float Angle_Sqr=pow(Theta,2); 
    float divisor=1+Theta*1.414+Angle_Sqr;

    char data[10];

    /* Initialaisation for unfiltered Ecg gnuplot */
    char * Gnuplot_RawECG[] = {"set title \"Unfiltered Ecg\"","set xlabel \"Time\"",
                               "set ylabel \"Amplitude\"", "plot [100:800][400:580] '............Unfiltered.temp...........' with l"};

    /* Initialaisation for filtered Ecg gnuplot */
    char * Gnuplot_FilteredECG[] = {"set title \"Filtered Ecg\"","set xlabel \"Time\"",
                                    "set ylabel \"Amplitude\"", "plot [100:800][400:580] '...........Filtered.temp..........' with l"};
    

    //Samples.csv is the values csv file, this file should be kept in same folder of this program
    FILE* IpFile = fopen("Samples.csv", "r");
    FILE * Unfiltered = fopen("............Unfiltered.temp...........", "w");
    FILE * Filtered = fopen("...........Filtered.temp..........", "w");
    
    while(fgetc(IpFile)!=EOF)
    {
         fscanf(IpFile,  "%s", data);
         fprintf(Unfiltered, "%d %f \n", SampleCount, atof(data));  
         SampleCount++;
    }
    
    rewind(IpFile);
    SampleCount=0;   
    while(fgetc(IpFile)!=EOF)
    {
       fscanf(IpFile,  "%s", data);
       if(SampleCount<2)
       {
           fprintf(Filtered, "%d %f \n", SampleCount, atof(data));
           RawValues[2-SampleCount]=atof(data);
           Filteredvalues[2-SampleCount]=atof(data);
           SampleCount++;
       }
       else
       {
           RawValues[0]=atof(data);
           Filteredvalues[0]=(Angle_Sqr*RawValues[0]+2*Angle_Sqr*RawValues[1]+Angle_Sqr*RawValues[2]-(2*Angle_Sqr-2)*Filteredvalues[1]-
                          (1-1.414*Theta+Angle_Sqr)*Filteredvalues[2])/(1+Theta*1.414+Angle_Sqr);
           fprintf(Filtered, "%d %f \n", SampleCount, Filteredvalues[0]);
           Filteredvalues[2]=Filteredvalues[1];
           Filteredvalues[1]=Filteredvalues[0];
           RawValues[2]=RawValues[1];
           RawValues[1]=RawValues[0];
           SampleCount++;
       }    
    }
    

    FILE * Pipe1 = popen ("gnuplot -persistent", "w");
    FILE * Pipe2 = popen ("gnuplot -persistent", "w");    

    for (int Command=0; Command < 4; Command++)
    {
        fprintf(Pipe1, "%s \n", Gnuplot_RawECG[Command]);
    }
    for (int Command=0; Command < 4; Command++)
    {
        fprintf(Pipe2, "%s \n", Gnuplot_FilteredECG[Command]);
    }
    fclose(IpFile);
    fclose(Pipe1);
    fclose(Unfiltered);
    fclose(Pipe2);
    fclose(Filtered);
    return 0;
}
