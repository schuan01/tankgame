// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Enemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDestroyed);

UCLASS()
class TANKGAME_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	void StartJumping(bool bShouldJump);
	void DestroyEnemy();

protected:
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(BlueprintAssignable, Category="Default")
	FOnEnemyDestroyed OnEnemyDestroyed;


private:
	FTimerHandle Timer;
	
};
