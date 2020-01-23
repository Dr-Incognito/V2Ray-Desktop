#ifndef CONSTANTS_H
#define CONSTANTS_H

#define APP_VERSION_MAJOR 1
#define APP_VERSION_MINOR 0
#define APP_VERSION_PATCH 0
#define DEFAULT_V2RAY_CORE_VERSION "v4.22.1"
#define V2RAY_RELEASES_URL \
  "https://api.github.com/repos/v2ray/v2ray-core/releases"
#define V2RAY_ASSETS_URL \
  "https://github.com/v2ray/v2ray-core/releases/download/%1/v2ray-%2.zip"
#define V2RAY_CORE_INSTALL_DIR "v2ray-core"
#define V2RAY_CORE_LOG_FILE_NAME "v2ray.log"
#define V2RAY_CORE_CFG_FILE_NAME "v2ray-config.json"
#define APP_LOG_FILE_NAME "v2ray-desktop.log"
#define APP_CFG_FILE_NAME "v2ray-desktop-config.json"

#define TCP_PING_TIMEOUT 10000

#define DEFAULT_AUTO_START true
#define DEFAULT_HIDE_WINDOW false
#define DEFAULT_AUTO_UPDATE true
#define DEFAULT_ENABLE_UDP false
#define DEFAULT_SERVER_PROTOCOL "SOCKS"
#define DEFAULT_SERVER_IP "127.0.0.1"
#define DEFAULT_SERVER_PORT 1080
#define DEFAULT_PAC_PORT 1085
#define DEFAULT_MUX 8
#define DEFAULT_DNS_SERVER "8.8.8.8,8.8.4.4"
#define DEFAULT_PROXY_MODE "PAC Mode"
#define DEFAULT_GFW_LIST_URL \
  "https://raw.githubusercontent.com/gfwlist/gfwlist/master/gfwlist.txt"

#endif  // CONSTANTS_H
