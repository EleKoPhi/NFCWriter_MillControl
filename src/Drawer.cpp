#include "Arduino.h"
#include "Drawer.h"
#include <U8g2lib.h>

Drawer::Drawer(int clk, int data) : _display(U8G2_R0, clk, data, U8X8_PIN_NONE)
{
        _display.begin();
        this->screensafer_x = _display.getWidth()/2;
        this->screensafer_y = _display.getHeight()/2;
        this->size = 5;
}

void Drawer::Clear()
{
        _display.clearBuffer();
        _display.sendBuffer();
}

void Drawer::DrawMain()
{

        int pos = 0;

        while(pos <= 64)
        {
                _display.clearBuffer();
                _display.setFont(u8g2_font_ncenB14_tr);
                _display.setDrawColor(1);
                this->DrawCenter("Km nEXt", 28);
                _display.setDrawColor(0);
                _display.drawBox(0,0,64-pos,32);
                _display.drawBox(64+pos,0,64-pos,32);
                _display.sendBuffer();
                _display.setDrawColor(1);
                pos = pos + 4;
        }
        while(pos > 0) {pos = pos - 4;}
}

void Drawer::DrawErr(bool sdStatus, bool nfcStatus, bool rtcStatus)
{
        _display.clearBuffer();
        _display.setFont(u8g2_font_ncenB08_tr);

        if (!sdStatus)
        {
                this->DrawCenter(" SD - Err", 10);
        }
        else
        {
                this->DrawCenter("SD    - i.O.", 10);
        }

        if (!nfcStatus)
        {
                this->DrawCenter("NFC - Err", 20);
        }
        else
        {
                this->DrawCenter("NFC - i.O.", 20);
        }
        if (!rtcStatus)
        {
                this->DrawCenter("RTC - Err", 30);
        }
        else
        {
                this->DrawCenter("RTC - i.O.", 30);
        }

        if (!sdStatus |!nfcStatus | !rtcStatus)
        {
                _display.sendBuffer();
                while (true);
        }
}

void Drawer::DrawLastUser(String lastUser)
{
        _display.clearBuffer();
        _display.setFont(u8g2_font_ncenB08_tr);
        this->DrawCenter("Letzte ID...", 8);
        _display.setFont(u8g2_font_ncenB12_tr);
        this->DrawCenter(lastUser, 30);
        _display.sendBuffer();
}

void Drawer::DisplayProgress(int progress)
{

        _display.clearBuffer();
        _display.setFont(u8g2_font_ncenB10_tr);
        this->DrawCenter("Zubereitung",14);

        for(int i=0; i<=progress/10; i++)
        {
                _display.drawBox(i*13+1, 21, 10, 10);
        }
        _display.sendBuffer();
}

void Drawer::DrawUnknown()
{
        _display.clearBuffer();
        _display.setFont(u8g2_font_ncenB08_tr);
        this->DrawCenter("Unbekannter Benutzer!",8);
        this->DrawCenter("Registrierung bei...",20);
        this->DrawCenter("Werner Schunn",32);
        _display.sendBuffer();
}

void Drawer::DrawCenter(String txt, int y)
{
        char pos = (128 - _display.getStrWidth(txt.c_str()))/2;
        _display.drawStr(pos, y, txt.c_str());
}

void Drawer::DrawWaitForUser()
{
        _display.clearBuffer();
        _display.setFont(u8g2_font_ncenB08_tr);
        this->DrawCenter("Bitte waehlen", 8);
        _display.setFont(u8g2_font_ncenB12_tr);
        this->DrawCenter("<- 1x  |  2x ->", 30);
        _display.sendBuffer();
}

void Drawer::DrawPayOne()
{
        _display.clearBuffer();
        _display.setFont(u8g2_font_ncenB08_tr);
        this->DrawCenter("Preis 1 Credit", 8);
        _display.setFont(u8g2_font_ncenB08_tr);
        this->DrawCenter("Bitte Karte auflegen", 30);
        _display.sendBuffer();

}

void Drawer::DrawSplitQ2()
{
        _display.clearBuffer();
        _display.setFont(u8g2_font_ncenB08_tr);
        this->DrawCenter("Split?", 8);
        _display.setFont(u8g2_font_ncenB12_tr);
        this->DrawCenter("<- Nein | Ja ->", 30);
        _display.sendBuffer();
}

