#include "Controller_defines.h"
#include "Controller.h"
#include "LogManager.h"
#include "MillWiFi.h"

Controller MillController(sd_CS_pin, nfc_SS_pin, nfc_RS_pin, SCL, SDA);

void setup()
{
        Serial.begin(9600);
        MillController.Begin();              // Display + NFC + Config zuerst
        LogManager::getInstance().begin();   // LittleFS danach
        MillWiFi::getInstance().begin();     // WiFi-AP zuletzt
}

void loop()
{
        MillWiFi::getInstance().handle();
        MillController.UpDateTime();
        MillController.States(MillController.StateTransitions());
        MillWiFi::getInstance().handle();
        delay(1);
}
