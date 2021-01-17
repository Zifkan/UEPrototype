#include "BufferSystem.h"
#include "RendererGroup.h"
#include "UEPrototype/Animations/Constants.h"

void BufferSystem::OnCreate()
{   
    pGroupData =RendererGroup::instance();
    
    SystemRun->action([this](flecs::iter it)
    {        
        const auto rootCount = pGroupData->RenderEntityList.Num();
        const auto totalBufferSize = Constants::BoneSize * rootCount;
      
        if (pGroupData->Buffer.Num() != totalBufferSize)
        {
            pGroupData->Buffer.Init(FMatrix(),totalBufferSize);
      //      UE_LOG(LogTemp, Warning, TEXT("buffer number: %i; render number: %i"),pGroupData->Buffer.Num(),rootCount);
        }
    }).kind(0);
}

