/*
 * Copyright (C) 2014 The CyanogenMod Project
 *               2020,2024 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "[CHRG_LED]"
#define KLOG_LEVEL 6

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <batteryservice/BatteryServiceConstants.h>
#include <cutils/klog.h>
#include <cutils/uevent.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>

#define POWER_SUPPLY_SUBSYSTEM "SUBSYSTEM=power_supply"

#define BATTERY_STATUS_FILE "/sys/class/power_supply/Battery/status"
#define BATTERY_CAPACITY_FILE "/sys/class/power_supply/Battery/capacity"
#define RED_LED "/sys/class/leds/red/brightness"
#define GREEN_LED "/sys/class/leds/green/brightness"
#define BLUE_LED "/sys/class/leds/blue/brightness"

#define LED_MAX "255"
#define LED_MID "150"
#define LED_MIN "0"

#define STR_BUF_SIZE 128
#define UEVENT_BUF_SIZE 64*1024
#define UEVENT_MSG_LEN 1024
static int uevent_fd;

struct sysfs_string_enum_map {
    const char *str;
    int val;
} battery_status_map[] = {
    { "Unknown", BATTERY_STATUS_UNKNOWN },
    { "Charging", BATTERY_STATUS_CHARGING },
    { "Discharging", BATTERY_STATUS_DISCHARGING },
    { "Not charging", BATTERY_STATUS_NOT_CHARGING },
    { "Full", BATTERY_STATUS_FULL },
    { NULL, 0 },
};

static int map_sysfs_string(const char* str) {
    int i;
    int ret = -1;

    for (i = 0; battery_status_map[i].str; i++) {
        if (!strcmp(str, battery_status_map[i].str))
            ret = battery_status_map[i].val;
    }

    if (ret == -1)
        KLOG_ERROR(LOG_TAG, "%s: unknown charging status '%s'\n",
            __func__, str);

    return ret;
}

static int get_charging_status() {
    char batt_stat_str[STR_BUF_SIZE];
    int ret = -1;
    size_t ln;

    FILE *bstat;
    bstat = fopen(BATTERY_STATUS_FILE, "r");
    if (bstat) {
        if (fgets(batt_stat_str, STR_BUF_SIZE, bstat) == NULL) {
            KLOG_ERROR(LOG_TAG, "%s: failed to read %s; errno=%s\n",
                __func__, BATTERY_STATUS_FILE, strerror(errno));
            return -(errno);
        }

        if (!strcmp(batt_stat_str, "")) {
            KLOG_ERROR(LOG_TAG, "%s: empty battery status file %s\n",
                __func__, BATTERY_STATUS_FILE);
            return 0;
        }

        ln = strlen(batt_stat_str) - 1;
        if (batt_stat_str[ln] == '\n')
            batt_stat_str[ln] = '\0';

        fclose(bstat);
        ret = map_sysfs_string(batt_stat_str);
    } else {
        KLOG_ERROR(LOG_TAG, "%s: could not open %s; errno=%s\n",
            __func__, BATTERY_STATUS_FILE, strerror(errno));
        return -(errno);
    }

    return ret;
}

static int get_battery_level() {
    char batt_cap_str[STR_BUF_SIZE];
    int batt_level = -1;

    FILE *bcap;
    bcap = fopen(BATTERY_CAPACITY_FILE, "r");
    if (bcap) {
        if (fgets(batt_cap_str, STR_BUF_SIZE, bcap) != NULL) {
            batt_level = atoi(batt_cap_str);
            if (batt_level < 0 || batt_level > 100) {
                KLOG_ERROR(LOG_TAG, "%s: invalid battery level %d in %s\n",
                    __func__, batt_level, BATTERY_CAPACITY_FILE);
                batt_level = -1;
            }
        } else {
            KLOG_ERROR(LOG_TAG, "%s: failed to read %s; errno=%s\n",
                __func__, BATTERY_CAPACITY_FILE, strerror(errno));
        }

        fclose(bcap);
    } else {
        KLOG_ERROR(LOG_TAG, "%s: could not open %s; errno=%s\n",
            __func__, BATTERY_CAPACITY_FILE, strerror(errno));
    }

    return batt_level;
}

static void update_led(int charge_status, int battery_level) {
    FILE *rled, *gled, *bled;
    rled = fopen(RED_LED, "w");
    gled = fopen(GREEN_LED, "w");
    bled = fopen(BLUE_LED, "w");

    if (!rled || !gled || !bled) {
        if (rled) fclose(rled);
        if (gled) fclose(gled);
        if (bled) fclose(bled);
        KLOG_ERROR(LOG_TAG, "%s: could not open one or more LED files\n", __func__);
        return;
    }

    KLOG_INFO(LOG_TAG, "%s: setting charging status '%d'\n",
            __func__, charge_status);

    switch (charge_status) {
        case BATTERY_STATUS_CHARGING:
            if (battery_level >= 0 && battery_level < 20) {
                fputs(LED_MAX, rled);
                fputs(LED_MIN, gled);
                fputs(LED_MIN, bled);
            } else if (battery_level >= 20 && battery_level < 80) {
                fputs(LED_MAX, rled);
                fputs(LED_MID, gled);
                fputs(LED_MIN, bled);
            } else if (battery_level >= 80 && battery_level < 100) {
                fputs(LED_MID, rled);
                fputs(LED_MAX, gled);
                fputs(LED_MIN, bled);
            } else {
                fputs(LED_MID, rled);
                fputs(LED_MIN, gled);
                fputs(LED_MAX, bled);
            }
            break;
        case BATTERY_STATUS_FULL:
            fputs(LED_MIN, rled);
            fputs(LED_MAX, gled);
            fputs(LED_MIN, bled);
            break;
        default:
            fputs(LED_MIN, rled);
            fputs(LED_MIN, gled);
            fputs(LED_MIN, bled);
            break;
    }

    fclose(rled);
    fclose(gled);
    fclose(bled);
}

static void chargeled_update() {
    static int last_charge_status = BATTERY_STATUS_UNKNOWN;
    int charge_status;
    int battery_level;

    charge_status = get_charging_status();
    if (charge_status <= 0 || charge_status == last_charge_status)
        return;
    battery_level = get_battery_level();

    last_charge_status = charge_status;
    update_led(charge_status, battery_level);
}

static int uevent_init() {
    uevent_fd = uevent_open_socket(UEVENT_BUF_SIZE, true);

    if (uevent_fd >= 0) {
        fcntl(uevent_fd, F_SETFL, O_NONBLOCK);
    } else {
        KLOG_ERROR(LOG_TAG, "%s: uevent_open_socket failed; errno=%s\n",
            __func__, strerror(errno));
        return errno;
    }

    return 0;
}

static void uevent_event() {
    char msg[UEVENT_MSG_LEN+2];
    char *cp;
    int n;

    n = uevent_kernel_multicast_recv(uevent_fd, msg, UEVENT_MSG_LEN);
    if (n <= 0)
        return;
    if (n >= UEVENT_MSG_LEN)   /* overflow -- discard */
        return;

    msg[n] = '\0';
    msg[n+1] = '\0';
    cp = msg;

    while (*cp) {
        if (!strcmp(cp, POWER_SUPPLY_SUBSYSTEM)) {
            chargeled_update();
            break;
        }

        /* advance to after the next \0 */
        while (*cp++)
            ;
    }
}

