#include "AbvaPlayer.h"

#include "Components/AudioComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"

#include "Abva/Player/AbvaMovement.h"
#include "Abva/Interaction/IInteractableItem.h"
#include "Abva/Item/ItemManager.h"
#include "Abva/Item/Item.h"

#include "ABVA/Dev/Dev.h"

AAbvaPlayer::AAbvaPlayer(const FObjectInitializer& initializer)
	:Super(initializer.SetDefaultSubobjectClass<UAbvaMovement>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	Cam = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	Cam->SetupAttachment(RootComponent);

	ItemManager = CreateDefaultSubobject<UItemManager>(FName("ItemManager"));

	WeaponLock = CreateDefaultSubobject<USceneComponent>(FName("WeaponLock"));
	WeaponLock->SetupAttachment(Cam);

	JumpMaxHoldTime = 0.1f;
	bIsCrouched = false;
	bUseControllerRotationRoll = false;
}

auto AAbvaPlayer::GetMovement() const
{
	return CastChecked<UAbvaMovement>(GetCharacterMovement());
}

auto AAbvaPlayer::GetPlayerController() const
{
	return CastChecked<APlayerController>(GetController());
}

double AAbvaPlayer::GetJumpHoldTime() const
{
	//if JumpTimerStamp is less than -1 that means the player had let go of jump the last tick
	if (JumpTimeStamp < 0)
	{
		return -1;
	}
	return GetWorld()->TimeSince(JumpTimeStamp);
}

void AAbvaPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Get the player controller
	APlayerController* PC = CastChecked<APlayerController>(GetController());


	UEnhancedInputComponent* EI = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	//Movement
	EI->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AAbvaPlayer::MoveDir);
	EI->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AAbvaPlayer::Look);

	EI->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &AAbvaPlayer::PJump);

	EI->BindAction(IA_Sprint, ETriggerEvent::Triggered, this, &AAbvaPlayer::Sprint);

	EI->BindAction(IA_Crouch, ETriggerEvent::Triggered, this, &AAbvaPlayer::PCrouch);

	//Combat
	EI->BindAction(IA_Reload, ETriggerEvent::Triggered, this, &AAbvaPlayer::Reload);

	EI->BindAction(IA_ThrowItem, ETriggerEvent::Triggered, this, &AAbvaPlayer::ThrowItem);

	EI->BindAction(IA_ChangeWeapon, ETriggerEvent::Triggered, this, &AAbvaPlayer::ChangeItem);

	EI->BindAction(IA_ADS, ETriggerEvent::Triggered, this, &AAbvaPlayer::ADS);

	EI->BindAction(IA_Attack, ETriggerEvent::Triggered, this, &AAbvaPlayer::Attack);

	//Misc
	EI->BindAction(IA_Interact, ETriggerEvent::Triggered, this, &AAbvaPlayer::Interact);
	
	// Get the local player subsystem	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	// Clear out existing mapping, and add our mapping

	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(IMC_FpsPlayerMapping, 0);

}

void AAbvaPlayer::BeginPlay()
{
	Super::BeginPlay();
	if (!ensureMsgf(SpeedLineMaterial != nullptr, TEXT("player's speed line arent set!")))
	{
		return;
	}
	SpeedLineMat = UMaterialInstanceDynamic::Create(SpeedLineMaterial, this);
	Cam->PostProcessSettings.AddBlendable(SpeedLineMat, 1);
}

void AAbvaPlayer::Tick(float deltaTick)
{
	HandleWallrun();
	ChangeFOVWithSpeed(deltaTick);
	HandleSpeedLines(deltaTick);
	Super::Tick(deltaTick);
}

void AAbvaPlayer::Jump()
{
	Super::Jump();
}

void AAbvaPlayer::StopJumping()
{
	Super::StopJumping();
}

void AAbvaPlayer::Landed(const FHitResult& floor)
{
	Super::Landed(floor);
	if (bPressedJump)
	{
		Jump();
	}
}

void AAbvaPlayer::GetActorEyesViewPoint(FVector& outLocation, FRotator& outRotation) const
{
	outLocation = Cam->GetComponentLocation();
	outRotation = Cam->GetComponentRotation();
}

bool AAbvaPlayer::CanWallrun() const
{
	const auto movement = GetMovement();
	return !movement->IsSliding && !movement->IsMovingOnGround() 
			&& FVector(movement->Velocity.X, movement->Velocity.Y, 0).Length() > 300.0;
}

