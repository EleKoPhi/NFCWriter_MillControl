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

void States(char Status);
char StateBegin(char state);
char StateTransitions();

void UpDateTime();

bool TimeOut(unsigned long time);
bool TimeOutWithBackPay(unsigned long time);

void Reset();

void ProcessInput();

// State transitions

char tr_WaitForUser();
char tr_EnterKey();
char tr_SelectToToAdapt();
char tr_ShowCredit();
char tr_Screensafer();
char tr_Single();
char tr_Dobule();
char tr_StopState();
char tr_FinishState();
char tr_FreePullState();
char tr_AskForSplitPayment();
char tr_ReadCreditUser();
char tr_AdaptTiSingle();
char tr_AdaptTiDouble();
char tr_PayOne();
char tr_PayTwo();
char tr_PayTwo_1();
char tr_PayTwo_2();
char tr_RepayState();
char tr_DoneState();

// Activation of mill

void MillOn();
void MillOff();

// &Getter and Setter

UserHandler &GetUserHandler();
Drawer &GetDrawer();
WDTZero &GetWatchDog();

char &GetCurrentStatus();
void SetCurrentStatus(char stat);

void SetCurrentKeyFlag(char key);
char GetCurrentKeyFlag();

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

unsigned long &GetTimer100ms();
void SetTimer100ms(unsigned long ti);

unsigned long &GetTimer50ms();
void SetTimer50ms(unsigned long ti);

unsigned long &GetDeltaTiLeft();
void SetDeltaTiLeft(unsigned long ti);

unsigned long &GetDeltaTiBoth();
void SetDeltaTiBoth(unsigned long ti);

int &GetLocalKey();
void SetLocalKey(int key);

int &GetActiveKeyElement();
void SetActiveKeyElement(int element);

int &GetLocalKeyDisplayed();
void SetLocalKeyDisplayed(int key);

int &GetActiveKeyElementDisplayed();
void SetActiveKeyElementDispayed(int element);

bool &GetUpdateDisplay();
void SetUpdateDisplay(bool st);

int &GetDisplayedProgress();
void SetDisplayedProgress(int progress);

int &GetProgress();
void SetProgress(int prog);


private:

Drawer _drawer;
UserHandler _userHandler;
WDTZero _watchDog;

String _additionalUser;
String _additionalUserId;
String _currentUser;
String _currentUserId;
String LastUser;

bool updateDisplay;

char _currentStatus;
char oldKey;
char key;
char temp_oldKey;

int localKey = 0;
int activeKeyElement = 0;
int localKey_displayed = 0;
int activeKeyElement_displayed = 0;
int progress = 0; 
int progress_temp = 0;
int user;
int credit;
int count_left;
int count_right;

unsigned long T_einfach;
unsigned long T_doppelt;
unsigned long T_rest;
unsigned long _startTime;
unsigned long _currentTime;
unsigned long _deltaTime;
unsigned long _timeInStopState;
unsigned long _stopBegin;
unsigned long _passedtime;
unsigned long _ti_l_down; 
unsigned long _ti_r_down;
unsigned long _delta_ti_l_down;
unsigned long _delta_ti_r_down;
unsigned long _ti_both_down;
unsigned long _delta_ti_both_down;
unsigned long timer100ms;
unsigned long timer10ms;

};


#endif
