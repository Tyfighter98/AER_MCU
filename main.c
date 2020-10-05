#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "pigpio.h"

void isr(int gpio, int level, uint32_t tick) {
     //initialize isr to execute another function
     //read state of MOM button and write to data buffer
    //bufMOM = gpioRead(gpio);
}

int init() {
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

int csvWrite(char * filename, int a[], int b[], int c[], int d[]){
     //runs on thread separate from main signal read loop
     //write data stored in arrays to CSV file on USB using the following functions: fopen(), fclose(), and fprintf()
    printf("\n Creating %s file", filename);
    
    FILE *fp;
    fp = fopen(filename, "w+");
    
    fprintf(fp, "BSPD, IMD, Shut Down Circuit, 5V Trigger");
    for(int i = 0; i < 1000; i++)
    {
        fprintf(fp, "\n%d", a[i]);      // BSPD
        fprintf(fp, " ,%d ", b[i]);     // IMD
        fprintf(fp, " ,%d ", c[i]);     // Shut Down Circuit
        fprintf(fp, " ,%d ", d[i]);     // 5V Trigger
    }
    
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
    
    int BSPD[1000];     // BSPD Array
    int IMD[1000];      // IMD Array
    int SDCircuit[1000]; // Shut Down Circuit Array
    int Trigger[1000];  // 5V Trigger Array
    char filename[100] = "filename.csv";
    char serialDevice[100] = "serialDevice";
    char bufCanBus[1000];
    
    //init()
    init();
    
    while(1)
    {
        for (int i = 0; i < 1000; i++ )
        {
            // reading each signal inputs sequentially (BSPD, IMD, Shut Down Circuit, 5v Trigger)
            BSPD[i] = gpioRead(2);
            IMD[i] = gpioRead(15);
            SDCircuit[i] = gpioRead(18);
            Trigger[i] = gpioRead(4);
            // delay between each iteration using pigpio gpioSleep() function
            gpioSleep(1, 0, 1);
        }//end loop
        
        //csvWrite()
        csvWrite(filename, BSPD, IMD, SDCircuit, Trigger);
        //readSerialCanbus
        readSerialCanbus(serialDevice, bufCanBus);
        
        // resetting the arrays
        for (int i = 0; i < 1000; i++) {
            BSPD[i] = 0;
            IMD[i] = 0;
            SDCircuit[i] = 0;
            Trigger[i] = 0;
        }
        
        gpioSleep(1, 0, 1);
    }
    return 0;
}
