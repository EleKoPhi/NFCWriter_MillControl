#ifndef WebHandler_h
#define WebHandler_h

#include "Controller_defines.h"
#include <WiFi101.h>
#include <SD.h>

class WebHandler
{
public:
    WebHandler(int sdPin);
    bool Begin(String pw, String ssid);
    void Run();
    void SetInputBuffer(String str);
    String GetInputBuffer();
    void SetInput(char c);
    char GetInput();
    void ClearInputBuffer();
    char ProcressInput();
    void ProcessOutput();
    void UpdateOutput();

    unsigned long GetServerWatchDogTimer();
    void StartServerWatchDog();
    bool IsTimeOut(unsigned long time);
    bool GetWasTimeOut();
    void SetWasTimeOut(bool st);

    void SetCurrentStatus(char stat);
    char &GetCurrentStatus();

    void SetProgressForHttps(int localProgress);
    int GetProgressForHttps();

    void SetWebHandlerActive(bool st);
    bool GetWebHandlerActive();

    String FormatMain(bool Single, bool Double, bool FreePull, bool RetrunKey);

private:
    WiFiServer millserver;
    WiFiClient millClient;
    String InputBuffer;
    char input;
    bool wasTimeOut = false;
    unsigned long serverWatchDogTimer = 0;
    int chipSelectSd = 0;
    String ProgressPage = "";
    char currentStatus = WaitForUser;
    int progress = 0;
    bool webHandlerActive = false;
};

#endif