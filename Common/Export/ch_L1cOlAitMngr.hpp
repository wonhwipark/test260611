#pragma once

namespace ch
{
    namespace l1c
    {

        using u8 = unsigned char;
        using u32 = unsigned int;
        using u64 = unsigned long long;
        using s32 = int;

        constexpr u8 NUM_OF_L1_DOMAIN = 4;
        constexpr u8 NUM_OF_STACK = 2;
        constexpr u8 MAX_CC_NUM = 8;
        constexpr u8 NUM_OF_RAT = 2;

        typedef enum
        {
            RAT_LTE = 0,
            RAT_NR,
            RAT_MAX
        } Rat_e;

        typedef enum
        {
            OLAIT_SCELL_MEASURE = 0,
            OLAIT_IRAT_MEASURE,
            OLAIT_DRX_WAKEUP,
            OLAIT_CDRX_WAKEUP,
            OLAIT_TX_SWITCH,
            OLAIT_AP_EVENT,
            OLAIT_PERIODIC_UPDATE,
            OLAIT_MAX
        } OlAitUpdateCause_e;

        typedef enum
        {
            SendNow = 0,
            Drop
        } AitSendAction_e;

        typedef enum
        {
            Idle = 0,
            RxPathUpdated,
            TxPathUpdated
        } AitCdrxState_e;

        struct ObserverPathInfo
        {
            bool  valid{};
            Rat_e rat{ RAT_LTE };
            u32   band{ 0 };
            u8    stack{ 0 };
            u32   rxFrequency{ 0 };
            u32   txFrequency{ 0 };
            u8    rxBandWidth{ 0 };
            u8    txBandWidth{ 0 };
            u8    rxDiversityMode{ 0 };
            u8    isVoice{ 0 };
        };

        struct olAit_db
        {
            Rat_e rat{ RAT_LTE };
            u32   band{ 0 };
            u8    stack{ 0 };
            u32   rxFrequency{ 0 };
            u32   txFrequency{ 0 };
            u8    rxBandWidth{ 0 };
            u8    txBandWidth{ 0 };
            u8    rxDiversityMode{ 0 };
            u8    TxPath{ 0 };
            u8    asEvent{ 0 };
            u8    isVoice{ 0 };
            u8    isRach{ 0 };
            u32   rsrp{ 0 };
            u32   ulBler{ 0 };
            u32   txPower{ 0 };
        };

        struct olAit_Info
        {
            u64 apEvent{ 0 };
            u8  ccNum{ 0 };
            olAit_db aitInfo[MAX_CC_NUM]{};
        };

        struct OLAIT_CMD_T
        {
            olAit_Info* pAitInfo{ nullptr };
        };

        class OlAitMngrDb
        {
        public:
            OlAitMngrDb() = default;
            ~OlAitMngrDb() = default;

            olAit_Info& GetAitInfo();
            const olAit_Info& GetAitInfo() const;

            void SetLatestApEvent(u64 apEvent);
            u64 GetLatestApEvent() const;

            void SetLastSentSignature(u64 signature);
            u64 GetLastSentSignature() const;

            void SetCdrxState(AitCdrxState_e state);
            AitCdrxState_e GetCdrxState() const;

            void SetPCellInfoIndex(u8 index);
            u8 GetPCellInfoIndex() const;

        private:
            olAit_Info     m_AitInfo{};
            u64            m_LatestApEvent{ 0 };
            u64            m_LastSentSignature{ 0 };
            AitCdrxState_e m_CdrxState{ Idle };
            u8             m_PcellInfoIndex{ 0 };
        };

        class IOlAitCmdSender;
        class OlAitInfoBuilder;
        class OlAitSendChecker;
        class OlAitCmdSender;
        class OlAitMngr;

#define GET_OLAIT() (ch::l1c::OlAitMngr::Instance())

        class OlAitMngr
        {
        public:
            static OlAitMngr& Instance()
            {
                static OlAitMngr instance;
                return instance;
            }

            OlAitMngr(const OlAitMngr&) = delete;
            OlAitMngr& operator=(const OlAitMngr&) = delete;

            void SetCmdSender(IOlAitCmdSender* pCmdSender);
            void RestoreDefaultCmdSender();

            void RequestOlAitUpdate(Rat_e rat, OlAitUpdateCause_e updateCause, u8 domainType);
            void RequestApEventUpdate(u64 apEvent);
            void NotifyRatResume(Rat_e rat, u8 domainType);
            void NotifyRatSuspend(Rat_e rat, u8 domainType);
            void Reset();

        private:
            OlAitMngr();
            OlAitMngr(
                OlAitMngrDb* pAitDb,
                OlAitInfoBuilder* pInfoBuilder,
                OlAitSendChecker* pSendChecker,
                OlAitCmdSender* pDefaultCmdSender,
                IOlAitCmdSender* pCmdSender);
            ~OlAitMngr();

            void ProcPeriodicWeightUpdate(u8 domainType);
            void StartOlAitTimer(u8 domainType);
            void StopOlAitTimer(u8 domainType);

        private:
            OlAitMngrDb* m_pAitDb;
            OlAitInfoBuilder* m_pInfoBuilder;
            OlAitSendChecker* m_pSendChecker;
            OlAitCmdSender* m_pDefaultCmdSender;
            IOlAitCmdSender* m_pCmdSender;
            bool              m_RatActive[NUM_OF_RAT][NUM_OF_STACK]{};
        };

    } // namespace l1c
} // namespace ch
