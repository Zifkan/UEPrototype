// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterConverterTool.h"



#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "IPersonaToolkit.h"
#include "ISkeletalMeshEditorModule.h"
#include "SkeletalRenderPublic.h"
#include "Animation/DebugSkelMeshComponent.h"
#include "Dialogs/DlgPickAssetPath.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Rendering/SkeletalMeshRenderData.h"

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
    FString PackageName = FString(TEXT("/Game/Blueprints/")) + NewNameSuggestion;
  
    TSharedPtr<SDlgPickAssetPath> PickAssetPathWidget =
        SNew(SDlgPickAssetPath)
        .Title(FText::FromString("Create Character Blueprint"))
        .DefaultAssetPath(FText::FromString(PackageName));

    if (EAppReturnType::Ok == PickAssetPathWidget->ShowModal())
    {
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
    }
    
    ACharacterActor* charActor = NewObject<ACharacterActor>(); 
    charActor->TestINT = 159;

    return charActor;
}


#undef LOCTEXT_NAMESPACE