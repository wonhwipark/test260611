#include "gtest/gtest.h"
#include "ch_L1cOlAitMngr.hpp"
#include "ch_L1cOlAitCmdSender.hpp"

using namespace ch::l1c;

class FakeOlAitCmdSender : public IOlAitCmdSender
{
public:
    FakeOlAitCmdSender() = default;
    ~FakeOlAitCmdSender() override = default;

    void Send_OLAIT_CMD(olAit_Info* pAitInfo) override
    {
        wasSent = true;

        if (pAitInfo != nullptr)
        {
            lastInfo = *pAitInfo;
        }
    }

public:
    bool wasSent{ false };
    olAit_Info lastInfo{};
};

class OlAitMngrUt : public ::testing::Test
{
protected:
    void SetUp() override
    {
        GET_OLAIT().Reset();
        GET_OLAIT().RestoreDefaultCmdSender();
    }

    void TearDown() override
    {
        GET_OLAIT().RestoreDefaultCmdSender();
        GET_OLAIT().Reset();
    }
};

TEST_F(OlAitMngrUt, RequestOlAitUpdate_WhenScellMeasure_ThenCommandSendInputMatches)
{
    FakeOlAitCmdSender fakeSender;

    GET_OLAIT().SetCmdSender(&fakeSender);
    GET_OLAIT().RequestOlAitUpdate(RAT_NR, OLAIT_SCELL_MEASURE, 0);

    EXPECT_EQ(true, fakeSender.wasSent);
    EXPECT_EQ(1, fakeSender.lastInfo.ccNum);
    EXPECT_EQ(RAT_NR, fakeSender.lastInfo.aitInfo[0].rat);
    EXPECT_EQ(0, fakeSender.lastInfo.aitInfo[0].stack);
    EXPECT_EQ(1u, fakeSender.lastInfo.aitInfo[0].asEvent);
    EXPECT_EQ(90u, fakeSender.lastInfo.aitInfo[0].rsrp);
}

TEST_F(OlAitMngrUt, RequestOlAitUpdate_WhenApEvent_ThenCommandSendInputContainsApEvent)
{
    FakeOlAitCmdSender fakeSender;

    GET_OLAIT().SetCmdSender(&fakeSender);
    GET_OLAIT().RequestApEventUpdate(0x1234ULL);
    GET_OLAIT().RequestOlAitUpdate(RAT_NR, OLAIT_AP_EVENT, 0);

    EXPECT_EQ(true, fakeSender.wasSent);
    EXPECT_EQ(0x1234ULL, fakeSender.lastInfo.apEvent);
    EXPECT_EQ(6u, fakeSender.lastInfo.aitInfo[0].asEvent);
}

TEST_F(OlAitMngrUt, RequestOlAitUpdate_WhenDomainTypeInvalid_ThenCommandIsNotSent)
{
    FakeOlAitCmdSender fakeSender;

    GET_OLAIT().SetCmdSender(&fakeSender);
    GET_OLAIT().RequestOlAitUpdate(RAT_NR, OLAIT_SCELL_MEASURE, 9);

    EXPECT_EQ(false, fakeSender.wasSent);
}

TEST_F(OlAitMngrUt, RequestOlAitUpdate_WhenSamePayloadTwice_ThenSecondIsDropped)
{
    FakeOlAitCmdSender fakeSender;

    GET_OLAIT().SetCmdSender(&fakeSender);
    GET_OLAIT().RequestOlAitUpdate(RAT_NR, OLAIT_SCELL_MEASURE, 0);

    EXPECT_EQ(true, fakeSender.wasSent);

    fakeSender.wasSent = false;
    fakeSender.lastInfo = olAit_Info();

    GET_OLAIT().RequestOlAitUpdate(RAT_NR, OLAIT_SCELL_MEASURE, 0);

    EXPECT_EQ(false, fakeSender.wasSent);
}

TEST_F(OlAitMngrUt, RequestOlAitUpdate_WhenPeriodicUpdate_ThenCommandSendInputMatches)
{
    FakeOlAitCmdSender fakeSender;

    GET_OLAIT().SetCmdSender(&fakeSender);
    GET_OLAIT().RequestOlAitUpdate(RAT_LTE, OLAIT_PERIODIC_UPDATE, 1);

    EXPECT_EQ(true, fakeSender.wasSent);
    EXPECT_EQ(RAT_LTE, fakeSender.lastInfo.aitInfo[0].rat);
    EXPECT_EQ(1, fakeSender.lastInfo.aitInfo[0].stack);
    EXPECT_EQ(7u, fakeSender.lastInfo.aitInfo[0].asEvent);
    EXPECT_EQ(70u, fakeSender.lastInfo.aitInfo[0].rsrp);
    EXPECT_EQ(5u, fakeSender.lastInfo.aitInfo[0].ulBler);
    EXPECT_EQ(23u, fakeSender.lastInfo.aitInfo[0].txPower);
}