
#pragma once

namespace FirstPersonController
{
    // System Component TypeIds
    inline constexpr const char* FirstPersonControllerSystemComponentTypeId = "{A62D19DD-985F-4985-BDD8-950DAFAAF2B6}";
    inline constexpr const char* FirstPersonControllerEditorSystemComponentTypeId = "{69C80134-3F7C-484F-BA68-C8BFEC62BE0B}";

    // Module derived classes TypeIds
    inline constexpr const char* FirstPersonControllerModuleInterfaceTypeId = "{2D9C5722-01A6-49F6-B59F-362B01E786C5}";
    inline constexpr const char* FirstPersonControllerModuleTypeId = "{A8FE8194-E0A3-4D08-BE42-CFD2A043733F}";
    // The Editor Module by default is mutually exclusive with the Client Module
    // so they use the Same TypeId
    inline constexpr const char* FirstPersonControllerEditorModuleTypeId = FirstPersonControllerModuleTypeId;

    // Interface TypeIds
    inline constexpr const char* FirstPersonControllerRequestsTypeId = "{E996380D-FE29-4B0D-BB20-AE62E426E996}";
} // namespace FirstPersonController
