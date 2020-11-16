#include "Arduino.h"
#include "UserHandler.h"
#include "Drawer.h"
#include "Controller.h"
#include "defines.h"
#include <ArduinoJson.h>
#include <WiFi101.h>
#include <SPI.h>

WiFiServer server(80);
WiFiClient client;

Controller :: Controller(int chipSelect, int slaveSelect, int rstPin, int clk, int data) : _drawer(clk, data), _userHandler(chipSelect, slaveSelect, rstPin) {
}

void Controller::Begin()
{
        _userHandler.StartUp();
        _drawer.DrawErr(_userHandler.SdStatus, _userHandler.NfcStatus, _userHandler.RtcStatus);
        _drawer.DrawMain();
        this->Reset();
        pinMode(5, OUTPUT);
        this->MillOff();

        this->LastUser = "";

        this->T_einfach = this->_userHandler.config.single_time;
        this->T_doppelt = this->_userHandler.config.double_time;


        if (this->_userHandler.config.ServerOn)
        {
                if (WiFi.status() == WL_NO_SHIELD) {
                        Serial.println("WiFi shield not present");
                        // don't continue
                        while (true);
                }

                Serial.println("WiFi shield present");
                String SSID = this->_userHandler.config.SSID;
                String PW = this->_userHandler.config.PW;
                status = WiFi.beginAP(SSID.c_str(),PW.c_str());
                server.begin();
                IPAddress ip =WiFi.localIP();
                Serial.println(ip);
        }

        this->StateBegin(WaitForUser);

}

void Controller::PutCurrentStatus(char stat)
{
        _currentStatus = stat;
}

char Controller::GetCurrentStatus()
{
        return _currentStatus;
}

