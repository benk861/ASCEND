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

#include "Wwise/WwiseConcurrencyModuleImpl.h"

#include "Wwise/WwiseGlobalCallbacks.h"

IMPLEMENT_MODULE(FWwiseConcurrencyModule, WwiseConcurrency)

FWwiseConcurrencyModule::FWwiseConcurrencyModule()
{
}

void FWwiseConcurrencyModule::StartupModule()
{
	ExecutionQueueLock.WriteLock();
	if (!ExecutionQueueThreadPool)
	{
		InitializeExecutionQueueThreadPool();
	}
	ExecutionQueueLock.WriteUnlock();

	GlobalCallbacksLock.WriteLock();
	if (!GlobalCallbacks)
	{
		InitializeGlobalCallbacks();
	}
	GlobalCallbacksLock.WriteUnlock();

	IWwiseConcurrencyModule::StartupModule();
}

void FWwiseConcurrencyModule::ShutdownModule()
{
	GlobalCallbacksLock.WriteLock();
	TerminateGlobalCallbacks();
	GlobalCallbacksLock.WriteUnlock();

	ExecutionQueueLock.WriteLock();
	TerminateExecutionQueueThreadPool();
	ExecutionQueueLock.WriteUnlock();

	IWwiseConcurrencyModule::ShutdownModule();
}

FQueuedThreadPool* FWwiseConcurrencyModule::GetExecutionQueueThreadPool()
{
	ExecutionQueueLock.ReadLock();
	if (LIKELY(ExecutionQueueThreadPool))
	{
		ExecutionQueueLock.ReadUnlock();
		return ExecutionQueueThreadPool;
	}

	ExecutionQueueLock.ReadUnlock();
	ExecutionQueueLock.WriteLock();
	if (UNLIKELY(ExecutionQueueThreadPool))
	{
		ExecutionQueueLock.WriteUnlock();
		return ExecutionQueueThreadPool;
	}

	InitializeExecutionQueueThreadPool();
	ExecutionQueueLock.WriteUnlock();
	return ExecutionQueueThreadPool;
}

FWwiseGlobalCallbacks* FWwiseConcurrencyModule::GetGlobalCallbacks()
{
	GlobalCallbacksLock.ReadLock();
	if (LIKELY(GlobalCallbacks))
	{
		GlobalCallbacksLock.ReadUnlock();
		return GlobalCallbacks;
	}

	GlobalCallbacksLock.ReadUnlock();
	GlobalCallbacksLock.WriteLock();
	if (UNLIKELY(GlobalCallbacks))
	{
		GlobalCallbacksLock.WriteUnlock();
		return GlobalCallbacks;
	}

	InitializeGlobalCallbacks();
	GlobalCallbacksLock.WriteUnlock();
	return GlobalCallbacks;
}

void FWwiseConcurrencyModule::InitializeExecutionQueueThreadPool()
{
	static constexpr int32 StackSize = 128 * 1024;
	static constexpr int32 MaxThreads = 3;

	ExecutionQueueThreadPool = FQueuedThreadPool::Allocate();
	const int32 NumThreadsInThreadPool = FPlatformProcess::SupportsMultithreading() ? FMath::Min(FPlatformMisc::NumberOfWorkerThreadsToSpawn(), MaxThreads) : 1;
	verify(ExecutionQueueThreadPool->Create(NumThreadsInThreadPool, StackSize, TPri_Normal, TEXT("Wwise ExecutionQueue Pool")));
}

void FWwiseConcurrencyModule::InitializeGlobalCallbacks()
{
	if (!ExecutionQueueThreadPool)
	{
		InitializeExecutionQueueThreadPool();
	}
	GlobalCallbacks = new FWwiseGlobalCallbacks;
	// GlobalCallbacks->Initialize(); Initialization requires memory allocation that is only available in AkInitializationSettings.
}

void FWwiseConcurrencyModule::TerminateExecutionQueueThreadPool()
{
	if (ExecutionQueueThreadPool)
	{
		ExecutionQueueThreadPool->Destroy();
		ExecutionQueueThreadPool = nullptr;
	}
}

void FWwiseConcurrencyModule::TerminateGlobalCallbacks()
{
	if (GlobalCallbacks)
	{
		GlobalCallbacks->Terminate();
		delete GlobalCallbacks;
		GlobalCallbacks = nullptr;
	}
}

