// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class UEPrototype : ModuleRules
{
	public UEPrototype(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay"});

        PublicIncludePaths.AddRange(new string[]
        {
            "UEPrototype/ThirdParty",
            "UEPrototype/Public",
            "UEPrototype/Private",
            "UEPrototype/ECS",
            "UEPrototype/Converters",
            /* Path.Combine(ThirdPartyPath, "ECSPlugin", "Source", "ECSPlugin","Public"),
             Path.Combine(ThirdPartyPath, "ECSPlugin", "Source", "ECSPlugin","Private"),
             Path.Combine(ThirdPartyPath, "ECSPlugin", "Source", "ECSPlugin","ECS"),
             Path.Combine(ThirdPartyPath, "ECSPlugin", "Source", "ECSPlugin","ECS","Systems"),
             Path.Combine(ThirdPartyPath, "ECSPlugin", "Source", "ECSPlugin","ECS","Systems","Transforms"),*/
        });
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../Plugins/")); }
    }
}

