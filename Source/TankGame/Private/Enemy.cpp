// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

void AEnemy::StartJumping(bool bShouldJump)
{
	if (bShouldJump)
	{
		if (const UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(Timer, this, &AEnemy::Jump, 1.0f, true, 1.0f);
		}
	}
}

void AEnemy::DestroyEnemy()
{
	if (OnEnemyDestroyed.IsBound())
	{
		OnEnemyDestroyed.Broadcast();
	}

	Destroy();
}


void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	Timer.Invalidate();
}


