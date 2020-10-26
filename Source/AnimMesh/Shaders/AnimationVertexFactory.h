#pragma once

#include "PrimitiveViewRelevance.h"
#include "RenderResource.h"
#include "RenderingThread.h"
#include "PrimitiveSceneProxy.h"
#include "Containers/ResourceArray.h"
#include "EngineGlobals.h"
#include "VertexFactory.h"
#include "MaterialShared.h"
#include "Materials/Material.h"
#include "LocalVertexFactory.h"
#include "Engine/Engine.h"
#include "SceneManagement.h"
#include "DynamicMeshBuilder.h"
#include "StaticMeshResources.h"
#include "MeshMaterialShader.h"
#include "ShaderParameters.h"
#include "RHIUtilities.h"
#include "AI/Navigation/NavCollisionBase.h"
#include "PhysicalMaterials/PhysicalMaterialMask.h"

class FAnimationVertexSceneProxy;
struct FAnimMeshVertexFactory;

struct FAnimMeshVertexFactory : FLocalVertexFactory
{
 	DECLARE_VERTEX_FACTORY_TYPE(FAnimMeshVertexFactory);
public:


	FAnimMeshVertexFactory(ERHIFeatureLevel::Type InFeatureLevel): FLocalVertexFactory(InFeatureLevel, "FAnimMeshVertexFactory")
	{
		//We're not interested in Manual vertex fetch so we disable it 
		bSupportsManualVertexFetch = true;
		
		
	}

    virtual void InitRHI() override
    {
		FLocalVertexFactory::InitRHI();
	}

	
	static bool ShouldCompilePermutation(const FVertexFactoryShaderPermutationParameters& Parameters)
	{
		/*if ((Parameters.MaterialParameters.MaterialDomain == MD_Surface &&
			Parameters.MaterialParameters.ShadingModels == MSM_DefaultLit) ||
			Parameters.MaterialParameters.bIsDefaultMaterial)
		{
			return true;
		}
		return false;*/
		
		// Only compile this permutation inside the editor - it's not applicable in games, but occasionally the editor needs it.
		if (Parameters.MaterialParameters.MaterialDomain == MD_UI)
		{
			return !!WITH_EDITOR;
		}

		return true; 
	}
	
	
	static void ModifyCompilationEnvironment(const FVertexFactoryShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		
		/*const bool ContainsManualVertexFetch = OutEnvironment.GetDefinitions().Contains("MANUAL_VERTEX_FETCH");
		if (!ContainsManualVertexFetch)
		{
			OutEnvironment.SetDefine(TEXT("MANUAL_VERTEX_FETCH"), TEXT("0"));
		}
	
		OutEnvironment.SetDefine(TEXT("ANIM_MESH"), TEXT("1"));	*/
		OutEnvironment.SetDefine(TEXT("VF_SUPPORTS_SPEEDTREE_WIND"),TEXT("1"));

		const bool ContainsManualVertexFetch = OutEnvironment.GetDefinitions().Contains("MANUAL_VERTEX_FETCH");
		if (!ContainsManualVertexFetch && RHISupportsManualVertexFetch(Parameters.Platform))
		{
			OutEnvironment.SetDefine(TEXT("MANUAL_VERTEX_FETCH"), TEXT("1"));
		}

		OutEnvironment.SetDefine(TEXT("VF_SUPPORTS_PRIMITIVE_SCENE_DATA"), Parameters.VertexFactoryType->SupportsPrimitiveIdStream() && UseGPUScene(Parameters.Platform, GetMaxSupportedFeatureLevel(Parameters.Platform)));
		OutEnvironment.SetDefine(TEXT("VF_GPU_SCENE_TEXTURE"), Parameters.VertexFactoryType->SupportsPrimitiveIdStream() && UseGPUScene(Parameters.Platform, GetMaxSupportedFeatureLevel(Parameters.Platform)) && GPUSceneUseTexture2D(Parameters.Platform));

		
	}	


	void SetTransformIndex(uint16 Index) { RendererIndex = Index; }
    void SetSceneProxy(FAnimationVertexSceneProxy* Proxy) { SceneProxy = Proxy; }	

private:
	//We need to pass this as a shader parameter, so we store it in the vertex factory and we use in the vertex factory shader parameters
	uint16 RendererIndex;	
	FAnimationVertexSceneProxy* SceneProxy;

