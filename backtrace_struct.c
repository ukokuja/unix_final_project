typedef struct backtrace {
    char **trace;
    int trace_count;
    char is_active;
} backtrace_s;
