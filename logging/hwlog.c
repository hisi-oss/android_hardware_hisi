#define LOG_TAG "libhwlog"

#include <log/log.h>
#include <stdint.h>
#include <stdio.h>

void __hwlog_jank_print(int p1, int id, char* comment) {
    ALOGV("%s: p1: %d, id: %d, comment: %s", __func__, p1, id, comment);
}

int __hwlog_jank_chgtm_print(int i, int id, char* fmt __unused, ...) {
    ALOGV("%s: i: %d, id: %d", __func__, i, id);
    return 0;
}

int __hwlog_write_kernel_printf(int id, int prio, const char* tag, const char* fmt __unused, ...) {
    ALOGV("%s: id: %d, prio: %d, tag: %s", __func__, id, prio, tag);
    return 0;
}

int __android_log_exception_buf_write(int id, int prio, const char* tag, const char* fmt __unused,
                                      ...) {
    ALOGV("%s: id: %d, prio: %d, tag: %s", __func__, id, prio, tag);
    return 0;
}
