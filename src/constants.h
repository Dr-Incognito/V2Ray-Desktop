#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

static const QString APP_NAME      = "V2Ray-Desktop";
static const int APP_VERSION_MAJOR = 1;
static const int APP_VERSION_MINOR = 0;
static const int APP_VERSION_PATCH = 0;

static const QString DEFAULT_V2RAY_CORE_VERSION = "v4.22.1";
static const QString V2RAY_RELEASES_URL =
  "https://api.github.com/repos/v2ray/v2ray-core/releases";
static const QString V2RAY_ASSETS_URL =
  "https://github.com/v2ray/v2ray-core/releases/download/%1/v2ray-%2.zip";
static const QString V2RAY_CORE_INSTALL_DIR   = "v2ray-core";
static const QString V2RAY_CORE_LOG_FILE_NAME = "v2ray.log";
static const QString V2RAY_CORE_CFG_FILE_NAME = "v2ray-config.json";
static const QString APP_LOG_FILE_NAME        = "v2ray-desktop.log";
static const QString APP_CFG_FILE_NAME        = "v2ray-desktop-config.json";
static const QString GFW_LIST_FILE_NAME       = "gfwlist.txt";

static const int TCP_PING_TIMEOUT = 2500;
static const int HTTP_GET_TIMEOUT = 2500;
static const int MAX_N_LOGS       = 2500;

static const bool DEFAULT_AUTO_START         = true;
static const bool DEFAULT_HIDE_WINDOW        = false;
static const bool DEFAULT_AUTO_UPDATE        = true;
static const bool DEFAULT_ENABLE_UDP         = false;
static const QString DEFAULT_SERVER_PROTOCOL = "SOCKS";
static const QString DEFAULT_SERVER_IP       = "127.0.0.1";
static const int DEFAULT_SERVER_PORT         = 1080;
static const int DEFAULT_PAC_PORT            = 1085;
static const QString DEFAULT_DNS_SERVER      = "8.8.8.8,8.8.4.4";
static const QString DEFAULT_PROXY_MODE      = "pac";
static const QString DEFAULT_GFW_LIST_URL =
  "https://raw.githubusercontent.com/gfwlist/gfwlist/master/gfwlist.txt";

#endif  // CONSTANTS_H
