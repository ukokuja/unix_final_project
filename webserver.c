void open_new_session(parameters p){
  FILE *apacheSer = fopen("/var/www/html/index.html", "w");
  fprintf(apacheSer, "<head><title>DIR_Watcher</title></head>");
  fprintf(apacheSer, "<h1 style=%s > Wellcome   to   diractory  watcher </h1>","color:red;");
  fprintf(apacheSer, "<h3 style='color: yellow'> &emsp; &emsp; &emsp; Made by  Lucas, David and Omri </h3> ");
  fprintf(apacheSer, " <h2 style='color: green'> Directory to watch :  %s </h2> <br>" , p.directory_to_be_watched);
  fclose(apacheSer);
}

void print_to_apache(char *file_or_dir, char *file_name, char *event_name, char *event_ctime) {
    FILE *apacheSer = fopen("/var/www/html/index.html", "a+");
    fprintf(apacheSer, "<h1 style='color: blue'> The %s %s was %s </h1> <h2 style='color:DodgerBlue'>The event time is %s </h2> <br>", file_or_dir, file_name, event_name,
            event_ctime);
    fclose(apacheSer);
}
