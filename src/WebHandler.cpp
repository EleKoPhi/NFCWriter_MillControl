/*
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
                                                /*client.print("<!doctype html> <html lang=\"en\"> <head> <style> .column { box-sizing: inherit; display: inline-block; margin-bottom: 0em; margin-top: 0em; vertical-align: middle; width: 100%; } .margin-top { margin-top: 1.6rem; } .centered { box-sizing: inherit; text-align: center; } .btn { -webkit-tap-highlight-color: transparent; background-color: #00bd9a; border-radius: 10px; box-shadow: rgba(0, 0, 0, 0.14) 0px 2px 2px 0px, rgba(0, 0, 0, 0.12) 0px 1px 5px 0px, rgba(0, 0, 0, 0.2) 0px 3px 1px -2px; box-sizing: inherit; color: white; display: inline-block; height: 200px; letter-spacing: 3px; line-height: 200px; padding: 0px 1rem; pointer-events: all; position: relative; text-decoration-line: none; text-transform: uppercase; vertical-align: middle; width: 80%; font-size: 30px; } </style> <meta charset=\"utf-8\"> <meta name=\"Homepage\" content=\"Starting page for the survey website \"> <title> Survey HomePage</title> </head> <body> <div class=\"column\"> <div class=\"margin-top centered\"> <form action=\"/action_page.php\"> <div class=\"btn\"> <label for=\"fname\">Einfacher Bezug:</label> <input type=\"number\" id=\"fsingle\" name=\"fsingle\"> </div> <br> <br> <div class=\"btn\"> <label for=\"lsingle\">Doppelter Bezug:</label> <input type=\"number\" id=\"ldouble\" name=\"ldouble\"> </div> <br> <br> <div class=\"btn\"> <input type=\"submit\" value=\"Bestätigen\"> </div> <br> <br> </form> </div> </div> </body> </html>");

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
*/


/*  

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

*/


/*

client = server.available();

WiFiServer server(80);
WiFiClient client;

*/

