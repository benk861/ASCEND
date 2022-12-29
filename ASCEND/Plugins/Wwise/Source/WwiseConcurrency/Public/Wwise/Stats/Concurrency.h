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

#pragma once

#include "Stats/Stats.h"
#include "Logging/LogMacros.h"

DECLARE_STATS_GROUP(TEXT("Concurrency"), STATGROUP_WwiseConcurrency, STATCAT_Wwise);

DECLARE_CYCLE_STAT_EXTERN(TEXT("Waiting for completion in Game Thread"), STAT_WwiseConcurrencyGameThreadWait, STATGROUP_WwiseConcurrency, WWISECONCURRENCY_API);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Waiting for completion"), STAT_WwiseConcurrencyWait, STATGROUP_WwiseConcurrency, WWISECONCURRENCY_API);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Executing asynchronous op"), STAT_WwiseConcurrencyAsync, STATGROUP_WwiseConcurrency, WWISECONCURRENCY_API);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Executing blocking synchronous op"), STAT_WwiseConcurrencySync, STATGROUP_WwiseConcurrency, WWISECONCURRENCY_API);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Executing op in Game Thread"), STAT_WwiseConcurrencyGameThread, STATGROUP_WwiseConcurrency, WWISECONCURRENCY_API);

WWISECONCURRENCY_API DECLARE_LOG_CATEGORY_EXTERN(LogWwiseConcurrency, Log, All);
