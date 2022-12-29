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

#include "Wwise/WwiseGlobalCallbacks.h"
#include "Wwise/API/WwiseSoundEngineAPI.h"
#include "Wwise/Stats/Concurrency.h"

#include "AkUnrealHelper.h"

AK::IAkGlobalPluginContext* FWwiseGlobalCallbacks::Context = nullptr;
const TCHAR* FWwiseGlobalCallbacks::CallbackName = nullptr;
AK::IAkGlobalPluginContext* FWwiseGlobalCallbacks::MonitorContext = nullptr;
const TCHAR* FWwiseGlobalCallbacks::MonitorCallbackName = nullptr;

FWwiseGlobalCallbacks::~FWwiseGlobalCallbacks()
{
	if (UNLIKELY(bInitialized))
	{
		UE_LOG(LogWwiseConcurrency, Error, TEXT("GlobalCallbacks not terminated at destruction."));
	}
}

bool FWwiseGlobalCallbacks::Initialize()
{
	if (UNLIKELY(bInitialized))
	{
		UE_LOG(LogWwiseConcurrency, Fatal, TEXT("Global Callbacks already initialized"));
		return false;
	}

	auto* SoundEngine = IWwiseSoundEngineAPI::Get();
	if (UNLIKELY(!SoundEngine))
	{
		UE_LOG(LogWwiseConcurrency, Fatal, TEXT("Could not implement callbacks."));
		return false;
	}
	bInitialized = true;

	bool bResult = true;
	AKRESULT Result;
	Result = SoundEngine->RegisterGlobalCallback(&FWwiseGlobalCallbacks::OnRegisterCallbackStatic, AkGlobalCallbackLocation_Register, (void*)this);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Error, TEXT("Cannot Register `Register` Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));
	bResult = bResult && (Result == AK_Success);

	Result = SoundEngine->RegisterGlobalCallback(&FWwiseGlobalCallbacks::OnBeginCallbackStatic, AkGlobalCallbackLocation_Begin, (void*)this);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Error, TEXT("Cannot Register `Begin` Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));
	bResult = bResult && (Result == AK_Success);

	Result = SoundEngine->RegisterGlobalCallback(&FWwiseGlobalCallbacks::OnPreProcessMessageQueueForRenderCallbackStatic, AkGlobalCallbackLocation_PreProcessMessageQueueForRender, (void*)this);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Error, TEXT("Cannot Register `PreProcessMessageQueueForRender` Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));
	bResult = bResult && (Result == AK_Success);

	Result = SoundEngine->RegisterGlobalCallback(&FWwiseGlobalCallbacks::OnPostMessagesProcessedCallbackStatic, AkGlobalCallbackLocation_PostMessagesProcessed, (void*)this);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Error, TEXT("Cannot Register `PostMessagesProcessed` Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));
	bResult = bResult && (Result == AK_Success);

	Result = SoundEngine->RegisterGlobalCallback(&FWwiseGlobalCallbacks::OnBeginRenderCallbackStatic, AkGlobalCallbackLocation_BeginRender, (void*)this);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Error, TEXT("Cannot Register `BeginRender` Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));
	bResult = bResult && (Result == AK_Success);

	Result = SoundEngine->RegisterGlobalCallback(&FWwiseGlobalCallbacks::OnEndRenderCallbackStatic, AkGlobalCallbackLocation_EndRender, (void*)this);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Error, TEXT("Cannot Register `EndRender` Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));
	bResult = bResult && (Result == AK_Success);

	Result = SoundEngine->RegisterGlobalCallback(&FWwiseGlobalCallbacks::OnEndCallbackStatic, AkGlobalCallbackLocation_End, (void*)this);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Error, TEXT("Cannot Register `End` Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));
	bResult = bResult && (Result == AK_Success);

	Result = SoundEngine->RegisterGlobalCallback(&FWwiseGlobalCallbacks::OnTermCallbackStatic, AkGlobalCallbackLocation_Term, (void*)this);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Error, TEXT("Cannot Register `Term` Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));
	bResult = bResult && (Result == AK_Success);

	Result = SoundEngine->RegisterGlobalCallback(&FWwiseGlobalCallbacks::OnMonitorCallbackStatic, AkGlobalCallbackLocation_Monitor, (void*)this);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Error, TEXT("Cannot Register `Monitor` Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));
	bResult = bResult && (Result == AK_Success);

	Result = SoundEngine->RegisterGlobalCallback(&FWwiseGlobalCallbacks::OnMonitorRecapCallbackStatic, AkGlobalCallbackLocation_MonitorRecap, (void*)this);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Error, TEXT("Cannot Register `MonitorRecap` Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));
	bResult = bResult && (Result == AK_Success);

	Result = SoundEngine->RegisterGlobalCallback(&FWwiseGlobalCallbacks::OnInitCallbackStatic, AkGlobalCallbackLocation_Init, (void*)this);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Error, TEXT("Cannot Register `Init` Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));
	bResult = bResult && (Result == AK_Success);

	Result = SoundEngine->RegisterGlobalCallback(&FWwiseGlobalCallbacks::OnSuspendCallbackStatic, AkGlobalCallbackLocation_Suspend, (void*)this);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Error, TEXT("Cannot Register `Suspend` Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));
	bResult = bResult && (Result == AK_Success);

	Result = SoundEngine->RegisterGlobalCallback(&FWwiseGlobalCallbacks::OnWakeupFromSuspendCallbackStatic, AkGlobalCallbackLocation_WakeupFromSuspend, (void*)this);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Error, TEXT("Cannot Register `WakeupFromSuspend` Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));
	bResult = bResult && (Result == AK_Success);

	return bResult;
}

void FWwiseGlobalCallbacks::Terminate()
{
	if (!bInitialized)
	{
		return;
	}

	auto* SoundEngine = IWwiseSoundEngineAPI::Get();
	if (UNLIKELY(!SoundEngine))
	{
		return;
	}
	bInitialized = false;

	AKRESULT Result;
	Result = SoundEngine->UnregisterGlobalCallback(&FWwiseGlobalCallbacks::OnRegisterCallbackStatic, AkGlobalCallbackLocation_Register);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Verbose, TEXT("Cannot Register Register Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));

	Result = SoundEngine->UnregisterGlobalCallback(&FWwiseGlobalCallbacks::OnBeginCallbackStatic, AkGlobalCallbackLocation_Begin);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Verbose, TEXT("Cannot Register Begin Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));

	Result = SoundEngine->UnregisterGlobalCallback(&FWwiseGlobalCallbacks::OnPreProcessMessageQueueForRenderCallbackStatic, AkGlobalCallbackLocation_PreProcessMessageQueueForRender);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Verbose, TEXT("Cannot Register PreProcessMessageQueueForRender Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));

	Result = SoundEngine->UnregisterGlobalCallback(&FWwiseGlobalCallbacks::OnPostMessagesProcessedCallbackStatic, AkGlobalCallbackLocation_PostMessagesProcessed);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Verbose, TEXT("Cannot Register PostMessagesProcessed Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));

	Result = SoundEngine->UnregisterGlobalCallback(&FWwiseGlobalCallbacks::OnBeginRenderCallbackStatic, AkGlobalCallbackLocation_BeginRender);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Verbose, TEXT("Cannot Register BeginRender Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));

	Result = SoundEngine->UnregisterGlobalCallback(&FWwiseGlobalCallbacks::OnEndRenderCallbackStatic, AkGlobalCallbackLocation_EndRender);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Verbose, TEXT("Cannot Register EndRender Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));

	Result = SoundEngine->UnregisterGlobalCallback(&FWwiseGlobalCallbacks::OnEndCallbackStatic, AkGlobalCallbackLocation_End);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Verbose, TEXT("Cannot Register End Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));

	Result = SoundEngine->UnregisterGlobalCallback(&FWwiseGlobalCallbacks::OnTermCallbackStatic, AkGlobalCallbackLocation_Term);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Verbose, TEXT("Cannot Register Term Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));

	Result = SoundEngine->UnregisterGlobalCallback(&FWwiseGlobalCallbacks::OnMonitorCallbackStatic, AkGlobalCallbackLocation_Monitor);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Verbose, TEXT("Cannot Register Monitor Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));

	Result = SoundEngine->UnregisterGlobalCallback(&FWwiseGlobalCallbacks::OnMonitorRecapCallbackStatic, AkGlobalCallbackLocation_MonitorRecap);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Verbose, TEXT("Cannot Register MonitorRecap Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));

	Result = SoundEngine->UnregisterGlobalCallback(&FWwiseGlobalCallbacks::OnInitCallbackStatic, AkGlobalCallbackLocation_Init);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Verbose, TEXT("Cannot Register Init Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));

	Result = SoundEngine->UnregisterGlobalCallback(&FWwiseGlobalCallbacks::OnSuspendCallbackStatic, AkGlobalCallbackLocation_Suspend);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Verbose, TEXT("Cannot Register Suspend Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));

	Result = SoundEngine->UnregisterGlobalCallback(&FWwiseGlobalCallbacks::OnWakeupFromSuspendCallbackStatic, AkGlobalCallbackLocation_WakeupFromSuspend);
	UE_CLOG(Result != AK_Success, LogWwiseConcurrency, Verbose, TEXT("Cannot Register WakeupFromSuspend Callback: %d (%s)"), Result, AkUnrealHelper::GetResultString(Result));
}

void FWwiseGlobalCallbacks::RegisterSync(FSyncFunction&& InFunction)
{
	RegisterQueue.SyncDefer([Function = MoveTemp(InFunction)]
	{
		return Function(Context);
	});
}

void FWwiseGlobalCallbacks::RegisterCompletion(FCompletionPromise&& Promise)
{
	RegisterAsync([Promise = MoveTemp(Promise)]() mutable
	{
		Promise.EmplaceValue();
		return EWwiseDeferredAsyncResult::Done;
	});
}

void FWwiseGlobalCallbacks::WaitForRegister()
{
	FEventRef Event;
	RegisterAsync([&Event]() {Event->Trigger(); return EWwiseDeferredAsyncResult::Done; });
	Event->Wait();
	Event->Reset();
}

void FWwiseGlobalCallbacks::BeginSync(FSyncFunction&& InFunction)
{
	BeginQueue.SyncDefer([Function = MoveTemp(InFunction)]
	{
		return Function(Context);
	});
}

void FWwiseGlobalCallbacks::BeginCompletion(FCompletionPromise&& Promise)
{
	BeginAsync([Promise = MoveTemp(Promise)]() mutable
	{
		Promise.EmplaceValue();
		return EWwiseDeferredAsyncResult::Done;
	});
}

void FWwiseGlobalCallbacks::WaitForBegin()
{
	FEventRef Event;
	BeginAsync([&Event]() {Event->Trigger(); return EWwiseDeferredAsyncResult::Done; });
	Event->Wait();
	Event->Reset();
}

void FWwiseGlobalCallbacks::PreProcessMessageQueueForRenderSync(FSyncFunction&& InFunction)
{
	PreProcessMessageQueueForRenderQueue.SyncDefer([Function = MoveTemp(InFunction)]
	{
		return Function(Context);
	});
}

void FWwiseGlobalCallbacks::PreProcessMessageQueueForRenderCompletion(FCompletionPromise&& Promise)
{
	PreProcessMessageQueueForRenderAsync([Promise = MoveTemp(Promise)]() mutable
	{
		Promise.EmplaceValue();
		return EWwiseDeferredAsyncResult::Done;
	});
}

void FWwiseGlobalCallbacks::WaitForPreProcessMessageQueueForRender()
{
	FEventRef Event;
	PreProcessMessageQueueForRenderAsync([&Event]() {Event->Trigger(); return EWwiseDeferredAsyncResult::Done; });
	Event->Wait();
	Event->Reset();
}

void FWwiseGlobalCallbacks::PostMessagesProcessedSync(FSyncFunction&& InFunction)
{
	PostMessagesProcessedQueue.SyncDefer([Function = MoveTemp(InFunction)]
	{
		return Function(Context);
	});
}

void FWwiseGlobalCallbacks::PostMessagesProcessedCompletion(FCompletionPromise&& Promise)
{
	PostMessagesProcessedAsync([Promise = MoveTemp(Promise)]() mutable
	{
		Promise.EmplaceValue();
		return EWwiseDeferredAsyncResult::Done;
	});
}

void FWwiseGlobalCallbacks::WaitForPostMessagesProcessed()
{
	FEventRef Event;
	PostMessagesProcessedAsync([&Event]() {Event->Trigger(); return EWwiseDeferredAsyncResult::Done; });
	Event->Wait();
	Event->Reset();
}

void FWwiseGlobalCallbacks::BeginRenderSync(FSyncFunction&& InFunction)
{
	BeginRenderQueue.SyncDefer([Function = MoveTemp(InFunction)]
	{
		return Function(Context);
	});
}

void FWwiseGlobalCallbacks::BeginRenderCompletion(FCompletionPromise&& Promise)
{
	BeginRenderAsync([Promise = MoveTemp(Promise)]() mutable
	{
		Promise.EmplaceValue();
		return EWwiseDeferredAsyncResult::Done;
	});
}

void FWwiseGlobalCallbacks::WaitForBeginRender()
{
	FEventRef Event;
	BeginRenderAsync([&Event]() {Event->Trigger(); return EWwiseDeferredAsyncResult::Done; });
	Event->Wait();
	Event->Reset();
}

void FWwiseGlobalCallbacks::EndRenderSync(FSyncFunction&& InFunction)
{
	EndRenderQueue.SyncDefer([Function = MoveTemp(InFunction)]
	{
		return Function(Context);
	});
}

void FWwiseGlobalCallbacks::EndRenderCompletion(FCompletionPromise&& Promise)
{
	EndRenderAsync([Promise = MoveTemp(Promise)]() mutable
	{
		Promise.EmplaceValue();
		return EWwiseDeferredAsyncResult::Done;
	});
}

void FWwiseGlobalCallbacks::WaitForEndRender()
{
	FEventRef Event;
	EndRenderAsync([&Event]() {Event->Trigger(); return EWwiseDeferredAsyncResult::Done; });
	Event->Wait();
	Event->Reset();
}

void FWwiseGlobalCallbacks::EndSync(FSyncFunction&& InFunction)
{
	EndQueue.SyncDefer([Function = MoveTemp(InFunction)]
	{
		return Function(Context);
	});
}

void FWwiseGlobalCallbacks::EndCompletion(FCompletionPromise&& Promise, int Count)
{
	if (Count <= 0)
	{
		return Promise.EmplaceValue();
	}

	EndAsync([Promise = MoveTemp(Promise), Count = Count - 1]() mutable
	{
		auto* WwiseGlobalCallbacks = FWwiseGlobalCallbacks::Get();
		WwiseGlobalCallbacks->EndCompletion(MoveTemp(Promise), Count);
		return EWwiseDeferredAsyncResult::Done;
	});
}

void FWwiseGlobalCallbacks::WaitForEnd()
{
	FEventRef Event;
	EndAsync([&Event]() {Event->Trigger(); return EWwiseDeferredAsyncResult::Done; });
	Event->Wait();
	Event->Reset();
}

void FWwiseGlobalCallbacks::TermSync(FSyncFunction&& InFunction)
{
	TermQueue.SyncDefer([Function = MoveTemp(InFunction)]
	{
		return Function(Context);
	});
}

void FWwiseGlobalCallbacks::TermCompletion(FCompletionPromise&& Promise)
{
	TermAsync([Promise = MoveTemp(Promise)]() mutable
	{
		Promise.EmplaceValue();
		return EWwiseDeferredAsyncResult::Done;
	});
}

void FWwiseGlobalCallbacks::WaitForTerm()
{
	FEventRef Event;
	TermAsync([&Event]() {Event->Trigger(); return EWwiseDeferredAsyncResult::Done; });
	Event->Wait();
	Event->Reset();
}

void FWwiseGlobalCallbacks::MonitorSync(FSyncFunction&& InFunction)
{
	MonitorQueue.SyncDefer([Function = MoveTemp(InFunction)]
	{
		return Function(MonitorContext);
	});
}

void FWwiseGlobalCallbacks::MonitorCompletion(FCompletionPromise&& Promise)
{
	MonitorAsync([Promise = MoveTemp(Promise)]() mutable
	{
		Promise.EmplaceValue();
		return EWwiseDeferredAsyncResult::Done;
	});
}

void FWwiseGlobalCallbacks::WaitForMonitor()
{
	FEventRef Event;
	MonitorAsync([&Event]() {Event->Trigger(); return EWwiseDeferredAsyncResult::Done; });
	Event->Wait();
	Event->Reset();
}

void FWwiseGlobalCallbacks::MonitorRecapSync(FSyncFunction&& InFunction)
{
	MonitorRecapQueue.SyncDefer([Function = MoveTemp(InFunction)]
	{
		return Function(MonitorContext);
	});
}

void FWwiseGlobalCallbacks::MonitorRecapCompletion(FCompletionPromise&& Promise)
{
	MonitorRecapAsync([Promise = MoveTemp(Promise)]() mutable
	{
		Promise.EmplaceValue();
		return EWwiseDeferredAsyncResult::Done;
	});
}

void FWwiseGlobalCallbacks::WaitForMonitorRecap()
{
	FEventRef Event;
	MonitorRecapAsync([&Event]() {Event->Trigger(); return EWwiseDeferredAsyncResult::Done; });
	Event->Wait();
	Event->Reset();
}

void FWwiseGlobalCallbacks::InitSync(FSyncFunction&& InFunction)
{
	InitQueue.SyncDefer([Function = MoveTemp(InFunction)]
	{
		return Function(Context);
	});
}

void FWwiseGlobalCallbacks::InitCompletion(FCompletionPromise&& Promise)
{
	InitAsync([Promise = MoveTemp(Promise)]() mutable
	{
		Promise.EmplaceValue();
		return EWwiseDeferredAsyncResult::Done;
	});
}

void FWwiseGlobalCallbacks::WaitForInit()
{
	FEventRef Event;
	InitAsync([&Event]() {Event->Trigger(); return EWwiseDeferredAsyncResult::Done; });
	Event->Wait();
	Event->Reset();
}

void FWwiseGlobalCallbacks::SuspendSync(FSyncFunction&& InFunction)
{
	SuspendQueue.SyncDefer([Function = MoveTemp(InFunction)]
	{
		return Function(Context);
	});
}

void FWwiseGlobalCallbacks::SuspendCompletion(FCompletionPromise&& Promise)
{
	SuspendAsync([Promise = MoveTemp(Promise)]() mutable
	{
		Promise.EmplaceValue();
		return EWwiseDeferredAsyncResult::Done;
	});
}

void FWwiseGlobalCallbacks::WaitForSuspend()
{
	FEventRef Event;
	SuspendAsync([&Event]() {Event->Trigger(); return EWwiseDeferredAsyncResult::Done; });
	Event->Wait();
	Event->Reset();
}

void FWwiseGlobalCallbacks::WakeupFromSuspendSync(FSyncFunction&& InFunction)
{
	WakeupFromSuspendQueue.SyncDefer([Function = MoveTemp(InFunction)]
	{
		return Function(Context);
	});
}

void FWwiseGlobalCallbacks::WakeupFromSuspendCompletion(FCompletionPromise&& Promise)
{
	WakeupFromSuspendAsync([Promise = MoveTemp(Promise)]() mutable
	{
		Promise.EmplaceValue();
		return EWwiseDeferredAsyncResult::Done;
	});
}

void FWwiseGlobalCallbacks::WaitForWakeupFromSuspend()
{
	FEventRef Event;
	WakeupFromSuspendAsync([&Event]() {Event->Trigger(); return EWwiseDeferredAsyncResult::Done; });
	Event->Wait();
	Event->Reset();
}

void FWwiseGlobalCallbacks::OnRegisterCallback(AK::IAkGlobalPluginContext* in_pContext)
{
	if (UNLIKELY(Context))
	{
		const TCHAR* PrevName = CallbackName;
		if (!CallbackName)
		{
			PrevName = TEXT("[unknown]");
		}
		UE_LOG(LogWwiseConcurrency, Error, TEXT("FWwiseGlobalCallbacks::OnRegisterCallback: Doing two callbacks at once (previous: %s)"), PrevName);
	}
	CallbackName = TEXT("Register");
	Context = in_pContext;
	RegisterQueue.Run();
	CallbackName = nullptr;
	Context = nullptr;
}

void FWwiseGlobalCallbacks::OnBeginCallback(AK::IAkGlobalPluginContext* in_pContext)
{
	if (UNLIKELY(Context))
	{
		const TCHAR* PrevName = CallbackName;
		if (!CallbackName)
		{
			PrevName = TEXT("[unknown]");
		}
		UE_LOG(LogWwiseConcurrency, Error, TEXT("FWwiseGlobalCallbacks::OnBeginCallback: Doing two callbacks at once (previous: %s)"), PrevName);
	}
	CallbackName = TEXT("Begin");
	Context = in_pContext;
	BeginQueue.Run();
	CallbackName = nullptr;
	Context = nullptr;
}

void FWwiseGlobalCallbacks::OnPreProcessMessageQueueForRenderCallback(AK::IAkGlobalPluginContext* in_pContext)
{
	if (UNLIKELY(Context))
	{
		const TCHAR* PrevName = CallbackName;
		if (!CallbackName)
		{
			PrevName = TEXT("[unknown]");
		}
		UE_LOG(LogWwiseConcurrency, Error, TEXT("FWwiseGlobalCallbacks::OnPreProcessMessageQueueForRenderCallback: Doing two callbacks at once (previous: %s)"), PrevName);
	}
	CallbackName = TEXT("PreProcessMessageQueueForRender");
	Context = in_pContext;
	PreProcessMessageQueueForRenderQueue.Run();
	CallbackName = nullptr;
	Context = nullptr;
}

void FWwiseGlobalCallbacks::OnPostMessagesProcessedCallback(AK::IAkGlobalPluginContext* in_pContext)
{
	if (UNLIKELY(Context))
	{
		const TCHAR* PrevName = CallbackName;
		if (!CallbackName)
		{
			PrevName = TEXT("[unknown]");
		}
		UE_LOG(LogWwiseConcurrency, Error, TEXT("FWwiseGlobalCallbacks::OnPostMessagesProcessedCallback: Doing two callbacks at once (previous: %s)"), PrevName);
	}
	CallbackName = TEXT("PostMessagesProcessed");
	Context = in_pContext;
	PostMessagesProcessedQueue.Run();
	CallbackName = nullptr;
	Context = nullptr;
}

void FWwiseGlobalCallbacks::OnBeginRenderCallback(AK::IAkGlobalPluginContext* in_pContext)
{
	if (UNLIKELY(Context))
	{
		const TCHAR* PrevName = CallbackName;
		if (!CallbackName)
		{
			PrevName = TEXT("[unknown]");
		}
		UE_LOG(LogWwiseConcurrency, Error, TEXT("FWwiseGlobalCallbacks::OnBeginRenderCallback: Doing two callbacks at once (previous: %s)"), PrevName);
	}
	CallbackName = TEXT("BeginRender");
	Context = in_pContext;
	BeginRenderQueue.Run();
	CallbackName = nullptr;
	Context = nullptr;
}

void FWwiseGlobalCallbacks::OnEndRenderCallback(AK::IAkGlobalPluginContext* in_pContext)
{
	if (UNLIKELY(Context))
	{
		const TCHAR* PrevName = CallbackName;
		if (!CallbackName)
		{
			PrevName = TEXT("[unknown]");
		}
		UE_LOG(LogWwiseConcurrency, Error, TEXT("FWwiseGlobalCallbacks::OnEndRenderCallback: Doing two callbacks at once (previous: %s)"), PrevName);
	}
	CallbackName = TEXT("EndRender");
	Context = in_pContext;
	EndRenderQueue.Run();
	CallbackName = nullptr;
	Context = nullptr;
}

void FWwiseGlobalCallbacks::OnEndCallback(AK::IAkGlobalPluginContext* in_pContext)
{
	if (UNLIKELY(Context))
	{
		const TCHAR* PrevName = CallbackName;
		if (!CallbackName)
		{
			PrevName = TEXT("[unknown]");
		}
		UE_LOG(LogWwiseConcurrency, Error, TEXT("FWwiseGlobalCallbacks::OnEndCallback: Doing two callbacks at once (previous: %s)"), PrevName);
	}
	CallbackName = TEXT("End");
	Context = in_pContext;
	EndQueue.Run();
	CallbackName = nullptr;
	Context = nullptr;
}

void FWwiseGlobalCallbacks::OnTermCallback(AK::IAkGlobalPluginContext* in_pContext)
{
	if (UNLIKELY(Context))
	{
		const TCHAR* PrevName = CallbackName;
		if (!CallbackName)
		{
			PrevName = TEXT("[unknown]");
		}
		UE_LOG(LogWwiseConcurrency, Error, TEXT("FWwiseGlobalCallbacks::OnTermCallback: Doing two callbacks at once (previous: %s)"), PrevName);
	}
	CallbackName = TEXT("Term");
	Context = in_pContext;
	TermQueue.Run();
	CallbackName = nullptr;
	Context = nullptr;
}

void FWwiseGlobalCallbacks::OnMonitorCallback(AK::IAkGlobalPluginContext* in_pContext)
{
	if (UNLIKELY(MonitorContext))
	{
		const TCHAR* PrevName = MonitorCallbackName;
		if (!MonitorCallbackName)
		{
			PrevName = TEXT("[unknown]");
		}
		UE_LOG(LogWwiseConcurrency, Error, TEXT("FWwiseGlobalCallbacks::OnMonitorCallback: Doing two monitor callbacks at once (previous: %s)"), PrevName);
	}
	MonitorCallbackName = TEXT("Monitor");
	MonitorContext = in_pContext;
	MonitorQueue.Run();
	MonitorCallbackName = nullptr;
	MonitorContext = nullptr;
}

void FWwiseGlobalCallbacks::OnMonitorRecapCallback(AK::IAkGlobalPluginContext* in_pContext)
{
	if (UNLIKELY(MonitorContext))
	{
		const TCHAR* PrevName = MonitorCallbackName;
		if (!MonitorCallbackName)
		{
			PrevName = TEXT("[unknown]");
		}
		UE_LOG(LogWwiseConcurrency, Error, TEXT("FWwiseGlobalCallbacks::OnMonitorRecapCallback: Doing two monitor callbacks at once (previous: %s)"), PrevName);
	}
	MonitorCallbackName = TEXT("MonitorRecap");
	MonitorContext = in_pContext;
	MonitorRecapQueue.Run();
	MonitorCallbackName = nullptr;
	MonitorContext = nullptr;
}

void FWwiseGlobalCallbacks::OnInitCallback(AK::IAkGlobalPluginContext* in_pContext)
{
	if (UNLIKELY(Context))
	{
		const TCHAR* PrevName = CallbackName;
		if (!CallbackName)
		{
			PrevName = TEXT("[unknown]");
		}
		UE_LOG(LogWwiseConcurrency, Error, TEXT("FWwiseGlobalCallbacks::OnInitCallback: Doing two callbacks at once (previous: %s)"), PrevName);
	}
	CallbackName = TEXT("Init");
	Context = in_pContext;
	InitQueue.Run();
	CallbackName = nullptr;
	Context = nullptr;
}

void FWwiseGlobalCallbacks::OnSuspendCallback(AK::IAkGlobalPluginContext* in_pContext)
{
	if (UNLIKELY(Context))
	{
		const TCHAR* PrevName = CallbackName;
		if (!CallbackName)
		{
			PrevName = TEXT("[unknown]");
		}
		UE_LOG(LogWwiseConcurrency, Error, TEXT("FWwiseGlobalCallbacks::OnSuspendCallback: Doing two callbacks at once (previous: %s)"), PrevName);
	}
	CallbackName = TEXT("Suspend");
	Context = in_pContext;
	SuspendQueue.Run();
	CallbackName = nullptr;
	Context = nullptr;
}

void FWwiseGlobalCallbacks::OnWakeupFromSuspendCallback(AK::IAkGlobalPluginContext* in_pContext)
{
	if (UNLIKELY(Context))
	{
		const TCHAR* PrevName = CallbackName;
		if (!CallbackName)
		{
			PrevName = TEXT("[unknown]");
		}
		UE_LOG(LogWwiseConcurrency, Error, TEXT("FWwiseGlobalCallbacks::OnWakeupFromSuspendCallback: Doing two callbacks at once (previous: %s)"), PrevName);
	}
	CallbackName = TEXT("WakeupFromSuspend");
	Context = in_pContext;
	WakeupFromSuspendQueue.Run();
	CallbackName = nullptr;
	Context = nullptr;
}

void FWwiseGlobalCallbacks::OnRegisterCallbackStatic(AK::IAkGlobalPluginContext* in_pContext,
	AkGlobalCallbackLocation in_eLocation, void* in_pCookie)
{
	static_cast<FWwiseGlobalCallbacks*>(in_pCookie)->OnRegisterCallback(in_pContext);
}

void FWwiseGlobalCallbacks::OnBeginCallbackStatic(AK::IAkGlobalPluginContext* in_pContext,
	AkGlobalCallbackLocation in_eLocation, void* in_pCookie)
{
	static_cast<FWwiseGlobalCallbacks*>(in_pCookie)->OnBeginCallback(in_pContext);
}

void FWwiseGlobalCallbacks::OnPreProcessMessageQueueForRenderCallbackStatic(AK::IAkGlobalPluginContext* in_pContext,
	AkGlobalCallbackLocation in_eLocation, void* in_pCookie)
{
	static_cast<FWwiseGlobalCallbacks*>(in_pCookie)->OnPreProcessMessageQueueForRenderCallback(in_pContext);
}

void FWwiseGlobalCallbacks::OnPostMessagesProcessedCallbackStatic(AK::IAkGlobalPluginContext* in_pContext,
	AkGlobalCallbackLocation in_eLocation, void* in_pCookie)
{
	static_cast<FWwiseGlobalCallbacks*>(in_pCookie)->OnPostMessagesProcessedCallback(in_pContext);
}

void FWwiseGlobalCallbacks::OnBeginRenderCallbackStatic(AK::IAkGlobalPluginContext* in_pContext,
	AkGlobalCallbackLocation in_eLocation, void* in_pCookie)
{
	static_cast<FWwiseGlobalCallbacks*>(in_pCookie)->OnBeginRenderCallback(in_pContext);
}

void FWwiseGlobalCallbacks::OnEndRenderCallbackStatic(AK::IAkGlobalPluginContext* in_pContext,
	AkGlobalCallbackLocation in_eLocation, void* in_pCookie)
{
	static_cast<FWwiseGlobalCallbacks*>(in_pCookie)->OnEndRenderCallback(in_pContext);
}

void FWwiseGlobalCallbacks::OnEndCallbackStatic(AK::IAkGlobalPluginContext* in_pContext,
	AkGlobalCallbackLocation in_eLocation, void* in_pCookie)
{
	static_cast<FWwiseGlobalCallbacks*>(in_pCookie)->OnEndCallback(in_pContext);
}

void FWwiseGlobalCallbacks::OnTermCallbackStatic(AK::IAkGlobalPluginContext* in_pContext,
	AkGlobalCallbackLocation in_eLocation, void* in_pCookie)
{
	static_cast<FWwiseGlobalCallbacks*>(in_pCookie)->OnTermCallback(in_pContext);
}

void FWwiseGlobalCallbacks::OnMonitorCallbackStatic(AK::IAkGlobalPluginContext* in_pContext,
	AkGlobalCallbackLocation in_eLocation, void* in_pCookie)
{
	static_cast<FWwiseGlobalCallbacks*>(in_pCookie)->OnMonitorCallback(in_pContext);
}

void FWwiseGlobalCallbacks::OnMonitorRecapCallbackStatic(AK::IAkGlobalPluginContext* in_pContext,
	AkGlobalCallbackLocation in_eLocation, void* in_pCookie)
{
	static_cast<FWwiseGlobalCallbacks*>(in_pCookie)->OnMonitorRecapCallback(in_pContext);
}

void FWwiseGlobalCallbacks::OnInitCallbackStatic(AK::IAkGlobalPluginContext* in_pContext,
	AkGlobalCallbackLocation in_eLocation, void* in_pCookie)
{
	static_cast<FWwiseGlobalCallbacks*>(in_pCookie)->OnInitCallback(in_pContext);
}

void FWwiseGlobalCallbacks::OnSuspendCallbackStatic(AK::IAkGlobalPluginContext* in_pContext,
	AkGlobalCallbackLocation in_eLocation, void* in_pCookie)
{
	static_cast<FWwiseGlobalCallbacks*>(in_pCookie)->OnSuspendCallback(in_pContext);
}

void FWwiseGlobalCallbacks::OnWakeupFromSuspendCallbackStatic(AK::IAkGlobalPluginContext* in_pContext,
	AkGlobalCallbackLocation in_eLocation, void* in_pCookie)
{
	static_cast<FWwiseGlobalCallbacks*>(in_pCookie)->OnWakeupFromSuspendCallback(in_pContext);
}