char Controller::StateTransitions()
{
        this->oldKey = this->key;
        this->key =  _userHandler.ReadUserInput();
        client = server.available();

        if (((key == 'l') | (key == 'r') && (key == this->oldKey)))
        {
                return _currentStatus;
        }

        if (((_currentStatus == WaitForUser) | (_currentStatus == ShowCredit) | (_currentStatus == SceenSaferState)) && key == 'l')
        {
                if (_currentStatus == SceenSaferState) _startTime = millis();
                return StateBegin(PayOne);
        }

        else if (((_currentStatus == Einfach) | (_currentStatus == Doppelt) | (_currentStatus == FinishState)) && ((key == 'l') | (key == 'r')))
        {

                if (_currentStatus == Einfach)
                {T_rest = T_einfach;}

                else if (_currentStatus == Doppelt)
                {T_rest = T_doppelt;}

                else if (_currentStatus == FinishState)
                {_passedtime = _deltaTime + _passedtime;}

                if (_currentStatus != FinishState)
                {
                        _passedtime = _deltaTime;
                }
                _stopBegin = millis();
                _timeInStopState = 0;
                return StateBegin(StopState);
        }

        else if ((_currentStatus == StopState) && key == 'l')
        {
                _timeInStopState = 0;
                _stopBegin = 0;
                return StateBegin(WaitForUser);
        }
        else if ((_currentStatus == StopState) && key == 'r')
        {
                Serial.println("Test");
                return StateBegin(FinishState);
        }


        else if ((_currentStatus == FreePullState) && key == 'l')
        {
                return StateBegin(Einfach);
        }

        else if ((_currentStatus == FreePullState) && key == 'r')
        {
                return StateBegin(Doppelt);
        }

        else if ((_currentStatus == WaitForUser  | _currentStatus == ShowCredit | _currentStatus == SceenSaferState) && key == 'b')
        {
                if (_currentStatus == SceenSaferState) _startTime = millis();
                return StateBegin(LstUserState);
        }

        else if (_currentStatus == WaitForUser && client)
        {
                if (client)
                {
                        Serial.println("new client");
                        String currentLine = "";
                        while (client.connected())
                        {
                                //Serial.println(millis());
                                if (client.available())
                                {
                                        char c = client.read();
                                        Serial.write(c);
                                        if (c == '\n')
                                        {

                                                if (currentLine.length() == 0)
                                                {

                                                        client.println("HTTP/1.1 200 OK");
                                                        client.println("Content-type:text/html");
                                                        client.println();

                                                        client.print("<!doctype html> <html lang=\"en\"> <head> <style> .column { box-sizing: inherit; display: inline-block; margin-bottom: 0em; margin-top: 0em; vertical-align: middle; width: 100%; } .margin-top { margin-top: 1.6rem; } .centered { box-sizing: inherit; text-align: center; } .btn { -webkit-tap-highlight-color: transparent; background-color: #00bd9a; border-radius: 10px; box-shadow: rgba(0, 0, 0, 0.14) 0px 2px 2px 0px, rgba(0, 0, 0, 0.12) 0px 1px 5px 0px, rgba(0, 0, 0, 0.2) 0px 3px 1px -2px; box-sizing: inherit; color: white; cursor: pointer; display: inline-block; height: 200px; letter-spacing: 3px; line-height: 200px; padding: 0px 1rem; pointer-events: all; position: relative; text-decoration-line: none; text-transform: uppercase; vertical-align: middle; width: 80%; font-size: 30px; } </style> <meta charset=\"utf-8\"> <meta name=\"Homepage\" content=\"Starting page for the survey website \"> <title> Survey HomePage</title> </head> <body> <div class=\"column\"> <div class=\"margin-top centered\"> <a class=\"btn\" href=\"/S\" style=\"\"> Einfacher Bezug </a> </div> <br> <div class=\"centered\"> <a class=\"btn\" href=\"D\" style=\"\"> Doppelter Bezug </a> </div> <br> <div class=\"centered\"> <a class=\"btn\" href=\"V\" style=\"\"> Freibezug </a> </div> </body> </html>");

                                                        client.println();

                                                        break;
                                                }
                                                else
                                                {       if (currentLine.lastIndexOf("action_page.php") != -1)
                                                        {
                                                                String SingleKey = "fsingle=";
                                                                String DoubleKey = "&ldouble=";
                                                                String EndKEy = " HTTP/1.1";
                                                                int s = currentLine.lastIndexOf(SingleKey);
                                                                int d = currentLine.lastIndexOf(DoubleKey);
                                                                int e = currentLine.lastIndexOf(EndKEy);
                                                                int Single_from_web = currentLine.substring(s+SingleKey.length(), d).toInt();
                                                                int Double_from_web = currentLine.substring(d+DoubleKey.length(), e).toInt();

                                                                if (_userHandler.saveConfiguration(Single_from_web, Double_from_web))
                                                                {
                                                                        this->T_einfach = Single_from_web;
                                                                        this->T_doppelt = Double_from_web;

                                                                        client.println("HTTP/1.1 200 OK");
                                                                        client.println("Content-type:text/html");
                                                                        client.println();

                                                                        String html_side = "<!doctype html> <html lang=\"en\"> <head> <style> .column { box-sizing: inherit; display: inline-block; margin-bottom: 0em; margin-top: 0em; vertical-align: middle; width: 100%; } .margin-top { margin-top: 1.6rem; } .centered { box-sizing: inherit; text-align: center; } .btn { -webkit-tap-highlight-color: transparent; background-color: #00bd9a; border-radius: 10px; box-shadow: rgba(0, 0, 0, 0.14) 0px 2px 2px 0px, rgba(0, 0, 0, 0.12) 0px 1px 5px 0px, rgba(0, 0, 0, 0.2) 0px 3px 1px -2px; box-sizing: inherit; color: white; cursor: pointer; display: inline-block; height: 200px; letter-spacing: 3px; line-height: 200px; padding: 0px 1rem; pointer-events: all; position: relative; text-decoration-line: none; text-transform: uppercase; vertical-align: middle; width: 80%; font-size: 30px; } </style> <meta charset=\"utf-8\"> <meta name=\"Homepage\" content=\"Starting page for the survey website \"> <title> Survey HomePage</title> </head> <body> <div class=\"column\"> <div class=\"margin-top centered\"> <a class=\"btn\" href=\"/S\" style=\"\"> $single$ </a> </div> <br> <div class=\"centered\"> <a class=\"btn\" href=\"D\" style=\"\"> $double$ </a> </div> <br> <div class=\"centered\"> <a class=\"btn\" href=\"V\" style=\"\"> Freibezug </a> </div> </body> </html>";

                                                                        html_side.replace("$single$",String(this->T_einfach));
                                                                        html_side.replace("$double$",String(this->T_doppelt));

                                                                        client.print(html_side);

                                                                        client.println();
                                                                        client.flush();
                                                                        client.stop();
                                                                }
                                                                else
                                                                {

                                                                        client.println("HTTP/1.1 200 OK");
                                                                        client.println("Content-type:text/html");
                                                                        client.println();

                                                                        String html_side = "<!doctype html> <html lang=\"en\"> <head> <style> .column { box-sizing: inherit; display: inline-block; margin-bottom: 0em; margin-top: 0em; vertical-align: middle; width: 100%; } .margin-top { margin-top: 1.6rem; } .centered { box-sizing: inherit; text-align: center; } .btn { -webkit-tap-highlight-color: transparent; background-color: #00bd9a; border-radius: 10px; box-shadow: rgba(0, 0, 0, 0.14) 0px 2px 2px 0px, rgba(0, 0, 0, 0.12) 0px 1px 5px 0px, rgba(0, 0, 0, 0.2) 0px 3px 1px -2px; box-sizing: inherit; color: white; cursor: pointer; display: inline-block; height: 200px; letter-spacing: 3px; line-height: 200px; padding: 0px 1rem; pointer-events: all; position: relative; text-decoration-line: none; text-transform: uppercase; vertical-align: middle; width: 80%; font-size: 30px; } </style> <meta charset=\"utf-8\"> <meta name=\"Homepage\" content=\"Km next\"> <title> Survey HomePage</title> </head> <body> <div class=\"column\"> <div class=\"margin-top centered\"> <a class=\"btn\" href=\"/S\" style=\"\"> Einfacher Bezug </a> </div> <br> <div class=\"centered\"> <a class=\"btn\" href=\"D\" style=\"\"> Doppelter Bezug </a> </div> <br> <div class=\"centered\"> <a class=\"btn\" href=\"V\" style=\"\"> Freibezug </a> </div> </body> </html>";

                                                                        client.print(html_side);

                                                                        client.println();
                                                                        client.flush();
                                                                        client.stop();
                                                                }


                                                        }
                                                        currentLine = "";}
                                        }
                                        else if (c != '\r')
                                        {
                                                currentLine += c;

                                        }

                                        if (currentLine.endsWith("GET /S"))
                                        {
                                                client.println("HTTP/1.1 200 OK");
                                                client.println("Content-type:text/html");
                                                client.println();

                                                client.print("<!doctype html> <html lang=\"en\"> <head> <style> .column { box-sizing: inherit; display: inline-block; margin-bottom: 0em; margin-top: 0em; vertical-align: middle; width: 100%; } .margin-top { margin-top: 1.6rem; } .centered { box-sizing: inherit; text-align: center; } .btn { -webkit-tap-highlight-color: transparent; background-color: #00bd9a; border-radius: 10px; box-shadow: rgba(0, 0, 0, 0.14) 0px 2px 2px 0px, rgba(0, 0, 0, 0.12) 0px 1px 5px 0px, rgba(0, 0, 0, 0.2) 0px 3px 1px -2px; box-sizing: inherit; color: white; cursor: pointer; display: inline-block; height: 200px; letter-spacing: 3px; line-height: 200px; padding: 0px 1rem; pointer-events: all; position: relative; text-decoration-line: none; text-transform: uppercase; vertical-align: middle; width: 80%; font-size: 30px; } </style> <meta charset=\"utf-8\"> <meta name=\"Homepage\" content=\"Starting page for the survey website \"> <title> Survey HomePage</title> </head> <body> <div class=\"column\"> <div class=\"margin-top centered\"> <a class=\"btn\" href=\"/S\" style=\"\"> Einfacher Bezug </a> </div> <br> <div class=\"centered\"> <a class=\"btn\" href=\"D\" style=\"\"> Doppelter Bezug </a> </div> <br> <div class=\"centered\"> <a class=\"btn\" href=\"V\" style=\"\"> Freibezug </a> </div> </body> </html>");

                                                client.println();
                                                client.flush();
                                                client.stop();
                                                return StateBegin(Einfach);
                                        }
                                        if (currentLine.endsWith("GET /D"))
                                        {
                                                client.println("HTTP/1.1 200 OK");
                                                client.println("Content-type:text/html");
                                                client.println();

                                                client.print("<!doctype html> <html lang=\"en\"> <head> <style> .column { box-sizing: inherit; display: inline-block; margin-bottom: 0em; margin-top: 0em; vertical-align: middle; width: 100%; } .margin-top { margin-top: 1.6rem; } .centered { box-sizing: inherit; text-align: center; } .btn { -webkit-tap-highlight-color: transparent; background-color: #00bd9a; border-radius: 10px; box-shadow: rgba(0, 0, 0, 0.14) 0px 2px 2px 0px, rgba(0, 0, 0, 0.12) 0px 1px 5px 0px, rgba(0, 0, 0, 0.2) 0px 3px 1px -2px; box-sizing: inherit; color: white; cursor: pointer; display: inline-block; height: 200px; letter-spacing: 3px; line-height: 200px; padding: 0px 1rem; pointer-events: all; position: relative; text-decoration-line: none; text-transform: uppercase; vertical-align: middle; width: 80%; font-size: 30px; } </style> <meta charset=\"utf-8\"> <meta name=\"Homepage\" content=\"Starting page for the survey website \"> <title> Survey HomePage</title> </head> <body> <div class=\"column\"> <div class=\"margin-top centered\"> <a class=\"btn\" href=\"/S\" style=\"\"> Einfacher Bezug </a> </div> <br> <div class=\"centered\"> <a class=\"btn\" href=\"D\" style=\"\"> Doppelter Bezug </a> </div> <br> <div class=\"centered\"> <a class=\"btn\" href=\"V\" style=\"\"> Freibezug </a> </div> </body> </html>");

                                                client.println();
                                                client.flush();
                                                client.stop();
                                                return StateBegin(Doppelt);
                                        }
                                        if (currentLine.endsWith("GET /V"))
                                        {
                                                Serial.println(currentLine);
                                                client.println("HTTP/1.1 200 OK");
                                                client.println("Content-type:text/html");
                                                client.println();

                                                client.print("<!doctype html> <html lang=\"en\"> <head> <style> .column { box-sizing: inherit; display: inline-block; margin-bottom: 0em; margin-top: 0em; vertical-align: middle; width: 100%; } .margin-top { margin-top: 1.6rem; } .centered { box-sizing: inherit; text-align: center; } .btn { -webkit-tap-highlight-color: transparent; background-color: #00bd9a; border-radius: 10px; box-shadow: rgba(0, 0, 0, 0.14) 0px 2px 2px 0px, rgba(0, 0, 0, 0.12) 0px 1px 5px 0px, rgba(0, 0, 0, 0.2) 0px 3px 1px -2px; box-sizing: inherit; color: white; cursor: pointer; display: inline-block; height: 200px; letter-spacing: 3px; line-height: 200px; padding: 0px 1rem; pointer-events: all; position: relative; text-decoration-line: none; text-transform: uppercase; vertical-align: middle; width: 80%; font-size: 30px; } </style> <meta charset=\"utf-8\"> <meta name=\"Homepage\" content=\"Starting page for the survey website \"> <title> Survey HomePage</title> </head> <body> <div class=\"column\"> <div class=\"margin-top centered\"> <a class=\"btn\" href=\"/S\" style=\"\"> Einfacher Bezug </a> </div> <br> <div class=\"centered\"> <a class=\"btn\" href=\"D\" style=\"\"> Doppelter Bezug </a> </div> <br> <div class=\"centered\"> <a class=\"btn\" href=\"V\" style=\"\"> Freibezug </a> </div> </body> </html>");

                                                client.println();
                                                client.flush();
                                                client.stop();
                                                return StateBegin(FreePullState);
                                        }
                                        if (currentLine.endsWith("GET /Q"))
                                        {
                                                client.println("HTTP/1.1 200 OK");
                                                client.println("Content-type:text/html");
                                                client.println();

                                                //client.print("<form action=\"/action_page.php\"> <label for=\"fname\">Einfacher Bezug:</label> <input type=\"number\" id=\"fsingle\" name=\"fsingle\"><br><br> <label for=\"lsingle\">Doppelter Bezug:</label> <input type=\"number\" id=\"ldouble\" name=\"ldouble\"><br><br> <input type=\"submit\" value=\"Submit\"> </form>");
                                                /*client.print("<!doctype html> <html lang=\"en\"> <head> <style> .column { box-sizing: inherit; display: inline-block; margin-bottom: 0em; margin-top: 0em; vertical-align: middle; width: 100%; } .margin-top { margin-top: 1.6rem; } .centered { box-sizing: inherit; text-align: center; } .btn { -webkit-tap-highlight-color: transparent; background-color: #00bd9a; border-radius: 10px; box-shadow: rgba(0, 0, 0, 0.14) 0px 2px 2px 0px, rgba(0, 0, 0, 0.12) 0px 1px 5px 0px, rgba(0, 0, 0, 0.2) 0px 3px 1px -2px; box-sizing: inherit; color: white; display: inline-block; height: 200px; letter-spacing: 3px; line-height: 200px; padding: 0px 1rem; pointer-events: all; position: relative; text-decoration-line: none; text-transform: uppercase; vertical-align: middle; width: 80%; font-size: 30px; } </style> <meta charset=\"utf-8\"> <meta name=\"Homepage\" content=\"Starting page for the survey website \"> <title> Survey HomePage</title> </head> <body> <div class=\"column\"> <div class=\"margin-top centered\"> <form action=\"/action_page.php\"> <div class=\"btn\"> <label for=\"fname\">Einfacher Bezug:</label> <input type=\"number\" id=\"fsingle\" name=\"fsingle\"> </div> <br> <br> <div class=\"btn\"> <label for=\"lsingle\">Doppelter Bezug:</label> <input type=\"number\" id=\"ldouble\" name=\"ldouble\"> </div> <br> <br> <div class=\"btn\"> <input type=\"submit\" value=\"Bestätigen\"> </div> <br> <br> </form> </div> </div> </body> </html>");*/

                                                String html_side ="<!doctype html> <html lang=\"en\"> <head> <style> .column { box-sizing: inherit; display: inline-block; margin-bottom: 0em; margin-top: 0em; vertical-align: middle; width: 100%; } .margin-top { margin-top: 1.6rem; } .centered { box-sizing: inherit; text-align: center; } .btn { -webkit-tap-highlight-color: transparent; background-color: #00bd9a; border-radius: 10px; box-shadow: rgba(0, 0, 0, 0.14) 0px 2px 2px 0px, rgba(0, 0, 0, 0.12) 0px 1px 5px 0px, rgba(0, 0, 0, 0.2) 0px 3px 1px -2px; box-sizing: inherit; color: white; display: inline-block; height: 200px; letter-spacing: 3px; line-height: 200px; padding: 0px 1rem; pointer-events: all; position: relative; text-decoration-line: none; text-transform: uppercase; vertical-align: middle; width: 80%; font-size: 30px; } </style> <meta charset=\"utf-8\"> <meta name=\"Homepage\" content=\"Starting page for the survey website \"> <title> Survey HomePage</title> </head> <body> <div class=\"column\"> <div class=\"margin-top centered\"> <form action=\"/action_page.php\"> <div class=\"btn\"> <label for=\"fname\">Einfacher Bezug $einfach$:</label> <input type=\"number\" id=\"fsingle\" name=\"fsingle\"> </div> <br> <br> <div class=\"btn\"> <label for=\"lsingle\">Doppelter Bezug $Key1$:</label> <input type=\"number\" id=\"ldouble\" name=\"ldouble\"> </div> <br> <br> <div class=\"btn\"> <input type=\"submit\" value=\"Bestätigen\"> </div> <br> <br> </form> </div> </div> </body> </html>";

                                                //Serial.println(html_side);
                                                html_side.replace("$einfach$",String(this->T_einfach));
                                                html_side.replace("$Key1$",String(this->T_doppelt));
                                                //Serial.println(html_side);

                                                client.print(html_side);

                                                client.println();
                                                client.flush();
                                                client.stop();
                                                return StateBegin(WaitForUser);
                                        }
                                }
                        }
                        client.stop();
                        Serial.println("client disonnected");
                }


        }

        else if ((_currentStatus == LstUserState) && (key == 'l' | key == 'r'))
        {

                return StateBegin(HoldState_2);
        }

        else if ((_currentStatus == SplitPaymentQ) && key == 'b')
        {

                return StateBegin(HoldState_2);
        }

        else if ((_currentStatus == WaitForUser  | _currentStatus == ShowCredit | _currentStatus == SceenSaferState) && key == 'r')
        {
                if (_currentStatus == SceenSaferState) _startTime = millis();
                return StateBegin(HoldState);
        }

        else if (_currentStatus == SplitPaymentQ && key == 'l')
        {
                return StateBegin(PayTwo);
        }

        else if (_currentStatus == SplitPaymentQ && key == 'r')
        {
                return StateBegin(PayTwo_1);
        }

        else if ((_currentStatus == WaitForUser | _currentStatus == SceenSaferState )&& _userHandler.HasCardToRead())
        {
                if (_currentStatus == SceenSaferState) _startTime = millis();
                return StateBegin(ReadCreditUser);
        }

        else if (_currentStatus == ReadCreditUser)
        {
                return StateBegin(ShowCredit);
        }

        else if (_currentStatus == PayOne && _userHandler.HasCardToRead())
        {
                int Credit = _userHandler.ReadCredit();
                int stat = 0;
                int count = 0;

                if (Credit == -1)
                {
                        return StateBegin(PayOne);
                }

                if (Credit >= 1)
                {
                        stat = _userHandler.WriteCredit(Credit - 1,false);

                        if (stat == 0)
                        {
                                delay(100);
                                return StateBegin(Einfach);
                        }
                        else
                        {
                                while ((stat != 0) && (count < 10))
                                {
                                        stat = _userHandler.WriteCredit(Credit - 1,false);
                                        if (stat == 0) break;
                                        count++;;
                                }

                                if (count > 8)
                                {
                                        _drawer.Err();
                                        delay(2000);
                                        return StateBegin(WaitForUser);
                                }
                                else
                                { delay(100);
                                  return StateBegin(Einfach);}
                        }
                }
                else
                {
                        return StateBegin(LowCredit);
                }
        }

        else if (_currentStatus == PayTwo && _userHandler.HasCardToRead())
        {
                int Credit = _userHandler.ReadCredit();
                int stat = 0;
                bool err = false;
                int count = 0;

                if (Credit == -1)
                {
                        return StateBegin(WaitForUser);
                }

                if (Credit >= 2)
                {
                        stat = _userHandler.WriteCredit(Credit - 2,true);

                        if (stat == 0)
                        {
                                delay(100);
                                return StateBegin(Doppelt);
                        }
                        else
                        {
                                while ((stat != 0) && (count < 10))
                                {
                                        stat = _userHandler.WriteCredit(Credit - 2,true);
                                        if (stat == 0) break;
                                        count++;;
                                }

                                if (count > 8)
                                {
                                        _drawer.Err();
                                        delay(2000);
                                        return StateBegin(WaitForUser);
                                }
                                else
                                { delay(100);
                                  return StateBegin(Doppelt);}
                        }
                }
                else
                {
                        return StateBegin(LowCredit);
                }
        }

        else if (_currentStatus == PayTwo_1 && _userHandler.HasCardToRead())
        {
                int Credit = _userHandler.ReadCredit();

                if (Credit == -1)
                {
                        return StateBegin(PayTwo_1);
                }

                if (Credit >= 1)
                {
                        _userHandler.WriteCredit(Credit - 1,false);
                        _userHandler.newRead();
                        while (_currentUser == "0" || _currentUser == "")
                        {
                                _currentUser = _userHandler.GetCardId();
                                _userHandler.ReadCredit();
                        }
                        return StateBegin(PayTwo_2);
                }
                else
                {
                        return StateBegin(LowCredit);
                }

        }
        else if (_currentStatus == PayTwo_2 && _userHandler.HasCardToRead())
        {
                String CurrentUser = _userHandler.GetCardId();
                int Credit = _userHandler.ReadCredit();

                if (Credit >= 1 && _currentUser != CurrentUser && CurrentUser != 0)
                {
                        _userHandler.WriteCredit(Credit - 1,false);
                        delay(100);
                        return StateBegin(Doppelt);
                }
                else
                {
                        return StateBegin(PayTwo_2);
                }
        }
        else if (_currentStatus == ReplayState && _userHandler.HasCardToRead())
        {
                String CurrentUser = _userHandler.GetCardId();
                int Credit = _userHandler.ReadCredit();
                if (Credit == -1)
                {
                        return StateBegin(ReplayState);
                }
                else
                {
                        _userHandler.WriteCredit(Credit + 1,false);
                        return StateBegin(DoneState);
                }
        }
        else if (_currentStatus == DoneState && !(_userHandler.HasCardToRead()))
        {
                if (!(_userHandler.HasCardToRead()))
                {return StateBegin(WaitForUser);}
                else
                {
                        return StateBegin(DoneState);
                }
        }
        else
        {
                return _currentStatus;
        }
}

