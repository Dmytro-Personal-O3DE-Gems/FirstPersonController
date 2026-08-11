
#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace FirstPersonController
{
    class FirstPersonRequests
        : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(FirstPersonController::FirstPersonRequests, "{52C9467E-BBAB-4478-B124-23589744B770}");

        // Put your public request methods here.
        
        // Put notification events here. Examples:
        // void RegisterEvent(AZ::EventHandler<...> notifyHandler);
        // AZ::Event<...> m_notifyEvent1;
        
    };

    using FirstPersonRequestBus = AZ::EBus<FirstPersonRequests>;

} // namespace FirstPersonController
