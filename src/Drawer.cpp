#include "Arduino.h"
#include "Drawer.h"
#include <U8g2lib.h>
#include "Drawer_defines.h"
#include "MillWiFi.h"

#ifdef SIXTYFOUR
U8G2_SSD1306_128X64_NONAME_F_SW_I2C &Drawer::GetDisplay() { return Screen; }
#else
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C &Drawer::GetDisplay() { return Screen; }
#endif

Drawer::Drawer(int clk, int data) : Screen(U8G2_R0, clk, data, U8X8_PIN_NONE), initialized(false)
{
}

void Drawer::Begin()
{
        if (initialized)
        {
                return;
        }

        GetDisplay().begin();
        GetDisplay().setFlipMode(TARGETFLIPMODE);
        SetScreensaverX(GetDisplay().getWidth() / 2);
        SetScreensaverY(GetDisplay().getHeight() / 2);
        SetScreensaverSize(SAFTERINITIALSIZE);
        initialized = true;
}

int Drawer::GetScreensaverSize() { return size; }
void Drawer::SetScreensaverSize(int Size) { size = Size; }
int Drawer::GetScreensaverX() { return screensafer_x; }
void Drawer::SetScreensaverX(int X) { screensafer_x = X; }
int Drawer::GetScreensaverY() { return screensafer_y; }
void Drawer::SetScreensaverY(int Y) { screensafer_y = Y; }
int Drawer::GetOldTime() { return oldTime; }
void Drawer::SetOldTime(int time) { oldTime = time; }

void Drawer::DrawClearDisplay()
{
        GetDisplay().clearBuffer();
        SendBuffer();
}

void Drawer::DrawCenteredText(String txt, int y)
{
        char _pos = (GetDisplay().getWidth() - GetDisplay().getStrWidth(txt.c_str())) / 2;
        GetDisplay().drawStr(_pos, y, txt.c_str());
}

void Drawer::DrawApActiveIndicator()
{
        uint16_t width = MillWiFi::getInstance().getActiveBarPixels(GetDisplay().getWidth());
        if (width == 0)
        {
                return;
        }

        int y = GetDisplay().getHeight() - 1;
        GetDisplay().drawHLine(0, y, width);
}

void Drawer::SendBuffer()
{
        DrawApActiveIndicator();
        GetDisplay().sendBuffer();
}

void Drawer::DrawScreenSafer(int time)
{
        GetDisplay().clearBuffer();
        if (time - GetOldTime() > UPDATETIME)
        {
                SetScreensaverSize(random(0, GetDisplay().getHeight() / 2 - 1));
                SetScreensaverX(random(GetScreensaverSize(), GetDisplay().getWidth() - GetScreensaverSize()));
                SetScreensaverY(random(GetScreensaverSize(), GetDisplay().getHeight() - GetScreensaverSize()));
                SetOldTime(time);
        }
        GetDisplay().drawCircle(GetScreensaverX(), GetScreensaverY(), GetScreensaverSize());
        SendBuffer();
}

void Drawer::DrawStartUpAnimation()
{

        int pos = 0;

        while (pos <= GetDisplay().getWidth() && STARTANIMATION)
        {
                GetDisplay().clearBuffer();
                GetDisplay().setFont(KMNEXT_font);
                GetDisplay().setDrawColor(1);
                DrawCenteredText(KMNEXT_txt, KMNEXT_y);
                GetDisplay().setDrawColor(0);
                GetDisplay().drawBox(0, 0, GetDisplay().getWidth() - pos, GetDisplay().getWidth() / 2);
                GetDisplay().drawBox(GetDisplay().getWidth() + pos, 0, GetDisplay().getWidth() - pos, GetDisplay().getWidth() / 2);
                SendBuffer();
                GetDisplay().setDrawColor(1);
                pos = pos + STEPSIZE;
        }
        while (pos > 0)
        {
                pos = pos - STEPSIZE;
        }
}

void Drawer::DrawSystemStatus(bool nfcStatus)
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(STATE_font);

        if (!nfcStatus)
        {
                DrawCenteredText(NFCERROR_txt, NFCSTATE_y);
        }
        else
        {
                DrawCenteredText(NFC_Ok_txt, NFCSTATE_y);
        }
}

void Drawer::DrawLastUser(String lastUser)
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(LASTID_font);
        DrawCenteredText(LASTID_txt, LASTID_y);
        GetDisplay().setFont(LASTID_value_font);
        DrawCenteredText(lastUser, LASTID_value_y);
        SendBuffer();
}

void Drawer::DrawProgress(int progress)
{

        GetDisplay().clearBuffer();
        GetDisplay().setFont(ZUBEREITUNG_font);
        DrawCenteredText(ZUBEREITUNG_txt, ZUBEREITUNG_y);

        for (int i = 0; i <= progress / PROGRESS_division; i++)
        {
                GetDisplay().drawBox(i * 13 + 1, PROGRESS_y, PROGRESS_width, PROGRESS_height);
        }
        SendBuffer();
}

void Drawer::DrawUnknown()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(UNKNOWNID_font);
        DrawCenteredText(UNKNOWNIDFILL_txt, UNKNOWNIDFILL_y1);
        DrawCenteredText(UNKNOWNID_txt, UNKNOWNID_y);
        DrawCenteredText(UNKNOWNIDFILL_txt, UNKNOWNIDFILL_y2);
        SendBuffer();
}

void Drawer::DrawWaitForUser()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(BITTEWAEHLEN_font);
        DrawCenteredText(BITTEWAEHLEN_txt, BITTEWAEHLEN_y);
        GetDisplay().setFont(ARROWS_font);
        DrawCenteredText(ARROWS_txt, ARROWS_y);
        SendBuffer();
}

