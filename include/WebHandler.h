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

    void ClearInputBuffer();
    char ProcressInput();
    void ProcessOutput();
    void UpdateOutput();

    String FormatMain(bool Single, bool Double, bool FreePull, bool RetrunKey);

    void SetInputBuffer(String str);
    String GetInputBuffer();

    void SetInput(char c);
    char GetInput();

    bool GetWasTimeOut();
    void SetWasTimeOut(bool st);

    void SetCurrentStatus(char stat);
    char &GetCurrentStatus();

    void SetProgressForHttps(int localProgress);
    int GetProgressForHttps();

    void SetWebHandlerActive(bool st);
    bool GetWebHandlerActive();

    unsigned long GetServerWatchDogTimer();
    void StartServerWatchDog();
    bool IsTimeOut(unsigned long time);

private:
    WiFiServer millserver;
    WiFiClient millClient;

    String InputBuffer;
    String ProgressPage = "";

    bool wasTimeOut = false;
    bool webHandlerActive = false;

    char input;
    char currentStatus = WaitForUser;

    int chipSelectSd = 0;
    int progress = 0;

    unsigned long serverWatchDogTimer = 0;

};

#endif