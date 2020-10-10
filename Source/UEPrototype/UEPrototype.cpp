// Copyright Epic Games, Inc. All Rights Reserved.

#include "UEPrototype.h"
#include "Modules/ModuleManager.h"

//IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, UEPrototype, "UEPrototype" );
IMPLEMENT_PRIMARY_GAME_MODULE(FUEPrototypeModule, UEPrototype, "UEPrototype" );

void FUEPrototypeModule::StartupModule()
{
    // Maps virtual shader source directory to actual shaders directory on disk.
    FString ShaderDirectory = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders/Private"));
    AddShaderSourceDirectoryMapping("/CustomShaders", ShaderDirectory);
}

void FUEPrototypeModule::ShutdownModule()
{
}
