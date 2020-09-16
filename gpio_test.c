#include <pigpio.h>
#include <stdio.h>
#include <unistd.h>

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
    gpioSetPullUpDown(17, PI_PUD_DOWN);
    gpioWrite(17, 1);
    sleep(2);
    gpioWrite(17, 0);

    return 0;

    
}