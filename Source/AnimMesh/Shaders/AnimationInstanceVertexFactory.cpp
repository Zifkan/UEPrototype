#include "AnimationInstanceVertexFactory.h"

#include "AnimVertexStaticMesh.h"
#include "RayTracingInstance.h"
#include "ProfilingDebugging/LoadTimeTracker.h"

IMPLEMENT_TYPE_LAYOUT(FAnimInstancedMeshVertexFactoryShaderParameters);


const int32 InstancedStaticMeshMaxTexCoord = 8;

//IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FInstancedStaticMeshVertexFactoryUniformShaderParameters, "InstanceVF");


TAutoConsoleVariable<int32> CVarMinLOD(
    TEXT("foliage.MinLOD"),
    -1,
    TEXT("Used to discard the top LODs for performance evaluation. -1: Disable all effects of this cvar."),
    ECVF_Scalability | ECVF_Default);

TAutoConsoleVariable<float> CVarFoliageMinimumScreenSize(
    TEXT("foliage.MinimumScreenSize"),
    0.000005f,
    TEXT("This controls the screen size at which we cull foliage instances entirely."),
    ECVF_Scalability);

TAutoConsoleVariable<float> CVarFoliageLODDistanceScale(
    TEXT("foliage.LODDistanceScale"),
    1.0f,
    TEXT("Scale factor for the distance used in computing LOD for foliage."));

TAutoConsoleVariable<float> CVarRandomLODRange(
    TEXT("foliage.RandomLODRange"),
    0.0f,
    TEXT("Random distance added to each instance distance to compute LOD."));



static TAutoConsoleVariable<int32> CVarCullAllInVertexShader(
    TEXT("foliage.CullAllInVertexShader"),
    0,
    TEXT("Debugging, if this is greater than 0, cull all instances in the vertex shader."));


static TAutoConsoleVariable<float> CVarRayTracingInstancesLowScaleCullRadius(
    TEXT("r.RayTracing.Geometry.InstancedStaticMeshes.LowScaleCullRadius"),
    1000.0f, 
    TEXT("Cull radius for small instances (default = 1000 (10m))"));


static TAutoConsoleVariable<float> CVarRayTracingInstancesCullClusterRadius(
    TEXT("r.RayTracing.Geometry.InstancedStaticMeshes.CullClusterRadius"),
    10000.0f, // 100 m
    TEXT("Ignore instances outside of this radius in ray tracing effects (default = 10000 (100m))"));

static TAutoConsoleVariable<float> CVarRayTracingInstancesLowScaleThreshold(
    TEXT("r.RayTracing.Geometry.InstancedStaticMeshes.LowScaleRadiusThreshold"),
    50.0f, // Instances with a radius smaller than this threshold get culled after CVarRayTracingInstancesLowScaleCullRadius
    TEXT("Threshold that classifies instances as small (default = 50cm))"));


static TAutoConsoleVariable<int32> CVarRayTracingRenderInstances(
    TEXT("r.RayTracing.Geometry.InstancedStaticMeshes"),
    1,
    TEXT("Include static mesh instances in ray tracing effects (default = 1 (Instances enabled in ray tracing))"));


static TAutoConsoleVariable<int32> CVarRayTracingRenderInstancesCulling(
    TEXT("r.RayTracing.Geometry.InstancedStaticMeshes.Culling"),
    1,
    TEXT("Enable culling for instances in ray tracing (default = 1 (Culling enabled))"));




