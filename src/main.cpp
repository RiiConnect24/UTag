#include <wups.h>
#include <utils/logger.h>
#include <cstring>
#include <nsysnet/socket.h>
#include <coreinit/title.h>
#include <curl/curl.h>
#include <whb/log_udp.h>

#define SERVER "tag.rc24.xyz"

WUPS_PLUGIN_NAME("UTag");
WUPS_PLUGIN_DESCRIPTION("Display the last played titles on your RiiTag!");
WUPS_PLUGIN_VERSION("v2.0");
WUPS_PLUGIN_AUTHOR("twosecslater, Brawl345");
WUPS_PLUGIN_LICENSE("GPLv3");

WUPS_USE_WUT_DEVOPTAB();

char key[129];

INITIALIZE_PLUGIN() {
    WHBLogUdpInit();

    FILE *fp = fopen("fs:/vol/external01/wiiu/utag.txt", "r");
    if (!fp) {
        DEBUG_FUNCTION_LINE("utag.txt not found in SD://wiiu/")
        return;
    }
    fread(key, 128, 1, fp);
    fclose(fp);
}

ON_APPLICATION_START() {
    WHBLogUdpInit();

    if (strlen(key) == 0) {
        DEBUG_FUNCTION_LINE("Key not loaded, so ignoring.");
        return;
    }

    socket_lib_init();
    char tempTID[17];
    char TID[17];
    char type[9];
    sprintf(tempTID, "%llx", OSGetTitleID());
    sprintf(TID, "%016s", tempTID);
    snprintf(type, sizeof(type), TID);

    char tagURL[180];
    snprintf(tagURL, sizeof(tagURL), "http://%s/wiiu?game=%s&key=%s", SERVER, TID, key);
    if (strcmp(type, "00050000") == 0) {
        CURL *curl = curl_easy_init();
        CURLcode ec;
        curl_easy_setopt(curl, CURLOPT_URL, tagURL);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
        ec = curl_easy_perform(curl);
        if (ec != CURLE_OK) {
            DEBUG_FUNCTION_LINE("curl failed with exit code %s", curl_easy_strerror(ec));
        }
        curl_easy_cleanup(curl);
    }
}
