#define TRACE_SIZE 127
#define BACKTRACE_LENGTH 100
typedef struct backtrace {
    int trace_count;
    char trace[BACKTRACE_LENGTH][TRACE_SIZE];
} backtrace;