void FAnimMeshInstanceVertexFactory::InitRHI()
{
	SCOPED_LOADTIMER(FInstancedStaticMeshVertexFactory_InitRHI);

	check(HasValidFeatureLevel());



	FVertexDeclarationElementList Elements;
	if(Data.PositionComponent.VertexBuffer != NULL)
	{
		Elements.Add(AccessStreamComponent(Data.PositionComponent,0));
	}

	// only tangent,normal are used by the stream. the binormal is derived in the shader
	uint8 TangentBasisAttributes[2] = { 1, 2 };
	for(int32 AxisIndex = 0;AxisIndex < 2;AxisIndex++)
	{
		if(Data.TangentBasisComponents[AxisIndex].VertexBuffer != NULL)
		{
			Elements.Add(AccessStreamComponent(Data.TangentBasisComponents[AxisIndex],TangentBasisAttributes[AxisIndex]));
		}
	}

	if (Data.ColorComponentsSRV == nullptr)
	{
		Data.ColorComponentsSRV = GNullColorVertexBuffer.VertexBufferSRV;
		Data.ColorIndexMask = 0;
	}

	if(Data.ColorComponent.VertexBuffer)
	{
		Elements.Add(AccessStreamComponent(Data.ColorComponent,3));
	}
	else
	{
		//If the mesh has no color component, set the null color buffer on a new stream with a stride of 0.
		//This wastes 4 bytes of bandwidth per vertex, but prevents having to compile out twice the number of vertex factories.
		FVertexStreamComponent NullColorComponent(&GNullColorVertexBuffer, 0, 0, VET_Color, EVertexStreamUsage::ManualFetch);
		Elements.Add(AccessStreamComponent(NullColorComponent, 3));
	}

	if(Data.TextureCoordinates.Num())
	{
		const int32 BaseTexCoordAttribute = 4;
		for(int32 CoordinateIndex = 0;CoordinateIndex < Data.TextureCoordinates.Num();CoordinateIndex++)
		{
			Elements.Add(AccessStreamComponent(
				Data.TextureCoordinates[CoordinateIndex],
				BaseTexCoordAttribute + CoordinateIndex
				));
		}

		for(int32 CoordinateIndex = Data.TextureCoordinates.Num(); CoordinateIndex < (InstancedStaticMeshMaxTexCoord + 1) / 2; CoordinateIndex++)
		{
			Elements.Add(AccessStreamComponent(
				Data.TextureCoordinates[Data.TextureCoordinates.Num() - 1],
				BaseTexCoordAttribute + CoordinateIndex
				));
		}
	}

	if(Data.LightMapCoordinateComponent.VertexBuffer)
	{
		Elements.Add(AccessStreamComponent(Data.LightMapCoordinateComponent,15));
	}
	else if(Data.TextureCoordinates.Num())
	{
		Elements.Add(AccessStreamComponent(Data.TextureCoordinates[0],15));
	}

	// toss in the instanced location stream
	check(Data.InstanceOriginComponent.VertexBuffer);
	if (Data.InstanceOriginComponent.VertexBuffer)
	{
		Elements.Add(AccessStreamComponent(Data.InstanceOriginComponent, 8));
	}

	check(Data.InstanceTransformComponent[0].VertexBuffer);
	if (Data.InstanceTransformComponent[0].VertexBuffer)
	{
		Elements.Add(AccessStreamComponent(Data.InstanceTransformComponent[0], 9));
		Elements.Add(AccessStreamComponent(Data.InstanceTransformComponent[1], 10));
		Elements.Add(AccessStreamComponent(Data.InstanceTransformComponent[2], 11));
	}

	if (Data.InstanceLightmapAndShadowMapUVBiasComponent.VertexBuffer)
	{
		Elements.Add(AccessStreamComponent(Data.InstanceLightmapAndShadowMapUVBiasComponent,12));
	}

	/*uint32 val = 1;
	TArray<uint32>TestBuffer;
	TestBuffer.Init(val,1);
	//We first create a resource array to use it in the create info for initializing the structured buffer on creation
	TResourceArray<uint32>* ResourceArray = new TResourceArray<uint32>(false);
	FRHIResourceCreateInfo CreateInfo;
	ResourceArray->Append(TestBuffer);
	CreateInfo.ResourceArray = ResourceArray;
	//Set the debug name so we can find the resource when debugging in RenderDoc
	CreateInfo.DebugName = TEXT("DeformMesh_TransformsSB");

	FVertexBuffer TestVertexBuffer;
	auto AccessFlags = BUF_Static;
	TestVertexBuffer.VertexBufferRHI = RHICreateVertexBuffer(1,AccessFlags | BUF_ShaderResource, CreateInfo);
	///////////////////////////////////////////////////////////////
	//// CREATING AN SRV FOR THE STRUCTUED BUFFER SO WA CAN USE IT AS A SHADER RESOURCE PARAMETER AND BIND IT TO THE VERTEX FACTORY
	FShaderResourceViewRHIRef testScaleBuffer = RHICreateShaderResourceView(TestVertexBuffer.VertexBufferRHI,32,PF_R32_UINT);


	
	FVertexStreamComponent testScale = FVertexStreamComponent(
            &TestVertexBuffer,
            0,
            8,
            VET_Short4N,
            EVertexStreamUsage::ManualFetch | EVertexStreamUsage::Instancing
        );
	Elements.Add(AccessStreamComponent(testScale,4));
	*/

	

	
	//Elements.Add(AccessStreamComponent(BoneIndices,3));

	// bone weights decls
	//Elements.Add(AccessStreamComponent(BoneWeights,4));

	// Extra bone indices & weights decls
	/*if (GetNumBoneInfluences() > MAX_INFLUENCES_PER_STREAM)
	{
		Elements.Add(AccessStreamComponent(ExtraBoneIndices, 14));
		Elements.Add(AccessStreamComponent(ExtraBoneWeights, 15));
	}
	else
	{
		Elements.Add(AccessStreamComponent(BoneIndices, 14));
		Elements.Add(AccessStreamComponent(BoneWeights, 15));
	}*/


	if (SceneProxy->SkinWeightVertexBuffer != NULL && SceneProxy->SkinWeightVertexBuffer->GetNumVertices()>0)
	{
		const FSkinWeightDataVertexBuffer* WeightDataVertexBuffer = SceneProxy->SkinWeightVertexBuffer->GetDataVertexBuffer();
		const uint32 Stride = SceneProxy->SkinWeightVertexBuffer->GetConstantInfluencesVertexStride();
		const uint32 WeightsOffset = SceneProxy->SkinWeightVertexBuffer->GetConstantInfluencesBoneWeightsOffset();
		auto BoneIndices = FVertexStreamComponent(WeightDataVertexBuffer, 0, Stride, /*SceneProxy->SkinWeightVertexBuffer->Use16BitBoneIndex() ? VET_UShort4 : */VET_UByte4);
		auto BoneWeights = FVertexStreamComponent(WeightDataVertexBuffer, WeightsOffset, Stride, VET_UByte4N);

	

	/*	auto currentBonesInfluence = SceneProxy->SkinWeightVertexBuffer->GetMaxBoneInfluences();
		uint32 vertexCount = SceneProxy->SkinWeightVertexBuffer->GetNumVertices();//123456;

		TArray<FVector4> weightsArray;
		TArray<FVector4> indicesArray;
		for (uint32 i = 0; i < vertexCount; ++i)
		{
			FVector4 weights;
			FVector4 indices;
			for (uint32 j = 0; j < currentBonesInfluence; ++j)
			{			
				weights[j] =  static_cast<double>(static_cast<int>(SceneProxy->SkinWeightVertexBuffer->GetBoneWeight(i,j))/255.0);
				indices[j] = static_cast<int>(SceneProxy->SkinWeightVertexBuffer->GetBoneIndex(i,j));
			}

			weightsArray.Add(weights);
			indicesArray.Add(indices);
		}
	
		TResourceArray<FVector4>* indicesResourceArray = new TResourceArray<FVector4>(true);
		FRHIResourceCreateInfo indicesCreateInfo;
		indicesResourceArray->Append(weightsArray);
		indicesCreateInfo.ResourceArray = indicesResourceArray;
		indicesCreateInfo.DebugName = TEXT("indicesResourceArray");
		auto indicesBuffer =  RHICreateVertexBuffer( sizeof(FVector4)*vertexCount, (BUF_Dynamic | BUF_ShaderResource), indicesCreateInfo ); 	
		FVertexBuffer indicesVertBuffer;
		//indicesVertBuffer.InitResource();
	


		indicesVertBuffer.ReleaseRHI();
		indicesVertBuffer.ReleaseDynamicRHI();
		indicesVertBuffer.InitDynamicRHI();
		indicesVertBuffer.InitRHI();
		indicesVertBuffer.VertexBufferRHI = indicesBuffer;
		
	//	auto BoneIndices = FVertexStreamComponent(&indicesVertBuffer, 0, currentBonesInfluence,SceneProxy->SkinWeightVertexBuffer->Use16BitBoneIndex() ? VET_UShort4 : VET_UByte4);


		TResourceArray<FVector4>* weightsResourceArray = new TResourceArray<FVector4>(true);
		FRHIResourceCreateInfo weightsCreateInfo;
		weightsResourceArray->Append(weightsArray);
		weightsCreateInfo.ResourceArray = weightsResourceArray;
		weightsCreateInfo.DebugName = TEXT("WeightsResourceArray");
		auto boneWeightsBuffer =  RHICreateVertexBuffer( sizeof(FVector4)*vertexCount, (BUF_Dynamic | BUF_ShaderResource), weightsCreateInfo );
		FVertexBuffer boneWeightsVertBuffer;
		//boneWeightsVertBuffer.InitResource();
	

		boneWeightsVertBuffer.ReleaseRHI();
		boneWeightsVertBuffer.ReleaseDynamicRHI();
		boneWeightsVertBuffer.InitDynamicRHI();
		boneWeightsVertBuffer.InitRHI();
		boneWeightsVertBuffer.VertexBufferRHI = boneWeightsBuffer;
	//	auto BoneWeights = FVertexStreamComponent(&boneWeightsVertBuffer, 0, currentBonesInfluence, VET_Float4);
		*/
		Elements.Add(AccessStreamComponent(BoneIndices, 13));
		Elements.Add(AccessStreamComponent(BoneWeights, 14));
	}	
	
	// we don't need per-vertex shadow or lightmap rendering
	InitDeclaration(Elements);

	{
		FInstancedStaticMeshVertexFactoryUniformShaderParameters UniformParameters;
		UniformParameters.VertexFetch_InstanceOriginBuffer = GetInstanceOriginSRV();
		UniformParameters.VertexFetch_InstanceTransformBuffer = GetInstanceTransformSRV();
		UniformParameters.VertexFetch_InstanceLightmapBuffer = GetInstanceLightmapSRV();
		UniformParameters.InstanceCustomDataBuffer = GetInstanceCustomDataSRV();
		UniformParameters.NumCustomDataFloats = Data.NumCustomDataFloats;
		UniformBuffer = TUniformBufferRef<FInstancedStaticMeshVertexFactoryUniformShaderParameters>::CreateUniformBufferImmediate(UniformParameters, UniformBuffer_MultiFrame, EUniformBufferValidation::None);
	}
}



