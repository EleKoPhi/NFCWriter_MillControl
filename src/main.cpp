 #include "defines.h"
 #include "Controller.h"
 
Controller MillController(sd_CS_pin, nfc_SS_pin, nfc_RS_pin, display_CLK_pin, display_DATA_pin);

long TimeOld, TimeNew;

void setup()
{
        Serial.begin(9600);
        MillController.Begin();
        TimeOld = 0;
        TimeNew = 0;
}

void loop()
{
        MillController.UpDateTime();
        TimeNew = micros();
        MillController.States(MillController.StateTransitions());
        TimeOld = micros();
        //Serial.println(TimeOld-TimeNew);
}
