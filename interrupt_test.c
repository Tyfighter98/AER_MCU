#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdint.h>

//argument is the sampling period in microseconds
//sampling frequency = 2 * (signal frequency)
//sampling period = 1 / sampling frequency with frequency in Hz and period in seconds
//i.e. if signal is 1kHz, sampling frequency is 2 * 1000 = 2000, 
//and sampling period = 1/2000 = .0005 seconds or 500 microseconds
int timer;
struct timespec t, dt;
FILE *fp;

void sig_handler(int signum){
    ualarm(timer,timer);
    clock_gettime(CLOCK_REALTIME, &dt);
    fprintf(fp, "0.%06ld\n", (dt.tv_nsec - t.tv_nsec) / 1000);
}

int main(int argc, char **argv){
    fp = fopen("test.log", "w");
    sscanf(argv[1], "%d", &timer);// Scheduled alarm in microseconds
    // printf("%d\n",timer);
    signal(SIGALRM, sig_handler); // Register signal handler
    // printf("%10jd.%03ld\n", (intmax_t) t.tv_sec, t.tv_nsec / 1000);
    ualarm(timer,timer);
    pause();
    while(1){
        clock_gettime(CLOCK_REALTIME, &t);
        //printf("%d ",i);
        pause(); //wait for signal
    }

    return 0;
}