void FAnimInstancedMeshVertexFactoryShaderParameters::GetElementShaderBindings(const FSceneInterface* Scene,
	const FSceneView* View, const FMeshMaterialShader* Shader, const EVertexInputStreamType InputStreamType,
	ERHIFeatureLevel::Type FeatureLevel, const FVertexFactory* VertexFactory, const FMeshBatchElement& BatchElement,
	FMeshDrawSingleShaderBindings& ShaderBindings, FVertexInputStreamArray& VertexStreams) const
{
	// Decode VertexFactoryUserData as VertexFactoryUniformBuffer
	FRHIUniformBuffer* VertexFactoryUniformBuffer = static_cast<FRHIUniformBuffer*>(BatchElement.VertexFactoryUserData);
	FLocalVertexFactoryShaderParametersBase::GetElementShaderBindingsBase(Scene, View, Shader, InputStreamType, FeatureLevel, VertexFactory, BatchElement, VertexFactoryUniformBuffer, ShaderBindings, VertexStreams);

	const FInstancingUserData* InstancingUserData = (const FInstancingUserData*)BatchElement.UserData;
	const auto* InstancedVertexFactory = static_cast<const FAnimMeshInstanceVertexFactory*>(VertexFactory);
	const int32 InstanceOffsetValue = BatchElement.UserIndex;

	ShaderBindings.Add(Shader->GetUniformBufferParameter<FInstancedStaticMeshVertexFactoryUniformShaderParameters>(), InstancedVertexFactory->GetUniformBuffer());
	ShaderBindings.Add(InstanceOffset, InstanceOffsetValue);

	if (InstancedVertexFactory->SupportsManualVertexFetch(FeatureLevel))
	{
		ShaderBindings.Add(VertexFetch_InstanceOriginBufferParameter, InstancedVertexFactory->GetInstanceOriginSRV());
		ShaderBindings.Add(VertexFetch_InstanceTransformBufferParameter, InstancedVertexFactory->GetInstanceTransformSRV());
		ShaderBindings.Add(VertexFetch_InstanceLightmapBufferParameter, InstancedVertexFactory->GetInstanceLightmapSRV());
	}
	if (InstanceOffsetValue > 0 && VertexStreams.Num() > 0)
	{
		VertexFactory->OffsetInstanceStreams(InstanceOffsetValue, InputStreamType, VertexStreams);
	}

	if( InstancingWorldViewOriginOneParameter.IsBound() )
	{
		FVector4 InstancingViewZCompareZero(MIN_flt, MIN_flt, MAX_flt, 1.0f);
		FVector4 InstancingViewZCompareOne(MIN_flt, MIN_flt, MAX_flt, 0.0f);
		FVector4 InstancingViewZConstant(ForceInit);
		FVector4 InstancingOffset(ForceInit);
		FVector4 InstancingWorldViewOriginZero(ForceInit);
		FVector4 InstancingWorldViewOriginOne(ForceInit);
		InstancingWorldViewOriginOne.W = 1.0f;
		if (InstancingUserData && BatchElement.InstancedLODRange)
		{
			int32 FirstLOD = InstancingUserData->MinLOD;

			int32 DebugMin = FMath::Min(CVarMinLOD.GetValueOnRenderThread(), InstancingUserData->MeshRenderData->LODResources.Num() - 1);
			if (DebugMin >= 0)
			{
				FirstLOD = FMath::Max(FirstLOD, DebugMin);
			}

			FBoxSphereBounds ScaledBounds = InstancingUserData->MeshRenderData->Bounds.TransformBy(FTransform(FRotator::ZeroRotator, FVector::ZeroVector, InstancingUserData->AverageInstancesScale));
			float SphereRadius = ScaledBounds.SphereRadius;
			float MinSize = View->ViewMatrices.IsPerspectiveProjection() ? CVarFoliageMinimumScreenSize.GetValueOnRenderThread() : 0.0f;
			float LODScale = CVarFoliageLODDistanceScale.GetValueOnRenderThread();
			float LODRandom = CVarRandomLODRange.GetValueOnRenderThread();
			float MaxDrawDistanceScale = GetCachedScalabilityCVars().ViewDistanceScale;

			if (BatchElement.InstancedLODIndex)
			{
				InstancingViewZConstant.X = -1.0f;
			}
			else
			{
				// this is the first LOD, so we don't have a fade-in region
				InstancingViewZConstant.X = 0.0f;
			}
			InstancingViewZConstant.Y = 0.0f;
			InstancingViewZConstant.Z = 1.0f;

			// now we subtract off the lower segments, since they will be incorporated 
			InstancingViewZConstant.Y -= InstancingViewZConstant.X;
			InstancingViewZConstant.Z -= InstancingViewZConstant.X + InstancingViewZConstant.Y;
			//not using W InstancingViewZConstant.W -= InstancingViewZConstant.X + InstancingViewZConstant.Y + InstancingViewZConstant.Z;

			for (int32 SampleIndex = 0; SampleIndex < 2; SampleIndex++)
			{
				FVector4& InstancingViewZCompare(SampleIndex ? InstancingViewZCompareOne : InstancingViewZCompareZero);

				float FinalCull = MAX_flt;
				if (MinSize > 0.0)
				{
					FinalCull = ComputeBoundsDrawDistance(MinSize, SphereRadius, View->ViewMatrices.GetProjectionMatrix()) * LODScale;
				}
				if (InstancingUserData->EndCullDistance > 0.0f)
				{
					FinalCull = FMath::Min(FinalCull, InstancingUserData->EndCullDistance * MaxDrawDistanceScale);
				}
				FinalCull *= MaxDrawDistanceScale;

				InstancingViewZCompare.Z = FinalCull;
				if (int(BatchElement.InstancedLODIndex) < InstancingUserData->MeshRenderData->LODResources.Num() - 1)
				{
					float NextCut = ComputeBoundsDrawDistance(InstancingUserData->MeshRenderData->ScreenSize[BatchElement.InstancedLODIndex + 1].GetValue(), SphereRadius, View->ViewMatrices.GetProjectionMatrix()) * LODScale;
					InstancingViewZCompare.Z = FMath::Min(NextCut, FinalCull);
				}

				InstancingViewZCompare.X = MIN_flt;
				if (int(BatchElement.InstancedLODIndex) > FirstLOD)
				{
					float CurCut = ComputeBoundsDrawDistance(InstancingUserData->MeshRenderData->ScreenSize[BatchElement.InstancedLODIndex].GetValue(), SphereRadius, View->ViewMatrices.GetProjectionMatrix()) * LODScale;
					if (CurCut < FinalCull)
					{
						InstancingViewZCompare.Y = CurCut;
					}
					else
					{
						// this LOD is completely removed by one of the other two factors
						InstancingViewZCompare.Y = MIN_flt;
						InstancingViewZCompare.Z = MIN_flt;
					}
				}
				else
				{
					// this is the first LOD, so we don't have a fade-in region
					InstancingViewZCompare.Y = MIN_flt;
				}
			}

			InstancingOffset = InstancingUserData->InstancingOffset;
			InstancingWorldViewOriginZero = View->GetTemporalLODOrigin(0);
			InstancingWorldViewOriginOne = View->GetTemporalLODOrigin(1);

			float Alpha = View->GetTemporalLODTransition();
			InstancingWorldViewOriginZero.W = 1.0f - Alpha;
			InstancingWorldViewOriginOne.W = Alpha;

			InstancingViewZCompareZero.W = LODRandom;
		}

		ShaderBindings.Add(InstancingViewZCompareZeroParameter, InstancingViewZCompareZero);
		ShaderBindings.Add(InstancingViewZCompareOneParameter, InstancingViewZCompareOne);
		ShaderBindings.Add(InstancingViewZConstantParameter, InstancingViewZConstant);
		ShaderBindings.Add(InstancingOffsetParameter, InstancingOffset);
		ShaderBindings.Add(InstancingWorldViewOriginZeroParameter, InstancingWorldViewOriginZero);
		ShaderBindings.Add(InstancingWorldViewOriginOneParameter, InstancingWorldViewOriginOne);
	}

	if( InstancingFadeOutParamsParameter.IsBound() )
	{
		FVector4 InstancingFadeOutParams(MAX_flt,0.f,1.f,1.f);
		if (InstancingUserData)
		{
			const float MaxDrawDistanceScale = GetCachedScalabilityCVars().ViewDistanceScale;
			const float StartDistance = InstancingUserData->StartCullDistance * MaxDrawDistanceScale;
			const float EndDistance = InstancingUserData->EndCullDistance * MaxDrawDistanceScale;

			InstancingFadeOutParams.X = StartDistance;
			if( EndDistance > 0 )
			{
				if( EndDistance > StartDistance )
				{
					InstancingFadeOutParams.Y = 1.f / (float)(EndDistance - StartDistance);
				}
				else
				{
					InstancingFadeOutParams.Y = 1.f;
				}
			}
			else
			{
				InstancingFadeOutParams.Y = 0.f;
			}
			if (CVarCullAllInVertexShader.GetValueOnRenderThread() > 0)
			{
				InstancingFadeOutParams.Z = 0.0f;
				InstancingFadeOutParams.W = 0.0f;
			}
			else
			{
				InstancingFadeOutParams.Z = InstancingUserData->bRenderSelected ? 1.f : 0.f;
				InstancingFadeOutParams.W = InstancingUserData->bRenderUnselected ? 1.f : 0.f;
			}
		}

		ShaderBindings.Add(InstancingFadeOutParamsParameter, InstancingFadeOutParams);

	}
	check(InstancedVertexFactory);
	ShaderBindings.Add(Instancing_MatrixBufferSRVParameter, InstancedVertexFactory->SceneProxy->MatrixBufferSRV);

	ShaderBindings.Add(InputWeightIndexSize, InstancedVertexFactory->SceneProxy->BoneIndicesSRV);

	if (InstancedVertexFactory->SceneProxy->BoneWeightsSRV!=nullptr)
	{
		ShaderBindings.Add(InputWeightStream, InstancedVertexFactory->SceneProxy->BoneWeightsSRV);
	}
}



