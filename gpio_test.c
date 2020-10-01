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

    gpioSetMode(7, PI_INPUT);
    gpioSetMode(31, PI_INPUT);
    gpioSetMode(33, PI_INPUT);
    gpioSetMode(3, PI_INPUT);
    gpioSetMode(11, PI_INPUT);
    gpioSetMode(5, PI_INPUT);

    gpioSetPullUpDown(7, PI_PUD_UP);
    gpioSetPullUpDown(31, PI_PUD_UP);
    gpioSetPullUpDown(33, PI_PUD_UP);
    gpioSetPullUpDown(3, PI_PUD_UP);
    gpioSetPullUpDown(11, PI_PUD_UP);
    gpioSetPullUpDown(5, PI_PUD_UP);

    // printf("%d %d %d %d %d %d\n", gpioRead(7), gpioRead(31), gpioRead(33), gpioRead(3), gpioRead(35), gpioRead(5));
    while(1) {
        printf("%d ", gpioRead(7));
        printf("%d ", gpioRead(31));
        printf("%d ", gpioRead(33));
        printf("%d ", gpioRead(3));
        printf("%d ", gpioRead(11));
        printf("%d\n", gpioRead(5));
        sleep(1);
    }

    return 0;
}
