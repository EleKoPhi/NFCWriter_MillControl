#ifndef CONTROLLER_h
#define CONTROLLER_h

#include "Arduino.h"
#include "UserHandler.h"
#include "Drawer.h"
#include <WiFi101.h>
#include <SPI.h>
#include <WDTZero.h>



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
void SetTempKeyFlag(char key);
char &GetTempKeyFlag();
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
WDTZero &GetWatchDog();
int &GetCreditAsInt();
void SetCreditAsInt(int credit);
int &GetUserAsInt();
void SetUserAsInt(int user);

unsigned long &GetTiRight();
void SetTiRight(unsigned long ti);
unsigned long &GetTiLeft();
void SetTiLeft(unsigned long ti);
unsigned long &GetTiBoth();
void SetTiBoth(unsigned long ti);

unsigned long &GetDeltaTiRight();
void SetDeltaTiRight(unsigned long ti);
unsigned long &GetDeltaTiLeft();
void SetDeltaTiLeft(unsigned long ti);
unsigned long &GetDeltaTiBoth();
void SetDeltaTiBoth(unsigned long ti);

int &GetLocalKey();
void SetLocalKey(int key);
int &GetActiveKeyElement();
void SetActiveKeyElement(int element);


private:

Drawer _drawer;
UserHandler _userHandler;
WDTZero _watchDog;

char _currentStatus;
String _currentUser;
String _currentUserId;

String _additionalUser;
String _additionalUserId;

int localKey = 0;
int activeKeyElement = 0;

int user;
int credit;
char oldKey;
char key;
char temp_oldKey;

int count_left;
int count_right;

unsigned long T_einfach;
unsigned long T_doppelt;
unsigned long T_rest;
String LastUser;

unsigned long _startTime, _currentTime, _deltaTime, _timeInStopState,
 _stopBegin, _passedtime, _ti_l_down, _ti_r_down, _delta_ti_l_down, _delta_ti_r_down,
 _ti_both_down, _delta_ti_both_down;
};


#endif
