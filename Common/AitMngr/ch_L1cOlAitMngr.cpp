#include "ch_L1cOlAitMngr.hpp"
#include "ch_L1cOlAitInfoBuilder.hpp"
#include "ch_L1cOlAitSendChecker.hpp"
#include "ch_L1cOlAitCmdSender.hpp"

using namespace ch::l1c;

namespace
{
    struct OlAitDeps
    {
        OlAitMngrDb      aitDb[NUM_OF_L1_DOMAIN]{};
        OlAitInfoBuilder infoBuilder{};
        OlAitSendChecker sendChecker{};
        OlAitCmdSender   cmdSender{};
    };

    OlAitDeps& GetOlAitDeps()
    {
        static OlAitDeps deps{};
        return deps;
    }
}

OlAitMngr::OlAitMngr()
    : OlAitMngr(
        GetOlAitDeps().aitDb,
        &GetOlAitDeps().infoBuilder,
        &GetOlAitDeps().sendChecker,
        &GetOlAitDeps().cmdSender,
        &GetOlAitDeps().cmdSender)
{
}

OlAitMngr::OlAitMngr(
    OlAitMngrDb* pAitDb,
    OlAitInfoBuilder* pInfoBuilder,
    OlAitSendChecker* pSendChecker,
    OlAitCmdSender* pDefaultCmdSender,
    IOlAitCmdSender* pCmdSender)
    : m_pAitDb(pAitDb)
    , m_pInfoBuilder(pInfoBuilder)
    , m_pSendChecker(pSendChecker)
    , m_pDefaultCmdSender(pDefaultCmdSender)
    , m_pCmdSender(pCmdSender)
{
}

OlAitMngr::~OlAitMngr() {};

void OlAitMngr::SetCmdSender(IOlAitCmdSender* pCmdSender)
{
    if (pCmdSender == nullptr)
    {
        return;
    }

    m_pCmdSender = pCmdSender;
}

void OlAitMngr::RestoreDefaultCmdSender()
{
    m_pCmdSender = m_pDefaultCmdSender;
}

void OlAitMngr::RequestOlAitUpdate(
    Rat_e rat,
    OlAitUpdateCause_e updateCause,
    u8 domainType)
{
    if (domainType >= NUM_OF_L1_DOMAIN)
    {
        return;
    }

    OlAitMngrDb& aitDb = m_pAitDb[domainType];

    m_pInfoBuilder->RunBuild(rat, updateCause, aitDb, domainType);

    if (m_pSendChecker->CheckSendAction(rat, updateCause, aitDb, domainType) == Drop)
    {
        return;
    }

    if (m_pCmdSender == nullptr)
    {
        return;
    }

    m_pCmdSender->Send_OLAIT_CMD(&aitDb.GetAitInfo());
}

void OlAitMngr::RequestApEventUpdate(u64 apEvent)
{
    for (u8 i = 0; i < NUM_OF_L1_DOMAIN; ++i)
    {
        m_pAitDb[i].SetLatestApEvent(apEvent);
    }
}

void OlAitMngr::NotifyRatResume(Rat_e rat, u8 domainType)
{
    if (domainType >= NUM_OF_L1_DOMAIN)
    {
        return;
    }

    const u8 stack = static_cast<u8>(domainType % 2);
    m_RatActive[static_cast<u8>(rat)][stack] = true;
    StartOlAitTimer(domainType);
}

void OlAitMngr::NotifyRatSuspend(Rat_e rat, u8 domainType)
{
    if (domainType >= NUM_OF_L1_DOMAIN)
    {
        return;
    }

    const u8 stack = static_cast<u8>(domainType % 2);
    m_RatActive[static_cast<u8>(rat)][stack] = false;

    bool anyActive = false;
    for (u8 i = 0; i < NUM_OF_RAT; ++i)
    {
        if (m_RatActive[i][stack] == true)
        {
            anyActive = true;
            break;
        }
    }

    if (anyActive == false)
    {
        StopOlAitTimer(domainType);
    }
}

void OlAitMngr::Reset()
{
    for (u8 i = 0; i < NUM_OF_L1_DOMAIN; ++i)
    {
        m_pAitDb[i] = OlAitMngrDb();
    }

    for (u8 i = 0; i < NUM_OF_RAT; ++i)
    {
        for (u8 j = 0; j < NUM_OF_STACK; ++j)
        {
            m_RatActive[i][j] = false;
        }
    }

    RestoreDefaultCmdSender();
}

void OlAitMngr::ProcPeriodicWeightUpdate(u8 domainType)
{
    (void)domainType;
}

void OlAitMngr::StartOlAitTimer(u8 domainType)
{
    (void)domainType;
}

void OlAitMngr::StopOlAitTimer(u8 domainType)
{
    (void)domainType;
}

olAit_Info& OlAitMngrDb::GetAitInfo()
{
    return m_AitInfo;
}

const olAit_Info& OlAitMngrDb::GetAitInfo() const
{
    return m_AitInfo;
}

void OlAitMngrDb::SetLatestApEvent(u64 apEvent)
{
    m_LatestApEvent = apEvent;
}

u64 OlAitMngrDb::GetLatestApEvent() const
{
    return m_LatestApEvent;
}

void OlAitMngrDb::SetLastSentSignature(u64 signature)
{
    m_LastSentSignature = signature;
}

u64 OlAitMngrDb::GetLastSentSignature() const
{
    return m_LastSentSignature;
}

void OlAitMngrDb::SetCdrxState(AitCdrxState_e state)
{
    m_CdrxState = state;
}

AitCdrxState_e OlAitMngrDb::GetCdrxState() const
{
    return m_CdrxState;
}

void OlAitMngrDb::SetPCellInfoIndex(u8 index)
{
    m_PcellInfoIndex = index;
}

u8 OlAitMngrDb::GetPCellInfoIndex() const
{
    return m_PcellInfoIndex;
}
