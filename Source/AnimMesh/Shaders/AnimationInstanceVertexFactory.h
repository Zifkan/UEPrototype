#pragma once

#include "RenderingThread.h"
#include "VertexFactory.h"
#include "MaterialShared.h"
#include "LocalVertexFactory.h"
#include "SceneManagement.h"
#include "MeshMaterialShader.h"
#include "TessellationRendering.h"
#include "Engine/InstancedStaticMesh.h"



struct FAnimMeshInstanceVertexFactory : FInstancedStaticMeshVertexFactory
{
	DECLARE_VERTEX_FACTORY_TYPE(FAnimMeshInstanceVertexFactory);
public:
	FAnimMeshInstanceVertexFactory(ERHIFeatureLevel::Type InFeatureLevel)
		: FInstancedStaticMeshVertexFactory(InFeatureLevel)
	{
	}
	
};

class FAnimInstancedMeshVertexFactoryShaderParameters : public FLocalVertexFactoryShaderParametersBase
{
	DECLARE_TYPE_LAYOUT(FAnimInstancedMeshVertexFactoryShaderParameters, NonVirtual);
public:
	void Bind(const FShaderParameterMap& ParameterMap) 
	{
		FLocalVertexFactoryShaderParametersBase::Bind(ParameterMap);

		InstancingFadeOutParamsParameter.Bind(ParameterMap, TEXT("InstancingFadeOutParams"));
		InstancingViewZCompareZeroParameter.Bind(ParameterMap, TEXT("InstancingViewZCompareZero"));
		InstancingViewZCompareOneParameter.Bind(ParameterMap, TEXT("InstancingViewZCompareOne"));
		InstancingViewZConstantParameter.Bind(ParameterMap, TEXT("InstancingViewZConstant"));
		InstancingOffsetParameter.Bind(ParameterMap, TEXT("InstancingOffset"));
		InstancingWorldViewOriginZeroParameter.Bind(ParameterMap, TEXT("InstancingWorldViewOriginZero"));
		InstancingWorldViewOriginOneParameter.Bind(ParameterMap, TEXT("InstancingWorldViewOriginOne"));
		VertexFetch_InstanceOriginBufferParameter.Bind(ParameterMap, TEXT("VertexFetch_InstanceOriginBuffer"));
		VertexFetch_InstanceTransformBufferParameter.Bind(ParameterMap, TEXT("VertexFetch_InstanceTransformBuffer"));
		VertexFetch_InstanceLightmapBufferParameter.Bind(ParameterMap, TEXT("VertexFetch_InstanceLightmapBuffer"));
		InstanceOffset.Bind(ParameterMap, TEXT("InstanceOffset"));
	}

	void GetElementShaderBindings(
		const class FSceneInterface* Scene,
		const FSceneView* View,
		const FMeshMaterialShader* Shader,
		const EVertexInputStreamType InputStreamType,
		ERHIFeatureLevel::Type FeatureLevel,
		const FVertexFactory* VertexFactory,
		const FMeshBatchElement& BatchElement,
		FMeshDrawSingleShaderBindings& ShaderBindings,
		FVertexInputStreamArray& VertexStreams
		) const;

private:
	
	LAYOUT_FIELD(FShaderParameter, InstancingFadeOutParamsParameter)
	LAYOUT_FIELD(FShaderParameter, InstancingViewZCompareZeroParameter)
	LAYOUT_FIELD(FShaderParameter, InstancingViewZCompareOneParameter)
	LAYOUT_FIELD(FShaderParameter, InstancingViewZConstantParameter)
	LAYOUT_FIELD(FShaderParameter, InstancingOffsetParameter);
	LAYOUT_FIELD(FShaderParameter, InstancingWorldViewOriginZeroParameter)
	LAYOUT_FIELD(FShaderParameter, InstancingWorldViewOriginOneParameter)

	LAYOUT_FIELD(FShaderResourceParameter, VertexFetch_InstanceOriginBufferParameter)
	LAYOUT_FIELD(FShaderResourceParameter, VertexFetch_InstanceTransformBufferParameter)
	LAYOUT_FIELD(FShaderResourceParameter, VertexFetch_InstanceLightmapBufferParameter)
	LAYOUT_FIELD(FShaderParameter, InstanceOffset)
};


