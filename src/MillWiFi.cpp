#include "MillWiFi.h"
#include "LogManager.h"

bool MillWiFi::startAp()
{
    WiFi.disconnect(true, true);
    delay(80);
    WiFi.mode(WIFI_MODE_NULL);
    delay(50);
    WiFi.mode(WIFI_AP);
    WiFi.setSleep(false);

    IPAddress apIp(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);
    bool configOk = WiFi.softAPConfig(apIp, gateway, subnet);
    if (!configOk)
        Serial.println("[MillWiFi] softAPConfig failed");

    if (!WiFi.softAP(_ssid.c_str(), _pw.c_str(), 1, 0, 4))
        return false;

    delay(300);
    Serial.println(
        "[MillWiFi] AP ready: ssid=" + _ssid
        + " ip=" + WiFi.softAPIP().toString()
        + " mac=" + WiFi.softAPmacAddress()
        + " channel=" + String(WiFi.channel())
    );
    return true;
}

void MillWiFi::logRequest(const char *route)
{
    String remote = "-";
    if (_server.client())
        remote = _server.client().remoteIP().toString();

    Serial.println(
        String("[MillWiFi] request route=") + route
        + " method=" + String((int)_server.method())
        + " remote=" + remote
        + " uri=" + _server.uri()
    );
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

    Serial.println(
        "[MillWiFi] begin ssid=" + _ssid
        + " pwLen=" + String(_pw.length())
        + " authLen=" + String(_authKey.length())
    );

    WiFi.persistent(false);
    WiFi.setSleep(false);
    if (!startAp())
        return false;

    const char *headerKeys[] = {"Authorization"};
    _server.collectHeaders(headerKeys, 1);

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
        delay(0);
    }
}

bool MillWiFi::checkAuth()
{
    if (!_server.hasHeader("Authorization"))
    {
        Serial.println("[MillWiFi] auth missing");
        return false;
    }
    String hdr = _server.header("Authorization");
    String expected = "Bearer " + _authKey;
    Serial.println(
        "[MillWiFi] auth received len=" + String(hdr.length())
        + " expected len=" + String(expected.length())
    );
    return hdr == expected;
}

void MillWiFi::handlePing()
{
    logRequest("/ping");
    _server.send(200, "text/plain", "pong");
}

void MillWiFi::handleGetLogs()
{
    logRequest("/logs");
    if (!checkAuth())
    {
        _server.send(401, "text/plain", "Unauthorized");
        return;
    }

    // Aggregate all log entries per card in RAM.
    // If the configured limit is exceeded, fail loudly instead of truncating silently.
    static const uint16_t MAX_CARDS = 512;
    CardSummary *summary = (CardSummary *)malloc(MAX_CARDS * sizeof(CardSummary));
    if (!summary)
    {
        _server.send(500, "text/plain", "Out of memory");
        return;
    }

    bool truncated = false;
    uint16_t cardCount = LogManager::getInstance().aggregate(summary, MAX_CARDS, &truncated);

    if (truncated)
    {
        free(summary);
        _server.send(
            507,
            "text/plain",
            "Card limit exceeded. Increase MAX_CARDS or fetch fewer unique cards."
        );
        return;
    }

    if (cardCount == 0)
    {
        free(summary);
        Serial.println("[MillWiFi] /logs empty");
        _server.send(200, "text/plain", "");
        return;
    }

    // Build the full response in RAM and send it in one shot.
    // This is more reliable for the ESP8266 HTTP client than incremental
    // sendContent() streaming, which can otherwise end in read timeouts.
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

    Serial.println("[MillWiFi] /logs cardCount=" + String(cardCount) + " bytes=" + String((unsigned long)totalLen));
    String payload;
    payload.reserve(totalLen + 1);
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
        payload += line;
    }

    _server.send(200, "text/plain", payload);
    free(summary);
}

void MillWiFi::handleDeleteLogs()
{
    logRequest("/logs DELETE");
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
    logRequest("/logs/reset");
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
    logRequest("404");
    _server.send(404, "text/plain", "Not found");
}
