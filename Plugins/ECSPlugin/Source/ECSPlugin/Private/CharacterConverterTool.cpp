// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterConverterTool.h"



#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "IPersonaToolkit.h"
#include "ISkeletalMeshEditorModule.h"
#include "SkeletalRenderPublic.h"
#include "Animation/AnimInstance.h"
#include "Animation/DebugSkelMeshComponent.h"
#include "Dialogs/DlgPickAssetPath.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Rendering/SkinWeightVertexBuffer.h"

class FAssetToolsModule;


#define LOCTEXT_NAMESPACE "FECSPluginModule"


void CharacterConverterTool::Startup()
{
	ModuleLoadedDelegateHandle = FModuleManager::Get().OnModulesChanged().AddLambda([this](FName InModuleName, EModuleChangeReason InChangeReason)
    {
        if (InChangeReason == EModuleChangeReason::ModuleLoaded)
        {
            if (InModuleName == moduleNameConst)
            {            	
                AddCharacterAnimEditorExtender();
            }
        }
    });
}


void CharacterConverterTool::Shutdown()
{
	RemoveCharacterAnimEditorExtender();
	FModuleManager::Get().OnModulesChanged().Remove(ModuleLoadedDelegateHandle);
}


static float EncodeFloat4toFloat(FVector4 enc )
{
	FVector4 kDecodeDot = FVector4(1.0, 1/255.0, 1/65025.0, 1/160581375.0);
	return FVector::DotProduct ( enc, kDecodeDot );
}

void CharacterConverterTool::AddCharacterAnimEditorExtender()
{
    ISkeletalMeshEditorModule& SkeletalMeshEditorModule = FModuleManager::Get().LoadModuleChecked<ISkeletalMeshEditorModule>(moduleNameConst);
    auto& ToolbarExtenders = SkeletalMeshEditorModule.GetAllSkeletalMeshEditorToolbarExtenders();

    ToolbarExtenders.Add(ISkeletalMeshEditorModule::FSkeletalMeshEditorToolbarExtender::CreateRaw(this, &CharacterConverterTool::GetCharacterAnimEditorExtender));
    SkeletalMeshEditorExtenderHandle = ToolbarExtenders.Last().GetHandle();
}

void CharacterConverterTool::RemoveCharacterAnimEditorExtender()
{
    ISkeletalMeshEditorModule* SkeletalMeshEditorModule = FModuleManager::Get().GetModulePtr<ISkeletalMeshEditorModule>(moduleNameConst);
    if (SkeletalMeshEditorModule)
    {
        typedef ISkeletalMeshEditorModule::FSkeletalMeshEditorToolbarExtender DelegateType;
        SkeletalMeshEditorModule->GetAllSkeletalMeshEditorToolbarExtenders().RemoveAll([=](const DelegateType& In) { return In.GetHandle() == SkeletalMeshEditorExtenderHandle; });
    }
}

TSharedRef<FExtender> CharacterConverterTool::GetCharacterAnimEditorExtender(const TSharedRef<FUICommandList> CommandList, TSharedRef<ISkeletalMeshEditor> InSkeletalMeshEditor)
{
    TSharedRef<FExtender> Extender = MakeShareable(new FExtender);
    UDebugSkelMeshComponent* MeshComponent = InSkeletalMeshEditor->GetPersonaToolkit()->GetPreviewMeshComponent();



    Extender->AddToolBarExtension(
        "Asset",
        EExtensionHook::After,
        CommandList,
        FToolBarExtensionDelegate::CreateRaw(this, &CharacterConverterTool::HandleAddCharacterAnimEditorExtenderToToolbar, MeshComponent)
    );

    return Extender;
}


