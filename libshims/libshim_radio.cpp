#include <string>

namespace android::hardware::radio::V1_0 {
enum PersoSubstate : int32_t {
    UNKNOWN,      // initial state
    IN_PROGRESS,  // in between each lock transition
    READY,        // when either SIM or RUIM Perso is finished since each
                  // app must only have 1 active perso involved
    SIM_NETWORK,
    SIM_NETWORK_SUBSET,
    SIM_CORPORATE,
    SIM_SERVICE_PROVIDER,
    SIM_SIM,
    SIM_NETWORK_PUK,  // The corresponding perso lock is blocked
    SIM_NETWORK_SUBSET_PUK,
    SIM_CORPORATE_PUK,
    SIM_SERVICE_PROVIDER_PUK,
    SIM_SIM_PUK,
    RUIM_NETWORK1,
    RUIM_NETWORK2,
    RUIM_HRPD,
    RUIM_CORPORATE,
    RUIM_SERVICE_PROVIDER,
    RUIM_RUIM,
    RUIM_NETWORK1_PUK,  // The corresponding perso lock is blocked
    RUIM_NETWORK2_PUK,
    RUIM_HRPD_PUK,
    RUIM_CORPORATE_PUK,
    RUIM_SERVICE_PROVIDER_PUK,
    RUIM_RUIM_PUK,
};

enum CallState : int32_t {
    ACTIVE,
    HOLDING,
    DIALING,   // MO call only
    ALERTING,  // MO call only
    INCOMING,  // MT call only
    WAITING,   // MT call only
};

std::string toString(PersoSubstate state) {
    switch (state) {
        case UNKNOWN:
            return "UNKNOWN";
        case IN_PROGRESS:
            return "IN_PROGRRESS";
        case READY:
            return "READY";
        case SIM_NETWORK:
            return "SIM_NETWORK";
        case SIM_NETWORK_SUBSET:
            return "SIM_NETWORK_SUBSET";
        case SIM_CORPORATE:
            return "SIM_CORPORATE";
        case SIM_SERVICE_PROVIDER:
            return "SIM_SERVICE_PROVIDER";
        case SIM_SIM:
            return "SIM_SIM";
        case SIM_NETWORK_PUK:
            return "SIM_NETWORK_PUK";
        case SIM_NETWORK_SUBSET_PUK:
            return "SIM_NETWORK_SUBSET_PUK";
        case SIM_CORPORATE_PUK:
            return "SIM_CORPORATE_PUK";
        case SIM_SERVICE_PROVIDER_PUK:
            return "SIM_SERVICE_PROVIDER_PUK";
        case SIM_SIM_PUK:
            return "SIM_SIM_PUK";
        case RUIM_NETWORK1:
            return "RUIM_NETWORK1";
        case RUIM_NETWORK2:
            return "RUIM_NETWORK2";
        case RUIM_HRPD:
            return "RUIM_HRPD";
        case RUIM_CORPORATE:
            return "RUIM_CORPORATE";
        case RUIM_SERVICE_PROVIDER:
            return "RUIM_SERVICE_PROVIDER";
        case RUIM_RUIM:
            return "RUIM_RUIM";
        case RUIM_NETWORK1_PUK:
            return "RUIM_NETWORK1_PUK";
        case RUIM_NETWORK2_PUK:
            return "RUIM_NETWORK2_PUK";
        case RUIM_HRPD_PUK:
            return "RUIM_HRPD_PUK";
        case RUIM_CORPORATE_PUK:
            return "RUIM_CORPORATE_PUK";
        case RUIM_SERVICE_PROVIDER_PUK:
            return "RUIM_SERVICE_PROVIDER_PUK";
        case RUIM_RUIM_PUK:
            return "RUIM_RUIM_PUK";
        default:
            return "";
    }
}

std::string toString(CallState state) {
    switch (state) {
        case ACTIVE:
            return "ACTIVE";
        case HOLDING:
            return "HOLDING";
        case DIALING:
            return "DIALING";
        case ALERTING:
            return "ALERTING";
        case INCOMING:
            return "INCOMING";
        case WAITING:
            return "WAITING";
        default:
            return "";
    }
}
}  // namespace android::hardware::radio::V1_0