	friend class FAnimMeshVertexFactoryShaderParameters;
};

class FAnimationVertexSceneProxy final : public FStaticMeshSceneProxy 
{
	public:
	
	FAnimationVertexSceneProxy(UStaticMeshComponent* Component, bool bForceLODsShareStaticLighting)
        : FStaticMeshSceneProxy(Component, bForceLODsShareStaticLighting)
		,animMeshVertexFactory(GetScene().GetFeatureLevel())
	{	
		
		animMeshVertexFactory.SetSceneProxy(this);
		auto& LODResources = Component->GetStaticMesh()->RenderData->LODResources;		
		

		for (int i = 0; i < Component->GetStaticMesh()->RenderData->LODVertexFactories.Num(); ++i)
		{
			Component->GetStaticMesh()->RenderData->LODVertexFactories[i].InitVertexFactory(LODResources[i],animMeshVertexFactory,i,Component->GetStaticMesh(),false);
		}
		
		Material = UMaterial::GetDefaultMaterial(MD_Surface);
		
		Buffer.AddZeroed(256);
		
	/*	TResourceArray<FMatrix>* ResourceArray = new TResourceArray<FMatrix>(true);
		FRHIResourceCreateInfo CreateInfo;
		ResourceArray->Append(Buffer);
		CreateInfo.ResourceArray = ResourceArray;
		//Set the debug name so we can find the resource when debugging in RenderDoc
		CreateInfo.DebugName = TEXT("DeformMesh_TransformsSB");
		
		BindMatricesSB = RHICreateStructuredBuffer(sizeof(FMatrix), 256 * sizeof(FMatrix), BUF_ShaderResource, CreateInfo);		
		BindMatricesSRV = RHICreateShaderResourceView(BindMatricesSB);
		*/


	}

	virtual ~FAnimationVertexSceneProxy()
	{
		Buffer.Reset();
		animMeshVertexFactory.ReleaseResource();
		//Release the structured buffer and the SRV
		BindMatricesSRV.SafeRelease();
		BindMatricesSB.SafeRelease();
	}
	
	SIZE_T GetTypeHash() const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}
    
	virtual uint32 GetMemoryFootprint(void) const
	{
		return(sizeof(*this) + GetAllocatedSize());
	}


	FShaderResourceViewRHIRef& GetDeformTransformsSRV() { return BindMatricesSRV; }


	void SetBuffer(TArray<FMatrix> buffer)
	{				
		Buffer = buffer;
		UE_LOG(LogTemp, Warning, TEXT("animMeshSceneProxy: %i"),Buffer.Num());
	}

	void UpdateBoneMatrixBufferSB_RenderThread()
	{
		check(IsInRenderingThread());

		
		if(bBonesTransformsDirty)
		{
			void* StructuredBufferData = RHILockStructuredBuffer(BindMatricesSB, 0, Buffer.Num() * sizeof(FMatrix), RLM_WriteOnly);
			FMemory::Memcpy(StructuredBufferData, Buffer.GetData(), Buffer.Num() * sizeof(FMatrix));
			RHIUnlockStructuredBuffer(BindMatricesSB);

			bBonesTransformsDirty = false;
		}
	}

