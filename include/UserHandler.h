#ifndef UserHandler_h
#define UserHandler_h

#include "Arduino.h"
#include <SD.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include "RTClib.h"



class UserHandler
{
public:
UserHandler(int chipSelect, int slaveSelect, int rstPin);

struct Config {
        int single_time;
        int double_time;
        int ServerOn;
        String SSID;
        String PW;
        int chipPage;
};
Config config;

const char *filename = "config.json";

String CheckIfExists(String cardID);
void StartUp();
char ReadUserInput();
bool HasCardToRead();
String GetCardId();
String GetMoment();
void WriteToLog(String userID, String credit,bool doppelt);
int ReadCredit();
int WriteCredit(int newCredit,bool doppelt);
String ID();
void newRead();
String getLastUser();
void loadConfiguration();
bool saveConfiguration(int tiSingle, int tiDobule);

bool SdStatus;
bool NfcStatus;
bool RtcStatus;
String User;

private:
MFRC522 _nfcReader;
File _userData;
File _logFile;
RTC_DS3231 _rtc;
int _cspin;
int _chipPage;

bool deboundeStatus;

};

#endif
