#ifndef LogManager_h
#define LogManager_h

#include <Arduino.h>
#include <LittleFS.h>

#define CARDLOG_FILE       "/txlog.bin"
#define CARDSTATE_FILE     "/cardstate.bin"
#define CARDLOG_MAGIC      0x4E464343UL
#define CARDLOG_ENTRY_SIZE 10
#define CARDSTATE_ENTRY_SIZE 9

// Each transaction is APPENDED as one entry (append-only, no in-place updates).
// A 4 KB flash block holds 409 entries and is erased exactly once per fill cycle.
// Aggregation (credits debited / refunds per card) is computed at read-out time.
struct __attribute__((packed)) LogEntry
{
    uint8_t uid[7];  // Raw NFC UID
    int8_t  delta;   // Credit change: -2 / -1 = debit, +1 = refund
    uint8_t crc;     // XOR checksum over bytes 0–8
    uint8_t pad;     // Padding to 10 bytes
};

// Aggregated per-card summary produced at read-out time (lives only in RAM)
struct CardSummary
{
    uint8_t  uid[7];
    uint16_t credits_debited;
    uint16_t refunds_given;
    int16_t  latest_credit;
};

struct __attribute__((packed)) CardStateEntry
{
    uint8_t uid[7];
    uint8_t credit_after;
    uint8_t crc;
};

class LogManager
{
public:
    static LogManager &getInstance()
    {
        static LogManager instance;
        return instance;
    }

    bool     begin();
    void     append(const uint8_t uid[7], int8_t delta, int16_t latestCredit = -1);
    uint32_t count() const;
    uint16_t aggregate(CardSummary *out, uint16_t maxCards, bool *truncated = nullptr) const;
    void     clear();

private:
    LogManager() : _ready(false) {}
    LogManager(const LogManager &) = delete;
    LogManager &operator=(const LogManager &) = delete;

    bool _ready;

    static uint8_t calcCrc(const LogEntry &e);
    static uint8_t calcStateCrc(const CardStateEntry &e);
    void upsertCardState(const uint8_t uid[7], uint8_t latestCredit);
    void applyLatestCredits(CardSummary *out, uint16_t cardCount) const;
};

#endif
