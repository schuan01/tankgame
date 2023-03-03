// Copyright Epic Games, Inc. All Rights Reserved.

#include "TankGameCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"
#include "Public/TankProjectile.h"


//////////////////////////////////////////////////////////////////////////
// ATankGameCharacter

ATankGameCharacter::ATankGameCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; 
	CameraBoom->bUsePawnControlRotation = true; 

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false; 

	TankBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TankBody"));
	TankBody->SetupAttachment(RootComponent);

	GunCenter = CreateDefaultSubobject<USceneComponent>(TEXT("GunCenter"));
	GunCenter->SetupAttachment(TankBody);

	TankGun = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TankGun"));
	TankGun->SetupAttachment(GunCenter);
	
	SpawnLoc = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnLoc"));
	SpawnLoc->SetupAttachment(TankGun);
}

void ATankGameCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		WeakPlayerController = PlayerController;
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		PlayerController->SetShowMouseCursor(true);
	}

	// For future checking if we can play or not
	bIsCurrentlyPlaying = true;
}

void ATankGameCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsCurrentlyPlaying && WeakPlayerController)
	{
		const FVector InitLoc = GunCenter->GetComponentLocation();
		const FRotator InitRot = GunCenter->GetComponentRotation();

		FHitResult OutResult;
		WeakPlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, true, OutResult);

		const FRotator NewRot = UKismetMathLibrary::FindLookAtRotation(InitLoc, OutResult.Location);
		const FRotator TargetRot = FRotator(InitRot.Pitch, NewRot.Yaw, InitRot.Roll);
		GunCenter->SetWorldRotation(TargetRot);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATankGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATankGameCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATankGameCharacter::Look);

		//Shooting
		EnhancedInputComponent->BindAction(ShootProjectileAction, ETriggerEvent::Started, this, &ATankGameCharacter::ShootProjectile);
	}
}

void ATankGameCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ATankGameCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ATankGameCharacter::ShootProjectile()
{
	if (bIsCurrentlyPlaying)
	{
		if (UWorld* World = GetWorld())
		{
			const FVector Loc = SpawnLoc->GetComponentLocation();
			const FRotator Rot = SpawnLoc->GetComponentRotation();
			
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			
			World->SpawnActor<ATankProjectile>(TankProjectileClass, Loc, Rot, ActorSpawnParams);

			// Call the BP for sounds FX
			BP_ShootProjectile();
		}
	}
}




