#include <libcli.h>

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <sys/types.h>
#ifdef WIN32
#include <windows.h>
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif
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

struct my_context {
    int value;
    char *message;
};

#ifdef WIN32
typedef int socklen_t;

int winsock_init() {
  WORD wVersionRequested;
  WSADATA wsaData;
  int err;

  // Start up sockets
  wVersionRequested = MAKEWORD(2, 2);

  err = WSAStartup(wVersionRequested, &wsaData);
  if (err != 0) {
    // Tell the user that we could not find a usable WinSock DLL.
    return 0;
  }

  /*
   * Confirm that the WinSock DLL supports 2.2
   * Note that if the DLL supports versions greater than 2.2 in addition to
   * 2.2, it will still return 2.2 in wVersion since that is the version we
   * requested.
   * */
  if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
    // Tell the user that we could not find a usable WinSock DLL.
    WSACleanup();
    return 0;
  }
  return 1;
}
#endif

int cmd_context(struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc)) {
    struct my_context *myctx = (struct my_context *)cli_get_context(cli);
    cli_print(cli, "User context has a value of %d and message saying %s", myctx->value, myctx->message);
    return CLI_OK;
}

int init_backtrace(struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc)) {
//    struct my_context *myctx = (struct my_context *)cli_get_context(cli);
    cli_print(cli, "This will print the backtrace\n");
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

    // Prepare a small user context
    char mymessage[] = "I contain user data!";
    struct my_context myctx;
    myctx.value = 5;
    myctx.message = mymessage;

    cli = cli_init();
    cli_set_banner(cli, "Welcome from Omry, David and Lucas!");
    cli_set_hostname(cli, "File system monitor");
    cli_telnet_protocol(cli, 1);
    cli_regular(cli, regular_callback);

    // change regular update to 5 seconds rather than default of 1 second
    cli_regular_interval(cli, 5);

    // set 60 second idle timeout
    cli_set_idle_timeout_callback(cli, 60, idle_timeout);

    // Set user context and its command
    cli_set_context(cli, (void *)&myctx);
    cli_register_command(cli, NULL, "context", cmd_context, PRIVILEGE_UNPRIVILEGED, MODE_EXEC,
                         "Test a user-specified context");

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
    int s, x;
    struct sockaddr_in addr;
    int on = 1;

#ifndef WIN32
    signal(SIGCHLD, SIG_IGN);
#endif
#ifdef WIN32
    if (!winsock_init()) {
    printf("Error initialising winsock\n");
    return 1;
  }
#endif

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
#ifndef WIN32
        int pid = fork();
        if (pid < 0) {
            perror("fork");
//            return 1; // TODO: Check
        }

        /* parent */
        if (pid > 0) {
            socklen_t len = sizeof(addr);
            if (getpeername(x, (struct sockaddr *)&addr, &len) >= 0)
                printf(" * accepted connection from %s\n", inet_ntoa(addr.sin_addr));

            close(x);
            continue;
        }

        /* child */
        close(s);
        run_child(x);
        exit(0);
#else
        run_child(x);
    shutdown(x, SD_BOTH);
    close(x);
#endif
    }

    return 0;
}
