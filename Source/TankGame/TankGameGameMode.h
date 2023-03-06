// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TankGameGameMode.generated.h"

class AEnemy;
class ATankGameCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreUpdated, int32, NewScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameOver, int32, Score);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerUpdated, int32, TimeRemaining);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimerEnded);

UCLASS(MinimalApi)
class ATankGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	void InitGameMode();

protected:
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Time")
	int32 TimeLimit;
	
	UPROPERTY(BlueprintAssignable, Category="Time")
	FOnTimerUpdated OnTimerUpdated;
	
	UPROPERTY(BlueprintAssignable, Category="Time")
	FOnTimerEnded OnTimerEnded;
	
	UPROPERTY(BlueprintAssignable, Category="Score")
	FOnScoreUpdated OnScoreUpdated;
	
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category="GameOver")
	FOnGameOver OnGameOver;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> SpawnLocationClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATankGameCharacter> TankClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEnemy> EnemyClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> EndGameWidgetClass;

	TObjectPtr<ATankGameCharacter> TankPtr;
	
protected:
	void UpdateTimer();
	void HandleOnEnemyDestroyed();
	void SetupEnemies();
	void InitTimers();
	void CheckEndGame(int32 TimeRemaining);
	
	void UpdateEnemyCount();
	

private:
	int32 EnemiesDestroyed;
	int32 CurrentTime;
	int32 TimeLeft;
	TArray<ACharacter*> Enemies;

	FTimerHandle TimerHandle;
	
public:
	

	
};



