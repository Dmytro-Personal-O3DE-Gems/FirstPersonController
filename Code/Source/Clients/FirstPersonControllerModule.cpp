
#include <FirstPersonController/FirstPersonControllerTypeIds.h>
#include <FirstPersonControllerModuleInterface.h>
#include "FirstPersonControllerSystemComponent.h"

namespace FirstPersonController
{
    class FirstPersonControllerModule
        : public FirstPersonControllerModuleInterface
    {
    public:
        AZ_RTTI(FirstPersonControllerModule, FirstPersonControllerModuleTypeId, FirstPersonControllerModuleInterface);
        AZ_CLASS_ALLOCATOR(FirstPersonControllerModule, AZ::SystemAllocator);
    };
}// namespace FirstPersonController

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME), FirstPersonController::FirstPersonControllerModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_FirstPersonController, FirstPersonController::FirstPersonControllerModule)
#endif
