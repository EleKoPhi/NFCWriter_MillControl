#include "WebHandler.h"
#include "WebHandler_defines.h"

#include <SD.h>
#include <SPI.h>

WebHandler::WebHandler(int sdPin) : millserver(SERVER_PORT), millClient()
{
        chipSelectSd = sdPin;
}

void WebHandler::SetInputBuffer(String str) { this->InputBuffer = str; };
String WebHandler::GetInputBuffer() { return this->InputBuffer; };
void WebHandler::ClearInputBuffer() { this->InputBuffer = ""; };
void WebHandler::SetInput(char c) { this->input = c; };
char WebHandler::GetInput() { return this->input; };

void WebHandler::SetCurrentStatus(char stat) { currentStatus = stat; }
char &WebHandler::GetCurrentStatus() { return currentStatus; }

unsigned long WebHandler::GetServerWatchDogTimer() { return serverWatchDogTimer; };
void WebHandler::StartServerWatchDog()
{
        SetWasTimeOut(false);
        this->serverWatchDogTimer = millis();
}
bool WebHandler::IsTimeOut(unsigned long time)
{
        return (millis() - GetServerWatchDogTimer()) > time;
}

void WebHandler::SetWasTimeOut(bool st) { this->wasTimeOut = st; }
bool WebHandler::GetWasTimeOut() { return this->wasTimeOut; }

void WebHandler::SetWebHandlerActive(bool st) { this->webHandlerActive = st; }
bool WebHandler::GetWebHandlerActive() { return this->webHandlerActive; }

void WebHandler::SetProgressForHttps(int localProgress) { progress = localProgress; }
int WebHandler::GetProgressForHttps() { return progress; }

String WebHandler::FormatMain(bool Single, bool Double, bool FreePull, bool RetrunKey)
{
        String htmlInitial = PAGE_MAIN;

        if (RetrunKey)
        {
                htmlInitial.replace(RETUNN_KEY_FLAG, RETURN_KEY);

                htmlInitial.replace(SINGLE_BUTTON_START, HTML_CROSSED_OUT_START);
                htmlInitial.replace(SINGLE_BUTTON_END, HTML_CROSSED_OUT_END);

                htmlInitial.replace(DOBULE_BUTTON_START, HTML_CROSSED_OUT_START);
                htmlInitial.replace(DOBULE_BUTTON_END, HTML_CROSSED_OUT_END);

                htmlInitial.replace(FREEPULL_BUTTON_START, HTML_CROSSED_OUT_START);
                htmlInitial.replace(FREEPULL_BUTTON_END, HTML_CROSSED_OUT_END);

        }
        else
        {
                if (Single)
                {
                        htmlInitial.replace(SINGLE_BUTTON_START, HTML_UNDERLINED_START);
                        htmlInitial.replace(SINGLE_BUTTON_END, HTML_UNDELINED_END);
                }
                else
                {
                        htmlInitial.replace(SINGLE_BUTTON_START, HTML_CROSSED_OUT_START);
                        htmlInitial.replace(SINGLE_BUTTON_END, HTML_CROSSED_OUT_END);
                }

                if (Double)
                {
                        htmlInitial.replace(DOBULE_BUTTON_START, HTML_UNDERLINED_START);
                        htmlInitial.replace(DOBULE_BUTTON_END, HTML_UNDELINED_END);
                }
                else
                {
                        htmlInitial.replace(DOBULE_BUTTON_START, HTML_CROSSED_OUT_START);
                        htmlInitial.replace(DOBULE_BUTTON_END, HTML_CROSSED_OUT_END);
                }

                if (FreePull)
                {
                        htmlInitial.replace(FREEPULL_BUTTON_START, HTML_UNDERLINED_START);
                        htmlInitial.replace(FREEPULL_BUTTON_END, HTML_UNDELINED_END);
                }
                else
                {
                        htmlInitial.replace(FREEPULL_BUTTON_START, HTML_CROSSED_OUT_START);
                        htmlInitial.replace(FREEPULL_BUTTON_END, HTML_CROSSED_OUT_END);
                }

                htmlInitial.replace(RETUNN_KEY_FLAG, "");
        }

        return htmlInitial;
}

bool WebHandler::Begin(String pw, String ssid)
{
        if (WiFi.status() == WL_NO_SHIELD)
                return FAIL;

        WiFi.beginAP(ssid.c_str(), pw.c_str());
        millserver.begin();
        return OK;
}

