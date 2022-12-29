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
using System.IO;
using System.Linq;
using System.Collections.Generic;

public class WwiseSoundEngine_2022_1 : ModuleRules
{
	private List<string> AkLibs = new List<string> 
	{
		"AkSoundEngine",
		"AkMemoryMgr",
		"AkStreamMgr",
		"AkMusicEngine",
		"AkSpatialAudio",
		"AkAudioInputSource",
		"AkVorbisDecoder",
		"AkMeterFX", // AkMeter does not have a dedicated DLL
	};
	
	public WwiseSoundEngine_2022_1(ReadOnlyTargetRules Target) : base(Target)
	{
		if (!WwiseSoundEngineVersion.IsSoundEngineVersionSupported(PluginDirectory, GetType().Name))
		{
			// We are skipping this version since this Wwise Sound Engine is for a particular version only.
			Type = ModuleType.External;
			return;
		}
		PublicDefinitions.AddRange(WwiseSoundEngineVersion.GetVersionDefinesFromClassName(GetType().Name));

		// If packaging as an Engine plugin, the UBT expects to already have a precompiled plugin available
		// This can be set to true so long as plugin was already precompiled
		bUsePrecompiled = false;
		bPrecompile = false;

		string ThirdPartyFolder = Path.Combine(ModuleDirectory, "../../ThirdParty");
		var VersionNumber = WwiseSoundEngineVersion.GetVersionFromClassName(GetType().Name);
		var WwiseUEPlatformInstance = WwiseUEPlatform.GetWwiseUEPlatformInstance(Target, VersionNumber, ThirdPartyFolder);
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bAllowConfidentialPlatformDefines = true;

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine"
			}
		);

		ExternalDependencies.Add("../WwiseSoundEngine/WwiseUEPlatform.Build.cs");
		ExternalDependencies.Add("../WwiseSoundEngine/WwiseSoundEngineVersion.Build.cs");
		foreach (var Platform in GetAvailablePlatforms(ModuleDirectory))
		{
			ExternalDependencies.Add(string.Format("WwiseUEPlatform_{0}_{1}.Build.cs", VersionNumber, Platform));
		}
		
		if (Target.bBuildEditor)
		{
			foreach (var Platform in GetAvailablePlatforms(ModuleDirectory))
			{
				PublicDefinitions.Add("AK_PLATFORM_" + Platform.ToUpper());
			}
		}

		PublicIncludePathModuleNames.Add("WwiseSoundEngine");
		PublicIncludePaths.Add(Path.Combine(ThirdPartyFolder, "include"));

		PublicDefinitions.Add("AK_UNREAL_MAX_CONCURRENT_IO=32");
		PublicDefinitions.Add("AK_UNREAL_IO_GRANULARITY=32768");
		if (Target.Configuration == UnrealTargetConfiguration.Shipping)
		{
			PublicDefinitions.Add("AK_OPTIMIZED");
		}

		if (Target.Configuration != UnrealTargetConfiguration.Shipping && WwiseUEPlatformInstance.SupportsCommunication)
		{
			AkLibs.Add("CommunicationCentral");
			PublicDefinitions.Add("AK_ENABLE_COMMUNICATION=1");
		}
		else
		{
			PublicDefinitions.Add("AK_ENABLE_COMMUNICATION=0");
		}

		if (WwiseUEPlatformInstance.SupportsAkAutobahn)
		{
			AkLibs.Add("AkAutobahn");
			PublicDefinitions.Add("AK_SUPPORT_WAAPI=1");
		}
		else
		{
			PublicDefinitions.Add("AK_SUPPORT_WAAPI=0");
		}

		if (WwiseUEPlatformInstance.SupportsOpus)
		{
			AkLibs.Add("AkOpusDecoder");
			PublicDefinitions.Add("AK_SUPPORT_OPUS=1");
		}
		else
		{
			PublicDefinitions.Add("AK_SUPPORT_OPUS=0");
		}

		if (WwiseUEPlatformInstance.SupportsDeviceMemory)
		{
			PublicDefinitions.Add("AK_SUPPORT_DEVICE_MEMORY=1");
		}
		else
		{
			PublicDefinitions.Add("AK_SUPPORT_DEVICE_MEMORY=0");
		}

		// Platform-specific dependencies
		PublicSystemLibraries.AddRange(WwiseUEPlatformInstance.GetPublicSystemLibraries());
		AkLibs.AddRange(WwiseUEPlatformInstance.GetAdditionalWwiseLibs());
		PublicDefinitions.AddRange(WwiseUEPlatformInstance.GetPublicDefinitions());
		PublicDefinitions.Add(string.Format("AK_CONFIGURATION=\"{0}\"", WwiseUEPlatformInstance.AkConfigurationDir));
		var AdditionalProperty = WwiseUEPlatformInstance.GetAdditionalPropertyForReceipt(ModuleDirectory);
		if (AdditionalProperty != null)
		{
			AdditionalPropertiesForReceipt.Add(AdditionalProperty.Item1, AdditionalProperty.Item2);
		}

		PublicFrameworks.AddRange(WwiseUEPlatformInstance.GetPublicFrameworks());
		
		PublicDelayLoadDLLs.AddRange(WwiseUEPlatformInstance.GetPublicDelayLoadDLLs());
		foreach(var RuntimeDependency in WwiseUEPlatformInstance.GetRuntimeDependencies())
		{
			RuntimeDependencies.Add(RuntimeDependency);
		}
		PublicAdditionalLibraries.AddRange(WwiseUEPlatformInstance.GetSanitizedAkLibList(AkLibs));
	}

	private static List<string> GetAvailablePlatforms(string ModuleDir)
	{
		var FoundPlatforms = new List<string>();
		const string StartPattern = "WwiseUEPlatform_";
		const string EndPattern = ".Build.cs";
		foreach (var BuildCsFile in System.IO.Directory.GetFiles(ModuleDir, "*" + EndPattern))
		{
			if (BuildCsFile.Contains(StartPattern) && BuildCsFile.EndsWith(EndPattern))
			{
				var Platform = BuildCsFile.Remove(BuildCsFile.Length - EndPattern.Length).Split('_').Last();
				FoundPlatforms.Add(Platform);
			}
		}

		return FoundPlatforms;
	}
}