void AAbvaPlayer::HandleWallrun()
{
	auto Capsule = GetCapsuleComponent();
	auto movement = GetMovement();

	GEngine->AddOnScreenDebugMessage((uint64)3243478234u, 3, FColor::Cyan, "Jump hold time = " + FString::SanitizeFloat(GetJumpHoldTime()));
	GEngine->AddOnScreenDebugMessage((uint64)453487u, 3, FColor::Cyan, "Is wall running: " + FString(movement->IsWallrunning ? "Yes" : "No"));
	GEngine->AddOnScreenDebugMessage((uint64)803457349u, 3, FColor::Cyan, "Can wall run: " + FString(CanWallrun() ? "Yes" : "No"));
	GEngine->AddOnScreenDebugMessage((uint64)2342384723u, 3, FColor::Cyan, "Movement input dir is: " + PlayerMoveInput.ToString());
	if (!movement->IsWallrunning)
	{
		if (CanWallrun() && GetJumpHoldTime() > 0.2f)
		{
			FVector LocalVel = Capsule->GetComponentQuat().UnrotateVector(movement->Velocity);
			bool IsRight;

			if (PlayerMoveInput.Y < -0.5f)
			{
				IsRight = false;
			}
			else if (PlayerMoveInput.Y > 0.5f)
			{
				IsRight = true;
			}
			else
			{
				return;
			}

			FHitResult WallHit;
			int8 TraceDir = IsRight ? 1 : -1;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this);

    		auto debug = Dev::TraceDev::DevSweepSingleByChannel(GetWorld(), WallHit, Capsule->GetComponentLocation(),
			Capsule->GetComponentLocation() + Capsule->GetRightVector() * TraceDir * (Capsule->GetScaledCapsuleRadius() + 40.f),
			FQuat::Identity, ECC_WorldStatic, FCollisionShape::MakeCapsule(FVector(20.f, 20.f, 40.f)), Params);
			if (debug)
			{
				if (FMath::IsWithinInclusive(abs(WallHit.ImpactNormal.Z), 0.f, 0.2f))
				{
					movement->WallrunInit(WallHit.ImpactNormal, WallHit.Distance, IsRight);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("the wall was too tilted for the player to wall run on."));
					movement->WallrunAbort(false);
				}
			}
			else
			{
				movement->WallrunAbort(false);
			}
			
			return;
		}
	}
	else if (FVector(movement->Velocity.X, movement->Velocity.Y, 0).Length() < 300.0)
	{
		movement->WallrunAbort(false);
	}
}
//TODO: make sure to make the "get hold jump time" func thing. ok thank you, thank you i love :) xoxoxo
void AAbvaPlayer::HandleSpeedLines(float deltaTime) const
{
	float speedLineOpacity;
	float speedLineDensity;
	SpeedLineMat->GetScalarParameterValue(FHashedMaterialParameterInfo(FName("Opacity")), speedLineOpacity);
	SpeedLineMat->GetScalarParameterValue(FHashedMaterialParameterInfo(FName("LineDensity")), speedLineDensity);
	
	const auto* const movement = GetMovement();
	constexpr float maxSpeedCap = 2000.f;
	
	float targetSpeedLineOpacity = FMath::GetMappedRangeValueClamped(FVector2D(600, maxSpeedCap), FVector2D(0, 1), movement->Velocity.Length());
	float targetSpeedLineDensity = FMath::GetMappedRangeValueClamped(FVector2D(600, maxSpeedCap), FVector2D(0, 0.45f), movement->Velocity.Length());

	float newSpeedLineOpacity = FMath::FInterpTo(speedLineOpacity, targetSpeedLineOpacity, deltaTime, 3.f);
	float newSpeedLineDensity = FMath::FInterpTo(speedLineDensity, targetSpeedLineDensity, deltaTime, 3.f);

	SpeedLineMat->SetScalarParameterValue(FName("Opacity"), newSpeedLineOpacity);
	SpeedLineMat->SetScalarParameterValue(FName("LineDensity"), newSpeedLineDensity);
	GEngine->AddOnScreenDebugMessage(1, 2, FColor::Cyan, "Speed line opacity: " + FString::SanitizeFloat(speedLineOpacity));
	GEngine->AddOnScreenDebugMessage(1, 2, FColor::Cyan, "speed line density: " + FString::SanitizeFloat(speedLineDensity));
}

