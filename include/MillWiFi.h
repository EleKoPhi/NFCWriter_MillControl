#ifndef MillWiFi_h
#define MillWiFi_h

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include "LogManager.h"

#define MILL_WIFI_NVS_NS    "mill_wifi"
#define MILL_WIFI_KEY_SSID  "ssid"
#define MILL_WIFI_KEY_PW    "pw"
#define MILL_WIFI_KEY_AUTH  "auth_key"

#define MILL_WIFI_DEFAULT_SSID     "MillControl"
#define MILL_WIFI_DEFAULT_PW       "mill1234"
#define MILL_WIFI_DEFAULT_AUTH_KEY "mill_secret_key"

#define MILL_WIFI_PORT 80

class MillWiFi
{
public:
    static MillWiFi &getInstance()
    {
        static MillWiFi instance;
        return instance;
    }

    bool begin();
    void handle();
    void activateFor(unsigned long durationMs);
    void deactivate();
    uint16_t getActiveBarPixels(uint16_t maxWidth) const;

private:
    MillWiFi() : _server(MILL_WIFI_PORT), _ready(false), _apRunning(false), _activeUntilMs(0), _activationDurationMs(0) {}
    MillWiFi(const MillWiFi &) = delete;
    MillWiFi &operator=(const MillWiFi &) = delete;

    WebServer _server;
    bool      _ready;
    bool      _apRunning;
    String    _ssid;
    String    _pw;
    String    _authKey;
    unsigned long _lastApHealthMs = 0;
    unsigned long _activeUntilMs;
    unsigned long _activationDurationMs;
    uint32_t _apRestartCount = 0;

    bool startAp();
    void stopAp();
    void ensureApHealthy();
    void logRequest(const char *route);

    bool checkAuth();
    void handlePing();
    void handleGetLogs();
    void handleGetResetStats();
    void handleDeleteLogs();
    void handleResetLogs();
    void handleNotFound();
};

#endif
