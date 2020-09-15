// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CharacterActor.h"
#include "RawMesh.h"

struct FRawMeshTracker
{
	FRawMeshTracker()
        : bValidColors(false)
	{
		FMemory::Memset(bValidTexCoords, 0);
	}

	bool bValidTexCoords[MAX_MESH_TEXTURE_COORDS];
	bool bValidColors;
};

class ISkeletalMeshEditor;

class ECSPLUGIN_API CharacterConverterTool
{
public:
	CharacterConverterTool(){};
	~CharacterConverterTool(){};
	
	void Startup();
	void Shutdown();
	
private:
	void AddCharacterAnimEditorExtender();
	void RemoveCharacterAnimEditorExtender();

	FDelegateHandle ModuleLoadedDelegateHandle;
	FDelegateHandle SkeletalMeshEditorExtenderHandle;
	TSharedRef<FExtender> GetCharacterAnimEditorExtender(const TSharedRef<FUICommandList> CommandList, TSharedRef<ISkeletalMeshEditor> InSkeletalMeshEditor);
	void HandleAddCharacterAnimEditorExtenderToToolbar(FToolBarBuilder& ParentToolbarBuilder, UDebugSkelMeshComponent* InMeshComponent);




	ACharacterActor* ConvertToMesh(UDebugSkelMeshComponent* PreviewComponent);
	void CreateCharacterBluePrint(UDebugSkelMeshComponent* PreviewComponent);
	
	const char* moduleNameConst = "SkeletalMeshEditor";
};