void AAbvaPlayer::ChangeFOVWithSpeed(float deltaTick) const
{
	const auto  movement = GetMovement();
	constexpr float baseFOV = 90.f; //TODO: make it so this value changes depending on the player's prefrences
	constexpr float maxSpeedCap = 2000.f;
	
	float targetFOV = baseFOV + FMath::GetMappedRangeValueClamped(FVector2D(600, maxSpeedCap), FVector2D(0, 10), movement->Velocity.Length());
	
	auto pc = GetPlayerController();
	float currentFOV = pc->PlayerCameraManager->GetFOVAngle();
	float newFOV = FMath::FInterpTo(currentFOV, targetFOV, deltaTick, 5.f);
	
	pc->PlayerCameraManager->SetFOV(newFOV);
}

void AAbvaPlayer::MoveDir(const FInputActionValue& Value)//FVector2D
{
	const auto AttemptedMove = Value.Get<FVector2D>();

	if (AttemptedMove.X)
	{
		AddMovementInput(GetCapsuleComponent()->GetForwardVector(), AttemptedMove.X, true);
	}

	if (AttemptedMove.Y)
	{
		AddMovementInput(GetCapsuleComponent()->GetRightVector(), AttemptedMove.Y, true);
	}

	PlayerMoveInput = AttemptedMove;
}

void AAbvaPlayer::Look(const FInputActionValue& Value)
{
	const auto AttemptedLook = Value.Get<FVector2D>();

	if (AttemptedLook.X)
	{
		AddControllerYawInput(AttemptedLook.X);
	}

	if (AttemptedLook.Y)
	{
		AddControllerPitchInput(AttemptedLook.Y);
	}
}

void AAbvaPlayer::PJump(const FInputActionValue& Value)
{
	const auto bJump = Value.Get<bool>();
	const auto movement = GetMovement();
	if (bJump)
	{
		movement->Jump();
		JumpTimeStamp = GetWorld()->GetTimeSeconds();
	}
	else
	{
		GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]() {JumpTimeStamp = -1; }));
		StopJumping();
	}
}

void AAbvaPlayer::Reload(const FInputActionValue& Value)
{
	const auto bReload = Value.Get<bool>();

	if (ItemManager->CurrentItem == nullptr)
	{
		return;
	}

	if (bReload)
	{
		ItemManager->CurrentItem->TertiaryInteract();
	}
	else
	{
		ItemManager->CurrentItem->StopTertiaryInteract();
	}
}

void AAbvaPlayer::ADS(const FInputActionValue& Value)
{
	const auto bAds = Value.Get<bool>();

	if (ItemManager->CurrentItem == nullptr)
	{
		return;
	}

	if (bAds)
	{
		ItemManager->CurrentItem->SecondaryInteract();
	}
	else
	{
		ItemManager->CurrentItem->StopSecondaryInteract();
	}
}

void AAbvaPlayer::ChangeItem(const FInputActionValue& Value)
{
	int8 InputChange = FMath::RoundToInt(Value.Get<float>());
	ItemManager->ChangeItem(InputChange);
}

void AAbvaPlayer::Attack(const FInputActionValue& Value)
{
	const auto bInteract = Value.Get<bool>();
	
	if (ItemManager->CurrentItem == nullptr)
	{
		return;
	}

	if (bInteract)
	{
		ItemManager->CurrentItem->MainInteract();
	}
	else
	{
		ItemManager->CurrentItem->StopMainInteract();
	}
}

void AAbvaPlayer::Interact(const FInputActionValue& Value)
{
	const auto bInteract = Value.Get<bool>();
	if (bInteract)
	{
		FHitResult result;
		FVector start;
		FRotator quat;
		GetActorEyesViewPoint(start, quat);
		FVector end = start + quat.Vector() * 600.f;
		FCollisionQueryParams params(FName("Debug trace"), false, this);
		if (GetWorld()->LineTraceSingleByChannel(result, start, end, ECollisionChannel::ECC_Camera, params))
		{
			AActor* HitActor = result.GetActor();
			if (IsValid(HitActor) && HitActor->Implements<UInteractableItem>())
			{
				IInteractableItem::Execute_Interact(HitActor);
			}
		}
	}
}

void AAbvaPlayer::ThrowItem(const FInputActionValue& Value)
{
}

//Sprint
void AAbvaPlayer::Sprint(const FInputActionValue& Value)
{
	const auto bSprint = Value.Get<bool>();
	if (bSprint)
	{
	}
	else
	{
	}
}

void AAbvaPlayer::PCrouch(const FInputActionValue& Value)
{
	const auto bCrouch = Value.Get<bool>();
	if (bCrouch)
	{
		Crouch();
		
	}
	else
	{
		UnCrouch();
	}
}