void Drawer::DrawPay2 ()
{
        _display.clearBuffer();
        _display.setFont(u8g2_font_ncenB08_tr);
        this->DrawCenter("Preis 2 Credits", 8);
        _display.setFont(u8g2_font_ncenB08_tr);
        this->DrawCenter("Bitte Karte auflegen", 30);
        _display.sendBuffer();

}

void Drawer::DrawPay2_1()
{
        _display.clearBuffer();
        _display.setFont(u8g2_font_ncenB08_tr);
        this->DrawCenter("0/2 Bezahlt !", 12);
        _display.setFont(u8g2_font_ncenB08_tr);
        this->DrawCenter("1. Karte auflegen", 28);
        _display.sendBuffer();

}

void Drawer::DrawPay2_2()
{
        _display.clearBuffer();
        _display.setFont(u8g2_font_ncenB08_tr);
        this->DrawCenter("1/2 Bezahlt !", 12);
        _display.setFont(u8g2_font_ncenB08_tr);
        this->DrawCenter("2. Karte auflegen", 28);
        _display.sendBuffer();
}

void Drawer::DrawLowCredit()
{
        _display.clearBuffer();
        _display.setFont(u8g2_font_ncenB12_tr);
        this->DrawCenter("Kein Guthaben ", 12);
        _display.setFont(u8g2_font_ncenB12_tr);
        this->DrawCenter("mehr !", 30);
        _display.sendBuffer();
}

void Drawer::DrawReplay(int progress)
{
        _display.clearBuffer();
        _display.setFont(u8g2_font_ncenB10_tr);
        this->DrawCenter("Karte Auflegen", 10);
        _display.setFont(u8g2_font_ncenB10_tr);
        this->DrawCenter("1x Gutschrift", 32);
        _display.drawLine(0,_display.getHeight()/2+1, _display.getWidth()*((progress)*0.01),_display.getHeight()/2+1);
        _display.drawLine(_display.getWidth(),_display.getHeight()/2-1, (_display.getWidth()-_display.getWidth()*((progress)*0.01)),_display.getHeight()/2-1);
        _display.sendBuffer();
}

void Drawer::DrawCredit(int ID, int Credit)
{
        _display.clearBuffer();
        _display.setFont(u8g2_font_ncenB10_tr);

        String IDs = "ID: " + String(ID);
        this->DrawCenter(IDs, 12);
        _display.setFont(u8g2_font_ncenB12_tr);
        String Cred = "Guthaben: " + String(Credit);
        this->DrawCenter(Cred, 30);
        _display.sendBuffer();

}

void Drawer::DrawDoneState()
{
        _display.clearBuffer();
        _display.setFont(u8g2_font_ncenB12_tr);
        this->DrawCenter("i.O.", 22);
        _display.sendBuffer();
}

void Drawer::DrawScreenSafer(int time)
{
        _display.clearBuffer();
        if (time-this->_oldTime>1000)
        {
                this->size = random(0,_display.getHeight()/2-1);
                this->screensafer_x = random(this->size, _display.getWidth()-this->size);
                this->screensafer_y = random(this->size, _display.getHeight()-this->size);
                this->_oldTime = time;
        }
        _display.drawCircle(this->screensafer_x, this->screensafer_y,this->size);
        _display.sendBuffer();
}

void Drawer::Drawer::Err()
{
        _display.clearBuffer();
        _display.setFont(u8g2_font_ncenB10_tr);
        this->DrawCenter("Err", 15);
        _display.sendBuffer();
}

void Drawer::DrawFreeState()
{
        _display.clearBuffer();
        _display.setFont(u8g2_font_ncenB08_tr);
        this->DrawCenter("EX-ES-2", 12);
        this->DrawCenter("gibt aus!", 28);
        _display.sendBuffer();
}

void Drawer::DrawStopState()
{
        _display.clearBuffer();
        _display.setFont(u8g2_font_ncenB08_tr);
        this->DrawCenter("<- Abbruch | Weiter ->", 24);
        _display.sendBuffer();
}
