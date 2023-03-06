// Fill out your copyright notice in the Description page of Project Settings.


#include "TankProjectile.h"

#include "Enemy.h"

// Sets default values
ATankProjectile::ATankProjectile()
{
	RootMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootMesh"));
	RootMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATankProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (RootMesh)
	{
		RootMesh->OnComponentHit.AddDynamic(this, &ATankProjectile::OnHitCallback);
	}
}

void ATankProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (RootMesh)
	{
		RootMesh->OnComponentHit.RemoveAll(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

void ATankProjectile::OnHitCallback(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                    FVector NormalImpulse, const FHitResult& Hit)
{
	if(AEnemy* Enemy = Cast<AEnemy>(OtherActor))
	{
		Enemy->DestroyEnemy();
	}

	Destroy();
}

