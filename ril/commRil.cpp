#include <dlfcn.h>

#define RIL_SHLIB

#include <log/log.h>
#include <telephony/ril.h>

typedef struct {
    int requestNumber;
    void (*dispatchFunction)(void* p, void* pRI);
    int (*responseFunction)(void* p, void* response, size_t responselen);
} CommandInfo;

typedef struct RequestInfo {
    int32_t token;
    CommandInfo* pCI;
    struct RequestInfo* p_next;
    char cancelled;
    char local;
} RequestInfo;

static size_t fixSignalStrength(const void* response) {
    RIL_SignalStrength_v10* rilResponse = (RIL_SignalStrength_v10*)response;

    // LTE
    if (rilResponse->LTE_SignalStrength.rsrp >= -97) {
        rilResponse->LTE_SignalStrength.signalStrength = 63;
        rilResponse->LTE_SignalStrength.rssnr = 130;
        rilResponse->LTE_SignalStrength.rsrp = -98;
    } else if (rilResponse->LTE_SignalStrength.rsrp >= -105) {
        rilResponse->LTE_SignalStrength.signalStrength = 10;
        rilResponse->LTE_SignalStrength.rssnr = 45;
        rilResponse->LTE_SignalStrength.rsrp = -108;
    } else if (rilResponse->LTE_SignalStrength.rsrp >= -113) {
        rilResponse->LTE_SignalStrength.signalStrength = 5;
        rilResponse->LTE_SignalStrength.rssnr = 10;
        rilResponse->LTE_SignalStrength.rsrp = -118;
    } else if (rilResponse->LTE_SignalStrength.rsrp >= -125) {
        rilResponse->LTE_SignalStrength.signalStrength = 3;
        rilResponse->LTE_SignalStrength.rssnr = -30;
        rilResponse->LTE_SignalStrength.rsrp = -128;
    } else if (rilResponse->LTE_SignalStrength.rsrp >= -44) {
        rilResponse->LTE_SignalStrength.signalStrength = 64;
        rilResponse->LTE_SignalStrength.rssnr = -200;
        rilResponse->LTE_SignalStrength.rsrp = -140;
    }

    // EvDO
    if (rilResponse->EVDO_SignalStrength.dbm >= -89) {
        rilResponse->EVDO_SignalStrength.dbm = -65;
        rilResponse->EVDO_SignalStrength.signalNoiseRatio = 7;
    } else if (rilResponse->EVDO_SignalStrength.dbm >= -99) {
        rilResponse->EVDO_SignalStrength.dbm = -75;
        rilResponse->EVDO_SignalStrength.signalNoiseRatio = 5;
    } else if (rilResponse->EVDO_SignalStrength.dbm >= -106) {
        rilResponse->EVDO_SignalStrength.dbm = -90;
        rilResponse->EVDO_SignalStrength.signalNoiseRatio = 3;
    } else if (rilResponse->EVDO_SignalStrength.dbm >= -112) {
        rilResponse->EVDO_SignalStrength.dbm = -105;
        rilResponse->EVDO_SignalStrength.signalNoiseRatio = 1;
    } else {
        rilResponse->EVDO_SignalStrength.dbm = -999;
        rilResponse->EVDO_SignalStrength.signalNoiseRatio = -999;
    }

    // CDMA
    if (rilResponse->CDMA_SignalStrength.dbm >= -89) {
        rilResponse->CDMA_SignalStrength.dbm = -75;
        rilResponse->CDMA_SignalStrength.ecio = -90;
    } else if (rilResponse->CDMA_SignalStrength.dbm >= -99) {
        rilResponse->CDMA_SignalStrength.dbm = -85;
        rilResponse->CDMA_SignalStrength.ecio = -110;
    } else if (rilResponse->CDMA_SignalStrength.dbm >= -106) {
        rilResponse->CDMA_SignalStrength.dbm = -95;
        rilResponse->CDMA_SignalStrength.ecio = -130;
    } else if (rilResponse->CDMA_SignalStrength.dbm >= -112) {
        rilResponse->CDMA_SignalStrength.dbm = -100;
        rilResponse->CDMA_SignalStrength.ecio = -130;
    } else {
        rilResponse->CDMA_SignalStrength.dbm = -100;
        rilResponse->CDMA_SignalStrength.ecio = -150;
    }

    // GSM
    if (rilResponse->GW_SignalStrength.signalStrength >= -89) {
        rilResponse->GW_SignalStrength.signalStrength = 12;
    } else if (rilResponse->GW_SignalStrength.signalStrength >= -97) {
        rilResponse->GW_SignalStrength.signalStrength = 8;
    } else if (rilResponse->GW_SignalStrength.signalStrength >= -103) {
        rilResponse->GW_SignalStrength.signalStrength = 5;
    } else {
        rilResponse->GW_SignalStrength.signalStrength = 0;
    }

    if (rilResponse->GW_SignalStrength.signalStrength != -1) {
        rilResponse->GW_SignalStrength.signalStrength =
                -(rilResponse->GW_SignalStrength.signalStrength - 113) / 2;
    }

    return sizeof(RIL_SignalStrength_v10);
}

