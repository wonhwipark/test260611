#include "gtest/gtest.h"
#include "ch_L1cOlAitSendChecker.hpp"

using namespace ch::l1c;

class OlAitSendCheckerUt : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_AitDb = OlAitMngrDb();

        olAit_Info& aitInfo = m_AitDb.GetAitInfo();
        aitInfo.ccNum = 1;
        aitInfo.aitInfo[0].band = 1;
        aitInfo.aitInfo[0].rxFrequency = 100;
        aitInfo.aitInfo[0].txFrequency = 200;
        aitInfo.aitInfo[0].TxPath = 0;
        aitInfo.aitInfo[0].asEvent = 1;
    }

protected:
    OlAitSendChecker m_Checker{};
    OlAitMngrDb      m_AitDb{};
};

TEST_F(OlAitSendCheckerUt, CheckSendAction_WhenFirstPayload_ThenSendNow)
{
    EXPECT_EQ(SendNow, m_Checker.CheckSendAction(RAT_NR, OLAIT_SCELL_MEASURE, m_AitDb, 0));
}

TEST_F(OlAitSendCheckerUt, CheckSendAction_WhenSamePayloadTwice_ThenSecondIsDrop)
{
    EXPECT_EQ(SendNow, m_Checker.CheckSendAction(RAT_NR, OLAIT_SCELL_MEASURE, m_AitDb, 0));
    EXPECT_EQ(Drop, m_Checker.CheckSendAction(RAT_NR, OLAIT_SCELL_MEASURE, m_AitDb, 0));
}

TEST_F(OlAitSendCheckerUt, CheckSendAction_WhenPayloadChanges_ThenSendNowAgain)
{
    EXPECT_EQ(SendNow, m_Checker.CheckSendAction(RAT_NR, OLAIT_SCELL_MEASURE, m_AitDb, 0));

    m_AitDb.GetAitInfo().aitInfo[0].TxPath = 1;

    EXPECT_EQ(SendNow, m_Checker.CheckSendAction(RAT_NR, OLAIT_SCELL_MEASURE, m_AitDb, 0));
}