void CharacterConverterTool::HandleAddCharacterAnimEditorExtenderToToolbar(FToolBarBuilder & ParentToolbarBuilder, UDebugSkelMeshComponent * InMeshComponent)
{

    ParentToolbarBuilder.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([this, InMeshComponent]()
    {
        CreateCharacterBluePrint(InMeshComponent);
    })),
        NAME_None,
        LOCTEXT("Convert Character", "Convert Character"),
        LOCTEXT("ConvertCharacterTooltip", "Convert Character to Static Mesh"),
        FSlateIcon("EditorStyle", "Persona.TogglePreviewAsset", "Persona.TogglePreviewAsset.Small")
        );
}
void CharacterConverterTool::CreateCharacterBluePrint(UDebugSkelMeshComponent* PreviewComponent)
{
    auto* NewNameSuggestion = TEXT("CharacterBlueprint");
    FString PackageName = FString(TEXT("/Game/Blueprints/")) + NewNameSuggestion;
  
    TSharedPtr<SDlgPickAssetPath> PickAssetPathWidget =
        SNew(SDlgPickAssetPath)
        .Title(FText::FromString("Create Character Blueprint"))
        .DefaultAssetPath(FText::FromString(PackageName));

    if (EAppReturnType::Ok == PickAssetPathWidget->ShowModal())
    {
        auto charActor =  ConvertToMesh(PreviewComponent);
       
        if(charActor!=nullptr)
        {
            FKismetEditorUtilities::CreateBlueprintFromActor(FString(PickAssetPathWidget->GetFullAssetPath().ToString()),charActor,false);            
        }
    }   
}

// Helper function for ConvertMeshesToStaticMesh
static void AddOrDuplicateMaterial(UMaterialInterface* InMaterialInterface, const FString& InPackageName, TArray<UMaterialInterface*>& OutMaterials)
{
	if (InMaterialInterface && !InMaterialInterface->GetOuter()->IsA<UPackage>())
	{
		// Convert runtime material instances to new concrete material instances
		// Create new package
		FString OriginalMaterialName = InMaterialInterface->GetName();
		FString MaterialPath = FPackageName::GetLongPackagePath(InPackageName) / OriginalMaterialName;
		FString MaterialName;
		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
		AssetToolsModule.Get().CreateUniqueAssetName(MaterialPath, TEXT(""), MaterialPath, MaterialName);
		UPackage* MaterialPackage = CreatePackage(NULL, *MaterialPath);

		// Duplicate the object into the new package
		UMaterialInterface* NewMaterialInterface = DuplicateObject<UMaterialInterface>(InMaterialInterface, MaterialPackage, *MaterialName);
		NewMaterialInterface->SetFlags(RF_Public | RF_Standalone);

		if (UMaterialInstanceDynamic* MaterialInstanceDynamic = Cast<UMaterialInstanceDynamic>(NewMaterialInterface))
		{
			UMaterialInstanceDynamic* OldMaterialInstanceDynamic = CastChecked<UMaterialInstanceDynamic>(InMaterialInterface);
			MaterialInstanceDynamic->K2_CopyMaterialInstanceParameters(OldMaterialInstanceDynamic);
		}

		NewMaterialInterface->MarkPackageDirty();

		FAssetRegistryModule::AssetCreated(NewMaterialInterface);

		InMaterialInterface = NewMaterialInterface;
	}

	OutMaterials.Add(InMaterialInterface);
}

// Helper function for ConvertMeshesToStaticMesh
template <typename ComponentType>
static void ProcessMaterials(ComponentType* InComponent, const FString& InPackageName, TArray<UMaterialInterface*>& OutMaterials)
{
	const int32 NumMaterials = InComponent->GetNumMaterials();
	for (int32 MaterialIndex = 0; MaterialIndex < NumMaterials; MaterialIndex++)
	{
		UMaterialInterface* MaterialInterface = InComponent->GetMaterial(MaterialIndex);
		AddOrDuplicateMaterial(MaterialInterface, InPackageName, OutMaterials);
	}
}


