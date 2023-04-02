// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeOfRun.h"

int UTimeOfRun::AddTime(int time)
{
	for (int i = 0; i < Time.Num(); ++i)
	{
		if (time < Time[i])
		{
			Time.Insert(time, i);
			if (Time.Num() > Length)
			{
				Time.RemoveAt(Length);
				return i;
			}
			return i;
		}
	}
	Time.Push(time);
	return Time.Num() - 1;
}

FString UTimeOfRun::GetStart() const
{
	return Start;
}

FString UTimeOfRun::GetTarget() const
{
	return Target;
}

void UTimeOfRun::SetStart(FString start)
{
	Start = start;
}

void UTimeOfRun::SetTarget(FString target)
{
	Target = target;
}

TArray<int> UTimeOfRun::GetTimes() const
{
	return Time;
}

