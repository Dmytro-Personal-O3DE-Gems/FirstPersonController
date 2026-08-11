
#include "FirstPersonControllerModuleInterface.h"
#include <AzCore/Memory/Memory.h>

#include <FirstPersonController/FirstPersonControllerTypeIds.h>

#include <Clients/FirstPersonControllerSystemComponent.h>

#include "FirstPersonController/FirstPersonComponent.h"

namespace FirstPersonController
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(FirstPersonControllerModuleInterface,
        "FirstPersonControllerModuleInterface", FirstPersonControllerModuleInterfaceTypeId);
    AZ_RTTI_NO_TYPE_INFO_IMPL(FirstPersonControllerModuleInterface, AZ::Module);
    AZ_CLASS_ALLOCATOR_IMPL(FirstPersonControllerModuleInterface, AZ::SystemAllocator);

    FirstPersonControllerModuleInterface::FirstPersonControllerModuleInterface()
    {
        // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
        // Add ALL components descriptors associated with this gem to m_descriptors.
        // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
        // This happens through the [MyComponent]::Reflect() function.
        m_descriptors.insert(m_descriptors.end(), {
            FirstPersonControllerSystemComponent::CreateDescriptor(),
            FirstPersonComponent::CreateDescriptor(),
            });
    }

    AZ::ComponentTypeList FirstPersonControllerModuleInterface::GetRequiredSystemComponents() const
    {
        return AZ::ComponentTypeList{
            azrtti_typeid<FirstPersonControllerSystemComponent>(),
        };
    }
} // namespace FirstPersonController
