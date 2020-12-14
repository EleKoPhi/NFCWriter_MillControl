#ifndef CONTROLLER_h
#define CONTROLLER_h

#include "Arduino.h"
#include "UserHandler.h"
#include "Drawer.h"
#include <WiFi101.h>
#include <SPI.h>



class Controller
{
public:
Controller(int chipSelect, int slaveSelect, int rstPin,int clk, int data);

void Begin();
char StateBegin(char state);

void SetCurrentStatus(char stat);
char StateTransitions();
char &GetCurrentStatus();
void UpDateTime();
void States(char Status);
bool TimeOut(unsigned long time);
bool TimeOutWithBackPay(unsigned long time);
void Reset();

void MillOn();
void MillOff();

// &Getter and Setter
void SetCurrentKeyFlag(char key);
char &GetCurrentKeyFlag();
void SetOldKeyFlag(char key);
char &GetOldKeyFlag();
void SetStartTime(unsigned long Time);
unsigned long  &GetStartTime();
void SetTimeSingle(unsigned long time);
unsigned long &GetTimeSingle();
void SetTimeDouble(unsigned long time);
unsigned long &GetTimeDouble();
void SetTimeRemaning(unsigned long time);
unsigned long &GetTimeRemaning();
void SetTimePassed(unsigned long time);
unsigned long &GetTimePassed();
void SetTimeDelta(unsigned long time);
unsigned long &GetTimeDelta();
void SetTimeStopBegin(unsigned long time);
unsigned long &GetTimeStopBegin();
void SetTimeInStop(unsigned long time);
unsigned long &GetTimeInStop();
void SetCurrentUser(String userID);
String &GetCurrentUser();
UserHandler &GetUserHandler();
Drawer &GetDrawer();
int &GetCreditAsInt();
void SetCreditAsInt(int credit);
int &GetUserAsInt();
void SetUserAsInt(int user);

private:

Drawer _drawer;
UserHandler _userHandler;

char _currentStatus;
String _currentUser;
String _currentUserId;

String _additionalUser;
String _additionalUserId;

int user;
int credit;
char oldKey;
char key;

unsigned long T_einfach;
unsigned long T_doppelt;
unsigned long T_rest;
String LastUser;

unsigned long _startTime, _currentTime, _deltaTime, _timeInStopState, _stopBegin, _passedtime;
};


#endif
