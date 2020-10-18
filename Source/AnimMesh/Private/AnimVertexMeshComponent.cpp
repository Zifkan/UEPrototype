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
 
    FAnimationVertexSceneProxy* Proxy = ::new FAnimationVertexSceneProxy(this,false);
	// GetStaticMesh()->RenderData->LODVertexFactories[0].VertexFactory = static_cast<FLocalVertexFactory>(Proxy->animMeshVertexFactory);
	//GetStaticMesh()->RenderData->LODVertexFactories[0].InitVertexFactory(LODResources[0],Proxy->animMeshVertexFactory,0,GetStaticMesh(),false);
  

#if STATICMESH_ENABLE_DEBUG_RENDERING
    SendRenderDebugPhysics(Proxy);
#endif

    return Proxy;

    
}


void FAnimationVertexSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views,
   const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const
{
 /*   Collector.AllocateMesh().VertexFactory = &animMeshVertexFactory;    
    FStaticMeshSceneProxy::GetDynamicMeshElements(Views, ViewFamily, VisibilityMap, Collector);*/


    // Set up wireframe material (if needed)
		const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;

		FColoredMaterialRenderProxy* WireframeMaterialInstance = NULL;
		if (bWireframe)
		{
			WireframeMaterialInstance = new FColoredMaterialRenderProxy(
				GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy() : NULL,
				FLinearColor(0, 0.5f, 1.f)
			);

			Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);
		}

				//Get the section's materil, or the wireframe material if we're rendering in wireframe mode
				FMaterialRenderProxy* MaterialProxy = bWireframe ? WireframeMaterialInstance : Material->GetRenderProxy();

				// For each view..
				for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
				{
					//Check if our mesh is visible from this view
					if (VisibilityMap & (1 << ViewIndex))
					{
						const FSceneView* View = Views[ViewIndex];
						// Allocate a mesh batch and get a ref to the first element
						FMeshBatch& Mesh = Collector.AllocateMesh();
						FMeshBatchElement& BatchElement = Mesh.Elements[0];
						//Fill this batch element with the mesh section's render data
					
						Mesh.bWireframe = bWireframe;
						Mesh.VertexFactory = &animMeshVertexFactory;
						Mesh.MaterialRenderProxy = MaterialProxy;

						//The LocalVertexFactory uses a uniform buffer to pass primitve data like the local to world transform for this frame and for the previous one
						//Most of this data can be fetched using the helper function below
						bool bHasPrecomputedVolumetricLightmap;
						FMatrix PreviousLocalToWorld;
						int32 SingleCaptureIndex;
						bool bOutputVelocity;
						GetScene().GetPrimitiveUniformShaderParameters_RenderThread(GetPrimitiveSceneInfo(), bHasPrecomputedVolumetricLightmap, PreviousLocalToWorld, SingleCaptureIndex, bOutputVelocity);
						//Alloate a temporary primitive uniform buffer, fill it with the data and set it in the batch element
						FDynamicPrimitiveUniformBuffer& DynamicPrimitiveUniformBuffer = Collector.AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();
						DynamicPrimitiveUniformBuffer.Set(GetLocalToWorld(), PreviousLocalToWorld, GetBounds(), GetLocalBounds(), true, bHasPrecomputedVolumetricLightmap, DrawsVelocity(), bOutputVelocity);
						BatchElement.PrimitiveUniformBufferResource = &DynamicPrimitiveUniformBuffer.UniformBuffer;
						BatchElement.PrimitiveIdMode = PrimID_DynamicPrimitiveShaderData;

						//Additional data 
						Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
						Mesh.Type = PT_TriangleList;
						Mesh.DepthPriorityGroup = SDPG_World;
						Mesh.bCanApplyViewModeOverrides = false;

						//Add the batch to the collector
						Collector.AddMesh(ViewIndex, Mesh);
					}				
			
		}
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