static void SkinnedMeshToRawMeshes(USkinnedMeshComponent* InSkinnedMeshComponent, int32 InOverallMaxLODs, const FMatrix& InComponentToWorld, const FString& InPackageName, TArray<FRawMeshTracker>& OutRawMeshTrackers, TArray<FRawMesh>& OutRawMeshes, TArray<UMaterialInterface*>& OutMaterials)
{
	const int32 BaseMaterialIndex = OutMaterials.Num();

	// Export all LODs to raw meshes
	const int32 NumLODs = InSkinnedMeshComponent->GetNumLODs();

	for (int32 OverallLODIndex = 0; OverallLODIndex < InOverallMaxLODs; OverallLODIndex++)
	{
		int32 LODIndexRead = FMath::Min(OverallLODIndex, NumLODs - 1);

		FRawMesh& RawMesh = OutRawMeshes[OverallLODIndex];
		FRawMeshTracker& RawMeshTracker = OutRawMeshTrackers[OverallLODIndex];
		const int32 BaseVertexIndex = RawMesh.VertexPositions.Num();

		FSkeletalMeshLODInfo& SrcLODInfo = *(InSkinnedMeshComponent->SkeletalMesh->GetLODInfo(LODIndexRead));

		// Get the CPU skinned verts for this LOD
		TArray<FFinalSkinVertex> FinalVertices;
		InSkinnedMeshComponent->GetCPUSkinnedVertices(FinalVertices, LODIndexRead);

		FSkeletalMeshRenderData& SkeletalMeshRenderData = InSkinnedMeshComponent->MeshObject->GetSkeletalMeshRenderData();
		FSkeletalMeshLODRenderData& LODData = SkeletalMeshRenderData.LODRenderData[LODIndexRead];

		// Copy skinned vertex positions
		for (int32 VertIndex = 0; VertIndex < FinalVertices.Num(); ++VertIndex)
		{
			RawMesh.VertexPositions.Add(InComponentToWorld.TransformPosition(FinalVertices[VertIndex].Position));
		}

		const uint32 NumTexCoords = FMath::Min(LODData.StaticVertexBuffers.StaticMeshVertexBuffer.GetNumTexCoords(), (uint32)MAX_MESH_TEXTURE_COORDS);
		const int32 NumSections = LODData.RenderSections.Num();
		FRawStaticIndexBuffer16or32Interface& IndexBuffer = *LODData.MultiSizeIndexContainer.GetIndexBuffer();

		for (int32 SectionIndex = 0; SectionIndex < NumSections; SectionIndex++)
		{
			const FSkelMeshRenderSection& SkelMeshSection = LODData.RenderSections[SectionIndex];
			if (InSkinnedMeshComponent->IsMaterialSectionShown(SkelMeshSection.MaterialIndex, LODIndexRead))
			{
				// Build 'wedge' info
				const int32 NumWedges = SkelMeshSection.NumTriangles * 3;
				for(int32 WedgeIndex = 0; WedgeIndex < NumWedges; WedgeIndex++)
				{
					const int32 VertexIndexForWedge = IndexBuffer.Get(SkelMeshSection.BaseIndex + WedgeIndex);

					RawMesh.WedgeIndices.Add(BaseVertexIndex + VertexIndexForWedge);

					const FFinalSkinVertex& SkinnedVertex = FinalVertices[VertexIndexForWedge];
					const FVector TangentX = InComponentToWorld.TransformVector(SkinnedVertex.TangentX.ToFVector());
					const FVector TangentZ = InComponentToWorld.TransformVector(SkinnedVertex.TangentZ.ToFVector());
					const FVector4 UnpackedTangentZ = SkinnedVertex.TangentZ.ToFVector4();
					const FVector TangentY = (TangentZ ^ TangentX).GetSafeNormal() * UnpackedTangentZ.W;

					RawMesh.WedgeTangentX.Add(TangentX);
					RawMesh.WedgeTangentY.Add(TangentY);
					RawMesh.WedgeTangentZ.Add(TangentZ);

					for (uint32 TexCoordIndex = 0; TexCoordIndex < MAX_MESH_TEXTURE_COORDS; TexCoordIndex++)
					{
						if (TexCoordIndex >= NumTexCoords)
						{
							RawMesh.WedgeTexCoords[TexCoordIndex].AddDefaulted();
						}
						else
						{
							RawMesh.WedgeTexCoords[TexCoordIndex].Add(LODData.StaticVertexBuffers.StaticMeshVertexBuffer.GetVertexUV(VertexIndexForWedge, TexCoordIndex));
							RawMeshTracker.bValidTexCoords[TexCoordIndex] = true;
						}
					}

					if (LODData.StaticVertexBuffers.ColorVertexBuffer.IsInitialized())
					{
						RawMesh.WedgeColors.Add(LODData.StaticVertexBuffers.ColorVertexBuffer.VertexColor(VertexIndexForWedge));
						RawMeshTracker.bValidColors = true;
					}
					else
					{
						RawMesh.WedgeColors.Add(FColor::White);
					}
				}

				int32 MaterialIndex = SkelMeshSection.MaterialIndex;
				// use the remapping of material indices if there is a valid value
				if (SrcLODInfo.LODMaterialMap.IsValidIndex(SectionIndex) && SrcLODInfo.LODMaterialMap[SectionIndex] != INDEX_NONE)
				{
					MaterialIndex = FMath::Clamp<int32>(SrcLODInfo.LODMaterialMap[SectionIndex], 0, InSkinnedMeshComponent->SkeletalMesh->Materials.Num());
				}

				// copy face info
				for (uint32 TriIndex = 0; TriIndex < SkelMeshSection.NumTriangles; TriIndex++)
				{
					RawMesh.FaceMaterialIndices.Add(BaseMaterialIndex + MaterialIndex);
					RawMesh.FaceSmoothingMasks.Add(0); // Assume this is ignored as bRecomputeNormals is false
				}
			}
		}
	}

	ProcessMaterials<USkinnedMeshComponent>(InSkinnedMeshComponent, InPackageName, OutMaterials);
}



