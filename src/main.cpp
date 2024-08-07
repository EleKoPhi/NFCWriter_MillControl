#include "Controller_defines.h"
#include "Controller.h"

Controller MillController(sd_CS_pin, nfc_SS_pin, nfc_RS_pin, SCL, SDA);

void setup()
{
        Serial.begin(9600);
        MillController.Begin();
}

void loop()
{
        MillController.UpDateTime();
        MillController.States(MillController.StateTransitions());
}
