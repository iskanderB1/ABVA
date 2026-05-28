// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Abva : ModuleRules
{
	public Abva(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "AIModule","InputCore", "EnhancedInput"});
	}
}
