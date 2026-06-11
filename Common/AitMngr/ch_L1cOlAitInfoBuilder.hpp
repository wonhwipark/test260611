#pragma once

#include <map>

namespace ch
{
    namespace l1c
    {

        class OlAitInfoBuildBase
        {
        public:
            OlAitInfoBuildBase() = default;
            virtual ~OlAitInfoBuildBase() = default;

            void Execute(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType);

        protected:
            virtual bool CheckEarlyReturn(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType);
            virtual void CollectInput(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType);
            virtual void UpdateAitInfo(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType) = 0;

        protected:
            void BuildCommonSkeleton(Rat_e rat, OlAitMngrDb& aitDb, u8 domainType);
            olAit_db& GetPCellInfo(OlAitMngrDb& aitDb);
            u8 ConvertDomainTypeToStack(u8 domainType) const;
        };

        class OlAitMeasureBuild : public OlAitInfoBuildBase
        {
        public:
            OlAitMeasureBuild() = default;
            ~OlAitMeasureBuild() override = default;

        protected:
            void UpdateAitInfo(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType) override;
        };

        class OlAitWakeupBuild : public OlAitInfoBuildBase
        {
        public:
            OlAitWakeupBuild() = default;
            ~OlAitWakeupBuild() override = default;

        protected:
            bool CheckEarlyReturn(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType) override;
            void UpdateAitInfo(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType) override;
        };

        class OlAitTxSwitchBuild : public OlAitInfoBuildBase
        {
        public:
            OlAitTxSwitchBuild() = default;
            ~OlAitTxSwitchBuild() override = default;

        protected:
            void UpdateAitInfo(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType) override;
        };

        class OlAitApEventBuild : public OlAitInfoBuildBase
        {
        public:
            OlAitApEventBuild() = default;
            ~OlAitApEventBuild() override = default;

        protected:
            void UpdateAitInfo(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType) override;
        };

        class OlAitPeriodicUpdateBuild : public OlAitInfoBuildBase
        {
        public:
            OlAitPeriodicUpdateBuild() = default;
            ~OlAitPeriodicUpdateBuild() override = default;

        protected:
            void UpdateAitInfo(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType) override;
        };

        class OlAitInfoBuilder
        {
        public:
            OlAitInfoBuilder();
            ~OlAitInfoBuilder();

            void RunBuild(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType);

        private:
            void RegisterBuildMap();

        private:
            std::map<OlAitUpdateCause_e, OlAitInfoBuildBase*> m_BuildMap{};
            OlAitMeasureBuild        m_MeasureBuild{};
            OlAitWakeupBuild         m_WakeupBuild{};
            OlAitTxSwitchBuild       m_TxSwitchBuild{};
            OlAitApEventBuild        m_ApEventBuild{};
            OlAitPeriodicUpdateBuild m_PeriodicUpdateBuild{};
        };

    } // namespace l1c
} // namespace ch