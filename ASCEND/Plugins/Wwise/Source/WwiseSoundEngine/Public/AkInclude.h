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
#include "HAL/PreprocessorHelpers.h"

#ifdef AK_WWISE_SOUNDENGINE_VERSION_PATH
#define WWISE_SOUNDENGINE_AKINCLUDE_VERSION_FILENAME PREPROCESSOR_TO_STRING(PREPROCESSOR_JOIN(Wwise/AkInclude_,AK_WWISE_SOUNDENGINE_VERSION_PATH).h)
#include WWISE_SOUNDENGINE_AKINCLUDE_VERSION_FILENAME
// The following lines are for hinting Distributed Build Systems, as AK_WWISE_SOUNDENGINE_VERSION_PATH is always set.
#else
#include "Wwise/AkInclude_2022_1.h"
#include "Wwise/AkInclude_2023_1.h"
#include "Wwise/AkInclude_2024_1.h"
#include "Wwise/AkInclude_2025_1.h"
#endif
