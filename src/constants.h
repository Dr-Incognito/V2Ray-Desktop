#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

static const QString APP_NAME      = "V2Ray-Desktop";
static const int APP_VERSION_MAJOR = 2;
static const int APP_VERSION_MINOR = 2;
static const int APP_VERSION_PATCH = 2;

static const QString APP_RELEASES_URL =
  "https://api.github.com/repos/Dr-Incognito/V2Ray-Desktop/releases";
static const QString APP_ASSETS_URL =
  "https://github.com/Dr-Incognito/V2Ray-Desktop/releases/download/%1/"
  "V2Ray-Desktop-v%2-%3.%4";
static const QString V2RAY_RELEASES_URL =
  "https://api.github.com/repos/Dreamacro/clash/releases";
static const QString V2RAY_ASSETS_URL =
  "https://github.com/Dreamacro/clash/releases/download/%1/clash-%2-%3.%4";

static const int RELEASE_CHECK_INTERVAL = 7200;
static const int TCP_PING_TIMEOUT       = 2500;
static const int HTTP_GET_TIMEOUT       = 2500;
static const int MAX_N_LOGS             = 2500;

static const bool V2RAY_USE_LOCAL_INSTALL     = true;
static const QString V2RAY_LOCAL_INSTALL_DIR  = "clash-core";
static const QString V2RAY_SYS_INSTALL_DIR    = "/usr/bin";
static const QString LOCALE_DIR               = "locales";
static const QString V2RAY_CORE_LOG_FILE_NAME = "clash.log";
static const QString V2RAY_CORE_CFG_FILE_NAME = "config.yaml";
static const QString APP_LOG_FILE_NAME        = "v2ray-desktop.log";
static const QString APP_CFG_FILE_NAME        = "config.json";
static const QString GFW_LIST_FILE_NAME       = "gfwlist.yml";

static const int DEFAULT_V2RAY_KCP_MTU            = 1350;
static const int DEFAULT_V2RAY_KCP_TTI            = 50;
static const int DEFAULT_V2RAY_KCP_UP_CAPACITY    = 5;
static const int DEFAULT_V2RAY_KCP_DOWN_CAPACITY  = 20;
static const int DEFAULT_V2RAY_KCP_READ_BUF_SIZE  = 2;
static const int DEFAULT_V2RAY_KCP_WRITE_BUF_SIZE = 2;
static const QString DEFAULT_TROJRAN_SNI          = "";
static const QString DEFAULT_TROJRAN_ALPN         = "h2; http/1.1";
static const bool DEFAULT_TROJRAN_ENABLE_UDP      = false;
static const bool DEFAULT_TROJRAN_ALLOW_INSECURE  = false;

static const bool DEFAULT_AUTO_START            = true;
static const bool DEFAULT_HIDE_WINDOW           = false;
static const bool AUTO_ENABLE_SYS_PROXY         = false;
static const QString DEFAULT_SYS_PROXY_PROTOCOL = "http";
static const QString DEFAULT_LANGUAGE           = "en-US";
static const QString DEFAULT_SERVER_IP          = "127.0.0.1";
static const int DEFAULT_SOCKS_PORT             = 1080;
static const int DEFAULT_HTTP_PORT              = 1087;
static const QString DEFAULT_DNS_SERVER         = "8.8.8.8; 4.4.4.4";
static const QString DEFAULT_PROXY_MODE         = "Rule";
static const QString DEFAULT_GFW_LIST_URL =
  "https://raw.githubusercontent.com/du5/gfwlist/master/Rules/Clash/"
  "gfwlist.yml";

#endif  // CONSTANTS_H
