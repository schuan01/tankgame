// Copyright Epic Games, Inc. All Rights Reserved.

#include "TankGameGameMode.h"

#include "Enemy.h"
#include "TankGameCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

void ATankGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Just delay a bit the execution for next tick so we can get all the Spawn Locs
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ATankGameGameMode::InitGameMode));
	}
}

void ATankGameGameMode::InitGameMode()
{
	UWorld* World = GetWorld();
	if (World && TankClass)
	{
		// Get the Tank from the World
		// Ideally the Game Mode will spawn the Tank and set the variable but this is a temporal thing for now
		TankPtr = Cast<ATankGameCharacter>(UGameplayStatics::GetActorOfClass(World, TankClass));

		if (TankPtr)
		{
			// Get all the Spawn Points
			if (SpawnLocationClass)
			{
				TArray<AActor*> FoundLocations;
				UGameplayStatics::GetAllActorsOfClass(World, SpawnLocationClass, FoundLocations);

				if (FoundLocations.Num() > 0)
				{
					const int32 RandomInit = FMath::RandRange(0, FoundLocations.Num() - 1);
					const int32 RandomFinish = FMath::RandRange(RandomInit, FoundLocations.Num() - 1);

					for (int i = RandomInit; i < RandomFinish; i++)
					{
						if (const AActor* Actor = FoundLocations[i])
						{
							const FVector Loc = Actor->GetActorLocation();
							const FRotator Rot = Actor->GetActorRotation();
			
							//Set Spawn Collision Handling Override
							FActorSpawnParameters ActorSpawnParams;
							ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

							// Spawn the Enemy
							AEnemy* EnemySpawned = World->SpawnActor<AEnemy>(EnemyClass, Loc, Rot, ActorSpawnParams);
							if (EnemySpawned)
							{
								EnemySpawned->StartJumping(FMath::RandBool());
								Enemies.AddUnique(EnemySpawned);
							}

							
						}
					}

					SetupEnemies();
					InitTimers();
				}
			}
		}
		
	}
}

void ATankGameGameMode::UpdateTimer()
{
	CurrentTime++;

	TimeLeft = TimeLimit - CurrentTime;

	if (OnTimerUpdated.IsBound())
	{
		OnTimerUpdated.Broadcast(TimeLeft);
	}

	CheckEndGame(TimeLeft);
}

void ATankGameGameMode::HandleOnEnemyDestroyed()
{
	
}

void ATankGameGameMode::SetupEnemies()
{
	for (const ACharacter* Element : Enemies)
	{
		//E
	}
}

void ATankGameGameMode::InitTimers()
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(TimerHandle, this, &ATankGameGameMode::UpdateTimer, 1.0f, true, 0.0f);
	}
}

void ATankGameGameMode::CheckEndGame(int32 TimeRemaining)
{
	if (TimeRemaining <= 0) 
	{
		// Stop the timer
		TimerHandle.Invalidate();

		TankPtr->SetIsCurrentlyPlaying(false);

		if (APlayerController* PlayerController = Cast<APlayerController>(TankPtr->Controller))
		{
			DisableInput(PlayerController);
			
			if (UUserWidget* EndGameScreen = CreateWidget<UUserWidget>(PlayerController, EndGameWidgetClass))
			{
				EndGameScreen->AddToViewport();
			}
		}

		if (OnGameOver.IsBound())
		{
			OnGameOver.Broadcast(EnemiesDestroyed);
		}
	}
}

void ATankGameGameMode::UpdateEnemyCount()
{
	EnemiesDestroyed++;

	if (OnScoreUpdated.IsBound())
	{
		OnScoreUpdated.Broadcast(EnemiesDestroyed);
	}
}
