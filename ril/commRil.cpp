#include <dlfcn.h>

#define RIL_SHLIB

#include <log/log.h>
#include <telephony/ril.h>

extern "C" const RIL_RadioFunctions* RIL_Init(const struct RIL_Env* env, int argc, char** argv) {
    auto orig_RIL_Init =
            reinterpret_cast<RIL_RadioFunctions* (*)(const struct RIL_Env*, int, char**)>(
                    dlsym(RTLD_NEXT, "RIL_Init"));
    ALOGV("%s: env = %p, argc = %d", __func__, env, argc);
    for (int i = 0; i < argc; ++i) ALOGV("%s: argv[%d] = %s", __func__, i, argv[i]);
    return orig_RIL_Init(env, argc, argv);
}
