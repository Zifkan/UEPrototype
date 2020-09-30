#include "BufferSystem.h"


#include "CharacterActor.h"
#include "RendererGroup.h"
#include "UEPrototype/Animations/Constants.h"

void BufferSystem::OnCreate()
{
    auto rendererGroup = RendererGroup::instance();
    SystemRun->action([rendererGroup](flecs::iter it)
    {
           auto groupData = rendererGroup;
           auto buffer = groupData->Buffer;
         
        
           auto rootCount = groupData->RenderEntityList.Num();

           auto totalBufferSize = Constants::BoneSize * rootCount;

           if (buffer.Num() != totalBufferSize)
           {
               buffer.Reset(totalBufferSize);             
               groupData->Buffer = buffer;           
           }
    });
}