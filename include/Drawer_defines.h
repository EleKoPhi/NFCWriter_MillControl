#ifndef UserHandler_defines
#define UserHandler_defines

#define UNDERSCORE "_"
#define UPDATETIME 1000
#define STEPSIZE 4
#define KEY_LENGTH 5

#ifdef SIXTYFOUR
#define TARGETFLIPMODE 0

#define SAFTERINITIALSIZE 5

#define STARTANIMATION 0

#define SPACE " "

#define BITTEWAEHLEN_y 21
#define BITTEWAEHLEN_txt "Bitte waehlen"
#define BITTEWAEHLEN_font u8g2_font_logisoso16_tr
#define ARROWS_y 59
#define ARROWS_txt "1x | 2x"
#define ARROWS_font u8g2_font_logisoso20_tr

#define STATE_font u8g2_font_logisoso16_tr
#define SDSTATE_y 16
#define NFCSTATE_y 33

#define SDERROR_txt "SD  - ERR"
#define SD_Ok_txt "SD  - _Ok "
#define NFCERROR_txt "NFC - ERR"
#define NFC_Ok_txt "NFC - _Ok "

#define LASTID_txt "Letzte ID"
#define LASTID_y 28
#define LASTID_font u8g2_font_logisoso16_tr
#define LASTID_value_y 52
#define LASTID_value_font u8g2_font_logisoso16_tr

#define ZUBEREITUNG_txt "Zubereitung"
#define ZUBEREITUNG_y 28
#define ZUBEREITUNG_font u8g2_font_logisoso16_tr
#define PROGRESS_division 10
#define PROGRESS_height 16
#define PROGRESS_width 10
#define PROGRESS_y 40

#define UNKNOWNID_font u8g2_font_logisoso16_tr
#define UNKNOWNID_txt "ID unbekannt"
#define UNKNOWNID_y 40
#define UNKNOWNIDFILL_txt "############"
#define UNKNOWNIDFILL_y1 19
#define UNKNOWNIDFILL_y2 61

#define PAYONE_txt "Preis 1 Credit"
#define PAYONE_y 28
#define PAYONE_font u8g2_font_logisoso16_tr

#define ADDCARD_txt "Chip auflegen"
#define ADDCARD_y 52
#define ADDCARD_font u8g2_font_logisoso16_tr

#define SPLITQ_txt "Split?"
#define SPLITQ_y 26
#define SPLITQ_font u8g2_font_logisoso16_tr
#define SPLITQARROWS_txt "N | J"
#define SPLITQARROWS_y 54
#define SPLITQARRWOS_font u8g2_font_logisoso20_tr

#define PAY2_txt "Preis 2 Credit"
#define PAY2_y 28
#define PAY2_font u8g2_font_logisoso16_tr

#define PAY2_1_txt "0/2 Bezahlt"
#define PAY2_1_y 28
#define PAY2_1_font u8g2_font_logisoso16_tr
#define ADD1CARD_txt 52
#define ADD1CARD_y "Chip auflegen"
#define ADD1CARD_font u8g2_font_logisoso16_tr

#define PAY2_2_txt "1/2 Bezahlt"
#define PAY2_2_y 28
#define PAY2_2_font u8g2_font_logisoso16_tr
#define ADD2CARD_txt "Chip auflegen"
#define ADD2CARD_y 52
#define ADD2CARD_font u8g2_font_logisoso16_tr

#define LOWCREDIT1_txt "Kein Guthaben"
#define LOWCREDIT1_y 28
#define LOWCREDIT1_font u8g2_font_logisoso16_tr
#define LOWCREDIT2_txt "mehr !"
#define LOWCREDIT2_y 52
#define LOWCREDIT2_font u8g2_font_logisoso16_tr

#define REPAY1_txt "Chip auflegen"
#define REPAY1_y 28
#define REPAY1_font u8g2_font_logisoso16_tr
#define REPAY2_txt "1x Gutschrift"
#define REPAY2_y 52
#define REPAY2_font u8g2_font_logisoso16_tr