///////////////////////////////////////////////////////////////////////

IMPLEMENT_VERTEX_FACTORY_PARAMETER_TYPE(FAnimMeshInstanceVertexFactory, SF_Vertex, FAnimInstancedMeshVertexFactoryShaderParameters);
#if RHI_RAYTRACING
IMPLEMENT_VERTEX_FACTORY_PARAMETER_TYPE(FAnimMeshInstanceVertexFactory, SF_RayHitGroup, FAnimInstancedMeshVertexFactoryShaderParameters);
#endif
IMPLEMENT_VERTEX_FACTORY_TYPE_EX(FAnimMeshInstanceVertexFactory, "/CustomShaders/CustomLocalVertexFactory.ush", true, true, true, true, false,true,false);

/**######################################*/


SIZE_T FAnimationInstanceVertexSceneProxy::GetTypeHash() const
{
	static size_t UniquePointer;
	return reinterpret_cast<size_t>(&UniquePointer);
}

void FAnimationInstanceVertexSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_InstancedStaticMeshSceneProxy_GetMeshElements);

	const bool bSelectionRenderEnabled = GIsEditor && ViewFamily.EngineShowFlags.Selection;

	// If the first pass rendered selected instances only, we need to render the deselected instances in a second pass
	const int32 NumSelectionGroups = (bSelectionRenderEnabled && bHasSelectedInstances) ? 2 : 1;

	const FInstancingUserData* PassUserData[2] =
	{
		bHasSelectedInstances && bSelectionRenderEnabled ? &UserData_SelectedInstances : &UserData_AllInstances,
		&UserData_DeselectedInstances
	};

	bool BatchRenderSelection[2] = 
	{
		bSelectionRenderEnabled && IsSelected(),
		false
	};

	const bool bIsWireframe = ViewFamily.EngineShowFlags.Wireframe;

	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (VisibilityMap & (1 << ViewIndex))
		{
			const FSceneView* View = Views[ViewIndex];

			for (int32 SelectionGroupIndex = 0; SelectionGroupIndex < NumSelectionGroups; SelectionGroupIndex++)
			{
				const int32 LODIndex = GetLOD(View);
				const FStaticMeshLODResources& LODModel = StaticMesh->RenderData->LODResources[LODIndex];

				for (int32 SectionIndex = 0; SectionIndex < LODModel.Sections.Num(); SectionIndex++)
				{
					const int32 NumBatches = GetNumMeshBatches();

					for (int32 BatchIndex = 0; BatchIndex < NumBatches; BatchIndex++)
					{
						FMeshBatch& MeshElement = Collector.AllocateMesh();

						if (GetMeshElement(LODIndex, BatchIndex, SectionIndex, GetDepthPriorityGroup(View), BatchRenderSelection[SelectionGroupIndex], true, MeshElement))
						{
							//@todo-rco this is only supporting selection on the first element
							MeshElement.Elements[0].UserData = PassUserData[SelectionGroupIndex];
							MeshElement.Elements[0].bUserDataIsColorVertexBuffer = false;
							MeshElement.bCanApplyViewModeOverrides = true;
							MeshElement.bUseSelectionOutline = BatchRenderSelection[SelectionGroupIndex];
							MeshElement.bUseWireframeSelectionColoring = BatchRenderSelection[SelectionGroupIndex];
							if (View->bRenderFirstInstanceOnly)
							{
								for (int32 ElementIndex = 0; ElementIndex < MeshElement.Elements.Num(); ElementIndex++)
								{
									MeshElement.Elements[ElementIndex].NumInstances = FMath::Min<uint32>(MeshElement.Elements[ElementIndex].NumInstances, 1);
								}
							}

							Collector.AddMesh(ViewIndex, MeshElement);
							INC_DWORD_STAT_BY(STAT_StaticMeshTriangles, MeshElement.GetNumPrimitives());
						}
					}
				}
			}
		}
	}
}