extern "C" void _ZN7android27Comm_RIL_onRequestComplete0EPv9RIL_ErrnoS0_m(RIL_Token t, RIL_Errno e,
                                                                          void* response,
                                                                          size_t responselen) {
    auto orig_Comm_RIL_onRequestComplete0 =
            reinterpret_cast<void (*)(RIL_Token, RIL_Errno, void*, size_t)>(
                    dlsym(RTLD_NEXT, "_ZN7android27Comm_RIL_onRequestComplete0EPv9RIL_ErrnoS0_m"));

    RequestInfo* requestInfo = (RequestInfo*)t;
    int request = (requestInfo && requestInfo->pCI) ? requestInfo->pCI->requestNumber : -1;

    if (!requestInfo) {
        ALOGE("%s: request info is NULL", "Comm_RIL_onRequestComplete0");
        goto do_not_handle;
    }

    if (request != -1) {
        switch (request) {
            case RIL_REQUEST_SIGNAL_STRENGTH:
                responselen = fixSignalStrength(response);
                break;
        }
    }

do_not_handle:
    orig_Comm_RIL_onRequestComplete0(t, e, response, responselen);
}

extern "C" void _ZN7android27Comm_RIL_onRequestComplete1EPv9RIL_ErrnoS0_m(RIL_Token t, RIL_Errno e,
                                                                          void* response,
                                                                          size_t responselen) {
    auto orig_Comm_RIL_onRequestComplete1 =
            reinterpret_cast<void (*)(RIL_Token, RIL_Errno, void*, size_t)>(
                    dlsym(RTLD_NEXT, "_ZN7android27Comm_RIL_onRequestComplete1EPv9RIL_ErrnoS0_m"));

    RequestInfo* requestInfo = (RequestInfo*)t;
    int request = (requestInfo && requestInfo->pCI) ? requestInfo->pCI->requestNumber : -1;

    if (!requestInfo) {
        ALOGE("%s: request info is NULL", "Comm_RIL_onRequestComplete1");
        goto do_not_handle;
    }

    if (request != -1) {
        switch (request) {
            case RIL_REQUEST_SIGNAL_STRENGTH:
                responselen = fixSignalStrength(response);
                break;
        }
    }

do_not_handle:
    orig_Comm_RIL_onRequestComplete1(t, e, response, responselen);
}

extern "C" void _ZN7android27Comm_RIL_onRequestComplete2EPv9RIL_ErrnoS0_m(RIL_Token t, RIL_Errno e,
                                                                          void* response,
                                                                          size_t responselen) {
    auto orig_Comm_RIL_onRequestComplete2 =
            reinterpret_cast<void (*)(RIL_Token, RIL_Errno, void*, size_t)>(
                    dlsym(RTLD_NEXT, "_ZN7android27Comm_RIL_onRequestComplete2EPv9RIL_ErrnoS0_m"));

    RequestInfo* requestInfo = (RequestInfo*)t;
    int request = (requestInfo && requestInfo->pCI) ? requestInfo->pCI->requestNumber : -1;

    if (!requestInfo) {
        ALOGE("%s: request info is NULL", "Comm_RIL_onRequestComplete2");
        goto do_not_handle;
    }

    if (request != -1) {
        switch (request) {
            case RIL_REQUEST_SIGNAL_STRENGTH:
                responselen = fixSignalStrength(response);
                break;
        }
    }

do_not_handle:
    orig_Comm_RIL_onRequestComplete2(t, e, response, responselen);
}

extern "C" void _ZN7android27Comm_RIL_onRequestComplete3EPv9RIL_ErrnoS0_m(RIL_Token t, RIL_Errno e,
                                                                          void* response,
                                                                          size_t responselen) {
    auto orig_Comm_RIL_onRequestComplete3 =
            reinterpret_cast<void (*)(RIL_Token, RIL_Errno, void*, size_t)>(
                    dlsym(RTLD_NEXT, "_ZN7android27Comm_RIL_onRequestComplete3EPv9RIL_ErrnoS0_m"));

    RequestInfo* requestInfo = (RequestInfo*)t;
    int request = (requestInfo && requestInfo->pCI) ? requestInfo->pCI->requestNumber : -1;

    if (!requestInfo) {
        ALOGE("%s: request info is NULL", "Comm_RIL_onRequestComplete3");
        goto do_not_handle;
    }

    if (request != -1) {
        switch (request) {
            case RIL_REQUEST_SIGNAL_STRENGTH:
                responselen = fixSignalStrength(response);
                break;
        }
    }

do_not_handle:
    orig_Comm_RIL_onRequestComplete3(t, e, response, responselen);
}

