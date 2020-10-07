
#include <libcli.h>

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include "libcli.h"

// vim:sw=4 tw=120 et

#define CLITEST_PORT 8000
#define MODE_CONFIG_INT 10

#ifdef __GNUC__
#define UNUSED(d) d __attribute__((unused))
#else
#define UNUSED(d) d
#endif
unsigned int regular_count = 0;
unsigned int debug_regular = 0;

backtrace_s* bt_p;

//Function called by backtrace command
int init_backtrace(struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc)) {

    //Initialize backtrace collection
    bt_p->is_active = 1;
    cli_print(cli, "backtrace() returned %d addresses\n", bt_p->trace_count);

    //Prints all backtrace
    for (int j = 0; j < bt_p->trace_count; j++) {
        cli_print(cli, "%s\n", bt_p->trace[j]);
    }

    //Turns off semaphore
    sem_post(&telnet_sem);

    return CLI_OK;
}

int check_auth(const char *username, const char *password) {
    if (strcasecmp(username, "fred") != 0) return CLI_ERROR;
    if (strcasecmp(password, "nerk") != 0) return CLI_ERROR;
    return CLI_OK;
}

int regular_callback(struct cli_def *cli) {
    regular_count++;
    if (debug_regular) {
        cli_print(cli, "Regular callback - %u times so far", regular_count);
        cli_reprompt(cli);
    }
    return CLI_OK;
}

int check_enable(const char *password) {
    return !strcasecmp(password, "topsecret");
}

int idle_timeout(struct cli_def *cli) {
    cli_print(cli, "Custom idle timeout");
    return CLI_QUIT;
}

void pc(UNUSED(struct cli_def *cli), const char *string) {
    printf("%s\n", string);
}


void run_child(int x) {
    struct cli_def *cli;


    cli = cli_init();
    cli_set_banner(cli, "Welcome.\nType 'help' to see all the possible commands");
    cli_set_hostname(cli, "File system monitor");
    cli_telnet_protocol(cli, 1);
    cli_regular(cli, regular_callback);

    // change regular update to 5 seconds rather than default of 1 second
    cli_regular_interval(cli, 5);

    // set 180 second idle timeout
    cli_set_idle_timeout_callback(cli, 180, idle_timeout);


    //Adds backtrace command
    cli_register_command(cli, NULL, "backtrace", init_backtrace, PRIVILEGE_UNPRIVILEGED, MODE_EXEC,
                                          "Executes backtrace on a thread");
    cli_set_auth_callback(cli, check_auth);
    cli_set_enable_callback(cli, check_enable);
    // Test reading from a file
    {
        FILE *fh;

        if ((fh = fopen("clitest.txt", "r"))) {
            // This sets a callback which just displays the cli_print() text to stdout
            cli_print_callback(cli, pc);
            cli_file(cli, fh, PRIVILEGE_UNPRIVILEGED, MODE_EXEC);
            cli_print_callback(cli, NULL);
            fclose(fh);
        }
    }
    cli_loop(cli, x);
    cli_done(cli);
}


//Init telnet thread
void* init_telnet_thread (void *args){
    bt_p = ((backtrace_s *)args);
    int s,x;
    struct sockaddr_in addr;
    int on = 1;

    //Fills thread id to compare on backtrace
    thread_telnet = pthread_self();

    signal(SIGCHLD, SIG_IGN);

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(0);
    }

    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) {
        perror("setsockopt");
        exit(0);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(CLITEST_PORT);
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(0);
    }

    if (listen(s, 50) < 0) {
        perror("listen");
        exit(0);
    }

    printf("Listening on port %d\n", CLITEST_PORT);
    while ((x = accept(s, NULL, 0))) {
        run_child(x);
        exit(0);
    }

    return 0;
}
