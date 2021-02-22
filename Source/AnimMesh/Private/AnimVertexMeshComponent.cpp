// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimMesh/Public/AnimVertexMeshComponent.h"


#include "SkeletalRenderPublic.h"
#include "AnimMesh/Shaders/AnimationInstanceVertexFactory.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Rendering/SkeletalMeshRenderData.h"


FAnimationInstanceVertexSceneProxy* UAnimVertexMeshComponent::CreateSceneProxy()
{

    auto scale=1;
    BoneArray.Init(FMatrix(FVector::OneVector*scale,FVector::OneVector*scale,FVector::OneVector*scale,FVector::OneVector*scale),256*16);
  
    for (uint32 i=0; i<256*16;++i)
    {
        temp.Add(FMatrix(FVector::OneVector * (i*0.1f) ,FVector::OneVector,FVector::OneVector,FVector::OneVector));
    }

   // temp.Init(FMatrix(FVector::OneVector,FVector::OneVector,FVector::OneVector,FVector::OneVector),256*16);
   
 //   FMemory::Memcpy(BoneArray.GetData(), SrcInts, 4*sizeof(int32));
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

        auto s= new FAnimationInstanceVertexSceneProxy(this, GetWorld()->FeatureLevel);
        
        UpdateBoneArray(BoneArray);

       
        return s;
    }
    else
    {
        return NULL;
    }
    
}

void UAnimVertexMeshComponent::UpdateBoneArray(TArray<FMatrix> buffer)
{
    SetBuffer(buffer);
    SetBufferFinish();
}


void UAnimVertexMeshComponent::SetRenderIndex(int index)
{
    //    auto* animMeshSceneProxy = (FAnimationVertexSceneProxy*)SceneProxy;
    //animMeshSceneProxy->SetRenderIndex(index);
}

void UAnimVertexMeshComponent::SetBuffer(TArray<FMatrix> buffer)
{
    auto* animMeshSceneProxy = (FAnimationInstanceVertexSceneProxy*)SceneProxy;    

    if (SceneProxy)
    {            
        ENQUEUE_RENDER_COMMAND(FDeformMeshAllTransformsSBUpdate)(
            [animMeshSceneProxy, buffer](FRHICommandListImmediate& RHICmdList)
            {
                 animMeshSceneProxy-> UpdateBoneMatrixBuffer_RenderThread(buffer);
            });
    }
}

void UAnimVertexMeshComponent::SetBufferFinish()
{
    if (SceneProxy)
    {
        // Enqueue command to modify render thread info
        auto* animMeshSceneProxy = (FAnimationInstanceVertexSceneProxy*)SceneProxy;    
        ENQUEUE_RENDER_COMMAND(FDeformMeshAllTransformsSBUpdate)(
            [animMeshSceneProxy](FRHICommandListImmediate& RHICmdList)
            {
                animMeshSceneProxy->UpdateMatrixBufferSB_RenderThread();
            });
    }
}

void UAnimVertexMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType,    FActorComponentTickFunction* ThisTickFunction)
{
    if (BoneArray.Num()<=0)return;


  
    
    UpdateBoneArray(temp);
    
}

