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
		bSupportsManualVertexFetch = false;
		
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
		
		for (int i = 0; i < LODResources.Num(); ++i)
		{
			InitVertexFactory(LODResources[i],animMeshVertexFactory,0,Component->GetStaticMesh(),false);
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

	void InitVertexFactory(const FStaticMeshLODResources& LodResources,FAnimMeshVertexFactory& InOutVertexFactory,	uint32 LODIndex,const UStaticMesh* InParentMesh,bool bInOverrideColorVertexBuffer) const
	{
		check( InParentMesh != NULL );

		struct InitStaticMeshVertexFactoryParams
		{
			FAnimMeshVertexFactory* VertexFactory;
			const FStaticMeshLODResources* LODResources;
			bool bOverrideColorVertexBuffer;
			uint32 LightMapCoordinateIndex;
			uint32 LODIndex;
		} Params;

		uint32 LightMapCoordinateIndex = (uint32)InParentMesh->LightMapCoordinateIndex;
		LightMapCoordinateIndex = LightMapCoordinateIndex < LodResources.VertexBuffers.StaticMeshVertexBuffer.GetNumTexCoords() ? LightMapCoordinateIndex : LodResources.VertexBuffers.StaticMeshVertexBuffer.GetNumTexCoords() - 1;

		Params.VertexFactory = &InOutVertexFactory;
		Params.LODResources = &LodResources;
		Params.bOverrideColorVertexBuffer = bInOverrideColorVertexBuffer;
		Params.LightMapCoordinateIndex = LightMapCoordinateIndex;
		Params.LODIndex = LODIndex;

		// Initialize the static mesh's vertex factory.
		ENQUEUE_RENDER_COMMAND(InitStaticMeshVertexFactory)(
            [Params](FRHICommandListImmediate& RHICmdList)
            {
                FLocalVertexFactory::FDataType Data;

                Params.LODResources->VertexBuffers.PositionVertexBuffer.BindPositionVertexBuffer(Params.VertexFactory, Data);
                Params.LODResources->VertexBuffers.StaticMeshVertexBuffer.BindTangentVertexBuffer(Params.VertexFactory, Data);
                Params.LODResources->VertexBuffers.StaticMeshVertexBuffer.BindPackedTexCoordVertexBuffer(Params.VertexFactory, Data);
                Params.LODResources->VertexBuffers.StaticMeshVertexBuffer.BindLightMapVertexBuffer(Params.VertexFactory, Data, Params.LightMapCoordinateIndex);

                // bOverrideColorVertexBuffer means we intend to override the color later.  We must construct the vertexfactory such that it believes a proper stride (not 0) is set for
                // the color stream so that the real stream works later.
                if(Params.bOverrideColorVertexBuffer)
                { 
                    FColorVertexBuffer::BindDefaultColorVertexBuffer(Params.VertexFactory, Data, FColorVertexBuffer::NullBindStride::FColorSizeForComponentOverride);
                }
                //otherwise just bind the incoming buffer directly.
                else
                {
                    Params.LODResources->VertexBuffers.ColorVertexBuffer.BindColorVertexBuffer(Params.VertexFactory, Data);
                }

                Data.LODLightmapDataIndex = Params.LODIndex;
                Params.VertexFactory->SetData(Data);
                Params.VertexFactory->InitResource();
            });
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
								
									//	MeshElement.VertexFactory = &animMeshVertexFactory;
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
	
#if STATICMESH_ENABLE_DEBUG_RENDERING
	// Collision and bounds drawing
	FColor SimpleCollisionColor = FColor(157, 149, 223, 255);
	FColor ComplexCollisionColor = FColor(0, 255, 255, 255);

	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (VisibilityMap & (1 << ViewIndex))
		{
			if(AllowDebugViewmodes())
			{
				// Should we draw the mesh wireframe to indicate we are using the mesh as collision
				bool bDrawComplexWireframeCollision = (EngineShowFlags.Collision && IsCollisionEnabled() && CollisionTraceFlag == ECollisionTraceFlag::CTF_UseComplexAsSimple);
				// Requested drawing complex in wireframe, but check that we are not using simple as complex
				bDrawComplexWireframeCollision |= (bDrawMeshCollisionIfComplex && CollisionTraceFlag != ECollisionTraceFlag::CTF_UseSimpleAsComplex);
				// Requested drawing simple in wireframe, and we are using complex as simple
				bDrawComplexWireframeCollision |= (bDrawMeshCollisionIfSimple && CollisionTraceFlag == ECollisionTraceFlag::CTF_UseComplexAsSimple);

				// If drawing complex collision as solid or wireframe
				if(bDrawComplexWireframeCollision || (bInCollisionView && bDrawComplexCollision))
				{
					// If we have at least one valid LOD to draw
					if(RenderData->LODResources.Num() > 0)
					{
						// Get LOD used for collision
						int32 DrawLOD = FMath::Clamp(LODForCollision, 0, RenderData->LODResources.Num() - 1);
						const FStaticMeshLODResources& LODModel = RenderData->LODResources[DrawLOD];

						UMaterial* MaterialToUse = UMaterial::GetDefaultMaterial(MD_Surface);
						FLinearColor DrawCollisionColor = GetWireframeColor();
						// Collision view modes draw collision mesh as solid
						if(bInCollisionView)
						{
							MaterialToUse = GEngine->ShadedLevelColorationUnlitMaterial;
						}
						// Wireframe, choose color based on complex or simple
						else
						{
							MaterialToUse = GEngine->WireframeMaterial;
							DrawCollisionColor = (CollisionTraceFlag == ECollisionTraceFlag::CTF_UseComplexAsSimple) ? SimpleCollisionColor : ComplexCollisionColor;
						}

						// Iterate over sections of that LOD
						for (int32 SectionIndex = 0; SectionIndex < LODModel.Sections.Num(); SectionIndex++)
						{
							// If this section has collision enabled
							if (LODModel.Sections[SectionIndex].bEnableCollision)
							{
#if WITH_EDITOR
								// See if we are selected
								const bool bSectionIsSelected = LODs[DrawLOD].Sections[SectionIndex].bSelected;
#else
								const bool bSectionIsSelected = false;
#endif

								// Create colored proxy
								FColoredMaterialRenderProxy* CollisionMaterialInstance = new FColoredMaterialRenderProxy(MaterialToUse->GetRenderProxy(), DrawCollisionColor);
								Collector.RegisterOneFrameMaterialProxy(CollisionMaterialInstance);

								// Iterate over batches
								for (int32 BatchIndex = 0; BatchIndex < GetNumMeshBatches(); BatchIndex++)
								{
									FMeshBatch& CollisionElement = Collector.AllocateMesh();
									if (GetCollisionMeshElement(DrawLOD, BatchIndex, SectionIndex, SDPG_World, CollisionMaterialInstance, CollisionElement))
									{
										Collector.AddMesh(ViewIndex, CollisionElement);
										INC_DWORD_STAT_BY(STAT_StaticMeshTriangles, CollisionElement.GetNumPrimitives());
									}
								}
							}
						}
					}
				}
			}

			// Draw simple collision as wireframe if 'show collision', collision is enabled, and we are not using the complex as the simple
			const bool bDrawSimpleWireframeCollision = (EngineShowFlags.Collision && IsCollisionEnabled() && CollisionTraceFlag != ECollisionTraceFlag::CTF_UseComplexAsSimple);

			if((bDrawSimpleCollision || bDrawSimpleWireframeCollision) && BodySetup)
			{
				if(FMath::Abs(GetLocalToWorld().Determinant()) < SMALL_NUMBER)
				{
					// Catch this here or otherwise GeomTransform below will assert
					// This spams so commented out
					//UE_LOG(LogStaticMesh, Log, TEXT("Zero scaling not supported (%s)"), *StaticMesh->GetPathName());
				}
				else
				{
					const bool bDrawSolid = !bDrawSimpleWireframeCollision;

					if(AllowDebugViewmodes() && bDrawSolid)
					{
						// Make a material for drawing solid collision stuff
						auto SolidMaterialInstance = new FColoredMaterialRenderProxy(
							GEngine->ShadedLevelColorationUnlitMaterial->GetRenderProxy(),
							GetWireframeColor()
							);

						Collector.RegisterOneFrameMaterialProxy(SolidMaterialInstance);

						FTransform GeomTransform(GetLocalToWorld());
						BodySetup->AggGeom.GetAggGeom(GeomTransform, GetWireframeColor().ToFColor(true), SolidMaterialInstance, false, true, DrawsVelocity(), ViewIndex, Collector);
					}
					// wireframe
					else
					{
						FTransform GeomTransform(GetLocalToWorld());
						BodySetup->AggGeom.GetAggGeom(GeomTransform, GetSelectionColor(SimpleCollisionColor, bProxyIsSelected, IsHovered()).ToFColor(true), NULL, ( Owner == NULL ), false, DrawsVelocity(), ViewIndex, Collector);
					}


					// The simple nav geometry is only used by dynamic obstacles for now
					if (StaticMesh->NavCollision && StaticMesh->NavCollision->IsDynamicObstacle())
					{
						// Draw the static mesh's body setup (simple collision)
						FTransform GeomTransform(GetLocalToWorld());
						FColor NavCollisionColor = FColor(118,84,255,255);
						StaticMesh->NavCollision->DrawSimpleGeom(Collector.GetPDI(ViewIndex), GeomTransform, GetSelectionColor(NavCollisionColor, bProxyIsSelected, IsHovered()).ToFColor(true));
					}
				}
			}

			if(EngineShowFlags.MassProperties && DebugMassData.Num() > 0)
			{
				DebugMassData[0].DrawDebugMass(Collector.GetPDI(ViewIndex), FTransform(GetLocalToWorld()));
			}
	
			if (EngineShowFlags.StaticMeshes)
			{
				RenderBounds(Collector.GetPDI(ViewIndex), EngineShowFlags, GetBounds(), !Owner || IsSelected());
			}
		}
	}
#endif // STATICMESH_ENABLE_DEBUG_RENDERING
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