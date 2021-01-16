// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class UEPrototype : ModuleRules
{
	public UEPrototype(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bAddDefaultIncludePaths = true;
        
        
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "AnimMesh", "FlecsModule"});

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
                "GraphEditor",
                "SkeletalMeshEditor",
                // ... add other private include paths required here ...
            }
        );

        PublicIncludePaths.AddRange(new string[]
        {
            "UEPrototype/ThirdParty",
            "UEPrototype/Public",
            "UEPrototype/Private",
            "UEPrototype/ECS",
            "UEPrototype/Converters",
            "UEPrototype/Animations"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InteractiveToolsFramework",
            "MeshDescription",
            "RenderCore",
            "RHI",
            "StaticMeshDescription"
        });
        
     //   RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ThirdPartyPath, "pthreadVC2.dll")));
		
        PublicDelayLoadDLLs.Add(Path.Combine(ThirdPartyPath,"pthreadVC2.dll"));
        PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath,"pthreadVC2.lib"));
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../Source/UEPrototype/")); }
    }
}

