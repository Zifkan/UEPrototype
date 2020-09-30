#include "BufferFillSystem.h"

#include "RendererGroup.h"

void BufferFillSystem::OnCreate()
{
    rendererGroup = RendererGroup::instance();
    SystemRun->action([this](flecs::iter it)
    {

        ENQUEUE_RENDER_COMMAND(UpdateBoneMatrixBufferSB)([this](FRHICommandListImmediate& RHICmdList)
        {
           UpdateBoneMatrixBufferSB_RenderThread();
        });
        
        
    });
}


void BufferFillSystem::UpdateBoneMatrixBufferSB_RenderThread() const
{
    check(IsInRenderingThread());
    //Update the structured buffer only if it needs update
   
    void* StructuredBufferData = RHILockStructuredBuffer(rendererGroup->BindMatricesSB, 0, rendererGroup->Buffer.Num() * sizeof(FMatrix), RLM_WriteOnly);
    FMemory::Memcpy(StructuredBufferData, rendererGroup->Buffer.GetData(), rendererGroup->Buffer.Num() * sizeof(FMatrix));
    RHIUnlockStructuredBuffer(rendererGroup->BindMatricesSB);
}
