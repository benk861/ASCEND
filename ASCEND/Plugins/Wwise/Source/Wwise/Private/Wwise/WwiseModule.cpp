// Copyright Epic Games, Inc. All Rights Reserved.

#include "Modules/ModuleManager.h"

#if !UE_SERVER
#include "Misc/CoreDelegates.h"
#include "Misc/ConfigCacheIni.h"
#include "AkUEFeatures.h"
#endif

class FWwiseModule : public IModuleInterface
{
#if !UE_SERVER
public:
	virtual void StartupModule() override
	{
		UE_LOG(LogLoad, Log, TEXT("WwiseModule: Loading AkAudio"));
		FModuleManager::Get().LoadModule(TEXT("AkAudio"));

		FCoreDelegates::OnPostEngineInit.AddRaw(this, &FWwiseModule::OnPostEngineInit);

		// Loading optional modules
		bool bAkAudioMixerEnabled = false;
		GConfig->GetBool(TEXT("/Script/AkAudio.AkSettings"), TEXT("bAkAudioMixerEnabled"), bAkAudioMixerEnabled, GGameIni);
		if (bAkAudioMixerEnabled)
		{
			UE_LOG(LogLoad, Log, TEXT("WwiseModule: Loading AkAudioMixer"));
			FModuleManager::Get().LoadModule(TEXT("AkAudioMixer"));
		}

		bool bWwiseAudioLinkEnabled = false;
		GConfig->GetBool(TEXT("/Script/AkAudio.AkSettings"), TEXT("bWwiseAudioLinkEnabled"), bWwiseAudioLinkEnabled, GGameIni);
		if (bWwiseAudioLinkEnabled)
		{
#if UE_5_1_OR_LATER
			UE_LOG(LogLoad, Log, TEXT("WwiseModule: Loading WwiseAudioLink"));
			FModuleManager::Get().LoadModule(TEXT("WwiseAudioLinkRuntime"));

#if WITH_EDITOR
			FModuleManager::Get().LoadModule(TEXT("WwiseAudioLinkEditor"));
#endif

#else
			UE_LOG(LogLoad, Error, TEXT("WwiseModule: AudioLink is not available in Unreal versions prior to 5.1. Ignoring."));
#endif
		}
	}
	
	void OnPostEngineInit()
	{
#if WITH_EDITOR
		UE_LOG(LogLoad, Log, TEXT("WwiseModule: Loading AudiokineticTools"));
		FModuleManager::Get().LoadModule(TEXT("AudiokineticTools"));
#endif
	}
#endif
};

IMPLEMENT_MODULE(FWwiseModule, Wwise);
