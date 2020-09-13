#include "FCharacterAnimEditorModule.h"

#include "Persona/Public/IPersonaToolkit.h"

#define LOCTEXT_NAMESPACE "FCharacterAnimEditorModule"

void FCharacterAnimEditorModule::StartupModule()
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

void FCharacterAnimEditorModule::ShutdownModule()
{
}

void FCharacterAnimEditorModule::AddCharacterAnimEditorExtender()
{
    ISkeletalMeshEditorModule& SkeletalMeshEditorModule = FModuleManager::Get().LoadModuleChecked<ISkeletalMeshEditorModule>(moduleNameConst);
     auto& ToolbarExtenders = SkeletalMeshEditorModule.GetAllSkeletalMeshEditorToolbarExtenders();

    ToolbarExtenders.Add(ISkeletalMeshEditorModule::FSkeletalMeshEditorToolbarExtender::CreateRaw(this, &FCharacterAnimEditorModule::GetCharacterAnimEditorExtender));
     SkeletalMeshEditorExtenderHandle = ToolbarExtenders.Last().GetHandle();
}

void FCharacterAnimEditorModule::RemoveCharacterAnimEditorExtender()
{

}

TSharedRef<FExtender> FCharacterAnimEditorModule::GetCharacterAnimEditorExtender( const TSharedRef<FUICommandList> CommandList, TSharedRef<ISkeletalMeshEditor> InSkeletalMeshEditor)
{
    TSharedRef<FExtender> Extender = MakeShareable(new FExtender);    
    UDebugSkelMeshComponent* MeshComponent = InSkeletalMeshEditor->GetPersonaToolkit()->GetPreviewMeshComponent();

  

    Extender->AddToolBarExtension(
        "Asset",
        EExtensionHook::After,
        CommandList,
        FToolBarExtensionDelegate::CreateRaw(this, &FCharacterAnimEditorModule::HandleAddCharacterAnimEditorExtenderToToolbar, MeshComponent)
    );

    return Extender;
}


void FCharacterAnimEditorModule::HandleAddCharacterAnimEditorExtenderToToolbar(FToolBarBuilder & ParentToolbarBuilder, UDebugSkelMeshComponent * InMeshComponent)
{

    ParentToolbarBuilder.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([this, InMeshComponent]()
    {
        ConvertToMesh(InMeshComponent);
    })),
        NAME_None,
        LOCTEXT("BakeAnim", "Bake Anim"),
        LOCTEXT("BakeAnimToTextureTooltip", "Bake animation frames to textures"),
        FSlateIcon("EditorStyle", "Persona.TogglePreviewAsset", "Persona.TogglePreviewAsset.Small")
        );
}

void FCharacterAnimEditorModule::ConvertToMesh(UDebugSkelMeshComponent* PreviewComponent)
{

}



