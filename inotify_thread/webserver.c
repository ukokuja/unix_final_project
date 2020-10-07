#define INDEX_FILE_LOCATION "/var/www/html/index.html"

// Resets index file for new sessions
void reset_index_file(parameters p){
    FILE *index_file = fopen(INDEX_FILE_LOCATION, "w");
    fprintf(index_file, "<head><title>My File System Monitor</title></head>");
    fprintf(index_file, "<h1 style='color:red;'>Welcome to directory watcher</h1>");
    fprintf(index_file, "<h3 style='color: yellow'>Made by Lucas, David and Omri</h3> ");
    fprintf(index_file, "<h2 style='color: green'>Directory to watch: %s </h2><br>" , p.directory_to_be_watched);
    fclose(index_file);
}

// Prints to index file
// Assumptions:
// 1. Specific format of line was not requested. We've chose our own format
void print_to_index_file(char *file_name, char *event_name, char *event_ctime) {
    FILE *index_file = fopen(INDEX_FILE_LOCATION, "a+");
    fprintf(index_file, "<h1 style='color: blue'> The file %s was %s </h1> <h2 style='color:DodgerBlue'>"
                       "The event time is %s </h2> <br>", file_name, event_name,event_ctime);
    fclose(index_file);
}