ACharacterActor* CharacterConverterTool::ConvertToMesh(UDebugSkelMeshComponent* PreviewComponent)
{
	
	
    auto* NewNameSuggestion = TEXT("StaticMesh");
    FString PackageName = FString(TEXT("/Game/Meshes/")) + NewNameSuggestion;
  
    TSharedPtr<SDlgPickAssetPath> PickAssetPathWidget =
        SNew(SDlgPickAssetPath)
        .Title(FText::FromString("Convert Character Skinned Mesh to Static mesh"))
        .DefaultAssetPath(FText::FromString(PackageName));

    if (EAppReturnType::Ok == PickAssetPathWidget->ShowModal())
    {
    	UStaticMesh* StaticMesh = nullptr;
    	
    	PreviewComponent->GlobalAnimRateScale = 0.f;
        int32 OverallMaxLODs = 0;
        OverallMaxLODs = FMath::Max(PreviewComponent->MeshObject->GetSkeletalMeshRenderData().LODRenderData.Num(), OverallMaxLODs);

        TArray<FRawMeshTracker> RawMeshTrackers;
        TArray<FRawMesh> RawMeshes;
        TArray<UMaterialInterface*> Materials;

        TArray <FVector2D> LODAnimUVs;
        TArray <int32> UniqueSourceIDs;
    
        // Resize raw meshes to accommodate the number of LODs we will need
        RawMeshes.SetNum(OverallMaxLODs);
        RawMeshTrackers.SetNum(OverallMaxLODs);
        // THIS SETS THE NUM ROWS Var
    	
        SkinnedMeshToRawMeshes(PreviewComponent, OverallMaxLODs, FTransform().ToMatrixWithScale(), PackageName,RawMeshTrackers, RawMeshes, Materials);


    	uint32 MaxInUseTextureCoordinate = 0;

    	// scrub invalid vert color & tex coord data
    	check(RawMeshes.Num() == RawMeshTrackers.Num());
    	for (int32 RawMeshIndex = 0; RawMeshIndex < RawMeshes.Num(); RawMeshIndex++)
    	{
    		if (!RawMeshTrackers[RawMeshIndex].bValidColors)
    		{
    			RawMeshes[RawMeshIndex].WedgeColors.Empty();
    		}

    		for (uint32 TexCoordIndex = 0; TexCoordIndex < MAX_MESH_TEXTURE_COORDS; TexCoordIndex++)
    		{
    			if (!RawMeshTrackers[RawMeshIndex].bValidTexCoords[TexCoordIndex])
    			{
    				RawMeshes[RawMeshIndex].WedgeTexCoords[TexCoordIndex].Empty();
    			}
    			else
    			{
    				// Store first texture coordinate index not in use
    				MaxInUseTextureCoordinate = FMath::Max(MaxInUseTextureCoordinate, TexCoordIndex);
    			}
    		}
    	}
    	
		// Check if we got some valid data.
		bool bValidData = false;
		for (FRawMesh& RawMesh : RawMeshes)
		{
			if (RawMesh.IsValidOrFixable())
			{
				bValidData = true;
				break;
			}
		}

    	TArray<FBoneDataInfo>  BoneInfoArray;
		if (bValidData)
		{
			auto MeshName = FPackageName::GetLongPackageAssetName(PackageName);
			// Then find/create it.
			UPackage* Package = CreatePackage(NULL, *PackageName);
			check(Package);

			// Create StaticMesh object
			StaticMesh = NewObject<UStaticMesh>(Package, *MeshName, RF_Public | RF_Standalone);
			StaticMesh->InitResources();

			StaticMesh->LightingGuid = FGuid::NewGuid();

			// Determine which texture coordinate map should be used for storing/generating the lightmap UVs
			const uint32 LightMapIndex = FMath::Min(MaxInUseTextureCoordinate + 1, (uint32)MAX_MESH_TEXTURE_COORDS - 1);

			// Add source to new StaticMesh
			for (FRawMesh& RawMesh : RawMeshes)
			{
				if (RawMesh.IsValidOrFixable())
				{
					FStaticMeshSourceModel& SrcModel = StaticMesh->AddSourceModel();
					SrcModel.BuildSettings.bRecomputeNormals = false;
					SrcModel.BuildSettings.bRecomputeTangents = false;
					SrcModel.BuildSettings.bRemoveDegenerates = true;
					SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
					SrcModel.BuildSettings.bUseFullPrecisionUVs = false;
					SrcModel.BuildSettings.bGenerateLightmapUVs = true;
					SrcModel.BuildSettings.SrcLightmapIndex = 0;
					SrcModel.BuildSettings.DstLightmapIndex = LightMapIndex;
					SrcModel.SaveRawMesh(RawMesh);
				}
			}

			// Copy materials to new mesh 
			for(UMaterialInterface* Material : Materials)
			{
				StaticMesh->StaticMaterials.Add(FStaticMaterial(Material));
			}
			
			//Set the Imported version before calling the build
			StaticMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;

			// Set light map coordinate index to match DstLightmapIndex
			StaticMesh->LightMapCoordinateIndex = LightMapIndex;

			// setup section info map
			for (int32 RawMeshLODIndex = 0; RawMeshLODIndex < RawMeshes.Num(); RawMeshLODIndex++)
			{
				const FRawMesh& RawMesh = RawMeshes[RawMeshLODIndex];
				TArray<int32> UniqueMaterialIndices;
				for (int32 MaterialIndex : RawMesh.FaceMaterialIndices)
				{
					UniqueMaterialIndices.AddUnique(MaterialIndex);
				}

				int32 SectionIndex = 0;
				for (int32 UniqueMaterialIndex : UniqueMaterialIndices)
				{
					StaticMesh->GetSectionInfoMap().Set(RawMeshLODIndex, SectionIndex, FMeshSectionInfo(UniqueMaterialIndex));
					SectionIndex++;
				}
			}
			StaticMesh->GetOriginalSectionInfoMap().CopyFrom(StaticMesh->GetSectionInfoMap());


		
			
			// Build mesh from source
			StaticMesh->Build(false);
			StaticMesh->PostEditChange();

			if (StaticMesh!=nullptr)
			{
				auto UVnum = StaticMesh->GetNumUVChannels(0);
				for (int i = UVnum; i < 8; ++i)
				{
					StaticMesh->AddUVChannel(0);
				}

				auto renderData = &PreviewComponent->GetSkeletalMeshRenderData()->LODRenderData[0];
				auto currentBonesInfluence = renderData->GetVertexBufferMaxBoneInfluences();
				auto vertexCount = renderData->GetSkinWeightVertexBuffer()->GetNumVertices();
				auto weightOffset= renderData->SkinWeightVertexBuffer.GetDataVertexBuffer()->GetConstantInfluencesBoneWeightsOffset();
				TMap<FVertexInstanceID, FVector2D> uv;
				for (uint32 i = 0; i < vertexCount; ++i)
				{            	
					FVector4 weights;
					FVector4 indices;
					
					for (uint32 j = 0; j < currentBonesInfluence; ++j)
					{			
						weights[j] =  static_cast<double>(static_cast<int>(renderData->SkinWeightVertexBuffer.GetBoneWeight(i,j))/255.0);
						indices[j] = static_cast<int>(renderData->SkinWeightVertexBuffer.GetBoneIndex(i,j));
					}  
            	
					uv.Add(FVertexInstanceID(i),FVector2D(EncodeFloat4toFloat(indices),EncodeFloat4toFloat(weights)));
            
				}
    		
				StaticMesh->SetUVChannel(0,7,uv);

				auto boneTree = PreviewComponent->SkeletalMesh->RefSkeleton.GetRefBoneInfo();			
				for (auto BoneTree : boneTree)
				{
					int bone_index = PreviewComponent->GetBoneIndex( BoneTree.Name );				
					auto boneMatrix = PreviewComponent->GetBoneMatrix( bone_index );
					auto boneTransform = PreviewComponent->GetBoneTransform( bone_index );
					BoneInfoArray.Add(FBoneDataInfo({boneTransform,boneMatrix}));
					UE_LOG(LogTemp, Warning, TEXT("%s = %i"),*BoneTree.Name.ToString(), bone_index);
				}
			
				/*	TArray<FSkinWeightInfo> verts;
				PreviewComponent->GetSkeletalMeshRenderData()->LODRenderData[0].GetSkinWeightVertexBuffer()->GetSkinWeights(verts);
    		
				for (auto vert : verts)
				{
				int index0 = static_cast<int>(vert.InfluenceBones[0]);
				int index1 = static_cast<int>(vert.InfluenceBones[1]);
				int index2 = static_cast<int>(vert.InfluenceBones[2]);
				int index3 = static_cast<int>(vert.InfluenceBones[3]);
    			
				double weight0 = static_cast<double>(static_cast<int>(vert.InfluenceWeights[0])/255.0);
				double weight1 = static_cast<double>(static_cast<int>(vert.InfluenceWeights[1])/255.0);
				double weight2 = static_cast<double>(static_cast<int>(vert.InfluenceWeights[2])/255.0);
				double weight3 = static_cast<double>(static_cast<int>(vert.InfluenceWeights[3])/255.0);
				*/
			}
    	

			
			StaticMesh->MarkPackageDirty();
			// Notify asset registry of new asset
			FAssetRegistryModule::AssetCreated(StaticMesh);

			// Display notification so users can quickly access the mesh
			if (GIsEditor)
			{
				FNotificationInfo Info(FText::Format(LOCTEXT("SkeletalMeshConverted", "Successfully Converted Mesh"), FText::FromString(StaticMesh->GetName())));
				Info.ExpireDuration = 8.0f;
				Info.bUseLargeFont = false;
				Info.Hyperlink = FSimpleDelegate::CreateLambda([=]() { GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAssets(TArray<UObject*>({ StaticMesh })); });
				Info.HyperlinkText = FText::Format(LOCTEXT("OpenNewAnimationHyperlink", "Open {0}"), FText::FromString(StaticMesh->GetName()));
				TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
				if ( Notification.IsValid() )
				{
					Notification->SetCompletionState( SNotificationItem::CS_Success );
				}
			}
		}
		
    	ACharacterActor* charActor = NewObject<ACharacterActor>();
    	charActor->AnimationVertexMeshComponent->SetStaticMesh(StaticMesh);
    	charActor->BoneInfoArray =BoneInfoArray;

    	return charActor;    	
    }
	
	return NULL;   
}




#undef LOCTEXT_NAMESPACE