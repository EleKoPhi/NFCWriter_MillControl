#ifndef Drawer_h
#define Drawer_h

#include "Arduino.h"
#include <U8g2lib.h>

#define DEBUG

class Drawer
{
public:

Drawer(int clk, int data);

void DrawClearDisplay();
void DrawCenteredText(String txt, int y);

void DrawStartUpAnimation();
void DrawSystemStatus(bool sdStatus, bool nfcStatus, bool rtcStatus);
void DrawProgress(int progress);
void DrawReplay(int progress);
void DrawDoneState();
void DrawScreenSafer(int time);
void DrawLastUser(String lastUser);
void DrawWaitForUser();
void DrawSplitQuestion();
void DrawSelectTime();
void DrawPayOne();
void DrawPayTwo ();
void DrawPayTwo_First();
void DrawPayTwo_Second();
void DrawUnknown();
void DrawTime (int Time, bool type);
void DrawLowCredit();
void DrawCredit(int ID, int Credit);
void DrawSystemError();
void DrawFreeState();
void DrawStopState();
void DrawKeyInput(int actualKey, int activeKeyElement);

int GetScreensaverSize();
void SetScreensaverSize(int Size);
int GetScreensaverX();
void SetScreensaverX(int Size);
int GetScreensaverY();
void SetScreensaverY(int Size);
int GetOldTime();
void SetOldTime(int time);

#ifdef SIXTYFOUR
    U8G2_SSD1306_128X64_NONAME_F_SW_I2C &GetDisplay();
#else
    U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C &GetDisplay();
#endif


private:

#ifdef SIXTYFOUR
    U8G2_SSD1306_128X64_NONAME_F_SW_I2C Screen;
#else
    U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C Screen;
#endif

int size;
int oldTime;
int screensafer_x;
int screensafer_y;

};

#endif
