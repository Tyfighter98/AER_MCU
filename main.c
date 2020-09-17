#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h> 
#include <semaphore.h>
#include <string.h>
#include "pigpio.h"

// Compile with "gcc -Wall -pthread main.c -lpthread -lpigpio"

sem_t buffMutex;

struct writer_args {
    char * filename;
    int *a;
    int *b;
    int *c;
    int *d;
    int *cursor;
};

// TODO Update arguments for gpioSetISRFuncEx
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

    // Init semaphore to be shared between theads and start with a value of 1
    sem_init(&buffMutex, 0, 1); 

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

        // TODO Update arguments and functinos for gpioSetISRFuncEx
        gpioSetISRFunc(14, RISING_EDGE, 1, isr);
        gpioSetISRFunc(3, RISING_EDGE, 1, isr);

        return 1;
}

void *csvWriter(void *arguments){
    struct writer_args *args = arguments;

    clock_t t;
    double dt;
    FILE *fp;

    int pointer;
    int BSPD[1000];         // BSPD Array
    int IMD[1000];          // IMD Array
    int SDCircuit[1000];    // Shut Down Circuit Array
    int Trigger[1000];      // 5V Trigger Array

    // Wait 10ms before starting main loop
    gpioSleep(PI_TIME_RELATIVE, 0, 10000);

    while(1) {
        fp = fopen((args -> filename), "a+");
        t = clock();
        
        // Enter critial section
        // sem_wait(&buffMutex);
        printf("Entering csvWriter critical section\n");
        // Copy data from buffers into local arrays
        pointer = *(args -> cursor);
        for(int i = 0; i < pointer; i++)
        {
            BSPD[i] = (args -> a)[i];
            IMD[i] = (args -> b)[i];
            SDCircuit[i] = (args -> c)[i];
            Trigger[i] = (args -> d)[i];
        }

        // reset arrays and pointer
        memset((args -> a), 0, sizeof(&(args -> a)));
        memset((args -> b), 0, sizeof(&(args -> b)));
        memset((args -> c), 0, sizeof(&(args -> c)));
        memset((args -> d), 0, sizeof(&(args -> d)));
        *(args -> cursor) = 0;
        // sem_post(&buffMutex);
        printf("Exited csvWriter critical section\n");
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
        
        // Wait 10ms
        gpioSleep(PI_TIME_RELATIVE, 0, 10000);
    }

    return 0;
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

    pthread_t t_writer;
    
    int BSPD[1000];    // BSPD Array
    int IMD[1000];     // IMD Array
    int SDCircuit[1000]; // Shut Down Circuit Array
    int Trigger[1000]; // 5V Trigger Array
    int pointer = 0;
    char filename[100] = "log.csv";
    char serialDevice[100] = "serialDevice";
    char bufCanBus[1000];

    struct writer_args args;
    args.filename = filename;
    args.a = BSPD;
    args.b = IMD;
    args.c = SDCircuit;
    args.d = Trigger;
    args.cursor = &pointer;
    
    //init()
    init(filename);
    pthread_create(&t_writer, NULL, &csvWriter, (void *)&args);

    while(1)
    {
        // Enter critical section
        // reading each signal inputs sequentially (BSPD, IMD, Shut Down Circuit, 5v Trigger)
        sem_wait(&buffMutex);
        printf("Entering main critical section\n");
        BSPD[pointer] = gpioRead(2);
        IMD[pointer] = gpioRead(15);
        SDCircuit[pointer] = gpioRead(18);
        Trigger[pointer] = gpioRead(4);
        pointer++;
        sem_post(&buffMutex);
        printf("Exited main critical section\n");
        // Exit critical section
        
        // Wait 0.5ms
        gpioSleep(PI_TIME_RELATIVE, 0, 500);
        
    }

    sem_destroy(&buffMutex); 

    return 0;
}
