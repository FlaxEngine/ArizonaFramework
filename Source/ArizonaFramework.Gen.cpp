// This code was auto-generated. Do not modify it.

#include "Engine/Scripting/BinaryModule.h"
#include "ArizonaFramework.Gen.h"

StaticallyLinkedBinaryModuleInitializer StaticallyLinkedBinaryModuleArizonaFramework(GetBinaryModuleArizonaFramework);

extern "C" BinaryModule* GetBinaryModuleArizonaFramework()
{
    static NativeBinaryModule module("ArizonaFramework", MAssemblyOptions());
    return &module;
}