void Drawer::DrawPayOne()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(PAYONE_font);
        DrawCenteredText(PAYONE_txt, PAYONE_y);
        GetDisplay().setFont(ADDCARD_font);
        DrawCenteredText(ADDCARD_txt, ADDCARD_y);
        SendBuffer();
}

void Drawer::DrawSplitQuestion()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(SPLITQ_font);
        DrawCenteredText(SPLITQ_txt, SPLITQ_y);
        GetDisplay().setFont(SPLITQARRWOS_font);
        DrawCenteredText(SPLITQARROWS_txt, SPLITQARROWS_y);
        SendBuffer();
}

void Drawer::DrawPayTwo()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(PAY2_font);
        DrawCenteredText(PAY2_txt, PAY2_y);
        GetDisplay().setFont(ADDCARD_font);
        DrawCenteredText(ADDCARD_txt, ADDCARD_y);
        SendBuffer();
}

void Drawer::DrawPayTwo_First()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(PAY2_1_font);
        DrawCenteredText(PAY2_1_txt, PAY2_1_y);
        GetDisplay().setFont(ADD1CARD_font);
        DrawCenteredText(ADD1CARD_y, ADD1CARD_txt);
        SendBuffer();
}

void Drawer::DrawPayTwo_Second()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(PAY2_2_font);
        DrawCenteredText(PAY2_2_txt, PAY2_2_y);
        GetDisplay().setFont(ADD2CARD_font);
        DrawCenteredText(ADD2CARD_txt, ADD2CARD_y);
        SendBuffer();
}

void Drawer::DrawLowCredit()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(LOWCREDIT1_font);
        DrawCenteredText(LOWCREDIT1_txt, LOWCREDIT1_y);
        GetDisplay().setFont(LOWCREDIT2_font);
        DrawCenteredText(LOWCREDIT2_txt, LOWCREDIT2_y);
        SendBuffer();
}

void Drawer::DrawReplay(int progress)
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(REPAY1_font);
        DrawCenteredText(REPAY1_txt, REPAY1_y);
        GetDisplay().setFont(REPAY2_font);
        DrawCenteredText(REPAY2_txt, REPAY2_y);
        GetDisplay().drawLine(0, GetDisplay().getHeight() / 2 + 1, GetDisplay().getWidth() * ((progress) * 0.01), GetDisplay().getHeight() / 2 + 1);
        GetDisplay().drawLine(GetDisplay().getWidth(), GetDisplay().getHeight() / 2 - 1, (GetDisplay().getWidth() - GetDisplay().getWidth() * ((progress) * 0.01)), GetDisplay().getHeight() / 2 - 1);
        SendBuffer();
}

void Drawer::DrawCredit(int ID, int Credit)
{
        String _id = ID_txt + String(ID);
        String _credit = CREDITS_txt + String(Credit);

        GetDisplay().clearBuffer();
        GetDisplay().setFont(ID_font);
        DrawCenteredText(_id, ID_y);
        GetDisplay().setFont(CREDITS_font);
        DrawCenteredText(_credit, CREDITS_y);
        SendBuffer();
}

void Drawer::DrawDoneState()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(DONE_font);
        DrawCenteredText(DONE_txt, DONE_y);
        SendBuffer();
}

void Drawer::Drawer::DrawSystemError()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(ERR_font);
        DrawCenteredText(ERR_txt, ERR_y);
        SendBuffer();
}

void Drawer::DrawFreeState()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(FREE_font);
        DrawCenteredText(FREE1_txt, FREE1_y);
        DrawCenteredText(FREE2_txt, FREE2_y);
        SendBuffer();
}

void Drawer::DrawStopState()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(STOP_font);
        DrawCenteredText(STOP1_txt, STOP1_y);
        DrawCenteredText(STOP2_txt, STOP2_y);
        SendBuffer();
}

void Drawer::DrawNvmErrorState()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(NVM_Error_font);
        DrawCenteredText(NVM_ERROR_txt_1, NVM_ERROR_1_y);
        DrawCenteredText(NVM_ERROR_txt_2, NVM_ERROR_2_y);
        SendBuffer();
}

void Drawer::DrawKeyInput(int actualKey, int activeKeyElement)
{
        GetDisplay().clearBuffer();

        String _selector = "";
        String _key = "";

        for (int i = 0; i < KEY_LENGTH; i++)
        {
                if (activeKeyElement == i)
                {
                        _selector = _selector + UNDERSCORE;
                }
                else
                {
                        _selector = _selector + SPACE;
                }
                _key = _key + String(actualKey >> i & 0x01);
        }

        GetDisplay().setFont(KEYINPUT_font);
        DrawCenteredText(KEYINPUT_txt, KEYINPUT_y);
        GetDisplay().setFont(KEY_FONT);
        DrawCenteredText(_key, KEY_y);
        DrawCenteredText(_selector, KEY_SELECTOR_y);
        SendBuffer();
}

void Drawer::DrawSelectTime()
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(TIMESELECT_font);
        DrawCenteredText(DOPPELT_txt, DOPPELT_y);
        DrawCenteredText(EINFACH_txt, EINFACH_y);
        SendBuffer();
}

void Drawer::DrawTime(int time, bool type)
{
        GetDisplay().clearBuffer();
        GetDisplay().setFont(TIME_font);

        if (type)
        {
                DrawCenteredText(TIME_DOUBLE_txt, TIME_y);
        }
        else
        {
                DrawCenteredText(TIME_SINGLE_txt, TIME_y);
        }

        DrawCenteredText(String(time) + String(" ms"), TIME_VALUE_y);
        SendBuffer();
}
