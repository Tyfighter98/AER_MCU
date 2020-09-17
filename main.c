#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h> 
#include <string.h>
#include "pigpio.h"


void isr(int gpio, int level, uint32_t tick) {
     //initialize isr to execute another function
     //read state of MOM button and write to data buffer
    //bufMOM = gpioRead(gpio);
}

int init(char * filename) {
    FILE *fp;
    int fileExists = 1;
    
    // Check if the log file already exits
    if(access(filename, F_OK) == -1) {
        printf("File does not exist, creating a new file!\n");
        fileExists = 0;
    }

    fp = fopen(filename, "a+");

    // Add CSV header if a new file was created
    if (!fileExists) {
        fprintf(fp, "BSPD, IMD, Shut Down Circuit, 5V Trigger");
    }

    fclose(fp);

    //initialize gpio pin to input on RPi Zero W using gpiosetmode() function
    //setup isr for both MOM inputs so that isr function runs whenever interrupt occurs from button being pressed using gpioSetISRFunc()
    if (gpioInitialise() < 0)
        {
            printf("GPIO initialization failed\n");
            return 0;
        }
        else
        {
            printf("GPIO initialization succeeded\n");
        }

        // Initialize GPIO modes
        gpioSetMode(4, PI_INPUT);       // 5V Trigger Signal (Pin IO4)
        gpioSetMode(15, PI_INPUT);      // IMD Signal (Pin IO15)
        gpioSetMode(18, PI_INPUT);      // Shut down circuit signal (Pin IO18)
        gpioSetMode(2, PI_INPUT);       // BSPD signal (Pin IO2)
        gpioSetMode(14, PI_INPUT);      // 5V Momentary ON Button1 (Pin IO14)
        gpioSetMode(3, PI_INPUT);       // 5V Momnetary ON Button2 (Pin IO3)
        
        // CAN Bus Controller Communication with Pi Zero W via serial communication
        gpioSetMode(8, PI_INPUT);   //(Pin IO8)
        gpioSetMode(9, PI_INPUT);   //(Pin IO9)
        gpioSetMode(10, PI_INPUT);  //(Pin IO10)
        gpioSetMode(25, PI_INPUT);  //(Pin IO25)

        // Assign Pull Ups
        gpioSetPullUpDown(4, PI_PUD_UP);        // 5V Trigger Signal
        gpioSetPullUpDown(15, PI_PUD_UP);       // IMD Signal
        gpioSetPullUpDown(18, PI_PUD_UP);       // Shut down circuit signal
        gpioSetPullUpDown(2, PI_PUD_UP);        // BSPD signal
        gpioSetPullUpDown(14, PI_PUD_UP);       // 5V Momentary ON Button1
        gpioSetPullUpDown(3, PI_PUD_UP);        // 5V Momentary ON Button2
    
        // CAN Bus Controller Commmunication Assigned Pull Ups
        gpioSetPullUpDown(8, PI_PUD_UP);
        gpioSetPullUpDown(9, PI_PUD_UP);
        gpioSetPullUpDown(10, PI_PUD_UP);
        gpioSetPullUpDown(25, PI_PUD_UP);

        gpioSetISRFunc(14, RISING_EDGE, 1, isr);
        gpioSetISRFunc(3, RISING_EDGE, 1, isr);

        return 1;
}

int csvWrite(char * filename, int a[], int b[], int c[], int d[], int *cursor){
    clock_t t;
    double dt;
    FILE *fp;

    int pointer;
    int BSPD[1000];         // BSPD Array
    int IMD[1000];          // IMD Array
    int SDCircuit[1000];    // Shut Down Circuit Array
    int Trigger[1000];      // 5V Trigger Array

    fp = fopen(filename, "a+");
    t = clock();
    
    // Enter critial section
    // Copy data from buffers into local arrays
    pointer = *cursor;
    for(int i = 0; i < pointer; i++)
    {
        BSPD[i] = a[i];
        IMD[i] = b[i];
        SDCircuit[i] = c[i];
        Trigger[i] = d[i];
    }

    // reset arrays and pointer
    memset(a, 0, sizeof(&a));
    memset(b, 0, sizeof(&b));
    memset(c, 0, sizeof(&c));
    memset(d, 0, sizeof(&d));
    *cursor = 0;
    // Exit critial section

    // Measure time lost where data was not able to be read
    t = clock() - t;
    dt = ((double)t)/CLOCKS_PER_SEC;
    printf("The program took %f seconds to copy the buffers\n", dt);

    t = clock();
    // Write data in local array to file
    for(int i = 0; i < pointer; i++)
    {
        fprintf(fp, "\n%d,", BSPD[i]);      // BSPD
        fprintf(fp, "%d,", IMD[i]);         // IMD
        fprintf(fp, "%d,", SDCircuit[i]);   // Shut Down Circuit
        fprintf(fp, "%d", Trigger[i]);      // 5V Trigger
    }
    t = clock() - t;
    dt = ((double)t)/CLOCKS_PER_SEC;
    printf("The program took %f seconds to write buffers to file\n", dt);
    
    fclose(fp);
    return 1;
}

int readSerialCanbus(char *serialDevice, char *buff) {
     //runs on thread separate from main signal read loop
     //read serial data incoming from CAN bus controller and write to buffer using following functions: serOpen(), serClose(), and serRead()
    int handle = serOpen(serialDevice, 9600, 0);
    serRead(handle, buff, 4);
    serClose(handle);
    
    return 1;
}

int main() {
    
    int BSPD[1000];    // BSPD Array
    int IMD[1000];     // IMD Array
    int SDCircuit[1000]; // Shut Down Circuit Array
    int Trigger[1000]; // 5V Trigger Array
    int pointer = 1000;
    char filename[100] = "log.csv";
    char serialDevice[100] = "serialDevice";
    char bufCanBus[1000];
    
    //init()
    init(filename);
    csvWrite(filename, BSPD, IMD, SDCircuit, Trigger, &pointer);
    
    // while(1)
    // {
    //     // for (int i = 0; i < 1000; i++ )
    //     // {
    //     //     // reading each signal inputs sequentially (BSPD, IMD, Shut Down Circuit, 5v Trigger)
    //     //     BSPD[i] = gpioRead(2);
    //     //     IMD[i] = gpioRead(15);
    //     //     SDCircuit[i] = gpioRead(18);
    //     //     Trigger[i] = gpioRead(4);
    //     //     // delay between each iteration using pigpio gpioSleep() function
    //     //     gpioSleep(1, 0, 1);
    //     // }//end loop
        
    //     //csvWrite()
    //     // csvWrite(filename, BSPD, IMD, SDCircuit, Trigger);
    //     //readSerialCanbus
    //     // readSerialCanbus(serialDevice, bufCanBus);
        
    //     gpioSleep(PI_TIME_RELATIVE, 2, 0);
    //     pointer++;
    //     printf("%d\n", pointer);
    // }
    return 0;
}
