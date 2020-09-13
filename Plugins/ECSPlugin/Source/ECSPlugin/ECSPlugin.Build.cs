// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ECSPlugin : ModuleRules
{
	public ECSPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "Projects",
                "TargetPlatform",
                "MeshDescription",
                "MeshDescriptionOperations",

                "CoreUObject",
                "Engine",
                "RawMesh",
                "Slate",
                "SlateCore",
                "UnrealEd",
                "EditorStyle",
                "RHI",
                "SlateCore",
                "Slate",
                "RenderCore",
                "AnimGraph",
                "AnimationBlueprintEditor",
                "AnimationEditor",
                "SkeletalMeshEditor",
				// ... add private dependencies that you statically link with here ...	
			}
            );
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