bool Controller::TimeOut(int time)
{
        if (_deltaTime >= time)
        {
                this->Reset();
                return true;
        }
        return false;
}

bool Controller::TimeOutWithBackPay(int time)
{
        if (_deltaTime >= time)
        {
                this->StateBegin(ReplayState);
                return true;
        }
        return false;
}

void Controller::UpDateTime()
{
        _deltaTime = millis() - _startTime;
}

void Controller::States(char Status)
{

        if (Status == Einfach)
        {
                this->MillOn();
                int progress = ((_deltaTime) / (this->T_einfach / 100));
                this->_drawer.DisplayProgress(progress);
                this->TimeOut(this->T_einfach);
        }

        else if (Status == Doppelt)
        {
                this->MillOn();
                this->_drawer.DisplayProgress(_deltaTime / (this->T_doppelt / 100));
                this->TimeOut(this->T_doppelt);
        }

        else if (Status == WaitForUser)
        {
                _drawer.DrawWaitForUser();

                //Serial.println(_deltaTime);

                if (this->_deltaTime > 3600000)
                {
                        this->StateBegin(SceenSaferState);
                }
        }

        else if (Status == FreePullState)
        {
                _drawer.DrawFreeState();

                if (this->_deltaTime > 60000)
                {
                        this->StateBegin(WaitForUser);
                }
        }

        else if (Status == PayOne)
        {
                _drawer.DrawPayOne();
                this->TimeOut(10000);
        }

        else if (Status == SceenSaferState)
        {
                _drawer.DrawScreenSafer(_deltaTime);
        }

        else if (Status == SplitPaymentQ)
        {
                _drawer.DrawSplitQ2();
                this->TimeOut(10000);
        }

        else if (Status == PayTwo)
        {
                _drawer.DrawPay2();
                this->TimeOut(10000);
        }

        else if (Status == PayTwo_1)
        {
                _drawer.DrawPay2_1();
                this->TimeOut(10000);
        }

        else if (Status == PayTwo_2)
        {
                _drawer.DrawPay2_2();
                this->TimeOutWithBackPay(15000);
        }

        else if (Status == LowCredit)
        {
                _drawer.DrawLowCredit();
                this->TimeOut(5000);
        }


        else if (Status == ReadCreditUser)
        {
                user = _userHandler.GetCardId().toInt();
                credit = _userHandler.ReadCredit();
        }

        else if (Status == ShowCredit)
        {
                if(user==0)
                {
                        this->TimeOut(0);
                        return;
                }
                _drawer.DrawCredit(user, credit);
                this->TimeOut(5000);
        }
        else if (Status == HoldState)
        {
                if (_userHandler.ReadUserInput() == 'n')
                {
                        StateBegin(SplitPaymentQ);
                }
        }
        else if (Status == HoldState_2)
        {
                if (_userHandler.ReadUserInput() == 'n')
                {
                        StateBegin(WaitForUser);
                }
        }
        else if (Status == ReplayState)
        {
                _drawer.DrawReplay(_deltaTime / (30000 / 100));
                this->TimeOut(30000);
        }
        else if (Status == DoneState)
        {
                _drawer.DrawDoneState();
                this->TimeOut(10000);
        }
        else if (Status == LstUserState)
        {
                _drawer.DrawLastUser(this->_userHandler.getLastUser());
                this->TimeOut(10000);
        }
        else if (Status == StopState)
        {
                _timeInStopState = millis() - _stopBegin;
                _drawer.DrawStopState();
                this->MillOff();
                this->TimeOut(60000);
        }
        else if (Status == FinishState)
        {
                this->MillOn();
                Serial.println(T_rest);
                int progress = ((_passedtime+_deltaTime) / (this->T_rest / 100));
                Serial.println(_deltaTime);
                this->_drawer.DisplayProgress(progress);
                this->TimeOut(this->T_rest-_passedtime);
        }

}

void Controller::Reset()
{
        this->MillOff();
        this->PutCurrentStatus(WaitForUser);
        _deltaTime = 0;
        _startTime = millis();
        _currentUser = String("");
        _additionalUser = String("");
        _currentUserId = String("");
        _additionalUserId = String("");
        user = 0;
        credit = 0;
}

String Controller::GetCurrentUser()
{
        return this->_currentUser;
}

void Controller::SetCurrentUser(String user)
{
        _currentUser = user;
}

char Controller::StateBegin(char state)
{
        this->PutCurrentStatus(state);
        _startTime = millis();
        this->UpDateTime();
        return state;
}

void Controller::MillOn()
{
        digitalWrite(RelayPin, HIGH);
}

void Controller::MillOff()
{
        digitalWrite(RelayPin, LOW);
}