int32 FAnimationInstanceVertexSceneProxy::CollectOccluderElements(FOccluderElementsCollector& Collector) const
{
	if (OccluderData)
	{	
		FStaticMeshInstanceBuffer& InstanceBuffer = InstancedRenderData.PerInstanceRenderData->InstanceBuffer;
		const int32 NumInstances = InstanceBuffer.GetNumInstances();
		
		for (int32 InstanceIndex = 0; InstanceIndex < NumInstances; ++InstanceIndex)
		{
			FMatrix InstanceToLocal;
			InstanceBuffer.GetInstanceTransform(InstanceIndex, InstanceToLocal);	
			InstanceToLocal.M[3][3] = 1.0f;
						
			Collector.AddElements(OccluderData->VerticesSP, OccluderData->IndicesSP, InstanceToLocal * GetLocalToWorld());
		}
		
		return NumInstances;
	}
	
	return 0;
}

void FAnimationInstanceVertexSceneProxy::SetupProxy(UInstancedStaticMeshComponent* InComponent)
{
#if WITH_EDITOR
	if (bHasSelectedInstances)
	{
		// if we have selected indices, mark scene proxy as selected.
		SetSelection_GameThread(true);
	}
#endif
	// Make sure all the materials are okay to be rendered as an instanced mesh.
	for (int32 LODIndex = 0; LODIndex < LODs.Num(); LODIndex++)
	{
		FStaticMeshSceneProxy::FLODInfo& LODInfo = LODs[LODIndex];
		for (int32 SectionIndex = 0; SectionIndex < LODInfo.Sections.Num(); SectionIndex++)
		{
			FStaticMeshSceneProxy::FLODInfo::FSectionInfo& Section = LODInfo.Sections[SectionIndex];
			if (!Section.Material->CheckMaterialUsage_Concurrent(MATUSAGE_InstancedStaticMeshes))
			{
				Section.Material = UMaterial::GetDefaultMaterial(MD_Surface);
			}
		}
	}

	// Copy the parameters for LOD - all instances
	UserData_AllInstances.MeshRenderData = InComponent->GetStaticMesh()->RenderData.Get();
	UserData_AllInstances.StartCullDistance = InComponent->InstanceStartCullDistance;
	UserData_AllInstances.EndCullDistance = InComponent->InstanceEndCullDistance;
	UserData_AllInstances.InstancingOffset = InComponent->GetStaticMesh()->GetBoundingBox().GetCenter();
	UserData_AllInstances.MinLOD = ClampedMinLOD;
	UserData_AllInstances.bRenderSelected = true;
	UserData_AllInstances.bRenderUnselected = true;
	UserData_AllInstances.RenderData = nullptr;

	FVector MinScale(0);
	FVector MaxScale(0);
	InComponent->GetInstancesMinMaxScale(MinScale, MaxScale);

	UserData_AllInstances.AverageInstancesScale = MinScale + (MaxScale - MinScale) / 2.0f;

	// selected only
	UserData_SelectedInstances = UserData_AllInstances;
	UserData_SelectedInstances.bRenderUnselected = false;

	// unselected only
	UserData_DeselectedInstances = UserData_AllInstances;
	UserData_DeselectedInstances.bRenderSelected = false;

//	MatrixBuffer.AddZeroed(1);
	MatrixBuffer.Init(FMatrix(FVector(1,1,1),FVector(1,1,1),FVector(1,1,1),FVector(1,1,1)),256*16);
	//We first create a resource array to use it in the create info for initializing the structured buffer on creation
	TResourceArray<FMatrix>* ResourceArray = new TResourceArray<FMatrix>(true);
	FRHIResourceCreateInfo CreateInfo;
	ResourceArray->Append(MatrixBuffer);
	CreateInfo.ResourceArray = ResourceArray;
	//Set the debug name so we can find the resource when debugging in RenderDoc
	CreateInfo.DebugName = TEXT("DeformMesh_TransformsSB");

	MatrixBufferSB = RHICreateStructuredBuffer(sizeof(FMatrix), 256*16 * sizeof(FMatrix), BUF_ShaderResource, CreateInfo);
	bMatrixBufferDirty = false;
	///////////////////////////////////////////////////////////////
	//// CREATING AN SRV FOR THE STRUCTUED BUFFER SO WA CAN USE IT AS A SHADER RESOURCE PARAMETER AND BIND IT TO THE VERTEX FACTORY
	MatrixBufferSRV = RHICreateShaderResourceView(MatrixBufferSB);

	///////////////////////////////////////////////////////////////

	auto animVertexStaticMesh = static_cast<UAnimVertexStaticMesh*>(InComponent->GetStaticMesh());

	UE_LOG(LogTemp, Warning, TEXT("GetNumBones: %i"),animVertexStaticMesh->TestValue);
	UE_LOG(LogTemp, Warning, TEXT("GetNumVertices: %i"),animVertexStaticMesh->SkinWeightVertexBuffer.GetNumVertices());
	
	
	BoneIndicesSRV = animVertexStaticMesh->BoneIndices;
	BoneWeightsSRV = animVertexStaticMesh->SkinWeightVertexBuffer.GetDataVertexBuffer()->GetSRV();
	
}

void FAnimationInstanceVertexSceneProxy::DestroyRenderThreadResources()
{
	InstancedRenderData.ReleaseResources(&GetScene(), StaticMesh);
	FStaticMeshSceneProxy::DestroyRenderThreadResources();
}

void FAnimationInstanceVertexSceneProxy::SetupInstancedMeshBatch(int32 LODIndex, int32 BatchIndex, FMeshBatch& OutMeshBatch) const
{
	OutMeshBatch.VertexFactory = &InstancedRenderData.VertexFactories[LODIndex];
	const uint32 NumInstances = InstancedRenderData.PerInstanceRenderData->InstanceBuffer.GetNumInstances();
	FMeshBatchElement& BatchElement0 = OutMeshBatch.Elements[0];
	BatchElement0.UserData = (void*)&UserData_AllInstances;
	BatchElement0.bUserDataIsColorVertexBuffer = false;
	BatchElement0.InstancedLODIndex = LODIndex;
	BatchElement0.UserIndex = 0;
	BatchElement0.PrimitiveUniformBuffer = GetUniformBuffer();

	BatchElement0.NumInstances = NumInstances;
}

