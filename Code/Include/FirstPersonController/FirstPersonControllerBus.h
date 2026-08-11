
#pragma once

#include <FirstPersonController/FirstPersonControllerTypeIds.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace FirstPersonController
{
    class FirstPersonControllerRequests
    {
    public:
        AZ_RTTI(FirstPersonControllerRequests, FirstPersonControllerRequestsTypeId);
        virtual ~FirstPersonControllerRequests() = default;
        // Put your public methods here
    };

    class FirstPersonControllerBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using FirstPersonControllerRequestBus = AZ::EBus<FirstPersonControllerRequests, FirstPersonControllerBusTraits>;
    using FirstPersonControllerInterface = AZ::Interface<FirstPersonControllerRequests>;

} // namespace FirstPersonController
