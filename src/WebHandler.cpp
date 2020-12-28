#include "WebHandler.h"
#include "WebHandler_defines.h"

WebHandler::WebHandler() : millserver(SERVER_PORT), millClient() {}

void WebHandler::SetInputBuffer(String str) { this->InputBuffer = str; };
String WebHandler::GetInputBuffer() { return this->InputBuffer; };
void WebHandler::ClearInputBuffer() { this->InputBuffer = ""; };
void WebHandler::SetInput(char c) { this->input = c; };
char WebHandler::GetInput() { return this->input; };

unsigned long WebHandler::GetServerWatchDogTimer() { return serverWatchDogTimer; };
void WebHandler::SetServerWatchDogTimer(unsigned long time) { this->serverWatchDogTimer = time; }
bool WebHandler::IsTimeOut(unsigned long time)
{
        return (millis() - GetServerWatchDogTimer()) > time;
}

void WebHandler::SetWasTimeOut(bool st) { this->wasTimeOut = st; }
bool WebHandler::GetWasTimeOut() { return this->wasTimeOut; }

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
                        Serial.println("Clear buffer");
                        ClearInputBuffer();
                }

                SetServerWatchDogTimer(millis());
                SetWasTimeOut(false);

                while (millClient.connected())
                {
                        if (IsTimeOut(SERVER_TIMEOUT))
                        {
                                SetWasTimeOut(true);
                                break;
                        }

                        if (millClient.available())
                        {
                                ProcressInput();
                        }

                        if (GetInputBuffer().endsWith("keep-alive"))
                        {
                                break;
                        };
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
        Serial.println(GetInputBuffer());
        if (GetInputBuffer().indexOf("GET /S") != -1)
        {
                millClient.println("HTTP/1.1 200 OK");
                millClient.println("Content-type:text/html");
                millClient.println();
                millClient.print("<!doctype html> <html lang=\"en\"> <head> <style> .column { box-sizing: inherit; display: inline-block; margin-bottom: 0em; margin-top: 0em; vertical-align: middle; width: 100%; } .margin-top { margin-top: 1.6rem; } .centered { box-sizing: inherit; text-align: center; } .btn { -webkit-tap-highlight-color: transparent; background-color: #00bd9a; border-radius: 10px; box-shadow: rgba(0, 0, 0, 0.14) 0px 2px 2px 0px, rgba(0, 0, 0, 0.12) 0px 1px 5px 0px, rgba(0, 0, 0, 0.2) 0px 3px 1px -2px; box-sizing: inherit; color: white; cursor: pointer; display: inline-block; height: 200px; letter-spacing: 3px; line-height: 200px; padding: 0px 1rem; pointer-events: all; position: relative; text-decoration-line: none; text-transform: uppercase; vertical-align: middle; width: 80%; font-size: 30px; } </style> <meta charset=\"utf-8\"> <meta name=\"Homepage\" content=\"Starting page for the survey website \"> <title> Survey HomePage</title> </head> <body> <div class=\"column\"> <div class=\"margin-top centered\"> <a class=\"btn\" href=\"/S\" style=\"\"> Einfacher Bezug </a> </div> <br> <div class=\"centered\"> <a class=\"btn\" href=\"D\" style=\"\"> Doppelter Bezug </a> </div> <br> <div class=\"centered\"> <a class=\"btn\" href=\"V\" style=\"\"> Freibezug </a> </div> </body> </html>");
                millClient.println();
                Serial.println("Einfach");
                return;
        }
        else
        {
                millClient.println("HTTP/1.1 200 OK");
                millClient.println("Content-type:text/html");
                millClient.println();
                millClient.println("<div id=\"main\"> <div class=\"fof\"> <h1>Error 404 Page not found... dont call me</h1> </div> </div>");
                millClient.println();
                Serial.println("404");
                return;
        }
}
char WebHandler::ProcressInput()
{
        SetInput(millClient.read());

        if (GetInput() == -1)
                return false;
        else if (GetInput() == '\n')
        {
                /* code */
        }
        else if (GetInput() != '\r')
        {
                SetInputBuffer(GetInputBuffer() + GetInput());
                return true;
        }
        else
        {
                return false;
        }
}
