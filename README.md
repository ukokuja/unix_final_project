                                                                    בס"ד 
![alt text](https://www.hit.ac.il/.upload/academic-entrepreneurship/iris/partners/shenkarLogo.jpg "Shenkar")

### UNIX Final Project for Shenkar College
Made by :
 Student name       | ID
| -------------     |-------------
| David Tsibulsky   | 309444065
| Omri Haham        | 308428226
| Lucas Kujawski    | 000331506


#### 1. Installing libcli
1. Download https://github.com/dparrish/libcli
2. Install: 
```shell script
$ make
$ make install
```

#### 2. Compiling
```shell script
$ export LD_LIBRARY_PATH=/usr/local/lib
$ gcc  myFileSystemMonitor.c  -lpthread -lcli -finstrument-functions  -rdynamic -ldl  -o main
$ sudo chmod 777 /var/www/html/index.html
```

#### 3. Executing the program
```shell script
$ ./main -d /tmp/here -i 127.0.0.1
```

#### 3. Listening to the udp client
```shell script
$ netcat -l -u -p 1234
```

#### 4. Running CLI for backtrace
```shell script
$ telnet localhost 8000
```
* user: fred
* password: nerk
