#ifndef WebHandler_defines
#define WebHandler_defines

#define SERVER_PORT             80

#define CARRIAGE_RETURN         '\r'

#define OK                      true
#define FAIL                    false

#define SERVER_TIMEOUT          1000

#define PAGE_MAIN               "<!doctype html> <html lang=\"en\"> <head> <style> .column { box-sizing: inherit; display: inline-block; margin-bottom: 0em; margin-top: 0em; vertical-align: middle; width: 100%; } .margin-top { margin-top: 1.6rem; } .centered { box-sizing: inherit; text-align: center; } .btn { -webkit-tap-highlight-color: transparent; background-color: #00bd9a; border-radius: 10px; box-shadow: rgba(0, 0, 0, 0.14) 0px 2px 2px 0px, rgba(0, 0, 0, 0.12) 0px 1px 5px 0px, rgba(0, 0, 0, 0.2) 0px 3px 1px -2px; box-sizing: inherit; color: white; cursor: pointer; display: inline-block; height: 200px; letter-spacing: 3px; line-height: 200px; padding: 0px 1rem; pointer-events: all; position: relative; text-decoration-line: none; text-transform: uppercase; vertical-align: middle; width: 80%; font-size: 30px; } </style> <meta charset=\"utf-8\"> <meta name=\"Homepage\" content=\"Starting page for the survey website \"> <title> Survey HomePage</title> </head> <body> <div class=\"column\"> <div class=\"margin-top centered\"> <a class=\"btn\" href=\"/Single\" style=\"\"> $SBS$Einfacher Bezug$SBE$ </a> </div> <br> <div class=\"centered\"> <a class=\"btn\" href=\"Double\" style=\"\"> $DBS$Doppelter Bezug$DBE$ </a> </div> <br> <div class=\"centered\"> <a class=\"btn\" href=\"FreePull\" style=\"\"> $FBS$Freibezug$FBE$ </a> </div> $returnKey$ </body> </html></body> </html> "
#define PAGE_404                "<div id=\"main\"> <div class=\"fof\"> <h1>Error 404 Page not found</h1> </div> </div>"
#define PAGE_PROGRESS           "<!DOCTYPE html> <html> <body> <label for=\"file\">Zubereitung:</label> <progress id=\"file\" value=$progress$ max=\"100\"> </progress> <a class=\"btn\" href=\"/Main\" style=\"\"> $txt$ </a> </body> </html>"
#define RETURN_KEY              "<br> <div class=\"centered\"> <a class=\"btn\" href=\"Main\" style=\"\"> Neue Auswahl </a> </div>"
#define RETUNN_KEY_FLAG         "$returnKey$"

#define HTML_CROSSED_OUT_START  "<s>"
#define HTML_CROSSED_OUT_END    "</s>"

#define HTML_UNDERLINED_START   "<u>"
#define HTML_UNDELINED_END      "</u>"      

#define SINGLE_BUTTON_START     "$SBS$"
#define SINGLE_BUTTON_END       "$SBE$"

#define DOBULE_BUTTON_START     "$DBS$"
#define DOBULE_BUTTON_END       "$DBE$"

#define FREEPULL_BUTTON_START   "$FBS$"
#define FREEPULL_BUTTON_END     "$FBE$"

#define PAGE_MAIN_GETKEY        "GET / "
#define PAGE_SINGE_GETKEY       "GET /Single"
#define PAGE_DOUBLE_GETKEY      "GET /Double"
#define PAGE_FREEPULL_GETKEY    "GET /FreePull"
#define PAGE_MAIN_GETBACKKEY    "GET /Main"

#define HTTP_STATUS             "HTTP/1.1 200 OK"
#define REFRESH_RATE            "Refresh: 1"
#define CONNCECTION_TYPE        "Connection: close"
#define CONNTENT_TYPE           "Content-Type: text/html"

#endif