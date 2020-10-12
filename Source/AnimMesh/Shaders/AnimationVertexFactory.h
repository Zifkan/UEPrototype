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

	
	static bool ShouldCompilePermutation(const FVertexFactoryShaderPermutationParameters& Parameters)
	{
		if ((Parameters.MaterialParameters.MaterialDomain == MD_Surface &&
			Parameters.MaterialParameters.ShadingModels == MSM_Unlit) ||
			Parameters.MaterialParameters.bIsDefaultMaterial)
		{
			return true;
		}
		return false;
	}
	
	
	static void ModifyCompilationEnvironment(const FVertexFactoryShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		
		const bool ContainsManualVertexFetch = OutEnvironment.GetDefinitions().Contains("MANUAL_VERTEX_FETCH");
		if (!ContainsManualVertexFetch)
		{
			OutEnvironment.SetDefine(TEXT("MANUAL_VERTEX_FETCH"), TEXT("0"));
		}

		OutEnvironment.SetDefine(TEXT("DEFORM_MESH"), TEXT("1"));
	}

	virtual void InitRHI() override 
	{

		// Check if this vertex factory has a valid feature level that is supported by the current platform
		check(HasValidFeatureLevel());


		//The vertex declaration element lists (Nothing but an array of FVertexElement)
		FVertexDeclarationElementList Elements; //Used for the Default vertex stream
		FVertexDeclarationElementList PosOnlyElements; // Used for the PositionOnly vertex stream

		if (Data.PositionComponent.VertexBuffer != NULL)
		{
			//We add the position stream component to both elemnt lists
			Elements.Add(AccessStreamComponent(Data.PositionComponent, 0));
			PosOnlyElements.Add(AccessStreamComponent(Data.PositionComponent, 0, EVertexInputStreamType::PositionOnly));
		}

		//Initialize the Position Only vertex declaration which will be used in the depth pass
		InitDeclaration(PosOnlyElements, EVertexInputStreamType::PositionOnly);

		//We add all the available texcoords to the default element list, that's all what we'll need for unlit shading
		if (Data.TextureCoordinates.Num())
		{
			const int32 BaseTexCoordAttribute = 4;
			for (int32 CoordinateIndex = 0; CoordinateIndex < Data.TextureCoordinates.Num(); CoordinateIndex++)
			{
				Elements.Add(AccessStreamComponent(
					Data.TextureCoordinates[CoordinateIndex],
					BaseTexCoordAttribute + CoordinateIndex
				));
			}

			for (int32 CoordinateIndex = Data.TextureCoordinates.Num(); CoordinateIndex < MAX_STATIC_TEXCOORDS / 2; CoordinateIndex++)
			{
				Elements.Add(AccessStreamComponent(
					Data.TextureCoordinates[Data.TextureCoordinates.Num() - 1],
					BaseTexCoordAttribute + CoordinateIndex
				));
			}
		}

		check(Streams.Num() > 0);

		InitDeclaration(Elements);
		check(IsValidRef(GetDeclaration()));
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

		Buffer.AddZeroed(256);

		TResourceArray<FMatrix>* ResourceArray = new TResourceArray<FMatrix>(true);
		FRHIResourceCreateInfo CreateInfo;
		ResourceArray->Append(Buffer);
		CreateInfo.ResourceArray = ResourceArray;
		//Set the debug name so we can find the resource when debugging in RenderDoc
		CreateInfo.DebugName = TEXT("DeformMesh_TransformsSB");
		
		BindMatricesSB = RHICreateStructuredBuffer(sizeof(FMatrix), 256 * sizeof(FMatrix), BUF_ShaderResource, CreateInfo);
		
		BindMatricesSRV = RHICreateShaderResourceView(BindMatricesSB);
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
    
    
private:

	TArray<FMatrix> Buffer;
    
	//The shader resource view of the structured buffer, this is what we bind to the vertex factory shader
	FShaderResourceViewRHIRef BindMatricesSRV;
   
	//The structured buffer that will contain all the deform transoform and going to be used as a shader resource
	FStructuredBufferRHIRef BindMatricesSB;

	FAnimMeshVertexFactory animMeshVertexFactory;

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

IMPLEMENT_VERTEX_FACTORY_TYPE(FAnimMeshVertexFactory, "/CustomShaders/AnimVertexFactory.ush", true, true, true, true, true);