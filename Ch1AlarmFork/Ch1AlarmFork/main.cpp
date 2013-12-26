//
//  main.cpp
//  Ch1AlarmFork
//
//  Created by matthew on 2013-12-25.
//  Copyright (c) 2013 matthew. All rights reserved.
//

#include <sys/types.h>
#include <sys/wait.h>
#include "errors.h"

int main(int argc, const char * argv[])
{
    char line[128];
    int seconds;
    pid_t pid;
    char message[64];
    
    while (1)
    {
        printf("Alarm> ");
        if (fgets(line, sizeof(line), stdin) == NULL) exit(0);
        if (strlen(line) <= 1) continue;
        
        if (sscanf(line, "%d %64[^\n]", &seconds, message) < 2)
        {
            fprintf(stderr, "Bad command\n");
        }
        else
        {
            pid = fork();
            if (pid == (pid_t)-1)
                errno_abort("Fork");
            if (pid == (pid_t)0)
            {
                // in the child
                sleep(seconds);
                printf("(%d) %s\n", seconds, message);
//                exit(0);
            }
            else
            {
                do
                {
                    pid = waitpid((pid_t)-1, NULL, WNOHANG);
                    if (pid == (pid_t)-1)
                        errno_abort("Wait for child");
                } while (pid != (pid_t)0);
            }
        }
    }
}