void FAnimationInstanceVertexSceneProxy::GetLightRelevance(const FLightSceneProxy* LightSceneProxy, bool& bDynamic, bool& bRelevant, bool& bLightMapped, bool& bShadowMapped) const
{
	FStaticMeshSceneProxy::GetLightRelevance(LightSceneProxy, bDynamic, bRelevant, bLightMapped, bShadowMapped);

	if (InstancedRenderData.PerInstanceRenderData->InstanceBuffer.GetNumInstances() == 0)
	{
		bRelevant = false;
	}
}

bool FAnimationInstanceVertexSceneProxy::GetShadowMeshElement(int32 LODIndex, int32 BatchIndex, uint8 InDepthPriorityGroup, FMeshBatch& OutMeshBatch, bool bDitheredLODTransition) const
{
	if (LODIndex < InstancedRenderData.VertexFactories.Num() && FStaticMeshSceneProxy::GetShadowMeshElement(LODIndex, BatchIndex, InDepthPriorityGroup, OutMeshBatch, bDitheredLODTransition))
	{
		SetupInstancedMeshBatch(LODIndex, BatchIndex, OutMeshBatch);
		return true;
	}
	return false;
}

/** Sets up a FMeshBatch for a specific LOD and element. */
bool FAnimationInstanceVertexSceneProxy::GetMeshElement(int32 LODIndex, int32 BatchIndex, int32 ElementIndex, uint8 InDepthPriorityGroup, bool bUseSelectionOutline, bool bAllowPreCulledIndices, FMeshBatch& OutMeshBatch) const
{
	if (LODIndex < InstancedRenderData.VertexFactories.Num() && FStaticMeshSceneProxy::GetMeshElement(LODIndex, BatchIndex, ElementIndex, InDepthPriorityGroup, bUseSelectionOutline, bAllowPreCulledIndices, OutMeshBatch))
	{
		SetupInstancedMeshBatch(LODIndex, BatchIndex, OutMeshBatch);
		return true;
	}
	return false;
};

/** Sets up a wireframe FMeshBatch for a specific LOD. */
bool FAnimationInstanceVertexSceneProxy::GetWireframeMeshElement(int32 LODIndex, int32 BatchIndex, const FMaterialRenderProxy* WireframeRenderProxy, uint8 InDepthPriorityGroup, bool bAllowPreCulledIndices, FMeshBatch& OutMeshBatch) const
{
	if (LODIndex < InstancedRenderData.VertexFactories.Num() && FStaticMeshSceneProxy::GetWireframeMeshElement(LODIndex, BatchIndex, WireframeRenderProxy, InDepthPriorityGroup, bAllowPreCulledIndices, OutMeshBatch))
	{
		SetupInstancedMeshBatch(LODIndex, BatchIndex, OutMeshBatch);
		return true;
	}
	return false;
}

void FAnimationInstanceVertexSceneProxy::GetDistancefieldAtlasData(FBox& LocalVolumeBounds, FVector2D& OutDistanceMinMax, FIntVector& OutBlockMin, FIntVector& OutBlockSize, bool& bOutBuiltAsIfTwoSided, bool& bMeshWasPlane, float& SelfShadowBias, TArray<FMatrix>& ObjectLocalToWorldTransforms, bool& bOutThrottled) const
{
	FStaticMeshSceneProxy::GetDistancefieldAtlasData(LocalVolumeBounds, OutDistanceMinMax, OutBlockMin, OutBlockSize, bOutBuiltAsIfTwoSided, bMeshWasPlane, SelfShadowBias, ObjectLocalToWorldTransforms, bOutThrottled);

	ObjectLocalToWorldTransforms.Reset();

	const uint32 NumInstances = InstancedRenderData.PerInstanceRenderData->InstanceBuffer.GetNumInstances();
	for (uint32 InstanceIndex = 0; InstanceIndex < NumInstances; InstanceIndex++)
	{
		FMatrix InstanceToLocal;
		InstancedRenderData.PerInstanceRenderData->InstanceBuffer.GetInstanceTransform(InstanceIndex, InstanceToLocal);	
		InstanceToLocal.M[3][3] = 1.0f;

		ObjectLocalToWorldTransforms.Add(InstanceToLocal * GetLocalToWorld());
	}
}

void FAnimationInstanceVertexSceneProxy::GetDistanceFieldInstanceInfo(int32& NumInstances, float& BoundsSurfaceArea) const
{
	NumInstances = DistanceFieldData ? InstancedRenderData.PerInstanceRenderData->InstanceBuffer.GetNumInstances() : 0;

	if (NumInstances > 0)
	{
		FMatrix InstanceToLocal;
		const int32 InstanceIndex = 0;
		InstancedRenderData.PerInstanceRenderData->InstanceBuffer.GetInstanceTransform(InstanceIndex, InstanceToLocal);
		InstanceToLocal.M[3][3] = 1.0f;

		const FMatrix InstanceTransform = InstanceToLocal * GetLocalToWorld();
		const FVector AxisScales = InstanceTransform.GetScaleVector();
		const FVector BoxDimensions = RenderData->Bounds.BoxExtent * AxisScales * 2;

		BoundsSurfaceArea = 2 * BoxDimensions.X * BoxDimensions.Y
			+ 2 * BoxDimensions.Z * BoxDimensions.Y
			+ 2 * BoxDimensions.X * BoxDimensions.Z;
	}
}

HHitProxy* FAnimationInstanceVertexSceneProxy::CreateHitProxies(UPrimitiveComponent* Component,TArray<TRefCountPtr<HHitProxy> >& OutHitProxies)
{
	if(InstancedRenderData.PerInstanceRenderData.IsValid() && InstancedRenderData.PerInstanceRenderData->HitProxies.Num() )
	{
		// Add any per-instance hit proxies.
		OutHitProxies += InstancedRenderData.PerInstanceRenderData->HitProxies;

		// No default hit proxy.
		return nullptr;
	}

	return FStaticMeshSceneProxy::CreateHitProxies(Component, OutHitProxies);
}


void FAnimationInstanceVertexSceneProxy::UpdateMatrixBufferSB_RenderThread()
{
	check(IsInRenderingThread());
	//Update the structured buffer only if it needs update
	if(bMatrixBufferDirty)
	{
		void* StructuredBufferData = RHILockStructuredBuffer(MatrixBufferSB, 0, MatrixBuffer.Num() * sizeof(FMatrix), RLM_WriteOnly);
		FMemory::Memcpy(StructuredBufferData, MatrixBuffer.GetData(), MatrixBuffer.Num() * sizeof(FMatrix));
		RHIUnlockStructuredBuffer(MatrixBufferSB);
		bMatrixBufferDirty = false;
	}
}

void FAnimationInstanceVertexSceneProxy::UpdateBoneMatrixBuffer_RenderThread(TArray<FMatrix> Array)
{
	check(IsInRenderingThread());
	bMatrixBufferDirty = true;
	MatrixBuffer = Array;
}


