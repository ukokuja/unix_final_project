# unix_final_project
UNIX Final Project for Shenkar College

## Compiling
1. export LD_LIBRARY_PATH=/usr/local/lib
2. gcc  myFileSystemMonitor.c  -lpthread -lcli -finstrument-functions  -rdynamic -ldl  -o main

## Execution
1. ./main -d /tmp/here -i 127.0.0.1
2. netcat -l -u -p 1234
3. telnet localhost 8000 (user: fred, password: nerk)
