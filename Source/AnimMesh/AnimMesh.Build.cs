// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class AnimMesh : ModuleRules
{
	public AnimMesh(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		//Whether to add all the default include paths to the module (eg. the Source/Classes folder, subfolders under Source/Public).
		bAddDefaultIncludePaths = true;

 
 		PrivateIncludePaths.Add(EngineDirectory + "/Shaders/Shared");
 
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
	}
}
