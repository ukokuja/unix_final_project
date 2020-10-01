#include <libcli.h>

void* init_telnet (void *args){
    struct cli_def *cli;
    cli = cli_init();
    cli_set_banner(cli, "Welcome!");
    cli_command *c = cli_register_command(NULL, "backtrace", init_backtrace, PRIVILEGE_UNPRIVILEGED, MODE_EXEC,
                                          "Executes backtrace on a thread");
    cli_done();
}