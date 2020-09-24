#include<stdio.h>
#include<sys/inotify.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<fcntl.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define MAX_EVENTS 1024  /* Maximum number of events to process*/
#define LEN_NAME 16  /* Assuming that the length of the filename
won't exceed 16 bytes*/
#define IP_MAX_LEN 16
#define EVENT_SIZE  ( sizeof (struct inotify_event) ) /*size of one event*/
#define BUF_LEN     ( MAX_EVENTS * ( EVENT_SIZE + LEN_NAME ))
/*buffer to store the data of events*/


int fd,wd;

int main(int argc, char **argv){

           int flags,IPflag, opt;
	   char diractory[LEN_NAME];
	   char IPaddress[IP_MAX_LEN];
       
           IPflag = 0;
           flags = 0;

           while ((opt = getopt(argc, argv, "d:i:")) != -1) {
               switch (opt) {
               case 'd':
                   flags = 1;
	 	   strcpy(diractory,optarg);
                   break;
               case 'i':
                   IPflag = 1;
		   strcpy(IPaddress,optarg);
                   break;
               default: /* '?' */
                   fprintf(stderr, "Usage: %s [-d] directory [-i] IP \n",
                           argv[0]);
                   exit(EXIT_FAILURE);
               }
           }

           printf("flags=%d; diractory=%s; IPaddress=%s; optind=%d\n",
                   flags,diractory,IPaddress, optind);

           if (optind >= argc) {
               fprintf(stderr, "Expected argument after options\n");
               exit(EXIT_FAILURE);
           }

           printf("name argument = %s\n", argv[optind]);

           /* Other code omitted */

           exit(EXIT_SUCCESS);



 }








 