/*
	virtual bool GetMeshElement(
	int32 LODIndex, 
	int32 BatchIndex, 
	int32 SectionIndex, 
	uint8 InDepthPriorityGroup, 
	bool bUseSelectionOutline,
	bool bAllowPreCulledIndices, 
	FMeshBatch& OutMeshBatch) const override
	{
	const ERHIFeatureLevel::Type FeatureLevel = GetScene().GetFeatureLevel();
	const FStaticMeshLODResources& LOD = RenderData->LODResources[LODIndex];
	const FStaticMeshVertexFactories& VFs = RenderData->LODVertexFactories[LODIndex];
	const FStaticMeshSection& Section  = LOD.Sections[SectionIndex];
	const FLODInfo& ProxyLODInfo = LODs[LODIndex];

	UMaterialInterface* MaterialInterface = ProxyLODInfo.Sections[SectionIndex].Material;
	FMaterialRenderProxy* MaterialRenderProxy = MaterialInterface->GetRenderProxy();
	//const FMaterial* Material = MaterialRenderProxy->GetMaterial(FeatureLevel);

	const FVertexFactory* VertexFactory = nullptr;

	FMeshBatchElement& OutMeshBatchElement = OutMeshBatch.Elements[0];

#if WITH_EDITORONLY_DATA
	// If material is hidden, then skip the draw.
	if ((MaterialIndexPreview >= 0) && (MaterialIndexPreview != Section.MaterialIndex))
	{
		return false;
	}
	// If section is hidden, then skip the draw.
	if ((SectionIndexPreview >= 0) && (SectionIndexPreview != SectionIndex))
	{
		return false;
	}

	OutMeshBatch.bUseSelectionOutline = bPerSectionSelection ? bUseSelectionOutline : true;
#endif

	// Has the mesh component overridden the vertex color stream for this mesh LOD?
	if (ProxyLODInfo.OverrideColorVertexBuffer)
	{
		// Make sure the indices are accessing data within the vertex buffer's
		check(Section.MaxVertexIndex < ProxyLODInfo.OverrideColorVertexBuffer->GetNumVertices())

		// Use the instanced colors vertex factory.
		VertexFactory = &animMeshVertexFactory;//&VFs.VertexFactoryOverrideColorVertexBuffer;

		OutMeshBatchElement.VertexFactoryUserData = ProxyLODInfo.OverrideColorVFUniformBuffer.GetReference();
		OutMeshBatchElement.UserData = ProxyLODInfo.OverrideColorVertexBuffer;
		OutMeshBatchElement.bUserDataIsColorVertexBuffer = true;
	}
	else
	{
		VertexFactory = &animMeshVertexFactory;

		OutMeshBatchElement.VertexFactoryUserData = VFs.VertexFactory.GetUniformBuffer();
	}

	const bool bWireframe = false;

	// Disable adjacency information when the selection outline is enabled, since tessellation won't be used.
	const bool bRequiresAdjacencyInformation = !bUseSelectionOutline;

	// Two sided material use bIsFrontFace which is wrong with Reversed Indices. AdjacencyInformation use another index buffer.
	const bool bUseReversedIndices =  IsLocalToWorldDeterminantNegative() && (LOD.bHasReversedIndices != 0) && !bRequiresAdjacencyInformation && !Material->IsTwoSided();

	// No support for stateless dithered LOD transitions for movable meshes
	const bool bDitheredLODTransition = !IsMovable() && Material->IsDitheredLODTransition();

	const uint32 NumPrimitives = SetMeshElementGeometrySource(LODIndex, SectionIndex, bWireframe, bRequiresAdjacencyInformation, bUseReversedIndices, bAllowPreCulledIndices, VertexFactory, OutMeshBatch);

	if(NumPrimitives > 0)
	{
		OutMeshBatch.SegmentIndex = SectionIndex;

		OutMeshBatch.LODIndex = LODIndex;
#if STATICMESH_ENABLE_DEBUG_RENDERING
		OutMeshBatch.VisualizeLODIndex = LODIndex;
		OutMeshBatch.VisualizeHLODIndex = HierarchicalLODIndex;
#endif
		OutMeshBatch.ReverseCulling = IsReversedCullingNeeded(bUseReversedIndices);
		OutMeshBatch.CastShadow = bCastShadow && Section.bCastShadow;
#if RHI_RAYTRACING
		OutMeshBatch.CastRayTracedShadow = OutMeshBatch.CastShadow;
#endif
		OutMeshBatch.DepthPriorityGroup = (ESceneDepthPriorityGroup)InDepthPriorityGroup;
		OutMeshBatch.LCI = &ProxyLODInfo;
		OutMeshBatch.MaterialRenderProxy = MaterialRenderProxy;

		OutMeshBatchElement.MinVertexIndex = Section.MinVertexIndex;
		OutMeshBatchElement.MaxVertexIndex = Section.MaxVertexIndex;
#if STATICMESH_ENABLE_DEBUG_RENDERING
		OutMeshBatchElement.VisualizeElementIndex = SectionIndex;
#endif

		SetMeshElementScreenSize(LODIndex, bDitheredLODTransition, OutMeshBatch);

		return true;
	}
	else
	{
		return false;
	}
}
	

	*/
	

virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_StaticMeshSceneProxy_GetMeshElements);
	checkSlow(IsInRenderingThread());

	const bool bIsLightmapSettingError = HasStaticLighting() && !HasValidSettingsForStaticLighting();
	const bool bProxyIsSelected = IsSelected();
	const FEngineShowFlags& EngineShowFlags = ViewFamily.EngineShowFlags;

	bool bDrawSimpleCollision = false, bDrawComplexCollision = false;
	const bool bInCollisionView = IsCollisionView(EngineShowFlags, bDrawSimpleCollision, bDrawComplexCollision);
	
	// Skip drawing mesh normally if in a collision view, will rely on collision drawing code below
	const bool bDrawMesh = !bInCollisionView && 
	(	IsRichView(ViewFamily) || HasViewDependentDPG()
		|| EngineShowFlags.Collision
#if STATICMESH_ENABLE_DEBUG_RENDERING
		|| bDrawMeshCollisionIfComplex
		|| bDrawMeshCollisionIfSimple
#endif
		|| EngineShowFlags.Bounds
		|| bProxyIsSelected 
		|| IsHovered()
		|| bIsLightmapSettingError 
		|| !IsStaticPathAvailable() );


	//	for (int i = 0; i < RenderData->LODVertexFactories.Num(); i++)
		{
	//		RenderData->LODVertexFactories[i].VertexFactory = animMeshVertexFactory;
		}
		
	// Draw polygon mesh if we are either not in a collision view, or are drawing it as collision.
	if (EngineShowFlags.StaticMeshes && bDrawMesh)
	{
		// how we should draw the collision for this mesh.
		const bool bIsWireframeView = EngineShowFlags.Wireframe;
		const bool bLevelColorationEnabled = EngineShowFlags.LevelColoration;
		const bool bPropertyColorationEnabled = EngineShowFlags.PropertyColoration;
		const ERHIFeatureLevel::Type FeatureLevel = ViewFamily.GetFeatureLevel();

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			const FSceneView* View = Views[ViewIndex];

			if (IsShown(View) && (VisibilityMap & (1 << ViewIndex)))
			{
				FFrozenSceneViewMatricesGuard FrozenMatricesGuard(*const_cast<FSceneView*>(Views[ViewIndex]));

				FLODMask LODMask = GetLODMask(View);

				for (int32 LODIndex = 0; LODIndex < RenderData->LODResources.Num(); LODIndex++)
				{
					if (LODMask.ContainsLOD(LODIndex) && LODIndex >= ClampedMinLOD)
					{
						const FStaticMeshLODResources& LODModel = RenderData->LODResources[LODIndex];
						const FLODInfo& ProxyLODInfo = LODs[LODIndex];
						
						if (AllowDebugViewmodes() && bIsWireframeView && !EngineShowFlags.Materials
							// If any of the materials are mesh-modifying, we can't use the single merged mesh element of GetWireframeMeshElement()
							&& !ProxyLODInfo.UsesMeshModifyingMaterials())
						{
							FLinearColor ViewWireframeColor( bLevelColorationEnabled ? GetLevelColor() : GetWireframeColor() );
							if ( bPropertyColorationEnabled )
							{
								ViewWireframeColor = GetPropertyColor();
							}

							auto WireframeMaterialInstance = new FColoredMaterialRenderProxy(
								GEngine->WireframeMaterial->GetRenderProxy(),
								GetSelectionColor(ViewWireframeColor,!(GIsEditor && EngineShowFlags.Selection) || bProxyIsSelected, IsHovered(), false)
								);

							Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);

							const int32 NumBatches = GetNumMeshBatches();

							for (int32 BatchIndex = 0; BatchIndex < NumBatches; BatchIndex++)
							{
								//GetWireframeMeshElement will try SetIndexSource at sectionindex 0
								//and GetMeshElement loops over sections, therefore does not have this issue
								if (LODModel.Sections.Num() > 0)
								{
									FMeshBatch& Mesh = Collector.AllocateMesh();									
									
									if (GetWireframeMeshElement(LODIndex, BatchIndex, WireframeMaterialInstance, SDPG_World, true, Mesh))
									{
										// We implemented our own wireframe
										Mesh.bCanApplyViewModeOverrides = false;
									
										
										
										Collector.AddMesh(ViewIndex, Mesh);
										INC_DWORD_STAT_BY(STAT_StaticMeshTriangles, Mesh.GetNumPrimitives());
									}
								}
							}
						}
						else
						{
							const FLinearColor UtilColor( GetLevelColor() );

							// Draw the static mesh sections.
							for (int32 SectionIndex = 0; SectionIndex < LODModel.Sections.Num(); SectionIndex++)
							{
								const int32 NumBatches = GetNumMeshBatches();

								for (int32 BatchIndex = 0; BatchIndex < NumBatches; BatchIndex++)
								{
									bool bSectionIsSelected = false;
									FMeshBatch& MeshElement = Collector.AllocateMesh();
						 		
	#if WITH_EDITOR
									if (GIsEditor)
									{
										const FLODInfo::FSectionInfo& Section = LODs[LODIndex].Sections[SectionIndex];

										bSectionIsSelected = Section.bSelected || (bIsWireframeView && bProxyIsSelected);
										MeshElement.BatchHitProxyId = Section.HitProxy ? Section.HitProxy->Id : FHitProxyId();
									}
	#endif // WITH_EDITOR

									
								
									MeshElement.VertexFactory = &animMeshVertexFactory;
									if (GetMeshElement(LODIndex, BatchIndex, SectionIndex, SDPG_World, bSectionIsSelected, true, MeshElement))
									{
										bool bDebugMaterialRenderProxySet = false;
#if STATICMESH_ENABLE_DEBUG_RENDERING

	#if WITH_EDITOR								
										if (bProxyIsSelected && EngineShowFlags.PhysicalMaterialMasks && AllowDebugViewmodes())
										{
											// Override the mesh's material with our material that draws the physical material masks
											UMaterial* PhysMatMaskVisualizationMaterial = GEngine->PhysicalMaterialMaskMaterial;
											check(PhysMatMaskVisualizationMaterial);
											
											FMaterialRenderProxy* PhysMatMaskVisualizationMaterialInstance = nullptr;

											const FLODInfo::FSectionInfo& Section = LODs[LODIndex].Sections[SectionIndex];
											
											if (UMaterialInterface* SectionMaterial = Section.Material)
											{
												if (UPhysicalMaterialMask* PhysicalMaterialMask = SectionMaterial->GetPhysicalMaterialMask())
												{
													if (PhysicalMaterialMask->MaskTexture)
													{
														PhysMatMaskVisualizationMaterialInstance = new FColoredTexturedMaterialRenderProxy(
															PhysMatMaskVisualizationMaterial->GetRenderProxy(),
															FLinearColor::White, NAME_Color, PhysicalMaterialMask->MaskTexture, NAME_LinearColor);
													}

													Collector.RegisterOneFrameMaterialProxy(PhysMatMaskVisualizationMaterialInstance);
													MeshElement.MaterialRenderProxy = PhysMatMaskVisualizationMaterialInstance;

													bDebugMaterialRenderProxySet = true;
												}
											}
										}

	#endif // WITH_EDITOR

										if (!bDebugMaterialRenderProxySet && bProxyIsSelected && EngineShowFlags.VertexColors && AllowDebugViewmodes())
										{
											// Override the mesh's material with our material that draws the vertex colors
											UMaterial* VertexColorVisualizationMaterial = NULL;
											switch( GVertexColorViewMode )
											{
											case EVertexColorViewMode::Color:
												VertexColorVisualizationMaterial = GEngine->VertexColorViewModeMaterial_ColorOnly;
												break;

											case EVertexColorViewMode::Alpha:
												VertexColorVisualizationMaterial = GEngine->VertexColorViewModeMaterial_AlphaAsColor;
												break;

											case EVertexColorViewMode::Red:
												VertexColorVisualizationMaterial = GEngine->VertexColorViewModeMaterial_RedOnly;
												break;

											case EVertexColorViewMode::Green:
												VertexColorVisualizationMaterial = GEngine->VertexColorViewModeMaterial_GreenOnly;
												break;

											case EVertexColorViewMode::Blue:
												VertexColorVisualizationMaterial = GEngine->VertexColorViewModeMaterial_BlueOnly;
												break;
											}
											check( VertexColorVisualizationMaterial != NULL );

											auto VertexColorVisualizationMaterialInstance = new FColoredMaterialRenderProxy(
												VertexColorVisualizationMaterial->GetRenderProxy(),
												GetSelectionColor( FLinearColor::White, bSectionIsSelected, IsHovered() )
												);

											Collector.RegisterOneFrameMaterialProxy(VertexColorVisualizationMaterialInstance);
											MeshElement.MaterialRenderProxy = VertexColorVisualizationMaterialInstance;

											bDebugMaterialRenderProxySet = true;
										}

	#endif // STATICMESH_ENABLE_DEBUG_RENDERING
	#if WITH_EDITOR
										if (!bDebugMaterialRenderProxySet && bSectionIsSelected)
										{
											// Override the mesh's material with our material that draws the collision color
											MeshElement.MaterialRenderProxy = new FOverrideSelectionColorMaterialRenderProxy(
												GEngine->ShadedLevelColorationUnlitMaterial->GetRenderProxy(),
												GetSelectionColor(GEngine->GetSelectedMaterialColor(), bSectionIsSelected, IsHovered())
											);
										}
	#endif
										if (MeshElement.bDitheredLODTransition && LODMask.IsDithered())
										{

										}
										else
										{
											MeshElement.bDitheredLODTransition = false;
										}


									
										
										
										
										
										MeshElement.bCanApplyViewModeOverrides = true;
										MeshElement.bUseWireframeSelectionColoring = bSectionIsSelected;
										Collector.AddMesh(ViewIndex, MeshElement);
										INC_DWORD_STAT_BY(STAT_StaticMeshTriangles,MeshElement.GetNumPrimitives());
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
	

	
	

	FAnimMeshVertexFactory animMeshVertexFactory;
	
	UMaterialInterface* Material;

private:

	TArray<FMatrix> Buffer;
    
	//The shader resource view of the structured buffer, this is what we bind to the vertex factory shader
	FShaderResourceViewRHIRef BindMatricesSRV;
   
	//The structured buffer that will contain all the deform transoform and going to be used as a shader resource
	FStructuredBufferRHIRef BindMatricesSB;
	

	//Whether the structured buffer needs to be updated or not
	bool bBonesTransformsDirty;
};

class FAnimMeshVertexFactoryShaderParameters : public FVertexFactoryShaderParameters
{
	DECLARE_TYPE_LAYOUT(FAnimMeshVertexFactoryShaderParameters, NonVirtual);

	void Bind(const FShaderParameterMap& ParameterMap)
	{	
		RendererIndex.Bind(ParameterMap, TEXT("AMRendererIndex"), SPF_Optional);
		MatrixBufferSRV.Bind(ParameterMap, TEXT("AMMatrixBuffer"), SPF_Optional);
	};

	void GetElementShaderBindings(
            const class FSceneInterface* Scene,
            const FSceneView* View,
            const class FMeshMaterialShader* Shader,
            const EVertexInputStreamType InputStreamType,
            ERHIFeatureLevel::Type FeatureLevel,
            const FVertexFactory* VertexFactory,
            const FMeshBatchElement& BatchElement,
            class FMeshDrawSingleShaderBindings& ShaderBindings,
            FVertexInputStreamArray& VertexStreams
        ) const
	{
		if (BatchElement.bUserDataIsColorVertexBuffer)
		{
			const auto* LocalVertexFactory = static_cast<const FLocalVertexFactory*>(VertexFactory);
			FColorVertexBuffer* OverrideColorVertexBuffer = (FColorVertexBuffer*)BatchElement.UserData;
			check(OverrideColorVertexBuffer);

			if (!LocalVertexFactory->SupportsManualVertexFetch(FeatureLevel))
			{
				LocalVertexFactory->GetColorOverrideStream(OverrideColorVertexBuffer, VertexStreams);
			}
		}
		const FAnimMeshVertexFactory* AnimMeshVertexFactory = ((FAnimMeshVertexFactory*)VertexFactory);
	
		ShaderBindings.Add(RendererIndex, AnimMeshVertexFactory->RendererIndex);		
		ShaderBindings.Add(MatrixBufferSRV, AnimMeshVertexFactory->SceneProxy->GetDeformTransformsSRV());
	};
	
private:
	LAYOUT_FIELD(FShaderParameter, RendererIndex);
	LAYOUT_FIELD(FShaderResourceParameter, MatrixBufferSRV);
};



IMPLEMENT_TYPE_LAYOUT(FAnimMeshVertexFactoryShaderParameters);

///////////////////////////////////////////////////////////////////////

IMPLEMENT_VERTEX_FACTORY_PARAMETER_TYPE(FAnimMeshVertexFactory, SF_Vertex, FAnimMeshVertexFactoryShaderParameters);

IMPLEMENT_VERTEX_FACTORY_TYPE_EX(FAnimMeshVertexFactory, "/CustomShaders/LocalVertexFactory.ush", true, true, true, true, true,true,true);