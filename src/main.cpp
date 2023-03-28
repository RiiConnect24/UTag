#include "main.h"
#include <cstring>
#include <curl/curl.h>
#include <malloc.h>
#include <mutex>
#include <nn/acp/title.h>
#include <utils/logger.h>
#include <wups.h>


#ifndef SERVER
#define SERVER "tag.rc24.xyz"
#endif

WUPS_PLUGIN_NAME("UTag");
WUPS_PLUGIN_DESCRIPTION("Display the last played titles on your RiiTag.");
WUPS_PLUGIN_VERSION(VERSION_FULL);
WUPS_PLUGIN_AUTHOR("RiiConnect24, WiiDatabase.de");
WUPS_PLUGIN_LICENSE("GPLv3");

WUPS_USE_WUT_DEVOPTAB();

char key[129];
uint64_t title_id;

INITIALIZE_PLUGIN() {
    initLogging();

    DEBUG_FUNCTION_LINE("Initializing");

    FILE *fp = fopen("fs:/vol/external01/wiiu/utag.txt", "r");
    if (!fp) {
        DEBUG_FUNCTION_LINE("utag.txt not found in SD://wiiu/utag.txt");
        return;
    }
    fread(key, 128, 1, fp);
    fclose(fp);

    deinitLogging();
}

ON_APPLICATION_START() {
    initLogging();
}

ON_APPLICATION_ENDS() {
    deinitLogging();
}

DECL_FUNCTION(int32_t, ACPCheckTitleLaunchByTitleListTypeEx,
              MCPTitleListType *title, uint32_t u2) {
    DEBUG_FUNCTION_LINE("TitleID: %016llX", title->titleId);

    if (strlen(key) != 0) {
        title_id = title->titleId;
    }

    return real_ACPCheckTitleLaunchByTitleListTypeEx(title, u2);
}

WUPS_MUST_REPLACE(ACPCheckTitleLaunchByTitleListTypeEx,
                  WUPS_LOADER_LIBRARY_NN_ACP,
                  ACPCheckTitleLaunchByTitleListTypeEx);

ON_APPLICATION_REQUESTS_EXIT() {
    initLogging();

    DEBUG_FUNCTION_LINE("ON_APPLICATION_REQUESTS_EXIT called");

    if (strlen(key) == 0) {
        DEBUG_FUNCTION_LINE("Key not loaded, so ignoring.");
        return;
    } else if (title_id == 0) {
        DEBUG_FUNCTION_LINE("Title ID is not set, so ignoring.");
        return;
    }

    char tagURL[180];
    char TID[17];

    uint32_t title_type     = title_id >> 32;
    uint16_t title_gid_high = (title_id >> 16) & 0xffff;
    uint16_t title_gid_low  = title_id & 0xffff;
    bool is_uwuvci          = false;

    if (title_type == 0x00050002 &&
        (title_gid_high >= 0x3000 &&
         title_gid_low >= 0x3000)) { // UWUVCI AIO Injected game
        // need to be aligned to 0x40 for IOSU
        auto *meta = (ACPMetaXml *) memalign(0x40, sizeof(ACPMetaXml));

        if (!meta) {
            DEBUG_FUNCTION_LINE("Allocation of meta failed!");
            return;
        }

        ACPResult result = ACPGetTitleMetaXml(title_id, meta);
        if (result == ACPResult::ACP_RESULT_SUCCESS) {
            uint32_t real_tid = meta->reserved_flag2;
            char real_t_type  = (char) (real_tid >> 24);
            if ((real_t_type > 'A' &&
                 real_t_type <
                         'Z') || // should be uppercase letter for correct titles
                (real_t_type > '0' &&
                 real_t_type < '9')) { // only 091E00 falls into this?
                // transform it to (TeconMoon's) vc injection id format
                sprintf(TID, "%08x%08x", title_type, real_tid);
                is_uwuvci = true;
            }
        }

        free(meta);
    }

    if (!is_uwuvci) { // normal wiiu tid
        sprintf(TID, "%016llX", title_id);
    }

    if (title_type == 0x00050000 || // normal wii u titles
        is_uwuvci ||
        // (TeconMoon's) vc injections
        // should exclude all demos accroding to
        // https://wiiubrew.org/w/index.php?title=Title_database&oldid=3473#00050002:_Kiosk_Interactive_Demo_and_eShop_Demo
        (title_type == 0x00050002 &&
         (title_gid_high < 0x1010 || title_gid_high > 0x1021))) {
        snprintf(tagURL, sizeof(tagURL), "http://%s/wiiu?game=%s&key=%s", SERVER,
                 TID, key);

        DEBUG_FUNCTION_LINE_VERBOSE("Tag URL is %s", tagURL);
        DEBUG_FUNCTION_LINE("Contacting RiiTag");

        CURL *curl = curl_easy_init();
        CURLcode ec;
        curl_easy_setopt(curl, CURLOPT_URL, tagURL);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
        ec = curl_easy_perform(curl);
        if (ec != CURLE_OK) {
            DEBUG_FUNCTION_LINE("curl failed with exit code %s",
                                curl_easy_strerror(ec));
        } else {
            DEBUG_FUNCTION_LINE("RiiTag updated");
        }
        curl_easy_cleanup(curl);
    }

    title_id = 0;
    deinitLogging();
}
