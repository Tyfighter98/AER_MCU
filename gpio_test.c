#include <pigpio.h>
#include <stdio.h>
#include <unistd.h>

// Compile with gcc -Wall -pthread gpio_test.c -lpigpio -lrt

int main() {
    if (gpioInitialise() < 0)
    {
        printf("GPIO initialization failed\n");
        return 0;
    }
    else
    {
        printf("GPIO initialization succeeded\n");
    }

    gpioSetMode(17, PI_OUTPUT);
    gpioSetMode(27, PI_INPUT);
    gpioSetPullUpDown(17, PI_PUD_DOWN);
    gpioSetPullUpDown(27, PI_PUD_UP); 
    gpioWrite(17, 1);
    sleep(2);
    gpioWrite(17, 0);

    while (1) {
        printf("GPIO27 is level %d\n", gpioRead(27));
        sleep(1);
    }

    return 0;

    
}