
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


int init_backtrace(struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc)) {
    bt_p->is_active = 1;
    cli_print(cli, "backtrace() returned %d addresses\n", bt_p->trace_count);
    FILE* file = fopen(bt_p->buffer_filename, "r");
    if (file) {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            if (strncmp(line, "./main", 6) == 0)
                cli_print(cli, "%s", line);
        }
        fclose(file);
    }
    sem_post(&telnet_sem);

//    c = fgetc(bt_p->trace);
//    while (c != EOF)
//    {
//        c = fgetc(bt_p->trace);
//        cli_print(cli, "%c", c);
//    }
//    for (int i = 0; i < bt_p->trace_count; i++) {
//        if (is_not_from_libcli_thread(bt_p->trace[i]))
//            cli_print(cli, "%s", bt_p->trace[i]);
//    }
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

    // set 300 second idle timeout
    cli_set_idle_timeout_callback(cli, 300, idle_timeout);


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

void* init_telnet (void *args){
    bt_p = ((backtrace_s *)args);
    int s,x;
    struct sockaddr_in addr;
    int on = 1;

    signal(SIGCHLD, SIG_IGN);

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
//        return 1; // TODO: Check
    }

    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) {
        perror("setsockopt");
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(CLITEST_PORT);
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
//        return 1; // TODO: Check
    }

    if (listen(s, 50) < 0) {
        perror("listen");
//        return 1; // TODO: Check
    }

    printf("Listening on port %d\n", CLITEST_PORT);
    while ((x = accept(s, NULL, 0))) {
        run_child(x);
        exit(0);
    }

    return 0;
}
