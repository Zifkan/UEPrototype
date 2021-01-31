// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimMesh/Public/AnimVertexMeshComponent.h"

#include "AnimMesh/Shaders/AnimationInstanceVertexFactory.h"


UAnimVertexMeshComponent::UAnimVertexMeshComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;
    
  
}

FAnimationInstanceVertexSceneProxy* UAnimVertexMeshComponent::CreateSceneProxy()
{
    LLM_SCOPE(ELLMTag::InstancedMesh);
    ProxySize = 0;

    // Verify that the mesh is valid before using it.
    const bool bMeshIsValid = 
        // make sure we have instances
        PerInstanceSMData.Num() > 0 &&
        // make sure we have an actual staticmesh
        GetStaticMesh() &&
        GetStaticMesh()->HasValidRenderData() &&
        // You really can't use hardware instancing on the consoles with multiple elements because they share the same index buffer. 
        // @todo: Level error or something to let LDs know this
        1;//GetStaticMesh()->LODModels(0).Elements.Num() == 1;

    if(bMeshIsValid)
    {
        check(InstancingRandomSeed != 0);
		
        // if instance data was modified, update GPU copy
        // generally happens only in editor 
        if (InstanceUpdateCmdBuffer.NumTotalCommands() != 0)
        {
            FlushInstanceUpdateCommands();
        }
		
        ProxySize = PerInstanceRenderData->ResourceSize;
        
        return ::new FAnimationInstanceVertexSceneProxy(this, GetWorld()->FeatureLevel);
    }
    else
    {
        return NULL;
    }
    
}


void UAnimVertexMeshComponent::SetRenderIndex(int index)
{
    //    auto* animMeshSceneProxy = (FAnimationVertexSceneProxy*)SceneProxy;
    //animMeshSceneProxy->SetRenderIndex(index);
}

void UAnimVertexMeshComponent::SetBuffer(TArray<FMatrix> buffer)
{
  /*  auto* animMeshSceneProxy = (FAnimationVertexSceneProxy*)SceneProxy;
    animMeshSceneProxy->SetBuffer(buffer);

    if (SceneProxy)
    {            
        ENQUEUE_RENDER_COMMAND(FDeformMeshAllTransformsSBUpdate)(
            [animMeshSceneProxy](FRHICommandListImmediate& RHICmdList)
            {
         //       animMeshSceneProxy-> UpdateBoneMatrixBufferSB_RenderThread();
            });
    }*/
}