extern "C" void _ZN7android31Comm_RIL_onUnsolicitedResponse0EiPvm13RIL_SOCKET_ID(
        int unsolResponse, const void* data, size_t datalen, RIL_SOCKET_ID modemid) {
    auto orig_Comm_RIL_onUnsolicitedResponse0 =
            reinterpret_cast<void (*)(int, const void*, size_t, RIL_SOCKET_ID)>(dlsym(
                    RTLD_NEXT, "_ZN7android31Comm_RIL_onUnsolicitedResponse0EiPvm13RIL_SOCKET_ID"));

    if (!data) {
        ALOGV("%s: data is NULL", "Comm_RIL_onUnsolicitedResponse0");
        goto do_not_handle;
    }

    switch (unsolResponse) {
        case RIL_UNSOL_SIGNAL_STRENGTH:
            datalen = fixSignalStrength(data);
            break;
    }

do_not_handle:
    orig_Comm_RIL_onUnsolicitedResponse0(unsolResponse, data, datalen, modemid);
}

extern "C" void _ZN7android31Comm_RIL_onUnsolicitedResponse1EiPvm13RIL_SOCKET_ID(
        int unsolResponse, const void* data, size_t datalen, RIL_SOCKET_ID modemid) {
    auto orig_Comm_RIL_onUnsolicitedResponse1 =
            reinterpret_cast<void (*)(int, const void*, size_t, RIL_SOCKET_ID)>(dlsym(
                    RTLD_NEXT, "_ZN7android31Comm_RIL_onUnsolicitedResponse1EiPvm13RIL_SOCKET_ID"));

    if (!data) {
        ALOGV("%s: data is NULL", "Comm_RIL_onUnsolicitedResponse1");
        goto do_not_handle;
    }

    switch (unsolResponse) {
        case RIL_UNSOL_SIGNAL_STRENGTH:
            datalen = fixSignalStrength(data);
            break;
    }

do_not_handle:
    orig_Comm_RIL_onUnsolicitedResponse1(unsolResponse, data, datalen, modemid);
}

extern "C" void _ZN7android31Comm_RIL_onUnsolicitedResponse2EiPvm13RIL_SOCKET_ID(
        int unsolResponse, const void* data, size_t datalen, RIL_SOCKET_ID modemid) {
    auto orig_Comm_RIL_onUnsolicitedResponse2 =
            reinterpret_cast<void (*)(int, const void*, size_t, RIL_SOCKET_ID)>(dlsym(
                    RTLD_NEXT, "_ZN7android31Comm_RIL_onUnsolicitedResponse2EiPvm13RIL_SOCKET_ID"));

    if (!data) {
        ALOGV("%s: data is NULL", "Comm_RIL_onUnsolicitedResponse2");
        goto do_not_handle;
    }

    switch (unsolResponse) {
        case RIL_UNSOL_SIGNAL_STRENGTH:
            datalen = fixSignalStrength(data);
            break;
    }

do_not_handle:
    orig_Comm_RIL_onUnsolicitedResponse2(unsolResponse, data, datalen, modemid);
}

extern "C" void _ZN7android31Comm_RIL_onUnsolicitedResponse3EiPvm13RIL_SOCKET_ID(
        int unsolResponse, const void* data, size_t datalen, RIL_SOCKET_ID modemid) {
    auto orig_Comm_RIL_onUnsolicitedResponse3 =
            reinterpret_cast<void (*)(int, const void*, size_t, RIL_SOCKET_ID)>(dlsym(
                    RTLD_NEXT, "_ZN7android31Comm_RIL_onUnsolicitedResponse3EiPvm13RIL_SOCKET_ID"));

    if (!data) {
        ALOGV("%s: data is NULL", "Comm_RIL_onUnsolicitedResponse3");
        goto do_not_handle;
    }

    switch (unsolResponse) {
        case RIL_UNSOL_SIGNAL_STRENGTH:
            datalen = fixSignalStrength(data);
            break;
    }

do_not_handle:
    orig_Comm_RIL_onUnsolicitedResponse3(unsolResponse, data, datalen, modemid);
}

extern "C" const RIL_RadioFunctions* RIL_Init(const struct RIL_Env* env, int argc, char** argv) {
    auto orig_RIL_Init =
            reinterpret_cast<RIL_RadioFunctions* (*)(const struct RIL_Env*, int, char**)>(
                    dlsym(RTLD_NEXT, "RIL_Init"));
    ALOGV("%s: env = %p, argc = %d", __func__, env, argc);
    for (int i = 0; i < argc; ++i) ALOGV("%s: argv[%d] = %s", __func__, i, argv[i]);
    return orig_RIL_Init(env, argc, argv);
}
