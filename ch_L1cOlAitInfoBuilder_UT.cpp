#include "gtest/gtest.h"
#include "ch_L1cOlAitInfoBuilder.hpp"

using namespace ch::l1c;

class OlAitInfoBuilderUt : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_AitDb = OlAitMngrDb();
    }

    olAit_db& GetPCell()
    {
        return m_AitDb.GetAitInfo().aitInfo[m_AitDb.GetPCellInfoIndex()];
    }

protected:
    OlAitInfoBuilder m_Builder{};
    OlAitMngrDb      m_AitDb{};
};

TEST_F(OlAitInfoBuilderUt, RunBuild_WhenScellMeasure_ThenBuildsMeasureInfo)
{
    m_Builder.RunBuild(RAT_NR, OLAIT_SCELL_MEASURE, m_AitDb, 0);

    const olAit_Info& aitInfo = m_AitDb.GetAitInfo();
    const olAit_db& pCell = aitInfo.aitInfo[0];

    EXPECT_EQ(1, aitInfo.ccNum);
    EXPECT_EQ(RAT_NR, pCell.rat);
    EXPECT_EQ(0, pCell.stack);
    EXPECT_EQ(1u, pCell.asEvent);
    EXPECT_EQ(90u, pCell.rsrp);
}

TEST_F(OlAitInfoBuilderUt, RunBuild_WhenIratMeasure_ThenBuildsIratMeasureInfo)
{
    m_Builder.RunBuild(RAT_LTE, OLAIT_IRAT_MEASURE, m_AitDb, 1);

    const olAit_db& pCell = GetPCell();

    EXPECT_EQ(RAT_LTE, pCell.rat);
    EXPECT_EQ(1, pCell.stack);
    EXPECT_EQ(2u, pCell.asEvent);
    EXPECT_EQ(80u, pCell.rsrp);
}

TEST_F(OlAitInfoBuilderUt, RunBuild_WhenDrxWakeup_ThenUpdatesWakeupInfo)
{
    m_Builder.RunBuild(RAT_NR, OLAIT_DRX_WAKEUP, m_AitDb, 2);

    const olAit_db& pCell = GetPCell();

    EXPECT_EQ(3u, pCell.asEvent);
    EXPECT_EQ(0u, pCell.TxPath);
    EXPECT_EQ(RxPathUpdated, m_AitDb.GetCdrxState());
}

TEST_F(OlAitInfoBuilderUt, RunBuild_WhenCdrxWakeup_ThenUpdatesTxPathAndState)
{
    m_Builder.RunBuild(RAT_NR, OLAIT_CDRX_WAKEUP, m_AitDb, 3);

    const olAit_db& pCell = GetPCell();

    EXPECT_EQ(4u, pCell.asEvent);
    EXPECT_EQ(1u, pCell.TxPath);
    EXPECT_EQ(TxPathUpdated, m_AitDb.GetCdrxState());
}

TEST_F(OlAitInfoBuilderUt, RunBuild_WhenApEvent_ThenReflectsLatestApEvent)
{
    m_AitDb.SetLatestApEvent(0x12345678ULL);

    m_Builder.RunBuild(RAT_NR, OLAIT_AP_EVENT, m_AitDb, 0);

    EXPECT_EQ(0x12345678ULL, m_AitDb.GetAitInfo().apEvent);
    EXPECT_EQ(6u, GetPCell().asEvent);
}