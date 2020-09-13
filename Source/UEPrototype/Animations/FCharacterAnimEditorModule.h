#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "SkeletalMeshEditor/Public/ISkeletalMeshEditorModule.h"

class FCharacterAnimEditorModule : public IModuleInterface
{
public:

    void StartupModule() override;
    void ShutdownModule() override;



    void AddCharacterAnimEditorExtender();
    void RemoveCharacterAnimEditorExtender();

    FDelegateHandle ModuleLoadedDelegateHandle;
    FDelegateHandle SkeletalMeshEditorExtenderHandle;
    TSharedRef<FExtender> GetCharacterAnimEditorExtender(const TSharedRef<FUICommandList> CommandList, TSharedRef<ISkeletalMeshEditor> InSkeletalMeshEditor);
    void HandleAddCharacterAnimEditorExtenderToToolbar(FToolBarBuilder& ParentToolbarBuilder, UDebugSkelMeshComponent* InMeshComponent);

private:


    void ConvertToMesh(UDebugSkelMeshComponent* PreviewComponent);

    const char* moduleNameConst = "SkeletalMeshEditor";

};
