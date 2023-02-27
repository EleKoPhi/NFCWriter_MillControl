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

        struct Config
        {
                String SSID;
                String PW;
                int single_time;
                int double_time;
                int ServerOn;
                int chipPage;
                int key;
                int split;
        };
        Config config;

        String CheckIfExists(String cardID);
        void begin();
        char ReadUserInput();
        bool HasCardToRead();
        String GetCardId();
        String GetTimeStamp();
        void WriteToLog(String userID, String credit, bool doppelt);
        int ReadCredit();
        int WriteCredit(int newCredit, bool doppelt);
        String ID();
        void newRead();

        bool loadConfiguration();
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
        int &GetChipSelectSD();

        MFRC522 &GetNFCReader();
        RTC_DS3231 &GetRTC();

        File &GetLogFile();
        void SetLogFile(File filename);

        int &GetUserKey();
        void SetUserKey(int key);

        static bool &GetStLeft();
        static void SetStLeft(bool st);
        static bool &GetStRigth();
        static void SetStRight(bool st);
        static bool &GetStBoth();
        static void SetStBoth(bool st);
        static void ResetInput();

        static unsigned long &GetTimer();
        static void SetTimer(long ti);

        static void StartKeyDebounce();
        static byte DebounceFinished(unsigned long maxTime, unsigned long minTime);

        ////////////// GETTER AND SETTER END ///////////////////

private:
        static void ISR_Left(void);
        static void ISR_Right(void);

        File _userData;
        File _logFile;
        RTC_DS3231 _rtc;
        MFRC522 _nfcReader;
        String User;

        int ChipSelect_SD;
        int UserKey;

        bool SdStatus;
        bool NfcStatus;
        bool RtcStatus;

        static bool KeyLeft;
        static bool KeyRight;
        static bool KeyBoth;

        static unsigned long debounce;
};

#endif