void WebHandler::Run()
{
        millClient = millserver.available();

        if (millClient)
        {

                if (!GetWasTimeOut())
                {
                        ClearInputBuffer();
                        millClient.println(HTTP_STATUS);
                        millClient.println(CONNTENT_TYPE);
                        millClient.println(REFRESH_RATE);
                        millClient.println(CONNCECTION_TYPE);
                        millClient.println();
                }

                StartServerWatchDog();

                while (millClient.available())
                {
                        if (IsTimeOut(SERVER_TIMEOUT))
                        {
                                SetWasTimeOut(true);
                                break;
                        }

                        ProcressInput();
                }

                if (!GetWasTimeOut())
                {
                        ProcessOutput();
                        millClient.stop();
                }
        }
}

void WebHandler::ProcessOutput()
{
        if ((GetInputBuffer().indexOf(PAGE_MAIN_GETBACKKEY) != -1))
        {
                SetWebHandlerActive(false);
                SetCurrentStatus(WaitForUser);
        }

        if ((GetInputBuffer().indexOf(PAGE_MAIN_GETKEY) != -1) || (GetInputBuffer().indexOf(PAGE_MAIN_GETBACKKEY) != -1))
        {
                if (GetCurrentStatus() == Single)
                {
                        millClient.print(FormatMain(true, false, false, GetWebHandlerActive()));
                }
                else if (GetCurrentStatus() == Double)
                {
                        millClient.print(FormatMain(false, true, false, GetWebHandlerActive()));
                }
                else if (GetCurrentStatus() == FreePullState)
                {
                        millClient.print(FormatMain(false, false, true, GetWebHandlerActive()));
                }
                else
                {
                        millClient.print(FormatMain(true, true, true, GetWebHandlerActive()));
                        SetCurrentStatus(WaitForUser);
                }
        }
        else if (GetInputBuffer().indexOf(PAGE_SINGE_GETKEY) != -1)
        {
                if (GetCurrentStatus() == Single)
                {
                        millClient.print(FormatMain(true, false, false, GetWebHandlerActive()));
                }
                else if (GetCurrentStatus() == Double)
                {
                        millClient.print(FormatMain(false, true, false, GetWebHandlerActive()));
                }
                else if (GetCurrentStatus() == FreePullState)
                {
                        millClient.print(FormatMain(false, false, true, GetWebHandlerActive()));
                }
                else
                {
                        millClient.print(FormatMain(true, false, false, GetWebHandlerActive()));
                        SetCurrentStatus(Single);
                }
        }
        else if (GetInputBuffer().indexOf(PAGE_DOUBLE_GETKEY) != -1)
        {
                if (GetCurrentStatus() == Single)
                {
                        millClient.print(FormatMain(true, false, false, GetWebHandlerActive()));
                }
                else if (GetCurrentStatus() == Double)
                {
                        millClient.print(FormatMain(false, true, false, GetWebHandlerActive()));
                }
                else if (GetCurrentStatus() == FreePullState)
                {
                        millClient.print(FormatMain(false, false, true, GetWebHandlerActive()));
                }
                else
                {
                        millClient.print(FormatMain(false, true, false, GetWebHandlerActive()));
                        SetCurrentStatus(Double);
                }
        }
        else if (GetInputBuffer().indexOf(PAGE_FREEPULL_GETKEY) != -1)
        {
                if (GetCurrentStatus() == Single)
                {
                        millClient.print(FormatMain(true, false, false, GetWebHandlerActive()));
                }
                else if (GetCurrentStatus() == Double)
                {
                        millClient.print(FormatMain(false, true, false, GetWebHandlerActive()));
                }
                else if (GetCurrentStatus() == FreePullState)
                {
                        millClient.print(FormatMain(false, false, true, GetWebHandlerActive()));
                }
                else
                {
                        millClient.print(FormatMain(false, false, true, GetWebHandlerActive()));
                        SetCurrentStatus(FreePullState);
                }
        }
        else
        {
                millClient.print(PAGE_404);
        }

        millClient.println();
        return;
}

char WebHandler::ProcressInput()
{
        SetInput(millClient.read());

        if (GetInput() != CARRIAGE_RETURN)
        {
                SetInputBuffer(GetInputBuffer() + GetInput());
                return true;
        }
        else
        {
                return false;
        }
}
