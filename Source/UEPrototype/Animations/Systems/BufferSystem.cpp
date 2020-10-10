#include "BufferSystem.h"
#include "RendererGroup.h"
#include "UEPrototype/Animations/Constants.h"

void BufferSystem::OnCreate()
{   
    pGroupData =RendererGroup::instance();
    
    SystemRun->action([this](flecs::iter it)
    {
        auto buffer = pGroupData->Buffer;
        const auto rootCount = pGroupData->RenderEntityList.Num();
        const auto totalBufferSize = Constants::BoneSize * rootCount;

        if (buffer.Num() != totalBufferSize)
        {
            buffer.Reset(totalBufferSize);             
            pGroupData->Buffer = buffer;
        }
    });
}