#define ID_txt "ID: "
#define ID_y 28
#define ID_font u8g2_font_logisoso16_tr
#define CREDITS_txt "Cred.: "
#define CREDITS_y 52
#define CREDITS_font u8g2_font_logisoso16_tr

#define DONE_txt "i.O."
#define DONE_y 44
#define DONE_font u8g2_font_logisoso24_tr

#define ERR_txt "Err"
#define ERR_y 44
#define ERR_font u8g2_font_logisoso24_tr

#define FREE1_txt "EX"
#define FREE2_txt "gibt aus!"
#define FREE1_y 28
#define FREE2_y 54
#define FREE_font u8g2_font_logisoso16_tr

#define STOP1_txt "<- Abbruch"
#define STOP1_y 28
#define STOP2_txt "Weiter ->"
#define STOP2_y 52
#define STOP_font u8g2_font_logisoso16_tr

#define NVM_ERROR_txt_1 "Config Error"
#define NVM_ERROR_1_y 28
#define NVM_ERROR_txt_2 "<- Weiter ->"
#define NVM_ERROR_2_y 52
#define NVM_Error_font u8g2_font_logisoso16_tr

#define KMNEXT_txt "Km nEXt"
#define KMNEXT_y 44
#define KMNEXT_font u8g2_font_logisoso24_tr

#define KEYINPUT_txt "Passwort"
#define KEYINPUT_y 21
#define KEYINPUT_font u8g2_font_logisoso16_tr
#define KEY_SELECTOR_y 52
#define KEY_FONT u8g2_font_logisoso24_tr
#define KEY_y 49

#define EINFACH_txt "<- Einfach"
#define EINFACH_y 28
#define DOPPELT_txt "Doppelt ->"
#define DOPPELT_y 52
#define TIMESELECT_font u8g2_font_logisoso16_tr

#define TIME_y 20
#define TIME_font u8g2_font_logisoso16_tr
#define TIME_SINGLE_txt "Zeit Einfach"
#define TIME_DOUBLE_txt "Zeit Doppelt"
#define TIME_VALUE_y 52

#else
#define TARGETFLIPMODE 0

#define SAFTERINITIALSIZE 5

#define STARTANIMATION 0

#define SPACE "  "

#define BITTEWAEHLEN_y 8
#define BITTEWAEHLEN_txt "Bitte waehlen"
#define BITTEWAEHLEN_font u8g2_font_ncenB08_tr
#define ARROWS_y 30
#define ARROWS_txt "<- 1x  |  2x ->"
#define ARROWS_font u8g2_font_ncenB12_tr

#define STATE_font u8g2_font_ncenB08_tr
#define SDSTATE_y 10
#define NFCSTATE_y 20

#define SDERROR_txt "SD  - ERR"
#define SD_Ok_txt "SD  - _Ok "
#define NFCERROR_txt "NFC - ERR"
#define NFC_Ok_txt "NFC - _Ok "

#define LASTID_txt "Letzte ID"
#define LASTID_y 8
#define LASTID_font u8g2_font_ncenB08_tr
#define LASTID_value_y 30
#define LASTID_value_font u8g2_font_ncenB12_tr

#define ZUBEREITUNG_txt "Zubereitung"
#define ZUBEREITUNG_y 14
#define ZUBEREITUNG_font u8g2_font_ncenB10_tr
#define PROGRESS_division 10
#define PROGRESS_height 10
#define PROGRESS_width 10
#define PROGRESS_y 21

#define UNKNOWNID_font u8g2_font_ncenB08_tr
#define UNKNOWNID_txt "ID unbekannt"
#define UNKNOWNID_y 20
#define UNKNOWNIDFILL_txt "############"
#define UNKNOWNIDFILL_y1 8
#define UNKNOWNIDFILL_y2 32

