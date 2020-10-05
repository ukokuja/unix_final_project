typedef struct backtrace {
    int trace_count;
    char buffer_filename[256];
    char is_active;
} backtrace_s;
