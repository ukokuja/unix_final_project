                                                                    בס"ד 
![alt text](https://www.hit.ac.il/.upload/academic-entrepreneurship/iris/partners/shenkarLogo.jpg "Shenkar")

### UNIX Final Project for Shenkar College
Made by :
 Student name       | ID
| -------------     |-------------
| David Tsibulsky   | 309444065
| Omri Haham        | 308428226
| Lucas Kujawski    | 000331506



#### 1. Compiling
1. install libcli
2. $ export LD_LIBRARY_PATH=/usr/local/lib
3. $ gcc  myFileSystemMonitor.c  -lpthread -lcli -finstrument-functions  -rdynamic -ldl  -o main
4. sudo chmod 777 /var/

#### 2. Executing the program
$ ./main -d /tmp/here -i 127.0.0.1

#### 3. Listening to the udp client
$ netcat -l -u -p 1234

#### 4. Running CLI for backtrace
$ telnet localhost 8000 (user: fred, password: nerk)