class  FAnimMeshInstancedStaticMeshRenderData
{
public:	
	FAnimMeshInstancedStaticMeshRenderData(UInstancedStaticMeshComponent* InComponent, ERHIFeatureLevel::Type InFeatureLevel)
   : Component(InComponent)
   , PerInstanceRenderData(InComponent->PerInstanceRenderData)
   , LODModels(Component->GetStaticMesh()->RenderData->LODResources)
   , FeatureLevel(InFeatureLevel)
	{
		check(PerInstanceRenderData.IsValid());
		// Allocate the vertex factories for each LOD
		InitVertexFactories();
	//	RegisterSpeedTreeWind();
	}

	void ReleaseResources(FSceneInterface* Scene, const UStaticMesh* StaticMesh)
	{
		// unregister SpeedTree wind with the scene
		if (Scene && StaticMesh && StaticMesh->SpeedTreeWind.IsValid())
		{
			for (int32 LODIndex = 0; LODIndex < VertexFactories.Num(); LODIndex++)
			{
				Scene->RemoveSpeedTreeWind_RenderThread(&VertexFactories[LODIndex], StaticMesh);
			}
		}

		for (int32 LODIndex = 0; LODIndex < VertexFactories.Num(); LODIndex++)
		{
			VertexFactories[LODIndex].ReleaseResource();
		}
	}

	/** Source component */
	UInstancedStaticMeshComponent* Component;

	/** Per instance render data, could be shared with component */
	TSharedPtr<FPerInstanceRenderData, ESPMode::ThreadSafe> PerInstanceRenderData;

	/** Vertex factory */
	TIndirectArray<FAnimMeshInstanceVertexFactory> VertexFactories;

	/** LOD render data from the static mesh. */
	FStaticMeshLODResourcesArray& LODModels;

	/** Feature level used when creating instance data */
	ERHIFeatureLevel::Type FeatureLevel;

private:
	void InitVertexFactories();

	void RegisterSpeedTreeWind()
	{
		// register SpeedTree wind with the scene
		if (Component->GetStaticMesh()->SpeedTreeWind.IsValid())
		{
			for (int32 LODIndex = 0; LODIndex < LODModels.Num(); LODIndex++)
			{
				if (Component->GetScene())
				{
					Component->GetScene()->AddSpeedTreeWind(&VertexFactories[LODIndex], Component->GetStaticMesh());
				}
			}
		}
	}
};





class FAnimationInstanceVertexSceneProxy : public FStaticMeshSceneProxy
{
public:
	SIZE_T GetTypeHash() const override;

	FAnimationInstanceVertexSceneProxy(UInstancedStaticMeshComponent* InComponent, ERHIFeatureLevel::Type InFeatureLevel)
	:	FStaticMeshSceneProxy(InComponent, true)
	,	StaticMesh(InComponent->GetStaticMesh())
	,	InstancedRenderData(InComponent, InFeatureLevel)
#if WITH_EDITOR
	,	bHasSelectedInstances(InComponent->SelectedInstances.Num() > 0)
#endif
	{
		bVFRequiresPrimitiveUniformBuffer = true;
		SetupProxy(InComponent);

#if RHI_RAYTRACING
		SetupRayTracingCullClusters();
#endif	
	}

	~FAnimationInstanceVertexSceneProxy()
	{
	}

	// FPrimitiveSceneProxy interface.

	virtual void DestroyRenderThreadResources() override;

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		FPrimitiveViewRelevance Result;
		if(View->Family->EngineShowFlags.InstancedStaticMeshes)
		{
			Result = FStaticMeshSceneProxy::GetViewRelevance(View);
#if WITH_EDITOR
			// use dynamic path to render selected indices
			if( bHasSelectedInstances )
			{
				Result.bDynamicRelevance = true;
			}
#endif
		}
		return Result;
	}

