#include <stdio.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define MAX_EVENTS 1024  /* Maximum number of events to process*/
#define LEN_NAME 25  /* Assuming that the length of the filename
won't exceed 16 bytes*/
#define IP_MAX_LEN 16
#define TIME_SIZE 50
#define EVENT_SIZE  ( sizeof (struct inotify_event) ) /*size of one event*/
#define BUF_LEN     ( MAX_EVENTS * ( EVENT_SIZE + LEN_NAME ))
/*buffer to store the data of events*/


int fd, wd;


void sig_handler(int sig) {

    /* Step 5. Remove the watch descriptor and close the inotify instance*/
    inotify_rm_watch(fd, wd);
    close(fd);
    exit(0);

}

void print_to_apache(char *file_or_dir, char *file_name, char *event_name, char *event_ctime) {
    FILE *apacheSer = fopen("/var/www/html/index.html", "a+");
    fprintf(apacheSer, "<h1> The %s %s was %s </h1> <h2>The event time is %s </h2> ", file_or_dir, file_name, event_name,
            event_ctime);
    fclose(apacheSer);
}


char* get_formatted_time() {
    char ctime_string[TIME_SIZE];
    time_t current_time;
    time(&current_time);    // current_time = time(NULL);
    struct tm *localtime = localtime(&rawtime);
    strftime(ctime_string, sizeof(ctime_string), "%d %B %Y: %H:%M", localtime);
    return ctime_string;
}
int main(int argc, char **argv) {
    signal(SIGINT, sig_handler);

    int opt;
    char directory_to_be_watched[LEN_NAME];
    char ip_address[IP_MAX_LEN];


    while ((opt = getopt(argc, argv, "d:i:")) != -1) {
        switch (opt) {
            case 'd':
                strcpy(directory_to_be_watched, optarg);
                break;
            case 'i':
                strcpy(ip_address, optarg);
                break;
            default: /* '?' */
                fprintf(stderr, "Usage: %s [-d] directory [-i] IP \n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    printf("flags=%d; directory=%s; IPaddress=%s; optind=%d\n",
           flags, directory_to_be_watched, ip_address, optind); // TODO: Remove




    /* Step 1. Initialize inotify */
    fd = inotify_init();


    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)  // error checking for fcntl
        exit(2);

    /* Step 2. Add Watch */
    wd = inotify_add_watch(fd, directory_to_be_watched, IN_MODIFY | IN_ACCESS);

    if (wd == -1) {
        printf("Could not watch : %s\n", directory_to_be_watched);
    } else {
        printf("Watching : %s\n", directory_to_be_watched);
    }


    while (1) {

        int i = 0, length;
        char buffer[BUF_LEN];

        /* Step 3. Read buffer*/
        length = read(fd, buffer, BUF_LEN);

        /* Step 4. Process the events which has occurred */
        while (i < length) {

            struct inotify_event *event = (struct inotify_event *) &buffer[i];

            if (event->len) {
                char ctime_string[TIME_SIZE] = get_formatted_time();
                if (event->mask & IN_MODIFY) {
                    if (event->mask & IN_ISDIR) {
                        printf("The directory %s was modified.\n", event->name);
                        print_to_apache("directory", event->name, "modified", ctime_string);
                    } else {
                        printf("The file %s was modified.\n", event->name);
                        print_to_apache("file", event->name, "modified", ctime_string);
                    }
                } else if (event->mask & IN_ACCESS) {
                    if (event->mask & IN_ISDIR) {
                        printf("The directory %s was read.\n", event->name);
                        print_to_apache("directory", event->name, "read", ctime_string);
                    } else {
                        printf("The file %s was read.\n", event->name);
                        print_to_apache("file", event->name, "read", ctime_string);
                    }
                }
            }
            i += EVENT_SIZE + event->len;
        }
    }

    exit(EXIT_SUCCESS);


}








 