void FAnimMeshInstancedStaticMeshRenderData::InitVertexFactories(FAnimationInstanceVertexSceneProxy* SceneProxy)
{
	// Allocate the vertex factories for each LOD
	for (int32 LODIndex = 0; LODIndex < LODModels.Num(); LODIndex++)
	{
		auto factory = new FAnimMeshInstanceVertexFactory(FeatureLevel);
		
		factory->SceneProxy = SceneProxy;
		VertexFactories.Add(factory);
	}

	const int32 LightMapCoordinateIndex = Component->GetStaticMesh()->LightMapCoordinateIndex;
	ENQUEUE_RENDER_COMMAND(InstancedStaticMeshRenderData_InitVertexFactories)([this, LightMapCoordinateIndex](FRHICommandListImmediate& RHICmdList)
    {
        for (int32 LODIndex = 0; LODIndex < VertexFactories.Num(); LODIndex++)
        {
            const FStaticMeshLODResources* RenderData = &LODModels[LODIndex];

            FAnimMeshInstanceVertexFactory::FDataType Data;
            // Assign to the vertex factory for this LOD.
            FAnimMeshInstanceVertexFactory& VertexFactory = VertexFactories[LODIndex];
        	
        
        	
            RenderData->VertexBuffers.PositionVertexBuffer.BindPositionVertexBuffer(&VertexFactory, Data);
            RenderData->VertexBuffers.StaticMeshVertexBuffer.BindTangentVertexBuffer(&VertexFactory, Data);
            RenderData->VertexBuffers.StaticMeshVertexBuffer.BindPackedTexCoordVertexBuffer(&VertexFactory, Data);
            if (LightMapCoordinateIndex < (int32)RenderData->VertexBuffers.StaticMeshVertexBuffer.GetNumTexCoords() && LightMapCoordinateIndex >= 0)
            {
                RenderData->VertexBuffers.StaticMeshVertexBuffer.BindLightMapVertexBuffer(&VertexFactory, Data, LightMapCoordinateIndex);
            }
            RenderData->VertexBuffers.ColorVertexBuffer.BindColorVertexBuffer(&VertexFactory, Data);

            check(PerInstanceRenderData);
           PerInstanceRenderData->InstanceBuffer.BindInstanceVertexBuffer(&VertexFactory, Data);

            VertexFactory.SetData(Data);
            VertexFactory.InitResource();
        }
    });


/*	const FSkinWeightDataVertexBuffer* WeightDataVertexBuffer = VertexBuffers.SkinWeightVertexBuffer->GetDataVertexBuffer();
	const uint32 Stride = VertexBuffers.SkinWeightVertexBuffer->GetConstantInfluencesVertexStride();
	const uint32 WeightsOffset = VertexBuffers.SkinWeightVertexBuffer->GetConstantInfluencesBoneWeightsOffset();
	VertexFactoryData->BoneIndices = FVertexStreamComponent(WeightDataVertexBuffer, 0, Stride, bUse16BitBoneIndex ? VET_UShort4 : VET_UByte4);
	VertexFactoryData->BoneWeights = FVertexStreamComponent(WeightDataVertexBuffer, WeightsOffset, Stride, VET_UByte4N);
*/

	
}


#if RHI_RAYTRACING
void FAnimationInstanceVertexSceneProxy::GetDynamicRayTracingInstances(struct FRayTracingMaterialGatheringContext& Context, TArray<FRayTracingInstance>& OutRayTracingInstances)
{
	
	if (!CVarRayTracingRenderInstances.GetValueOnRenderThread())
	{
		return;
	}

	uint32 LOD = GetCurrentFirstLODIdx_RenderThread();
	const int32 InstanceCount = InstancedRenderData.PerInstanceRenderData->InstanceBuffer.GetNumInstances();

	if (InstanceCount == 0)
	{
		return;
	}
	//setup a 'template' for the instance first, so we aren't duplicating work
	//#dxr_todo: when multiple LODs are used, template needs to be an array of templates, probably best initialized on-demand via a lamda
	FRayTracingInstance RayTracingInstanceTemplate;
	RayTracingInstanceTemplate.Geometry = &RenderData->LODResources[LOD].RayTracingGeometry;

	//preallocate the worst-case to prevent an explosion of reallocs
	//#dxr_todo: possibly track used instances and reserve based on previous behavior
	RayTracingInstanceTemplate.InstanceTransforms.Reserve(InstanceCount);

	if (CVarRayTracingRenderInstancesCulling.GetValueOnRenderThread() > 0 && RayTracingCullClusters.Num() > 0)
	{
		const float BVHCullRadius = CVarRayTracingInstancesCullClusterRadius.GetValueOnRenderThread();
		const float BVHLowScaleThreshold = CVarRayTracingInstancesLowScaleThreshold.GetValueOnRenderThread();
		const float BVHLowScaleRadius = CVarRayTracingInstancesLowScaleCullRadius.GetValueOnRenderThread();
		const bool ApplyGeneralCulling = BVHCullRadius > 0.0f;
		const bool ApplyLowScaleCulling = BVHLowScaleThreshold > 0.0f && BVHLowScaleRadius > 0.0f;
		FMatrix ToWorld = GetLocalToWorld();

		// Iterate over all culling clusters
		for (int32 ClusterIdx = 0; ClusterIdx < RayTracingCullClusters.Num(); ++ClusterIdx)
		{
			FRayTracingCullCluster& Cluster = RayTracingCullClusters[ClusterIdx];

			FVector VClusterBBoxSize = Cluster.BoundsMax - Cluster.BoundsMin;
			FVector VClusterCenter = 0.5f * (Cluster.BoundsMax + Cluster.BoundsMin);
			FVector VToClusterCenter = VClusterCenter - Context.ReferenceView->ViewLocation;
			float ClusterRadius = 0.5f * VClusterBBoxSize.Size();
			float DistToClusterCenter = VToClusterCenter.Size();

			// Cull whole cluster if the bounding sphere is too far away
			if ((DistToClusterCenter - ClusterRadius) > BVHCullRadius && ApplyGeneralCulling)
			{
				continue;
			}

			// Cull instances in the cluster
			for (FCullNode& Node : Cluster.Nodes)
			{
				const uint32 InstanceIdx = Node.Instance;

				FVector InstanceLocation = Node.Center;
				FVector VToInstanceCenter = Context.ReferenceView->ViewLocation - InstanceLocation;
				float DistanceToInstanceCenter = VToInstanceCenter.Size();
				float InstanceRadius = Node.Radius;
				float DistanceToInstanceStart = DistanceToInstanceCenter - InstanceRadius;

				// Cull instance based on distance
				if (DistanceToInstanceStart > BVHCullRadius && ApplyGeneralCulling)
					continue;

				// Special culling for small scale objects
				if (InstanceRadius < BVHLowScaleThreshold && ApplyLowScaleCulling)
				{
					if (DistanceToInstanceStart > BVHLowScaleRadius)
						continue;
				}

				FMatrix Transform;
				InstancedRenderData.PerInstanceRenderData->InstanceBuffer.GetInstanceTransform(InstanceIdx, Transform);
				Transform.M[3][3] = 1.0f;
				FMatrix InstanceTransform = Transform * GetLocalToWorld();

				RayTracingInstanceTemplate.InstanceTransforms.Add(InstanceTransform);
			}
		}
	}
	else
	{
		// No culling
		for (int32 InstanceIdx = 0; InstanceIdx < InstanceCount; ++InstanceIdx)
		{
			FMatrix Transform;
			InstancedRenderData.PerInstanceRenderData->InstanceBuffer.GetInstanceTransform(InstanceIdx, Transform);
			Transform.M[3][3] = 1.0f;
			FMatrix InstanceTransform = Transform * GetLocalToWorld();

			RayTracingInstanceTemplate.InstanceTransforms.Add(InstanceTransform);
		}
	}


	if (RayTracingInstanceTemplate.InstanceTransforms.Num() > 0)
	{
		int32 SectionCount = InstancedRenderData.LODModels[LOD].Sections.Num();

		for (int32 SectionIdx = 0; SectionIdx < SectionCount; ++SectionIdx)
		{
			//#dxr_todo: so far we use the parent static mesh path to get material data
			FMeshBatch MeshBatch;
			FStaticMeshSceneProxy::GetMeshElement(LOD, 0, SectionIdx, 0, false, false, MeshBatch);

			RayTracingInstanceTemplate.Materials.Add(MeshBatch);
		}
		RayTracingInstanceTemplate.BuildInstanceMaskAndFlags();

		OutRayTracingInstances.Add(RayTracingInstanceTemplate);
	}
	
}

