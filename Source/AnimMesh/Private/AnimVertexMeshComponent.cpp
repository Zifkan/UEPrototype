// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimMesh/Public/AnimVertexMeshComponent.h"

#include "AnimMesh/Shaders/AnimationVertexFactory.h"

UAnimVertexMeshComponent::UAnimVertexMeshComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    // ...
}

FPrimitiveSceneProxy* UAnimVertexMeshComponent::CreateSceneProxy()
{
    if (GetStaticMesh() == nullptr || GetStaticMesh()->RenderData == nullptr)
    {
        return nullptr;
    }

    const FStaticMeshLODResourcesArray& LODResources = GetStaticMesh()->RenderData->LODResources;
    if (LODResources.Num() == 0    || LODResources[FMath::Clamp<int32>(GetStaticMesh()->MinLOD.Default, 0, LODResources.Num()-1)].VertexBuffers.StaticMeshVertexBuffer.GetNumVertices() == 0)
    {
        return nullptr;
    }
    LLM_SCOPE(ELLMTag::StaticMesh);

    FPrimitiveSceneProxy* Proxy = ::new FAnimationVertexSceneProxy(this,false);
#if STATICMESH_ENABLE_DEBUG_RENDERING
    SendRenderDebugPhysics(Proxy);
#endif

    return Proxy;

    
}

void UAnimVertexMeshComponent::SetRenderIndex(int index)
{
    //    auto* animMeshSceneProxy = (FAnimationVertexSceneProxy*)SceneProxy;
    //animMeshSceneProxy->SetRenderIndex(index);
}

void UAnimVertexMeshComponent::SetBuffer(TArray<FMatrix> buffer)
{
    auto* animMeshSceneProxy = (FAnimationVertexSceneProxy*)SceneProxy;
    animMeshSceneProxy->SetBuffer(buffer);

    if (SceneProxy)
    {            
        ENQUEUE_RENDER_COMMAND(FDeformMeshAllTransformsSBUpdate)(
            [animMeshSceneProxy](FRHICommandListImmediate& RHICmdList)
            {
                animMeshSceneProxy-> UpdateBoneMatrixBufferSB_RenderThread();
            });
    }
}

