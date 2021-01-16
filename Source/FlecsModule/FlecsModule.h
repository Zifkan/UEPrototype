// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"


class   FFlecsModule : public IModuleInterface
{
public:
	static inline FFlecsModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FFlecsModule>("FlecsModule");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("FlecsModule");
	}

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
