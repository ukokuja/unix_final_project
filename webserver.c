
void print_to_apache(char *file_or_dir, char *file_name, char *event_name, char *event_ctime) {
    FILE *apacheSer = fopen("/var/www/html/index.html", "a+");
    fprintf(apacheSer, "<h1> The %s %s was %s </h1> <h2>The event time is %s </h2> ", file_or_dir, file_name, event_name,
            event_ctime);
    fclose(apacheSer);
}
