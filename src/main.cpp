#include "Controller_defines.h"
#include "Controller.h"
#include "LogManager.h"
#include "MillWiFi.h"

Controller MillController(sd_CS_pin, nfc_SS_pin, nfc_RS_pin, SCL, SDA);

void setup()
{
        Serial.begin(9600);
        pinMode(RelayPin, OUTPUT);
        digitalWrite(RelayPin, LOW);
        // Let supply rails and peripherals settle after cold power-on before
        // initializing OLED/NFC. Keep the mill relay explicitly off meanwhile.
        delay(BOOT_POWER_STABILIZE_MS);
        MillController.Begin();              // Display + NFC + Config zuerst
        LogManager::getInstance().begin();   // LittleFS danach
        MillWiFi::getInstance().begin();     // WiFi-Server vorbereiten
}

void loop()
{
        MillWiFi::getInstance().handle();
        MillController.UpDateTime();
        MillController.States(MillController.StateTransitions());
        MillWiFi::getInstance().handle();
        delay(1);
}
