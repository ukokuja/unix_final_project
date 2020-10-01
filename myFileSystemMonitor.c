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

/*buffer to store the data of events*/


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
    signal(SIGINT, sig_handler);

    parameters p;

    fill_parameters(argc, argv, &p);

    pthread_t thread_inotify;
    pthread_t thread_telnet;
    if (pthread_create(&thread_inotify, NULL, init_inotify, (void*)&p))
        return 1;
    if (pthread_create(&thread_telnet, NULL, init_telnet, NULL);
        return 1;
    pthread_join(thread_inotify, NULL);
    pthread_join(thread_telnet, NULL);
    exit(EXIT_SUCCESS);


}








 
