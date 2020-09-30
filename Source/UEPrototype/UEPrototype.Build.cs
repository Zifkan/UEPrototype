// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class UEPrototype : ModuleRules
{
	public UEPrototype(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bAddDefaultIncludePaths = true;
        
        
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay"});

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
            "UEPrototype/Animations",
            /* Path.Combine(ThirdPartyPath, "ECSPlugin", "Source", "ECSPlugin","Public"),
             Path.Combine(ThirdPartyPath, "ECSPlugin", "Source", "ECSPlugin","Private"),
             Path.Combine(ThirdPartyPath, "ECSPlugin", "Source", "ECSPlugin","ECS"),
             Path.Combine(ThirdPartyPath, "ECSPlugin", "Source", "ECSPlugin","ECS","Systems"),
             Path.Combine(ThirdPartyPath, "ECSPlugin", "Source", "ECSPlugin","ECS","Systems","Transforms"),*/
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
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../Plugins/")); }
    }
}

