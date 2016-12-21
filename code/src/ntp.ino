/*

ESPURNA
NTP MODULE

Copyright (C) 2016 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <TimeLib.h>
#include <NtpClientLib.h>
#include <WiFiClient.h>

// -----------------------------------------------------------------------------
// NTP
// -----------------------------------------------------------------------------

void ntpConfigure() {
    NTP.stop();
    NTP.setNtpServerName(getSetting("timeServer2", ""), 1);
    NTP.setNtpServerName(getSetting("timeServer3", ""), 2);
    NTP.begin(
        getSetting("timeServer1", NTP_SERVER),
        getSetting("timeZone", String(NTP_TIME_ZONE)).toInt(),
        getSetting("timeSaving", String(NTP_DAY_LIGHT)).toInt() == 1
    );
}

void ntpSetup() {

    NTP.onNTPSyncEvent([](NTPSyncEvent_t error) {
        if (error) {
            if (error == noResponse) {
                DEBUG_MSG("[NTP] Error: NTP server not reachable\n");
            } else if (error == invalidAddress) {
                DEBUG_MSG("[NTP] Error: Invalid NTP server address\n");
            }
        } else {
            DEBUG_MSG("[NTP] Time: %s\n", (char *) NTP.getTimeDateString(NTP.getLastNTPSync()).c_str());
            char buffer[30];
            sprintf(buffer, "{\"time\": \"%s\"}", NTP.getTimeDateString(NTP.getLastNTPSync()).c_str());
            wsSend(buffer);
        }
    });

    static WiFiEventHandler e;
    e = WiFi.onStationModeGotIP([](WiFiEventStationModeGotIP ipInfo) {
        NTP.setInterval(NTP_SHORT_INTERVAL, NTP_LONG_INTERVAL);
        ntpConfigure();
    });

}

void ntpLoop() {
    now();
}
