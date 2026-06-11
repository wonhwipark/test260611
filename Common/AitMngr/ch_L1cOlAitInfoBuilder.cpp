#include "ch_L1cOlAitInfoBuilder.hpp"

using namespace ch::l1c;

void OlAitInfoBuildBase::Execute(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType)
{
    if (CheckEarlyReturn(rat, updateCause, aitDb, domainType) == true)
    {
        return;
    }

    CollectInput(rat, updateCause, aitDb, domainType);
    UpdateAitInfo(rat, updateCause, aitDb, domainType);
}

bool OlAitInfoBuildBase::CheckEarlyReturn(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType)
{
    (void)rat;
    (void)updateCause;
    (void)aitDb;
    (void)domainType;
    return false;
}

void OlAitInfoBuildBase::CollectInput(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType)
{
    (void)updateCause;
    BuildCommonSkeleton(rat, aitDb, domainType);
}

void OlAitInfoBuildBase::BuildCommonSkeleton(Rat_e rat, OlAitMngrDb& aitDb, u8 domainType)
{
    olAit_Info& aitInfo = aitDb.GetAitInfo();

    aitInfo.ccNum = 1;

    olAit_db& pCell = aitInfo.aitInfo[0];
    pCell.rat = rat;
    pCell.band = 1;
    pCell.stack = ConvertDomainTypeToStack(domainType);
    pCell.rxFrequency = 100;
    pCell.txFrequency = 200;
    pCell.rxBandWidth = 20;
    pCell.txBandWidth = 20;
    pCell.rxDiversityMode = 0;
    pCell.isVoice = 0;

    aitDb.SetPCellInfoIndex(0);
}

olAit_db& OlAitInfoBuildBase::GetPCellInfo(OlAitMngrDb& aitDb)
{
    return aitDb.GetAitInfo().aitInfo[aitDb.GetPCellInfoIndex()];
}

u8 OlAitInfoBuildBase::ConvertDomainTypeToStack(u8 domainType) const
{
    return static_cast<u8>(domainType % 2);
}

void OlAitMeasureBuild::UpdateAitInfo(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType)
{
    (void)rat;
    (void)domainType;

    olAit_db& pCell = GetPCellInfo(aitDb);

    if (updateCause == OLAIT_SCELL_MEASURE)
    {
        pCell.asEvent = 1;
        pCell.rsrp = 90;
    }
    else if (updateCause == OLAIT_IRAT_MEASURE)
    {
        pCell.asEvent = 2;
        pCell.rsrp = 80;
    }
}

bool OlAitWakeupBuild::CheckEarlyReturn(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType)
{
    (void)rat;
    (void)domainType;

    if (updateCause == OLAIT_CDRX_WAKEUP)
    {
        if (aitDb.GetCdrxState() == TxPathUpdated)
        {
            return true;
        }
    }

    return false;
}

void OlAitWakeupBuild::UpdateAitInfo(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType)
{
    (void)rat;
    (void)domainType;

    olAit_db& pCell = GetPCellInfo(aitDb);

    if (updateCause == OLAIT_DRX_WAKEUP)
    {
        pCell.asEvent = 3;
        pCell.TxPath = 0;
        aitDb.SetCdrxState(RxPathUpdated);
    }
    else if (updateCause == OLAIT_CDRX_WAKEUP)
    {
        pCell.asEvent = 4;
        pCell.TxPath = 1;
        aitDb.SetCdrxState(TxPathUpdated);
    }
}

void OlAitTxSwitchBuild::UpdateAitInfo(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType)
{
    (void)rat;
    (void)updateCause;
    (void)domainType;

    olAit_db& pCell = GetPCellInfo(aitDb);
    pCell.asEvent = 5;
    pCell.TxPath = (pCell.TxPath == 0) ? 1 : 0;
}

void OlAitApEventBuild::UpdateAitInfo(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType)
{
    (void)rat;
    (void)updateCause;
    (void)domainType;

    aitDb.GetAitInfo().apEvent = aitDb.GetLatestApEvent();

    olAit_db& pCell = GetPCellInfo(aitDb);
    pCell.asEvent = 6;
}

void OlAitPeriodicUpdateBuild::UpdateAitInfo(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType)
{
    (void)rat;
    (void)updateCause;
    (void)domainType;

    olAit_db& pCell = GetPCellInfo(aitDb);
    pCell.asEvent = 7;
    pCell.rsrp = 70;
    pCell.ulBler = 5;
    pCell.txPower = 23;
}

OlAitInfoBuilder::OlAitInfoBuilder()
{
    RegisterBuildMap();
}

OlAitInfoBuilder::~OlAitInfoBuilder() = default;

void OlAitInfoBuilder::RegisterBuildMap()
{
    m_BuildMap[OLAIT_SCELL_MEASURE] = &m_MeasureBuild;
    m_BuildMap[OLAIT_IRAT_MEASURE] = &m_MeasureBuild;
    m_BuildMap[OLAIT_DRX_WAKEUP] = &m_WakeupBuild;
    m_BuildMap[OLAIT_CDRX_WAKEUP] = &m_WakeupBuild;
    m_BuildMap[OLAIT_TX_SWITCH] = &m_TxSwitchBuild;
    m_BuildMap[OLAIT_AP_EVENT] = &m_ApEventBuild;
    m_BuildMap[OLAIT_PERIODIC_UPDATE] = &m_PeriodicUpdateBuild;
}

void OlAitInfoBuilder::RunBuild(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType)
{
    std::map<OlAitUpdateCause_e, OlAitInfoBuildBase*>::iterator it = m_BuildMap.find(updateCause);
    if (it == m_BuildMap.end() || it->second == nullptr)
    {
        return;
    }

    it->second->Execute(rat, updateCause, aitDb, domainType);
}