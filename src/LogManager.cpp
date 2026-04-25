#include "LogManager.h"

// ---------------------------------------------------------------------------
// Append-only transaction log on LittleFS
//
// Flash-wear analysis (10-year target, 100 tx/day, 10.000 erase cycles):
//   - Each 10-byte entry is APPENDED into the already-erased current block.
//     No erase is needed until the block (4 KB) is full.
//   - 4096 / 10 = 409 entries per 4 KB block  → 1 block erase every ~4 days.
//   - Data-block erases over 10 years: ~880 total.
//   - Spread across 64 blocks (256 KB partition): ~14 erases/block → safe.
//   - Metadata pair (2 blocks): ~1350 erases/block over 10 years → safe.
//
// Contrast with in-place update (COW): every write forces a full block
// re-read + re-write → 200 erases/day → 11.400 erases/block in 10 years
// → exceeds the 10.000-cycle limit on a 256 KB partition.
// ---------------------------------------------------------------------------

bool LogManager::begin()
{
    _ready = LittleFS.begin(true);
    return _ready;
}

uint8_t LogManager::calcCrc(const LogEntry &e)
{
    uint8_t crc = 0;
    const uint8_t *p = reinterpret_cast<const uint8_t *>(&e);
    for (int i = 0; i < CARDLOG_ENTRY_SIZE - 2; i++)   // exclude crc + pad
        crc ^= p[i];
    return crc;
}

uint8_t LogManager::calcStateCrc(const CardStateEntry &e)
{
    uint8_t crc = 0;
    for (int i = 0; i < 7; i++)
        crc ^= e.uid[i];
    crc ^= e.credit_after;
    return crc;
}

void LogManager::upsertCardState(const uint8_t uid[7], uint8_t latestCredit)
{
    if (!_ready)
        return;

    File f = LittleFS.open(CARDSTATE_FILE, "r+");
    if (!f)
        f = LittleFS.open(CARDSTATE_FILE, "w+");
    if (!f)
        return;

    CardStateEntry state;
    memset(&state, 0, sizeof(state));
    memcpy(state.uid, uid, 7);
    state.credit_after = latestCredit;
    state.crc = calcStateCrc(state);

    CardStateEntry existing;
    size_t pos = 0;
    while (f.read(reinterpret_cast<uint8_t *>(&existing), CARDSTATE_ENTRY_SIZE) == CARDSTATE_ENTRY_SIZE)
    {
        if (calcStateCrc(existing) != existing.crc)
        {
            pos += CARDSTATE_ENTRY_SIZE;
            continue;
        }
        if (memcmp(existing.uid, uid, 7) == 0)
        {
            f.seek(pos, SeekSet);
            f.write(reinterpret_cast<const uint8_t *>(&state), CARDSTATE_ENTRY_SIZE);
            f.close();
            return;
        }
        pos += CARDSTATE_ENTRY_SIZE;
    }

    f.seek(0, SeekEnd);
    f.write(reinterpret_cast<const uint8_t *>(&state), CARDSTATE_ENTRY_SIZE);
    f.close();
}

void LogManager::applyLatestCredits(CardSummary *out, uint16_t cardCount) const
{
    if (!_ready || cardCount == 0 || !LittleFS.exists(CARDSTATE_FILE))
        return;

    File f = LittleFS.open(CARDSTATE_FILE, "r");
    if (!f)
        return;

    CardStateEntry state;
    while (f.read(reinterpret_cast<uint8_t *>(&state), CARDSTATE_ENTRY_SIZE) == CARDSTATE_ENTRY_SIZE)
    {
        if (calcStateCrc(state) != state.crc)
            continue;

        for (uint16_t i = 0; i < cardCount; i++)
        {
            if (memcmp(out[i].uid, state.uid, 7) == 0)
            {
                out[i].latest_credit = (int16_t)state.credit_after;
                break;
            }
        }
    }
    f.close();
}

void LogManager::append(const uint8_t uid[7], int8_t delta, int16_t latestCredit)
{
    if (!_ready)
        return;

    LogEntry e;
    memset(&e, 0, sizeof(e));
    memcpy(e.uid, uid, 7);
    e.delta = delta;
    e.crc   = calcCrc(e);
    e.pad   = 0;

    // Open in append mode: seeks to EOF, writes into remaining space
    // of the last flash block without requiring an erase operation.
    File f = LittleFS.open(CARDLOG_FILE, "a");
    if (!f)
        return;
    f.write(reinterpret_cast<const uint8_t *>(&e), CARDLOG_ENTRY_SIZE);
    f.close();

    if (latestCredit >= 0 && latestCredit <= 255)
        upsertCardState(uid, (uint8_t)latestCredit);
}

uint32_t LogManager::count() const
{
    if (!_ready || !LittleFS.exists(CARDLOG_FILE))
        return 0;
    File f = LittleFS.open(CARDLOG_FILE, "r");
    if (!f)
        return 0;
    uint32_t c = f.size() / CARDLOG_ENTRY_SIZE;
    f.close();
    return c;
}

uint16_t LogManager::aggregate(CardSummary *out, uint16_t maxCards, bool *truncated) const
{
    if (!_ready || !LittleFS.exists(CARDLOG_FILE) || maxCards == 0)
        return 0;

    if (truncated)
        *truncated = false;

    File f = LittleFS.open(CARDLOG_FILE, "r");
    if (!f)
        return 0;

    uint16_t cardCount = 0;
    LogEntry e;

    while (f.read(reinterpret_cast<uint8_t *>(&e), CARDLOG_ENTRY_SIZE) == CARDLOG_ENTRY_SIZE)
    {
        // Validate CRC
        if (calcCrc(e) != e.crc)
            continue;

        // Find existing card in summary array
        int idx = -1;
        for (int i = 0; i < (int)cardCount; i++)
        {
            if (memcmp(out[i].uid, e.uid, 7) == 0)
            {
                idx = i;
                break;
            }
        }

        // New card – add to summary
        if (idx < 0)
        {
            if (cardCount >= maxCards)
            {
                if (truncated)
                    *truncated = true;
                continue;
            }
            idx = (int)cardCount++;
            memset(&out[idx], 0, sizeof(CardSummary));
            memcpy(out[idx].uid, e.uid, 7);
            out[idx].latest_credit = -1;
        }

        if (e.delta < 0)
        {
            uint16_t abs_d = (uint16_t)(-(int16_t)e.delta);
            if ((uint32_t)out[idx].credits_debited + abs_d <= 65535u)
                out[idx].credits_debited += abs_d;
        }
        else if (e.delta > 0)
        {
            if ((uint32_t)out[idx].refunds_given + (uint8_t)e.delta <= 65535u)
                out[idx].refunds_given += (uint8_t)e.delta;
        }
    }

    f.close();
    applyLatestCredits(out, cardCount);
    return cardCount;
}

void LogManager::clear()
{
    if (!_ready)
        return;
    LittleFS.remove(CARDLOG_FILE);
    LittleFS.remove(CARDSTATE_FILE);
}
