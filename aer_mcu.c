#include <pigpio.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

void isr(int gpio, int level, uint32_t tick) {
    int s0, s1, s2;
    int s = 0b0;
    
    s0 = 0b001 * gpioRead(2);
    s1 = 0b010 * gpioRead(3);
    s2 = 0b100 * gpioRead(4);

    s = (s << 4) | s2 | s1 | s0;

    printf("Current Mux Selection: %d\n", s);
}

int init() {
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
    gpioSetMode(17, PI_INPUT);
    gpioSetMode(2, PI_INPUT);
    gpioSetMode(3, PI_INPUT);
    gpioSetMode(4, PI_INPUT);

    // Assign Pull Ups
    gpioSetPullUpDown(17, PI_PUD_UP);
    gpioSetPullUpDown(2, PI_PUD_UP);
    gpioSetPullUpDown(3, PI_PUD_UP); 
    gpioSetPullUpDown(4, PI_PUD_UP);

    gpioSetISRFunc(17, EITHER_EDGE, 2000, isr);

    return 1;
}

int main() {
    init();

    while(1) time_sleep(60);
    
    return 0;
}