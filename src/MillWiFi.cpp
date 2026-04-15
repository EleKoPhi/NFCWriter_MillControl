#include "MillWiFi.h"
#include "LogManager.h"

bool MillWiFi::startAp()
{
    WiFi.mode(WIFI_AP);
    if (!WiFi.softAP(_ssid.c_str(), _pw.c_str()))
        return false;

    Serial.println("[MillWiFi] AP ready: ssid=" + _ssid + " ip=" + WiFi.softAPIP().toString());
    return true;
}

void MillWiFi::ensureApHealthy()
{
    unsigned long now = millis();
    if ((now - _lastApHealthMs) < 2000UL)
        return;
    _lastApHealthMs = now;

    bool modeOk = (WiFi.getMode() & WIFI_AP) != 0;
    IPAddress ip = WiFi.softAPIP();
    bool ipOk = !(ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0);
    if (modeOk && ipOk)
        return;

    _apRestartCount++;
    Serial.println("[MillWiFi] AP unhealthy -> restart #" + String(_apRestartCount));
    WiFi.softAPdisconnect(true);
    delay(20);

    if (!startAp())
    {
        Serial.println("[MillWiFi] AP restart failed");
        _ready = false;
        return;
    }

    _server.begin();
    _ready = true;
}

bool MillWiFi::begin()
{
    Preferences prefs;
    prefs.begin(MILL_WIFI_NVS_NS, true);
    _ssid          = prefs.getString(MILL_WIFI_KEY_SSID,  MILL_WIFI_DEFAULT_SSID);
    _pw            = prefs.getString(MILL_WIFI_KEY_PW,    MILL_WIFI_DEFAULT_PW);
    _authKey       = prefs.getString(MILL_WIFI_KEY_AUTH,  MILL_WIFI_DEFAULT_AUTH_KEY);
    prefs.end();

    WiFi.persistent(false);
    WiFi.setSleep(false);
    if (!startAp())
        return false;

    _server.on("/ping",  HTTP_GET,    [this]() { handlePing(); });
    _server.on("/logs",  HTTP_GET,    [this]() { handleGetLogs(); });
    _server.on("/logs",  HTTP_DELETE, [this]() { handleDeleteLogs(); });
    _server.on("/logs/reset", HTTP_POST, [this]() { handleResetLogs(); });
    _server.onNotFound(              [this]() { handleNotFound(); });

    _server.begin();
    _ready = true;
    return true;
}

void MillWiFi::handle()
{
    if (_ready)
    {
        ensureApHealthy();
        _server.handleClient();
    }
}

bool MillWiFi::checkAuth()
{
    if (!_server.hasHeader("Authorization"))
        return false;
    String hdr = _server.header("Authorization");
    String expected = "Bearer " + _authKey;
    return hdr == expected;
}

void MillWiFi::handlePing()
{
    _server.send(200, "text/plain", "pong");
}

void MillWiFi::handleGetLogs()
{
    if (!checkAuth())
    {
        _server.send(401, "text/plain", "Unauthorized");
        return;
    }

    // Aggregate all log entries per card in RAM (max 200 unique cards)
    static const uint16_t MAX_CARDS = 200;
    CardSummary *summary = (CardSummary *)malloc(MAX_CARDS * sizeof(CardSummary));
    if (!summary)
    {
        _server.send(500, "text/plain", "Out of memory");
        return;
    }

    uint16_t cardCount = LogManager::getInstance().aggregate(summary, MAX_CARDS);

    if (cardCount == 0)
    {
        free(summary);
        _server.send(200, "text/plain", "");
        return;
    }

    // Send plain text response without chunked transfer.
    // This avoids chunk-size marker lines (e.g. "13", "0") on simple clients.
    size_t totalLen = 0;
    for (uint16_t i = 0; i < cardCount; i++)
    {
        char uidHex[15];
        for (int b = 0; b < 7; b++)
            sprintf(uidHex + b * 2, "%02X", summary[i].uid[b]);
        uidHex[14] = '\0';

        char line[40];
        int n = snprintf(
            line,
            sizeof(line),
            "%s;%d;%d;%d\n",
            uidHex,
            (int)summary[i].credits_debited,
            (int)summary[i].refunds_given,
            (int)summary[i].latest_credit
        );
        if (n > 0)
            totalLen += (size_t)n;
    }

    _server.setContentLength(totalLen);
    _server.send(200, "text/plain", "");
    for (uint16_t i = 0; i < cardCount; i++)
    {
        char uidHex[15];
        for (int b = 0; b < 7; b++)
            sprintf(uidHex + b * 2, "%02X", summary[i].uid[b]);
        uidHex[14] = '\0';

        char line[40];
        snprintf(
            line,
            sizeof(line),
            "%s;%d;%d;%d\n",
            uidHex,
            (int)summary[i].credits_debited,
            (int)summary[i].refunds_given,
            (int)summary[i].latest_credit
        );
        _server.sendContent(line);
        delay(0);
    }

    free(summary);
}

void MillWiFi::handleDeleteLogs()
{
    if (!checkAuth())
    {
        _server.send(401, "text/plain", "Unauthorized");
        return;
    }

    LogManager::getInstance().clear();
    _server.send(200, "text/plain", "ok");
}

void MillWiFi::handleResetLogs()
{
    if (!checkAuth())
    {
        _server.send(401, "text/plain", "Unauthorized");
        return;
    }

    LogManager::getInstance().clear();
    _server.send(200, "text/plain", "ok");
}

void MillWiFi::handleNotFound()
{
    _server.send(404, "text/plain", "Not found");
}
