#include <wups.h>
#include <malloc.h>
#include <string.h>
#include <nsysnet/socket.h>
#include <utils/logger.h>
#include <coreinit/title.h>
#include <curl/curl.h>

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
    char tempTID[17];
    char TID[17];
    char type[9];
    sprintf(tempTID, "%llx", OSGetTitleID());
    sprintf(TID, "%016s", tempTID);
    snprintf(type, sizeof(type), TID);
    char tagURL[180];
    snprintf(tagURL, sizeof(tagURL), "http://%s/wiiu?game=%s&key=%s", HOMESERVER, TID, key);
    if (strcmp(type, "00050000") == 0) {
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