static int chargeled_mainloop() {
    struct epoll_event ev;
    int epollfd;
    int maxevents = 1;
    struct epoll_event events[maxevents];
    int nevents;
    int n;

    epollfd = epoll_create(1);
    if (epollfd == -1) {
        KLOG_ERROR(LOG_TAG, "%s: epoll_create failed; errno=%s\n",
            __func__, strerror(errno));
        return errno;
    }

    ev.events = EPOLLIN | EPOLLWAKEUP;
    ev.data.ptr = (void *)uevent_event;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, uevent_fd, &ev) == -1) {
        KLOG_ERROR(LOG_TAG,
            "%s: epoll_ctl for uevent_fd failed; errno=%s\n",
            __func__, strerror(errno));
        return errno;
    }

    /* If we've gotten this far, perform an LED update before the loop */
    chargeled_update();

    while (1) {
        nevents = epoll_wait(epollfd, events, maxevents, -1);

        if (nevents == -1) {
            if (errno == EINTR)
                continue;
            KLOG_ERROR(LOG_TAG, "%s: epoll_wait failed\n", __func__);
            break;
        }

        for (n = 0; n < nevents; ++n) {
            if (events[n].data.ptr)
                (*(void (*)())events[n].data.ptr)();
        }
    }

    return errno;
}

int main() {
    int ret;

    klog_set_level(KLOG_LEVEL);

    ret = uevent_init();
    if (ret == 0)
        ret = chargeled_mainloop();

    return ret;
}
