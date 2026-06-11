#pragma once


namespace ch
{
    namespace l1c
    {

        class IOlAitCmdSender
        {
        public:
            IOlAitCmdSender() = default;
            virtual ~IOlAitCmdSender() = default;

            virtual void Send_OLAIT_CMD(olAit_Info* pAitInfo) = 0;
        };

        class OlAitCmdSender : public IOlAitCmdSender
        {
        public:
            OlAitCmdSender() = default;
            ~OlAitCmdSender() override = default;

            void Send_OLAIT_CMD(olAit_Info* pAitInfo) override;

        private:
            void Build_OLAIT_CMD(OLAIT_CMD_T& cmd, olAit_Info* pAitInfo);
        };

    } // namespace l1c
} // namespace ch