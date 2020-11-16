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
void PutCurrentStatus(char stat);
char StateTransitions();
char GetCurrentStatus();
void UpDateTime();
void States(char Status);
bool TimeOut(int time);
bool TimeOutWithBackPay(int time);
void Reset();
char StateBegin(char state);
void MillOn();
void MillOff();

String GetCurrentUser();
void SetCurrentUser(String user);

int status = WL_IDLE_STATUS;

int T_einfach;
int T_doppelt;
int T_rest;
String LastUser;

private:

char ssid[16] = "Km nEXt Service";
char pass[16] = "ESEX2servicePW";
int keyIndex = 0;
Drawer _drawer;
UserHandler _userHandler;
//WebServer _wifi;
char _currentStatus;

String _currentUser;
String _currentUserId;

String _additionalUser;
String _additionalUserId;

int user;
int credit;
char oldKey;
char key;

unsigned long _startTime, _currentTime, _deltaTime, _timeInStopState, _stopBegin, _passedtime;
};


#endif
