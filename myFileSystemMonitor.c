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
#include "inotify.c"
#include "telnet.c"
#include <dlfcn.h>

backtrace bt;
pthread_mutex_t lock;
void  __attribute__ ((no_instrument_function))  __cyg_profile_func_enter (void *this_fn,
                                                                          void *call_site)
{
    Dl_info info_source;
    Dl_info info_fn;
    if (dladdr(call_site, &info_source) && dladdr(this_fn, &info_fn)) {
        if (info_source.dli_sname && strcmp(info_source.dli_sname, "init_inotify") == 0) {
            pthread_mutex_lock(&lock);
            sprintf (bt.trace[bt.trace_count], "%d) %p [%s] %s",  bt.trace_count + 1, this_fn,
                     info_fn.dli_fname ? info_fn.dli_fname : "?",
                     info_fn.dli_sname ? info_fn.dli_sname : "?");
            bt.trace_count++;
            pthread_mutex_unlock(&lock);
        }
    }
}

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

int main(int argc, char **argv) {
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

    signal(SIGINT, sig_handler);

    parameters p;

    fill_parameters(argc, argv, &p);
    open_new_session(p);  // start new index.html with title and header
    bt.trace_count = 0;
    pthread_t thread_inotify;
    pthread_t thread_telnet;
    if (pthread_create(&thread_inotify, NULL, init_inotify, (void*)&p))
        return 1;
    if (pthread_create(&thread_telnet, NULL, init_telnet, (void*)&bt))
        return 1;
    pthread_join(thread_inotify, NULL);
    pthread_join(thread_telnet, NULL);
//    free(trace);
    exit(EXIT_SUCCESS);
    pthread_mutex_destroy(&lock);


}








 
