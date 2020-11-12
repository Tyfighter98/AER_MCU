#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h> 
#include <semaphore.h>
#include <string.h>
#include "pigpio.h"

// Compile with "gcc -Wall -pthread main.c -lpthread -lpigpio"

sem_t buffMutex;
sem_t serialMutex;

struct writer_args {
    char *csvFile;
    char *txtFile;
    int *a;
    int *b;
    int *c;
    int *d;
    char *e;
    int *f;
    int *g;
    int *ioCursor;
    int *serialCursor;
};

struct serial_args {
    char *serialDevice;
    char *buff;
    int *serialCursor;
};   

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
    sem_init(&serialMutex, 0, 1); 

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
        gpioSetMode(6, PI_INPUT);      // IMD Signal (Pin IO15)
        gpioSetMode(13, PI_INPUT);      // Shut down circuit signal (Pin IO18)
        gpioSetMode(17, PI_INPUT);       // BSPD signal (Pin IO2)
        gpioSetMode(19, PI_INPUT);      // 5V Momentary ON Button1 (Pin IO14)
        gpioSetMode(27, PI_INPUT);       // 5V Momnetary ON Button2 (Pin IO3)
        
        // CAN Bus Controller Communication with Pi Zero W via serial communication
        gpioSetMode(8, PI_INPUT);   //(Pin IO8)
        gpioSetMode(9, PI_INPUT);   //(Pin IO9)
        gpioSetMode(10, PI_INPUT);  //(Pin IO10)
        gpioSetMode(25, PI_INPUT);  //(Pin IO25)

        // Assign Pull Ups
        gpioSetPullUpDown(4, PI_PUD_DOWN);        // 5V Trigger Signal
        gpioSetPullUpDown(6, PI_PUD_DOWN);       // IMD Signal
        gpioSetPullUpDown(13, PI_PUD_DOWN);       // Shut down circuit signal
        gpioSetPullUpDown(17, PI_PUD_DOWN);        // BSPD signal
        gpioSetPullUpDown(19, PI_PUD_DOWN);       // 5V Momentary ON Button1
        gpioSetPullUpDown(27, PI_PUD_DOWN);        // 5V Momentary ON Button2
    
        // CAN Bus Controller Commmunication Assigned Pull Ups
        gpioSetPullUpDown(8, PI_PUD_UP);
        gpioSetPullUpDown(9, PI_PUD_UP);
        gpioSetPullUpDown(10, PI_PUD_UP);
        gpioSetPullUpDown(25, PI_PUD_UP);

        return 1;
}

void *csvWriter(void *arguments){
    struct writer_args *args = arguments;

    clock_t t;
    double dt;
    FILE *csv_fp;
    FILE *txt_fp;

    int ioPointer;
    int serialPointer;
    int BSPD[1000];         // BSPD Array
    int IMD[1000];          // IMD Array
    int SDCircuit[1000];    // Shut Down Circuit Array
    int Trigger[1000];      // 5V Trigger Array
    char bufCanBus[1000];   // Canbus Array
    int MOM1[1000];         // MOM1 Array
    int MOM2[1000];         // MOM2 Array

    // Wait 10ms before starting main loop
    gpioSleep(PI_TIME_RELATIVE, 0, 10000);

    while(1) {
        csv_fp = fopen((args -> csvFile), "a+");
        txt_fp = fopen((args -> txtFile), "a+");
        t = clock();
        
        // Enter buff critial section
        sem_wait(&buffMutex);
        // printf("Entering buff csvWriter critical section\n");
        // Copy data from buffers into local arrays
        ioPointer = *(args -> ioCursor);
        for(int i = 0; i < ioPointer; i++)
        {
            BSPD[i] = (args -> a)[i];
            IMD[i] = (args -> b)[i];
            SDCircuit[i] = (args -> c)[i];
            Trigger[i] = (args -> d)[i];
            MOM1[i] = (args -> f)[i];
            MOM2[i] = (args -> g)[i];
        }

        // reset arrays and pointer
        memset((args -> a), 0, sizeof(&(args -> a)));
        memset((args -> b), 0, sizeof(&(args -> b)));
        memset((args -> c), 0, sizeof(&(args -> c)));
        memset((args -> d), 0, sizeof(&(args -> d)));
        memset((args -> f), 0, sizeof(&(args -> f)));
        memset((args -> g), 0, sizeof(&(args -> g)));
        *(args -> ioCursor) = 0;
        sem_post(&buffMutex);
        // printf("Exited buff csvWriter critical section\n");
        // Exit buff critial section

        // Enter serial critial section
        sem_wait(&serialMutex);
        // printf("Entering serial csvWriter critical section\n");
        // Copy data from buffers into local arrays
        serialPointer = *(args -> serialCursor);
        for(int i = 0; i < serialPointer; i++)
        {
            bufCanBus[i] = (args -> e)[i];
        }

        // reset arrays and pointer
        memset((args -> e), 0, sizeof(&(args -> e)));
        *(args -> serialCursor) = 0;
        sem_post(&serialMutex);
        // printf("Exited serial csvWriter critical section\n");
        // Exit serial critial section

        // Measure time lost where data was not able to be read
        t = clock() - t;
        dt = ((double)t)/CLOCKS_PER_SEC;
        printf("The program took %f seconds to copy the buffers\n", dt);

        t = clock();
        // Write data in local array to file
        for(int i = 0; i < ioPointer; i++)
        {
            fprintf(csv_fp, "\n%d,", BSPD[i]);      // BSPD
            fprintf(csv_fp, "%d,", IMD[i]);         // IMD
            fprintf(csv_fp, "%d,", SDCircuit[i]);   // Shut Down Circuit
            fprintf(csv_fp, "%d,", Trigger[i]);      // 5V Trigger
            fprintf(csv_fp, "%d,", MOM1[i]);      // 5V Trigger
            fprintf(csv_fp, "%d", MOM2[i]);      // 5V Trigger
        }
        t = clock() - t;
        dt = ((double)t)/CLOCKS_PER_SEC;
        printf("The program took %f seconds to write buffers to file\n", dt);

        // Write data in local array to file
        fputs(bufCanBus, txt_fp);
        
        fclose(csv_fp);
        fclose(txt_fp);
        
        // Wait 10ms
        gpioSleep(PI_TIME_RELATIVE, 0, 10000);
    }

    return 0;
}

