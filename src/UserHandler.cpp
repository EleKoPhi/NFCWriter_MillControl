#include "Arduino.h"
#include "UserHandler.h"
#include "defines.h"
#include <SPI.h>
#include <SD.h>
#include <MFRC522.h>
#include <ArduinoJson.h>

byte PSWBuff[] = {0xFF, 0xAB, 0xBA, 0xFF};
byte pACK[] = {0xE, 0x5};

UserHandler::UserHandler(int chipSelect, int slaveSelect, int rstPin) : _nfcReader(slaveSelect, rstPin)
{
        _cspin = chipSelect;
        SdStatus = false;
        deboundeStatus = false;
        this->User = "";
        this->_chipPage = 0x0;
}

void UserHandler::loadConfiguration()
{
        SD.begin(_cspin);
        File file = SD.open("config.txt", FILE_READ);

        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, file);
        if (error)
                Serial.println(F("Failed to read file, using default configuration"));

        config.single_time = doc["Zeit_einfach"] | 1;
        config.double_time = doc["Zeit_doppelt"] | 1;
        config.ServerOn = doc["ServerOn"] | 0;
        config.SSID = doc["SSID"] | "";
        config.PW = doc["PW"] | "";
        config.chipPage = doc["ChipPage"] | 1;

        Serial.println(config.SSID);
        Serial.println(config.chipPage );

        file.close();
}

void UserHandler::StartUp()
{
        SPI.begin();
        RtcStatus = _rtc.begin();
        _nfcReader.PCD_Init();
        SdStatus = SD.begin(_cspin);
        _nfcReader.PCD_Init();
        NfcStatus = _nfcReader.PCD_PerformSelfTest();
        _nfcReader.PCD_Init();
        pinMode(taster_LINKS_pin, INPUT);
        pinMode(taster_RECHTS_pin, INPUT);
        this->loadConfiguration();
}


char UserHandler::ReadUserInput()
{
        if (digitalRead(taster_LINKS_pin))
        {
                if (digitalRead(taster_RECHTS_pin))
                {
                        return 'b';
                }
                else
                {
                        return 'l';
                }
        }
        else if (digitalRead(taster_RECHTS_pin))
        {
                if (digitalRead(taster_LINKS_pin))
                {
                        return 'b';
                }
                else
                {
                        return 'r';
                }

        }
        else
        {
                return 'n';
        }
}

bool UserHandler::HasCardToRead()
{
        if (!_nfcReader.PICC_IsNewCardPresent())
        {
                return false;
        }
        else
        {
                return true;
        }
}

String UserHandler::GetCardId()
{
        long code = 0;

        if (_nfcReader.PICC_ReadCardSerial())
        {
                for (byte i = 0; i < _nfcReader.uid.size; i++)
                {
                        code = ((code + _nfcReader.uid.uidByte[i]) * 10);
                }
        }

        Serial.println(String(code, DEC));
        return String(code, DEC);
}

String UserHandler::GetMoment()
{
        String Moment = "";
        _rtc.begin();
        DateTime now = _rtc.now();
        _nfcReader.PCD_Init(); // Shit :O
        Moment = String(now.year(), DEC) + "-";
        if (now.month() < 10) Moment += "0";
        Moment += String(now.month(), DEC) + "-";
        if (now.day() < 10) Moment += "0";
        Moment += String(now.day(), DEC) + " ";
        if (now.hour() < 10) Moment += "0";
        Moment += String(now.hour(), DEC) + ":";
        if (now.minute() < 10) Moment += "0";
        Moment += String(now.minute(), DEC) + " ";

        return Moment;
}

void UserHandler::WriteToLog(String userID, String credit, bool doppelt)
{

        this->_logFile = SD.open("UserLog.txt", FILE_WRITE);

        if (this->_logFile)
        {
                String logLine = "";
                if(doppelt)
                {
                        logLine = GetMoment() + ";" + userID + ";" + credit + ";Doppelt";
                }
                else
                {
                        logLine = GetMoment() + ";" + userID + ";" + credit + ";Einfach";
                }
                Serial.println(logLine);
                this->_logFile.println(logLine);
        }
        else
        {
                Serial.println("Cant write to card");
                while (true) {}
        }
        this->_logFile.close();
}

int UserHandler::ReadCredit()
{
        byte buffer[18];
        byte byteCount;
        int stat = 0;

        byteCount = sizeof(buffer);

        stat = _nfcReader.MIFARE_Read(config.chipPage, buffer, &byteCount);

        int key_AND = (buffer[0] & buffer[1] & buffer[2] & buffer[3]);
        int key_SUM = ((buffer[0] + buffer[1] + buffer[2] + buffer[3]) / 4);


        if (stat == 0 && key_AND == key_SUM)
        {
                return (buffer[0] + buffer[1] + buffer[2] + buffer[3]) / 4;
        }
        else
        {
                return -1;
        }
}

int UserHandler::WriteCredit(int newCredit,bool paymentType)
{

        byte PSWBuff[] = {0xFF, 0xAB, 0xBA, 0xFF};
        byte pACK[] = {0xE, 0x5};
        byte WBuff[] = {newCredit, newCredit, newCredit, newCredit};
        int _stat = 0;

        ReadCredit();
        _nfcReader.PCD_NTAG216_AUTH(&PSWBuff[0], pACK);
        _stat = _nfcReader.MIFARE_Ultralight_Write(config.chipPage, WBuff, 4); // EE-5 KM CP = 0x04 

        if (_stat != 0)
        {
                return -1;
        }

        ReadCredit();

        int i = 0;

        while (!this->HasCardToRead())
        {
                i++;
                if (i>10) break;
        }

        String UserTxT =   String(this->GetCardId().toInt());
        this->User = UserTxT;
        Serial.println("########");
        Serial.println(UserTxT);
        String CreditTxT = String(newCredit,DEC);
        this->WriteToLog(UserTxT.c_str(), CreditTxT.c_str(),paymentType);
        return 0;
}

String UserHandler::getLastUser()
{
        if(this->User != "" && this->User != "0")
        {
                return this->User;
        }
        else
        {
                return "Unbekannt";
        }

}

String UserHandler::ID()
{
        long code = 0;

        _nfcReader.PICC_ReadCardSerial();
        for (byte i = 0; i < _nfcReader.uid.size; i++)
        {
                code = ((code + _nfcReader.uid.uidByte[i]) * 10);
        }
        return String(code, DEC);

}

void UserHandler::newRead()
{
        while (!_nfcReader.PICC_IsNewCardPresent()) {};
}

bool UserHandler::saveConfiguration(int tiSingle, int tiDobule) {


        if (tiSingle < 1000 || tiSingle > 20000 || tiSingle >= tiDobule || tiDobule < 1000 || tiDobule > 40000)
        {return false;}

        Serial.println(tiSingle);

        SD.remove("Config.txt");

        File file = SD.open("Config.txt", FILE_WRITE);
        if (!file) {
                Serial.println(F("Failed to create file"));
                return false;
        }

        StaticJsonDocument<512> doc;

        doc["Zeit_einfach"] = tiSingle;
        doc["Zeit_doppelt"] = tiDobule;
        doc["ServerOn"] = config.ServerOn;
        doc["SSID"] = config.SSID;
        doc["PW"] = config.PW;
        doc["ChipPage"] = config.chipPage;


        if (serializeJson(doc, file) == 0) {
                Serial.println(F("Failed to write to file"));
        }

        file.close();
        return true;
}
