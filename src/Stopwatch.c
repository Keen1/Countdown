#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
    clock_t start_time, curr_time;
    double elapsed_time;
    int milliseconds, seconds, minutes, hours;
    
    start_time = clock();
    printf("Stopwatch started. Press Ctrl+C to stop. \n");
    while(1){
        curr_time = clock();
        elapsed_time = (double)(curr_time - start_time) / CLOCKS_PER_SEC;
        milliseconds = (int)((elapsed_time - (int)elapsed_time) * 1000);
        seconds = (int)elapsed_time % 60;
        minutes = ((int)elapsed_time / 60) % 60;
        hours = (int)elapsed_time / 3600;
        printf("\r%02d:%02d:%02d.%03d", hours, minutes, seconds, milliseconds);
        fflush(stdout);
    }
    return 0;
}
