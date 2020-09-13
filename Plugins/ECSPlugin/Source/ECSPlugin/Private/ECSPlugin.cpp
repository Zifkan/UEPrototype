// Copyright Epic Games, Inc. All Rights Reserved.

#include "ECSPlugin.h"


#include "IPersonaToolkit.h"
#include "ISkeletalMeshEditorModule.h"

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

}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FECSPluginModule, ECSPlugin)