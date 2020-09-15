// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "CharacterActor.h"
#include "ISkeletalMeshEditor.h"

class FECSPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

    void AddCharacterAnimEditorExtender();
    void RemoveCharacterAnimEditorExtender();

    FDelegateHandle ModuleLoadedDelegateHandle;
    FDelegateHandle SkeletalMeshEditorExtenderHandle;
    TSharedRef<FExtender> GetCharacterAnimEditorExtender(const TSharedRef<FUICommandList> CommandList, TSharedRef<ISkeletalMeshEditor> InSkeletalMeshEditor);
    void HandleAddCharacterAnimEditorExtenderToToolbar(FToolBarBuilder& ParentToolbarBuilder, UDebugSkelMeshComponent* InMeshComponent);

private:


    ACharacterActor* ConvertToMesh(UDebugSkelMeshComponent* PreviewComponent);
	void CreateCharacterBluePrint(UDebugSkelMeshComponent* PreviewComponent);

	
    const char* moduleNameConst = "SkeletalMeshEditor";
};
