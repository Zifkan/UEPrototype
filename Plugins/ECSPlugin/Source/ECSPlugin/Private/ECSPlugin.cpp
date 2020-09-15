// Copyright Epic Games, Inc. All Rights Reserved.

#include "ECSPlugin.h"
#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "GameFramework/GameModeBase.h"
#include "Animation/DebugSkelMeshComponent.h"


#define LOCTEXT_NAMESPACE "FECSPluginModule"


void FECSPluginModule::StartupModule()
{
	characterConverterTool.Startup();
}

void FECSPluginModule::ShutdownModule()
{
	characterConverterTool.Shutdown();
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FECSPluginModule, ECSPlugin)