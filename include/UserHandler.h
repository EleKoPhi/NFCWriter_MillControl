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
        int key;
};
Config config;

String CheckIfExists(String cardID);
void begin();
char ReadUserInput();
bool HasCardToRead();
String GetCardId();
String GetTimeStamp();
void WriteToLog(String userID, String credit,bool doppelt);
int ReadCredit();
int WriteCredit(int newCredit,bool doppelt);
String ID();
void newRead();

void loadConfiguration();
bool saveConfiguration(int tiSingle, int tiDobule);

bool AuthenticateUser(int localKey); 

////////////// GETTER AND SETTER BEGIN /////////////////

bool &GetSDStatus();
void SetSDStatus(bool Status);
bool &GetRTCStatus();
void SetRTCStatus(bool Status);
bool &GetNFCStatus();
void SetNFCStatus(bool Status);

String getLastUser();

String &GetUser();
void SetUser(String user);

void SetChipSelectSD(int PIN);
int &GetChipSelectSD();;

MFRC522 &GetNFCReader();
RTC_DS3231 &GetRTC();

File &GetLogFile();
void SetLogFile(File filename);

int &GetUserKey();
void SetUserKey(int key);

////////////// GETTER AND SETTER END ///////////////////

private:

File _userData;
File _logFile;
RTC_DS3231 _rtc;
MFRC522 _nfcReader;
String User;

int ChipSelect_SD;
bool SdStatus;
bool NfcStatus;
bool RtcStatus;

int UserKey;

};

#endif