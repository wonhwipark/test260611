#include "ch_L1cOlAitCmdSender.hpp"

using namespace ch::l1c;

void OlAitCmdSender::Send_OLAIT_CMD(olAit_Info* pAitInfo)
{
    if (pAitInfo == nullptr)
    {
        return;
    }

    OLAIT_CMD_T cmd = {};
    Build_OLAIT_CMD(cmd, pAitInfo);

    // TODO: actual HAL / RF send here
    (void)cmd;
}

void OlAitCmdSender::Build_OLAIT_CMD(OLAIT_CMD_T& cmd, olAit_Info* pAitInfo)
{
    cmd.pAitInfo = pAitInfo;
}