#include "webserver.c"
#include "udp_client.c"

#define MAX_EVENTS 1024  /* Maximum number of events to process*/
#define LEN_NAME 25  /* Assuming that the length of the filename
won't exceed 16 bytes*/
#define IP_MAX_LEN 16

#define EVENT_SIZE  ( sizeof (struct inotify_event) ) /*size of one event*/
#define BUF_LEN     ( MAX_EVENTS * ( EVENT_SIZE + LEN_NAME ))
#define TIME_SIZE 50

int fd, wd;
udp_client uc;


void get_formatted_time(char *ctime_string) {
    time_t current_time;
    time(&current_time);    // current_time = time(NULL);
    struct tm *local_time;
    local_time = localtime(&current_time);
    strftime(ctime_string, TIME_SIZE, "%d %B %Y: %H:%M", local_time);
}


void* init_inotify (void *args){
    char* directory_to_be_watched = ((parameters *)args)->directory_to_be_watched;
    char* ip_address = ((parameters *)args)->ip_address;

    threadInotify = pthread_self();
    
    /* Step 1. Initialize inotify */
    fd = inotify_init();
    init_udp_client(ip_address, &uc);


    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)  // error checking for fcntl
        exit(2);

/* Step 2. Add Watch */
    wd = inotify_add_watch(fd, directory_to_be_watched, IN_MODIFY | IN_ACCESS);

    if (wd == -1) {
        printf("Could not watch : %s\n", directory_to_be_watched);
    } else {
        printf("Watching: %s\n", directory_to_be_watched);
    }


    while (1) {

        int i = 0, length;
        char buffer[BUF_LEN];
        char message[BUF_LEN];

/* Step 3. Read buffer*/
        length = read(fd, buffer, BUF_LEN);

/* Step 4. Process the events which has occurred */
        while (i < length) {

            struct inotify_event *event = (struct inotify_event *) &buffer[i];

            if (event->len) {
                char ctime_string[TIME_SIZE];
                get_formatted_time(ctime_string);
                if(!(event->mask & IN_ISDIR)) {
                    if (event->mask & IN_MODIFY) {
                        print_to_apache("file", event->name, "modified", ctime_string);
                        sprintf( message, "FILE ACCESSED: %s\nACCESS: %s\nTIME OF ACCESS: %s\n",
                                 event->name, "WRITE", ctime_string );

                        send_message(&uc, message);
                    } else if (event->mask & IN_ACCESS) {
                        print_to_apache("file", event->name, "read", ctime_string);
                        sprintf( message, "FILE ACCESSED: %s\nACCESS: %s\nTIME OF ACCESS: %s\n",
                                 event->name, "READ", ctime_string );
                        send_message(&uc, message);
                    }
                }

            }
            i += EVENT_SIZE + event->
                    len;
        }
    }
}

void sig_handler_inotify(int sig) {

    /* Step 5. Remove the watch descriptor and close the inotify instance*/
    inotify_rm_watch(fd, wd);
    close(uc.sockfd);
    close(fd);
}
