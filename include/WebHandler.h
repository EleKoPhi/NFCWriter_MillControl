#ifndef WebHandler_h
#define WebHandler_h

#include <WiFi101.h>

class WebHandler
{
public:
    WebHandler();
    bool Begin(String pw, String ssid);
    void Run();
    void SetInputBuffer(String str);
    String GetInputBuffer();
    void SetInput(char c);
    char GetInput();
    void ClearInputBuffer();
    char ProcressInput();
    void ProcessOutput();

    unsigned long GetServerWatchDogTimer();
    void SetServerWatchDogTimer(unsigned long time);
    bool IsTimeOut(unsigned long time);
    bool GetWasTimeOut();
    void SetWasTimeOut(bool st);

private:
    WiFiServer millserver;
    WiFiClient millClient;
    String InputBuffer;
    char input;
    bool wasTimeOut = false;
    unsigned long serverWatchDogTimer = 0;
};

#endif