#include <wups.h>
#include <malloc.h>
#include <nsysnet/socket.h>
#include <utils/logger.h>
#include <coreinit/title.h>
#include <curl/curl.h>

#include <nn/acp/title.h>

#define HOMESERVER "tag.rc24.xyz"

WUPS_PLUGIN_NAME("UTag");
WUPS_PLUGIN_DESCRIPTION("Display the titles you play on your tag!");
WUPS_PLUGIN_VERSION("v1.0.1");
WUPS_PLUGIN_AUTHOR("twosecslater");
WUPS_PLUGIN_LICENSE("GPLv3");

WUPS_FS_ACCESS();

char key[129];

INITIALIZE_PLUGIN() {
    int fd=-1;
    fd = open("sd:/utag.txt", O_RDONLY);
    read(fd, key, 128);
    close(fd);
}

ON_APPLICATION_START(args) {
    socket_lib_init();
    log_init();
    uint64_t title_id = OSGetTitleID();
    uint32_t t_type = title_id >> 32;
    uint32_t t_gid = title_id & 0xffffffff;
    uint16_t t_gid_high = t_gid >> 16;
    uint16_t t_gid_low = t_gid & 0xffff;
    bool is_uwuvci = false;
    /*
     * should be reserved_flag2 according to meta.xml
     * but seems there's some bug related to memory alignment issue?
     * (for ACPGetTitleMetaXml, which the address of ACPMetaXml is somehow critical)
     * the TID is actually only required to be 17bytes,
     * but added with 16 more bytes to make the alignment of ACPMetaXml is able to work.
     * note: it's werid that if you try to make it just 8 bytes more,
     * which should make the ACPMetaXml aligned perfectly,
     * it just shifted wayyyyy more, which is totally not working.
     */
    char TID[33];
    TID[17] = 0; // make sure it always 16 chars, may not required?
    if (t_type == 0x00050002 &&
            (t_gid_high > 0x3000 && t_gid_high < 0x10000) &&
            (t_gid_low > 0x3000 && t_gid_low < 0x10000)) { // UWUVCI AIO Injected game
        ACPMetaXml meta;
        ACPResult result = ACPGetTitleMetaXml(title_id, &meta);
        if (result == ACPResult::ACP_RESULT_SUCCESS) {

            /*
             * this is the code i used to find offset
             * removing this code seems to also affect the offset...
             * so i decide to leave it there
             */
            char* buf = (char*)&(meta.reserved_flag2);
            char* buf2 = TID;//key;
            for (int i = 0; i < 8/*64*/; i++)
            {
                buf2 += sprintf(buf2, "%02X", buf[i]);
            }
            *buf2 = 0;

            // there's 8 bytes offset, thus make reserved_flag2 becomes reserved_flag4
            //uint32_t real_tid = meta.reserved_flag2;
            uint32_t real_tid = meta.reserved_flag4;
            char real_t_type = (char)(real_tid >> 24);
            if ((real_t_type > 'A' && real_t_type < 'Z') || // should be uppercase letter for correct titles
                    (real_t_type > '0' && real_t_type < '9')) { // only 091E00 falls into this?
                // transform it to (TeconMoon's) vc injection id format
                sprintf(TID, "%08x%08x", t_type, real_tid);
                is_uwuvci = true;
            }
        }
    }
    if (!is_uwuvci) { // normal wiiu tid
        sprintf(TID, "%016llx", title_id);
    }
    char tagURL[180];
    snprintf(tagURL, sizeof(tagURL), "http://%s/wiiu?game=%s&key=%s", HOMESERVER, TID, key);
    if (t_type == 0x00050000 || // normal wii u titles
            is_uwuvci ||
            // (TeconMoon's) vc injections
            // (should excluded all demos accroding to https://wiiubrew.org/wiki/Title_database#00050002:_Kiosk_Interactive_Demo_and_eShop_Demo @ 21/4/2022)
            (t_type == 0x00050002 && (t_gid_high < 0x1010 || t_gid_high > 0x1021))) {
        CURL *curl = curl_easy_init();
        CURLcode ec;
        curl_easy_setopt(curl, CURLOPT_URL, tagURL);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
        ec = curl_easy_perform(curl);
        if(ec != CURLE_OK) {
            DEBUG_FUNCTION_LINE("curl failed with exit code %s", curl_easy_strerror(ec));
        }   
        curl_easy_cleanup(curl);
        //curl_global_cleanup();
    }
} 

