#define TRACE_FD 3
#define _GNU_SOURCE
#include <stdio.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <execinfo.h>
#include <semaphore.h>
#include "backtrace_struct.c"
#include "parameters_struct.c"

sem_t telnet_sem;
backtrace_s bt;
#define BACKTRACE_LENGTH 100
#define BUFFER_FILENAME ".buffer.txt"
void *backtrace_buffer[BACKTRACE_LENGTH];

void reset_buffer_file () {
    FILE* file = fopen(BUFFER_FILENAME, "w");
    fclose(file);
}

void  __attribute__ ((no_instrument_function))  __cyg_profile_func_enter (void *this_fn,
                                                                          void *call_site)
{
        if(bt.is_active == 1) {
            sem_wait(&telnet_sem);
            FILE* file = fopen(BUFFER_FILENAME, "w");
            fclose(file);
            bt.trace_count = 0;
            bt.is_active = 0;
        }
        int trace_count = backtrace(backtrace_buffer, BACKTRACE_LENGTH);
        char** string = backtrace_symbols(backtrace_buffer, trace_count);
        FILE* file = fopen(BUFFER_FILENAME, "a+");
        if (file) {
            for (int j = 0; j < trace_count; j++) {
                if (strncmp(string[j], "./main", 6) == 0) {
                    fprintf(file, "%s\n", string[j]);
                    bt.trace_count++;
                }
            }
            fclose(file);
        }

}

#include "inotify.c"
#include "telnet.c"


void fill_parameters(int argc, char **argv, parameters* p) {
    int opt;
    while ((opt = getopt(argc, argv, "d:i:")) != -1) {
        switch (opt) {
            case 'd':
                p->directory_to_be_watched = (char*)malloc(128*sizeof(char));
                strcpy(p->directory_to_be_watched, optarg);
                break;
            case 'i':
                p->ip_address = (char*)malloc(128*sizeof(char));
                strcpy(p->ip_address, optarg);
                break;
            default: /* '?' */
                fprintf(stderr, "Usage: %s [-d] directory [-i] IP \n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }
}

void sig_handler(int sig) {

    sem_close (&telnet_sem);
    sig_handler_inotify(sig);
    exit(0);
}


int main(int argc, char **argv) {
    reset_buffer_file();
    signal(SIGINT, sig_handler);
    signal(SIGABRT, sig_handler);
    strcpy(bt.buffer_filename, BUFFER_FILENAME);
    if (sem_init(&telnet_sem, 0, 0) == -1){
        printf("sem_init failed\n");
        return 1;
    }
    sem_post(&telnet_sem);
    parameters p;

    fill_parameters(argc, argv, &p);
    open_new_session(p);  // start new index.html with title and header
    pthread_t thread_inotify;
    pthread_t thread_telnet;
    if (pthread_create(&thread_inotify, NULL, init_inotify, (void*)&p))
        return 1;
    if (pthread_create(&thread_telnet, NULL, init_telnet, (void*)&bt))
        return 1;
    pthread_join(thread_inotify, NULL);
    pthread_join(thread_telnet, NULL);
    sem_close (&telnet_sem);
    exit(EXIT_SUCCESS);
}








 
