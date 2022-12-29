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

#include "Wwise/WwiseDeferredQueue.h"

#include "Wwise/Stats/AsyncStats.h"
#include "Wwise/Stats/Concurrency.h"

FWwiseDeferredQueue::FWwiseDeferredQueue()
{
}

FWwiseDeferredQueue::~FWwiseDeferredQueue()
{
	if (!IsEmpty())
	{
		Wait();
		UE_CLOG(UNLIKELY(!IsEmpty()), LogWwiseConcurrency, Error, TEXT("Still operations in queue while deleting Deferred Queue"));
	}
}

void FWwiseDeferredQueue::AsyncDefer(FFunction&& InFunction)
{
	AsyncOpQueue.Enqueue(MoveTemp(InFunction));
}

void FWwiseDeferredQueue::SyncDefer(FFunction&& InFunction)
{
	SyncOpQueue.Enqueue(MoveTemp(InFunction));
}

void FWwiseDeferredQueue::GameDefer(FFunction&& InFunction)
{
	GameOpQueue.Enqueue(MoveTemp(InFunction));
}

void FWwiseDeferredQueue::Run()
{
	SCOPED_NAMED_EVENT_TCHAR(TEXT("WwiseDeferredQueue_SyncExec"), FColor::Red);
	FWwiseAsyncCycleCounter OpCycleCounter(GET_STATID(STAT_WwiseConcurrencySync));

	if (!AsyncOpQueue.IsEmpty())
	{
		AsyncExecutionQueue.Async([this]() mutable
		{
			AsyncExec();
		});
	}
	if (!GameOpQueue.IsEmpty())
	{
		GameThreadExec();
	}
	if (!SyncOpQueue.IsEmpty())
	{
		SyncExec();
	}
}

void FWwiseDeferredQueue::Wait()
{
	const bool bIsInGameThread = IsInGameThread();
	SCOPED_NAMED_EVENT_TCHAR(
		bIsInGameThread ? TEXT("WwiseDeferredQueue_Wait_GameThread") : TEXT("WwiseDeferredQueue_Wait"),
		bIsInGameThread ? FColor::Red : FColor::Orange);
	CONDITIONAL_SCOPE_CYCLE_COUNTER(STAT_WwiseConcurrencyGameThreadWait, bIsInGameThread);
	CONDITIONAL_SCOPE_CYCLE_COUNTER(STAT_WwiseConcurrencyWait, !bIsInGameThread);

	if (!AsyncOpQueue.IsEmpty())
	{
		AsyncExecutionQueue.AsyncWait([this]() mutable
		{
			AsyncExec();
		});
	}
	if (!GameOpQueue.IsEmpty())
	{
		FEventRef Done;
		if (bIsInGameThread)
		{
			const bool bNeedToStartLoop = GameThreadExecuting.IncrementExchange() == 0;

			GameDefer([this, &Done]() mutable
			{
				Done->Trigger();
				GameThreadExecuting.DecrementExchange();
				return EWwiseDeferredAsyncResult::Done;
			});

			if (bNeedToStartLoop)
			{
				FFunction Func;
				while (GameThreadExecuting.Load() > 0 && SyncOpQueue.Dequeue(Func))
				{
					if (Func() == EWwiseDeferredAsyncResult::KeepRunning)
					{
						GameDefer(MoveTemp(Func));
					}
				}
			}
		}
		else
		{
			GameDefer([&Done]() mutable
			{
				Done->Trigger();
				return EWwiseDeferredAsyncResult::Done;
			});
			GameThreadExec();
		}
		Done->Wait();
	}
	if (!SyncOpQueue.IsEmpty())
	{
		SCOPED_NAMED_EVENT_TCHAR(TEXT("WwiseDeferredQueue_SyncExec"), FColor::Red);
		FWwiseAsyncCycleCounter OpCycleCounter(GET_STATID(STAT_WwiseConcurrencySync));

		SyncExec();
	}
}

void FWwiseDeferredQueue::AsyncExec()
{
	SCOPED_NAMED_EVENT_TCHAR(TEXT("WwiseDeferredQueue_AsyncExec"), FColor::Blue);
	SCOPE_CYCLE_COUNTER(STAT_WwiseConcurrencyAsync);

	bool bDone = false;
	AsyncDefer([&bDone]() mutable
	{
		bDone = true;
		return EWwiseDeferredAsyncResult::Done;
	});

	while (!bDone)
	{
		FFunction Func;
		const bool bResult = AsyncOpQueue.Dequeue(Func);
		if (UNLIKELY(!bResult))
		{
			UE_LOG(LogWwiseConcurrency, Error, TEXT("FWwiseDeferredQueue: No Result dequeuing Async Deferred Queue"));
			break;
		}
		if (Func() == EWwiseDeferredAsyncResult::KeepRunning)
		{
			AsyncDefer(MoveTemp(Func));
		}
	}
}

void FWwiseDeferredQueue::SyncExec()
{
	SyncDefer([this]() mutable
	{
		bSyncThreadDone = true;
		return EWwiseDeferredAsyncResult::Done;
	});

	SyncExecLoop();
}

void FWwiseDeferredQueue::SyncExecLoop()
{
	FFunction Func;
	while (!bSyncThreadDone && SyncOpQueue.Dequeue(Func))
	{
		if (Func() == EWwiseDeferredAsyncResult::KeepRunning)
		{
			SyncDefer(MoveTemp(Func));
		}
	}
	bSyncThreadDone = false;
}

void FWwiseDeferredQueue::GameThreadExec()
{
	const bool bNeedToStartLoop = GameThreadExecuting.IncrementExchange() == 0;

	GameDefer([this]() mutable
	{
		GameThreadExecuting.DecrementExchange();
		return EWwiseDeferredAsyncResult::Done;
	});

	if (bNeedToStartLoop)
	{
		GameThreadExecLoop();
	}
}

void FWwiseDeferredQueue::GameThreadExecLoop()
{
	AsyncTask(ENamedThreads::GameThread, [this]() mutable
	{
		SCOPED_NAMED_EVENT_TCHAR(TEXT("WwiseDeferredQueue_GameThreadExecLoop"), FColor::Red);
		SCOPE_CYCLE_COUNTER(STAT_WwiseConcurrencyGameThread);

		FFunction Func;
		const bool bResult = GameOpQueue.Dequeue(Func);
		if (UNLIKELY(!bResult))
		{
			UE_LOG(LogWwiseConcurrency, Error, TEXT("FWwiseDeferredQueue: No Result dequeuing Game Deferred Queue"));
			return;
		}
		if (Func() == EWwiseDeferredAsyncResult::KeepRunning)
		{
			GameDefer(MoveTemp(Func));
		}

		if (GameThreadExecuting.Load() > 0)
		{
			GameThreadExecLoop();
		}
	});
}