#if RHI_RAYTRACING
	virtual bool IsRayTracingStaticRelevant() const override
	{
		return false;
	}

	virtual void GetDynamicRayTracingInstances(struct FRayTracingMaterialGatheringContext& Context, TArray<FRayTracingInstance>& OutRayTracingInstances) final override;
	void SetupRayTracingCullClusters();

#endif

	virtual void GetLightRelevance(const FLightSceneProxy* LightSceneProxy, bool& bDynamic, bool& bRelevant, bool& bLightMapped, bool& bShadowMapped) const override;
	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override;

	virtual int32 GetNumMeshBatches() const override
	{
		return 1;
	}

	/** Sets up a shadow FMeshBatch for a specific LOD. */
	virtual bool GetShadowMeshElement(int32 LODIndex, int32 BatchIndex, uint8 InDepthPriorityGroup, FMeshBatch& OutMeshBatch, bool bDitheredLODTransition) const override;

	/** Sets up a FMeshBatch for a specific LOD and element. */
	virtual bool GetMeshElement(int32 LODIndex, int32 BatchIndex, int32 ElementIndex, uint8 InDepthPriorityGroup, bool bUseSelectionOutline, bool bAllowPreCulledIndices, FMeshBatch& OutMeshBatch) const override;

	/** Sets up a wireframe FMeshBatch for a specific LOD. */
	virtual bool GetWireframeMeshElement(int32 LODIndex, int32 BatchIndex, const FMaterialRenderProxy* WireframeRenderProxy, uint8 InDepthPriorityGroup, bool bAllowPreCulledIndices, FMeshBatch& OutMeshBatch) const override;

	virtual void GetDistancefieldAtlasData(FBox& LocalVolumeBounds, FVector2D& OutDistanceMinMax, FIntVector& OutBlockMin, FIntVector& OutBlockSize, bool& bOutBuiltAsIfTwoSided, bool& bMeshWasPlane, float& SelfShadowBias, TArray<FMatrix>& ObjectLocalToWorldTransforms, bool& bOutThrottled) const override;

	virtual void GetDistanceFieldInstanceInfo(int32& NumInstances, float& BoundsSurfaceArea) const override;

	virtual int32 CollectOccluderElements(FOccluderElementsCollector& Collector) const override;

	/**
	 * Creates the hit proxies are used when DrawDynamicElements is called.
	 * Called in the game thread.
	 * @param OutHitProxies - Hit proxes which are created should be added to this array.
	 * @return The hit proxy to use by default for elements drawn by DrawDynamicElements.
	 */
	virtual HHitProxy* CreateHitProxies(UPrimitiveComponent* Component,TArray<TRefCountPtr<HHitProxy> >& OutHitProxies) override;
	
	virtual bool IsDetailMesh() const override
	{
		return true;
	}

	/** Per component render data */
	FAnimMeshInstancedStaticMeshRenderData InstancedRenderData;

protected:
	/** Cache of the StaticMesh asset, needed to release SpeedTree resources*/
	UStaticMesh* StaticMesh;

	

#if WITH_EDITOR
	/* If we we have any selected instances */
	bool bHasSelectedInstances;
#else
	static const bool bHasSelectedInstances = false;
#endif

	/** LOD transition info. */
	FInstancingUserData UserData_AllInstances;
	FInstancingUserData UserData_SelectedInstances;
	FInstancingUserData UserData_DeselectedInstances;

#if RHI_RAYTRACING
	
	/* Precomputed bounding spheres for culling */
	struct FCullNode
	{
		FVector Center;
		float Radius;
		uint32 Instance;
	};

	struct FRayTracingCullCluster
	{
		FVector BoundsMin;
		FVector BoundsMax;
		TArray<FCullNode> Nodes;
	};

	TArray<FRayTracingCullCluster> RayTracingCullClusters;
#endif

	/** Common path for the Get*MeshElement functions */
	void SetupInstancedMeshBatch(int32 LODIndex, int32 BatchIndex, FMeshBatch& OutMeshBatch) const;

private:

	void SetupProxy(UInstancedStaticMeshComponent* InComponent);
};