void *readSerialCanbus(void *arguments) {
    struct serial_args *args = arguments;
    //runs on thread separate from main signal read loop
    //read serial data incoming from CAN bus controller and write to buffer using following functions: serOpen(), serClose(), and serRead()
    int handle = serOpen((args -> serialDevice), 9600, 0);
    while (1) {
        sem_wait(&serialMutex);
        *(args -> serialCursor) = serRead(handle, (args -> buff), 96);
        serClose(handle);
        sem_post(&serialMutex);

        // Wait 0.25ms (loses ~2.4bits)
        // Allows for writer to grab semaphore
        gpioSleep(PI_TIME_RELATIVE, 0, 250);
    }
    
    
    return 0;
}

int main() {

    pthread_t t_writer;
    pthread_t t_serial;
    
    int BSPD[1000];         // BSPD Array
    int IMD[1000];          // IMD Array
    int SDCircuit[1000];    // Shut Down Circuit Array
    int Trigger[1000];      // 5V Trigger Array
    int ioPointer = 0;
    int serialPointer = 0;
    char csvFile[100] = "log.csv";
    char txtFile[100] = "log.txt";
    char serialDevice[100] = "/dev/ttyAMA0"; // PL011 Serial Port
    char bufCanBus[1000];
    int MOM1[1000];
    int MOM2[1000];

    struct writer_args w_args;
    w_args.csvFile = csvFile;
    w_args.txtFile = txtFile;
    w_args.a = BSPD;
    w_args.b = IMD;
    w_args.c = SDCircuit;
    w_args.d = Trigger;
    w_args.e = bufCanBus;
    w_args.f = MOM1;
    w_args.g = MOM2;
    w_args.ioCursor = &ioPointer;
    w_args.serialCursor = &serialPointer;

    struct serial_args s_args;
    s_args.serialDevice = serialDevice;
    s_args.buff = bufCanBus;
    s_args.serialCursor = &serialPointer;
    
    init(csvFile);
    pthread_create(&t_writer, NULL, &csvWriter, (void *)&w_args);
    // pthread_create(&t_serial, NULL, &readSerialCanbus, (void *)&s_args);

    while(1)
    {
        // Enter critical section
        // reading each signal inputs sequentially (BSPD, IMD, Shut Down Circuit, 5v Trigger)
        sem_wait(&buffMutex);
        // printf("Entering main critical section\n");
        BSPD[ioPointer] = gpioRead(17);
        IMD[ioPointer] = gpioRead(6);
        SDCircuit[ioPointer] = gpioRead(13);
        Trigger[ioPointer] = gpioRead(4);
        MOM1[ioPointer] = gpioRead(19);
        MOM2[ioPointer] = gpioRead(27);
        ioPointer++;
        sem_post(&buffMutex);
        // printf("Exited main critical section\n");
        // Exit critical section
        
        // Wait 0.5ms
        gpioSleep(PI_TIME_RELATIVE, 0, 500);
        
    }

    sem_destroy(&buffMutex); 

    return 0;
}