void FAnimationInstanceVertexSceneProxy::SetupRayTracingCullClusters()
{
	if (!IsRayTracingEnabled())
	{
		return;
	}

	const int32 InstanceCount = InstancedRenderData.Component->PerInstanceSMData.Num();
	if (InstanceCount <= 0)
	{
		return;
	}

	//#dxr_todo: select the appropriate LOD depending on Context.View
	int32 LOD = 0;
	if (RenderData->LODResources.Num() > LOD && RenderData->LODResources[LOD].RayTracingGeometry.IsInitialized())
	{
		const float MaxClusterRadiusMultiplier =0.f;/// CVarRayTracingInstancesCullClusterMaxRadiusMultiplier.GetValueOnAnyThread();
		const int32 Batches = GetNumMeshBatches();
		int32 ClusterIndex = 0;
		// We're in game thread and at this point this scene proxy hasn't been added to FScene, thus GetLocalToWorld() returns undefined transform
		FMatrix ComponentLocalToWorld = InstancedRenderData.Component->GetComponentTransform().ToMatrixWithScale();
		float MaxInstanceRadius = 0.0f;

		// Init first cluster
		{
			FRayTracingCullCluster &Cluster = RayTracingCullClusters.Emplace_GetRef();
			Cluster.BoundsMin = FVector(MAX_FLT, MAX_FLT, MAX_FLT);
			Cluster.BoundsMax = FVector(-MAX_FLT, -MAX_FLT, -MAX_FLT);
		}

		TArray<float> InstanceRadii;
		InstanceRadii.Reserve(InstanceCount);

		// Traverse instances to find maximum radius
		for (int32 Instance = 0; Instance < InstanceCount; ++Instance)
		{
			if (InstancedRenderData.Component->PerInstanceSMData.IsValidIndex(Instance))
			{
				const FInstancedStaticMeshInstanceData& InstanceData = InstancedRenderData.Component->PerInstanceSMData[Instance];
				FMatrix InstanceTransform = InstanceData.Transform * ComponentLocalToWorld;
				InstanceTransform.M[3][3] = 1.0f;

				FVector VMin, VMax;
				InstancedRenderData.Component->GetLocalBounds(VMin, VMax);
				VMin = InstanceTransform.TransformPosition(VMin);
				VMax = InstanceTransform.TransformPosition(VMax);
				FVector VBBoxSize = VMax - VMin;

				MaxInstanceRadius = FMath::Max(0.5f * VBBoxSize.Size(), MaxInstanceRadius);
				InstanceRadii.Add(0.5f * VBBoxSize.Size());
			}
			else
			{
				InstanceRadii.Add(0.0f);
			}
		}

		float MaxClusterRadius = MaxInstanceRadius * MaxClusterRadiusMultiplier;

		// Build clusters
		for (int32 Instance = 0; Instance < InstanceCount; ++Instance)
		{
			if (InstancedRenderData.Component->PerInstanceSMData.IsValidIndex(Instance))
			{
				const FInstancedStaticMeshInstanceData& InstanceData = InstancedRenderData.Component->PerInstanceSMData[Instance];
				FMatrix InstanceTransform = InstanceData.Transform * ComponentLocalToWorld;
				InstanceTransform.M[3][3] = 1.0f;
				FVector InstanceLocation = InstanceTransform.TransformPosition({ 0.0f,0.0f,0.0f });
				FVector VMin = InstanceLocation - FVector(MaxInstanceRadius, MaxInstanceRadius, MaxInstanceRadius);
				FVector VMax = InstanceLocation + FVector(MaxInstanceRadius, MaxInstanceRadius, MaxInstanceRadius);
				bool bClusterFound = false;

				// Try to find suitable cluster
				for (int32 CandidateCluster = 0; CandidateCluster <= ClusterIndex; ++CandidateCluster)
				{
					// Build new candidate cluster bounds
					FVector VCandidateMin = VMin.ComponentMin(RayTracingCullClusters[CandidateCluster].BoundsMin);
					FVector VCandidateMax = VMax.ComponentMax(RayTracingCullClusters[CandidateCluster].BoundsMax);

					FVector VCandidateBBoxSize = VCandidateMax - VCandidateMin;
					float MaxCandidateRadius = 0.5f * VCandidateBBoxSize.Size();

					// If new candidate is still small enough, update current cluster
					if (MaxCandidateRadius <= MaxClusterRadius)
					{
						RayTracingCullClusters[CandidateCluster].BoundsMin = VCandidateMin;
						RayTracingCullClusters[CandidateCluster].BoundsMax = VCandidateMax;
						FCullNode& Node = RayTracingCullClusters[CandidateCluster].Nodes.Emplace_GetRef();
						Node.Radius = InstanceRadii[Instance];
						Node.Center = InstanceLocation;
						Node.Instance = Instance;
						bClusterFound = true;
						break;
					}
				}

				// if we couldn't add the instance to an existing cluster create a new one
				if (!bClusterFound)
				{
					++ClusterIndex;

					FRayTracingCullCluster& Cluster = RayTracingCullClusters.Emplace_GetRef();
					Cluster.BoundsMin = VMin;
					Cluster.BoundsMax = VMax;
					FCullNode Node;

					Node.Radius = InstanceRadii[Instance];
					Node.Center = InstanceLocation;
					Node.Instance = Instance;
					Cluster.Nodes.Add(Node);
				}
			}
		}
	}
}

#endif