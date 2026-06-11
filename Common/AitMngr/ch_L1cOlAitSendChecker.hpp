#pragma once


namespace ch
{
    namespace l1c
    {

        class OlAitSendChecker
        {
        public:
            OlAitSendChecker() = default;
            ~OlAitSendChecker() = default;

            AitSendAction_e CheckSendAction(Rat_e rat, OlAitUpdateCause_e updateCause, OlAitMngrDb& aitDb, u8 domainType);

        private:
            bool IsDuplicatePayload(OlAitMngrDb& aitDb, u8 domainType);
            u64 CalculateAitInfoSignature(OlAitMngrDb& aitDb);
        };

    } // namespace l1c
} // namespace ch