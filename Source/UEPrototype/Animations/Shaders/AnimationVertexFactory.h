#pragma once
#include "PrimitiveViewRelevance.h"
#include "RenderResource.h"
#include "RenderingThread.h"
#include "VertexFactory.h"
#include "MaterialShared.h"
#include "LocalVertexFactory.h"
#include "Engine/Engine.h"
#include "StaticMeshResources.h"
#include "MeshMaterialShader.h"
#include "ShaderParameters.h"

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

		OutEnvironment.SetDefine(TEXT("ANIM_MESH"), TEXT("1"));
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
	//inline void SetSceneProxy(FDeformMeshSceneProxy* Proxy) { SceneProxy = Proxy; }
private:
	//We need to pass this as a shader parameter, so we store it in the vertex factory and we use in the vertex factory shader parameters
	uint16 RendererIndex;
	

	friend class FAnimMeshVertexFactoryShaderParameters;
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

IMPLEMENT_VERTEX_FACTORY_TYPE(FAnimMeshVertexFactory, "/Animations/Shaders/AnimLocalVertexFactory.ush", true, true, true, true, true);