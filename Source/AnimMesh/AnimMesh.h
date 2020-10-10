// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"



class ANIMMESH_API FAnimMeshModule : public IModuleInterface
{
public:
	static inline FAnimMeshModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FAnimMeshModule>("AnimMesh");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("AnimMesh");
	}

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
