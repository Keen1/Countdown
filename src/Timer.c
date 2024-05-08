#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

int convert_to_milliseconds(int value, char* unit){
    if (strcmp(unit, "s") == 0){
        return value * 1000;
        
    }else if(strcmp(unit, "m") == 0){
        return value * 1000 * 60;
    }else if(strcmp(unit, "h") == 0){
        return value * 1000 * 3600;
    }else{
        fprintf(stderr, "Invalid time unit, use 's' for seconds, 'm' for minutes, or 'h' for hours. \n");
        exit(1);
    }
}

int main(int argc, char *argv[]){
    if (argc < 3){
        fprintf(stderr, "Usage: %s <time_value> <unit>\n", argv[0]);
        return 1;
    }
    
    int time_value = atoi(argv[1]);
    if(time_value <= 0){
        fprintf(stderr, "Please enter a positive time value.\n");
        
    }
    int total_milliseconds = convert_to_milliseconds(time_value, argv[2]);
    clock_t start_time = clock();
    double elapsed_time;
    int remaining_milliseconds, seconds, minutes, hours;
    while(1){
        clock_t current_time = clock();
        elapsed_time = (double)(current_time - start_time) / CLOCKS_PER_SEC * 1000;
        remaining_milliseconds = total_milliseconds - (int)elapsed_time;
        if(remaining_milliseconds <= 0){
            printf("\r00:00:00.0000\n");
            printf("Timer completed.\n");
            break;
        }
        int milliseconds = remaining_milliseconds % 1000;
        seconds = (remaining_milliseconds / 1000) % 60;
        minutes = (remaining_milliseconds / (1000 * 60)) % 60;
        hours = remaining_milliseconds / (1000 * 3600);
        
        printf("\r%02d:%02d:%02d.%03d", hours, minutes, seconds, milliseconds);
        fflush(stdout);
        
    }
    return 0;
}