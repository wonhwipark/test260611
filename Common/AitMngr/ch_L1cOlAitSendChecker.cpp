#include "ch_L1cOlAitSendChecker.hpp"

using namespace ch::l1c;

AitSendAction_e OlAitSendChecker::CheckSendAction(
    Rat_e rat,
    OlAitUpdateCause_e updateCause,
    OlAitMngrDb& aitDb,
    u8 domainType)
{
    (void)rat;
    (void)updateCause;

    if (IsDuplicatePayload(aitDb, domainType) == true)
    {
        return Drop;
    }

    aitDb.SetLastSentSignature(CalculateAitInfoSignature(aitDb));
    return SendNow;
}

bool OlAitSendChecker::IsDuplicatePayload(OlAitMngrDb& aitDb, u8 domainType)
{
    (void)domainType;
    return (CalculateAitInfoSignature(aitDb) == aitDb.GetLastSentSignature());
}

u64 OlAitSendChecker::CalculateAitInfoSignature(OlAitMngrDb& aitDb)
{
    const olAit_Info& aitInfo = aitDb.GetAitInfo();
    u64 signature = 1469598103934665603ULL;

    signature ^= aitInfo.apEvent;
    signature *= 1099511628211ULL;

    signature ^= aitInfo.ccNum;
    signature *= 1099511628211ULL;

    for (u8 i = 0; i < MAX_CC_NUM; ++i)
    {
        signature ^= aitInfo.aitInfo[i].band;
        signature *= 1099511628211ULL;

        signature ^= aitInfo.aitInfo[i].rxFrequency;
        signature *= 1099511628211ULL;

        signature ^= aitInfo.aitInfo[i].txFrequency;
        signature *= 1099511628211ULL;

        signature ^= aitInfo.aitInfo[i].TxPath;
        signature *= 1099511628211ULL;

        signature ^= aitInfo.aitInfo[i].asEvent;
        signature *= 1099511628211ULL;
    }

    return signature;
}