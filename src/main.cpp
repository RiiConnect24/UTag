#include <wups.h>
#include <utils/logger.h>
#include <cstring>
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
char titleId[17];

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

DECL_FUNCTION(uint32_t, MCP_RightCheckLaunchable, uint32_t *u1, uint32_t *u2, uint32_t u3, uint32_t u4, uint32_t u5) {
    WHBLogUdpInit();
    DEBUG_FUNCTION_LINE("UTag: Entered MCP_RightCheckLaunchable()");
    uint32_t result = real_MCP_RightCheckLaunchable(u1, u2, u3, u4, u5);

    if (result == 0 && strlen(key) != 0) {
        uint64_t tid = ((uint64_t) u3) << 32 | u4;
        sprintf(titleId, "%016llX", tid);
        DEBUG_FUNCTION_LINE("TitleID: %s", titleId);
    }

    return result;
}

WUPS_MUST_REPLACE(MCP_RightCheckLaunchable, WUPS_LOADER_LIBRARY_COREINIT, MCP_RightCheckLaunchable);

ON_APPLICATION_REQUESTS_EXIT() {
    WHBLogUdpInit();

    DEBUG_FUNCTION_LINE("UTag: ON_APPLICATION_REQUESTS_EXIT() called");

    if (strlen(key) == 0) {
        DEBUG_FUNCTION_LINE("Key not loaded, so ignoring.");
        return;
    } else if (strlen(titleId) == 0) {
        DEBUG_FUNCTION_LINE("TitleID is not set, so ignoring.")
        return;
    }

    char type[9];
    char tagURL[180];
    snprintf(type, sizeof(type), titleId);

    if (strcmp(type, "00050000") == 0 || strcmp(type, "00050002") == 0) {
        snprintf(tagURL, sizeof(tagURL), "http://%s/wiiu?game=%s&key=%s", SERVER, titleId, key);
    } else {
        memset(&titleId[0], 0, sizeof(titleId));;
        return;
    }

    // DEBUG_FUNCTION_LINE("Tag URL is %s", tagURL);
    DEBUG_FUNCTION_LINE("Starting cURL");

    CURL *curl = curl_easy_init();
    CURLcode ec;
    curl_easy_setopt(curl, CURLOPT_URL, tagURL);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    ec = curl_easy_perform(curl);
    if (ec != CURLE_OK) {
        DEBUG_FUNCTION_LINE("curl failed with exit code %s", curl_easy_strerror(ec));
    }
    curl_easy_cleanup(curl);
    memset(&titleId[0], 0, sizeof(titleId));;
}
