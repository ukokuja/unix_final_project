# unix_final_project
UNIX Final Project for Shenkar College

## Compiling
1. export LD_LIBRARY_PATH=/usr/local/lib
2. gcc  myFileSystemMonitor.c  -lpthread -lcli -finstrument-functions  -rdynamic -ldl  -o main