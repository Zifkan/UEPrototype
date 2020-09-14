// Copyright Epic Games, Inc. All Rights Reserved.

#include "ECSPlugin.h"



#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "CharacterActor.h"
#include "IPersonaToolkit.h"
#include "ISkeletalMeshEditorModule.h"
#include "CharacterRendererData.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet2/KismetEditorUtilities.h"

#define LOCTEXT_NAMESPACE "FECSPluginModule"



void FECSPluginModule::StartupModule()
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

void FECSPluginModule::ShutdownModule()
{
    RemoveCharacterAnimEditorExtender();
    FModuleManager::Get().OnModulesChanged().Remove(ModuleLoadedDelegateHandle);
}

void FECSPluginModule::AddCharacterAnimEditorExtender()
{
    ISkeletalMeshEditorModule& SkeletalMeshEditorModule = FModuleManager::Get().LoadModuleChecked<ISkeletalMeshEditorModule>(moduleNameConst);
    auto& ToolbarExtenders = SkeletalMeshEditorModule.GetAllSkeletalMeshEditorToolbarExtenders();

    ToolbarExtenders.Add(ISkeletalMeshEditorModule::FSkeletalMeshEditorToolbarExtender::CreateRaw(this, &FECSPluginModule::GetCharacterAnimEditorExtender));
    SkeletalMeshEditorExtenderHandle = ToolbarExtenders.Last().GetHandle();
}

void FECSPluginModule::RemoveCharacterAnimEditorExtender()
{
    ISkeletalMeshEditorModule* SkeletalMeshEditorModule = FModuleManager::Get().GetModulePtr<ISkeletalMeshEditorModule>(moduleNameConst);
    if (SkeletalMeshEditorModule)
    {
        typedef ISkeletalMeshEditorModule::FSkeletalMeshEditorToolbarExtender DelegateType;
        SkeletalMeshEditorModule->GetAllSkeletalMeshEditorToolbarExtenders().RemoveAll([=](const DelegateType& In) { return In.GetHandle() == SkeletalMeshEditorExtenderHandle; });
    }
}

TSharedRef<FExtender> FECSPluginModule::GetCharacterAnimEditorExtender(const TSharedRef<FUICommandList> CommandList, TSharedRef<ISkeletalMeshEditor> InSkeletalMeshEditor)
{
    TSharedRef<FExtender> Extender = MakeShareable(new FExtender);
    UDebugSkelMeshComponent* MeshComponent = InSkeletalMeshEditor->GetPersonaToolkit()->GetPreviewMeshComponent();



    Extender->AddToolBarExtension(
        "Asset",
        EExtensionHook::After,
        CommandList,
        FToolBarExtensionDelegate::CreateRaw(this, &FECSPluginModule::HandleAddCharacterAnimEditorExtenderToToolbar, MeshComponent)
    );

    return Extender;
}


void FECSPluginModule::HandleAddCharacterAnimEditorExtenderToToolbar(FToolBarBuilder & ParentToolbarBuilder, UDebugSkelMeshComponent * InMeshComponent)
{

    ParentToolbarBuilder.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([this, InMeshComponent]()
    {
        ConvertToMesh(InMeshComponent);
    })),
        NAME_None,
        LOCTEXT("Convert Character", "Convert Character"),
        LOCTEXT("ConvertCharacterTooltip", "Convert Character to Static Mesh"),
        FSlateIcon("EditorStyle", "Persona.TogglePreviewAsset", "Persona.TogglePreviewAsset.Small")
        );
}

void FECSPluginModule::ConvertToMesh(UDebugSkelMeshComponent* PreviewComponent)
{
    UBlueprint* Blueprint = FKismetEditorUtilities::CreateBlueprintFromClass(FText::FromString(TEXT("CreateNewBlueprint")), ACharacterActor::StaticClass(),TEXT("CharacterBP"));
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FECSPluginModule, ECSPlugin)