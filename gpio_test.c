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

    gpioSetMode(17, PI_INPUT);
    gpioSetMode(27, PI_INPUT);
    gpioSetMode(4, PI_INPUT);
    gpioSetMode(6, PI_INPUT);
    gpioSetMode(13, PI_INPUT);
    gpioSetMode(19, PI_INPUT);

    gpioSetPullUpDown(17, PI_PUD_DOWN);
    gpioSetPullUpDown(27, PI_PUD_DOWN);
    gpioSetPullUpDown(4, PI_PUD_DOWN);
    gpioSetPullUpDown(6, PI_PUD_DOWN);
    gpioSetPullUpDown(13, PI_PUD_DOWN);
    gpioSetPullUpDown(19, PI_PUD_DOWN);

    // printf("%d %d %d %d %d %d\n", gpioRead(7), gpioRead(31), gpioRead(33), gpioRead(3), gpioRead(35), gpioRead(5));
    while(1) {
        printf("%d ", gpioRead(17));
        printf("%d ", gpioRead(27));
        printf("%d ", gpioRead(4));
        printf("%d ", gpioRead(6));
        printf("%d ", gpioRead(13));
        printf("%d\n", gpioRead(19));
        sleep(1);
    }

    return 0;
}
