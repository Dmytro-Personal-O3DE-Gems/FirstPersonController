
#include "FirstPersonControllerSystemComponent.h"

#include <FirstPersonController/FirstPersonControllerTypeIds.h>

#include <AzCore/Serialization/SerializeContext.h>

namespace FirstPersonController
{
    AZ_COMPONENT_IMPL(FirstPersonControllerSystemComponent, "FirstPersonControllerSystemComponent",
        FirstPersonControllerSystemComponentTypeId);

    void FirstPersonControllerSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<FirstPersonControllerSystemComponent, AZ::Component>()
                ->Version(0)
                ;
        }
    }

    void FirstPersonControllerSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("FirstPersonControllerService"));
    }

    void FirstPersonControllerSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("FirstPersonControllerService"));
    }

    void FirstPersonControllerSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void FirstPersonControllerSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    FirstPersonControllerSystemComponent::FirstPersonControllerSystemComponent()
    {
        if (FirstPersonControllerInterface::Get() == nullptr)
        {
            FirstPersonControllerInterface::Register(this);
        }
    }

    FirstPersonControllerSystemComponent::~FirstPersonControllerSystemComponent()
    {
        if (FirstPersonControllerInterface::Get() == this)
        {
            FirstPersonControllerInterface::Unregister(this);
        }
    }

    void FirstPersonControllerSystemComponent::Init()
    {
    }

    void FirstPersonControllerSystemComponent::Activate()
    {
        FirstPersonControllerRequestBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();
    }

    void FirstPersonControllerSystemComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        FirstPersonControllerRequestBus::Handler::BusDisconnect();
    }

    void FirstPersonControllerSystemComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
    }

} // namespace FirstPersonController