#define PAYONE_txt "Preis 1 Credit"
#define PAYONE_y 8
#define PAYONE_font u8g2_font_ncenB08_tr

#define ADDCARD_txt "Bitte Karte auflegen"
#define ADDCARD_y 30
#define ADDCARD_font u8g2_font_ncenB08_tr

#define SPLITQ_txt "Split?"
#define SPLITQ_y 8
#define SPLITQ_font u8g2_font_ncenB08_tr
#define SPLITQARROWS_txt "<- Nein | Ja ->"
#define SPLITQARROWS_y 30
#define SPLITQARRWOS_font u8g2_font_ncenB12_tr

#define PAY2_txt "Preis 2 Credits"
#define PAY2_y 8
#define PAY2_font u8g2_font_ncenB08_tr

#define PAY2_1_txt "0/2 Bezahlt !"
#define PAY2_1_y 12
#define PAY2_1_font u8g2_font_ncenB08_tr
#define ADD1CARD_txt 28
#define ADD1CARD_y "1. Karte auflegen"
#define ADD1CARD_font u8g2_font_ncenB08_tr

#define PAY2_2_txt "1/2 Bezahlt !"
#define PAY2_2_y 12
#define PAY2_2_font u8g2_font_ncenB08_tr
#define ADD2CARD_txt "2. Karte auflegen"
#define ADD2CARD_y 28
#define ADD2CARD_font u8g2_font_ncenB08_tr

#define LOWCREDIT1_txt "Kein Guthaben"
#define LOWCREDIT1_y 12
#define LOWCREDIT1_font u8g2_font_ncenB12_tr
#define LOWCREDIT2_txt "mehr !"
#define LOWCREDIT2_y 30
#define LOWCREDIT2_font u8g2_font_ncenB12_tr

#define REPAY1_txt "Karte Auflegen"
#define REPAY1_y 10
#define REPAY1_font u8g2_font_ncenB10_tr
#define REPAY2_txt "1x Gutschrift"
#define REPAY2_y 32
#define REPAY2_font u8g2_font_ncenB10_tr

#define ID_txt "ID: "
#define ID_y 12
#define ID_font u8g2_font_ncenB10_tr
#define CREDITS_txt "Guthaben: "
#define CREDITS_y 30
#define CREDITS_font u8g2_font_ncenB12_tr

#define DONE_txt "i.O."
#define DONE_y 22
#define DONE_font u8g2_font_ncenB12_tr

#define ERR_txt "Err"
#define ERR_y 15
#define ERR_font u8g2_font_ncenB10_tr

#define FREE1_txt "EX-ES-2"
#define FREE2_txt "gibt aus!"
#define FREE1_y 12
#define FREE2_y 28
#define FREE_font u8g2_font_ncenB08_tr

#define STOP1_txt "<- Abbruch | Weiter ->"
#define STOP1_y 24
#define STOP2_txt ""
#define STOP2_y 0
#define STOP_font u8g2_font_ncenB08_tr

#define KMNEXT_txt "Km nEXt"
#define KMNEXT_y 28
#define KMNEXT_font u8g2_font_ncenB14_tr

#define KEYINPUT_txt "Enter Key"
#define KEYINPUT_y 12
#define KEYINPUT_font u8g2_font_ncenB12_tr
#define KEY_SELECTOR_y 28
#define KEY_FONT u8g2_font_ncenB12_tr
#define KEY_y 26

#define EINFACH_txt " "
#define EINFACH_y 0
#define DOPPELT_txt "<- 1x  |  2x ->"
#define DOPPELT_y 22
#define TIMESELECT_font u8g2_font_ncenB12_tr

#define TIME_y 12
#define TIME_font u8g2_font_ncenB12_tr
#define TIME_SINGLE_txt "Zeit Einfach"
#define TIME_DOUBLE_txt "Zeit Doppelt"
#define TIME_VALUE_y 30
/* TODO */

#endif
#endif