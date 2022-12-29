/*******************************************************************************
The content of this file includes portions of the proprietary AUDIOKINETIC Wwise
Technology released in source code form as part of the game integration package.
The content of this file may not be used without valid licenses to the
AUDIOKINETIC Wwise Technology.
Note that the use of the game engine is subject to the Unreal(R) Engine End User
License Agreement at https://www.unrealengine.com/en-US/eula/unreal
 
License Usage
 
Licensees holding valid licenses to the AUDIOKINETIC Wwise Technology may use
this file in accordance with the end user license agreement provided with the
software or, alternatively, in accordance with the terms contained
in a written agreement between you and Audiokinetic Inc.
Copyright (c) 2022 Audiokinetic Inc.
*******************************************************************************/

using UnrealBuildTool;

public class Wwise : ModuleRules
{
	public static void AddDependencies(ModuleRules Module, ReadOnlyTargetRules Target)
	{
		Module.PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"AkAudio",
				"WwiseSoundEngine"
			}
		);

		// Optional modules
		Module.PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"AkAudioMixer",
#if UE_5_1_OR_LATER
				"WwiseAudioLinkRuntime",
#endif
				"WwiseConcurrency",
				"WwiseSimpleExternalSource"
			}
		);

		// Editor-only optional modules
		if (Target.bBuildEditor)
		{
			Module.PrivateDependencyModuleNames.AddRange(
				new string[]
				{
#if UE_5_1_OR_LATER
					"WwiseAudioLinkEditor"
#endif
				}
			);
		}
	}


	public Wwise(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		Wwise.AddDependencies(this, Target);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
			}
		);


		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"AudiokineticTools"
				}
			);
		}
	}
}
