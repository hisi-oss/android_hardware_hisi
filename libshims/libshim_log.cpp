#include <log/log.h>
#include <stdio.h>
#include <string.h>

struct AndroidLogEntry {
    time_t tv_sec;
    long tv_nsec;
    int priority;
    int32_t uid;
    int32_t pid;
    int32_t tid;
    const char* tag;
    size_t tagLen;
    size_t messageLen;
    const char* message;
};

// This stub is used because the original android_log_shouldPrintLine and the
// AndroidLogFormat struct it relied on have both been removed from the codebase.
extern "C" int android_log_shouldPrintLine(void* p_format __unused, const char* tag __unused,
                                           int pri __unused) {
    return 1;  // Return 1 regardless the parameters, we should always print the line.
}

extern "C" size_t android_log_printLogLine(void* p_format __unused, FILE* fp,
                                           AndroidLogEntry* entry) {
    if (fwrite(entry->message, 1, entry->messageLen, fp) != entry->messageLen) {
        return -1;
    }
    return entry->messageLen;
}

// This function is defined in the system library `libpowergenie_native3.so`
// and it's dynamically loaded by `libpowerlog.so` with `dlsym()`.
extern "C" int __android_logPower_print(int bufID, int priority, char* tag, char* fmt, ...) {
    char message[512];
    char new_tag[128];

    // Make sure the tag isn't empty.
    if (!tag) tag = strdup("");

    // Here, the original implementation modifies both the tag and the format.
    //   - tag: It formats the tag with the priority by using `snprintf()`.
    //   - fmt: Adds a separator (|) and additional format specifier(s).
    snprintf(new_tag, sizeof(new_tag), "%d", priority);
    snprintf(message, sizeof(message), "%s", fmt);

    // Continue normally, craft the arguments list.
    va_list args;
    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);

    // In this case, the original implementation would call the logPower_buf_write
    // function, but we can just use AOSP's __android_log_buf_write and call it a
    // day.
    return __android_log_buf_write(bufID, priority, new_tag, message);
}
