#include "BufferFillSystem.h"
#include "Constants.h"
#include "RendererGroup.h"
#include "Components/RendererRoot.h"

void BufferFillSystem::OnCreate()
{
    pGroupData = RendererGroup::instance();
    SystemRun->action([this](flecs::iter it)
    {
        auto buffer = pGroupData->Buffer;
        auto rootCount = pGroupData->RenderEntityList.Num();
        auto totalBufferSize = Constants::BoneSize * rootCount;

        if (buffer.Num() != totalBufferSize)
        {
            SetComputeBuffer();
        }
    });
}


void BufferFillSystem::SetComputeBuffer() const
{
    for (int i = 0; i < pGroupData->RenderEntityList.Num(); i++)
    {
        auto entity = pGroupData->RenderEntityList[i];
        auto rootRendererData = entity.get<CharacterActorComponent>();
        rootRendererData->Character->SetSetBuffer(pGroupData->Buffer);
    }
}


