// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class FlecsModule : ModuleRules
{
	public FlecsModule(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		//Whether to add all the default include paths to the module (eg. the Source/Classes folder, subfolders under Source/Public).
		bAddDefaultIncludePaths = true;

		PublicDependencyModuleNames.AddRange(new string[] { });
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
		
		PublicIncludePaths.AddRange(new string[]
		{
			"FlecsModule/Public",
		});
		
		PrivateIncludePaths.AddRange(new string[]
		{
			"FlecsModule/Private",
		});

	
	}
	
	private string ThirdPartyPath
	{
		get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../Source/FlecsModule/")); }
	}
}
