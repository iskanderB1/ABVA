#include "AbvaMovement.h"
#include "AbvaPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Abva/Dev/Dev.h"

UAbvaMovement::UAbvaMovement()
{
}

void UAbvaMovement::HandleWalkingOffLedge(const FVector& previousFloorImpactNormal, const FVector& previousFloorContactNormal, const FVector& previousLocation, float timeDelta)
{
	Super::HandleWalkingOffLedge(previousFloorImpactNormal, previousFloorContactNormal, previousLocation, timeDelta);

	GetWorld()->GetTimerManager().SetTimer(CoyoteTimer, CoyoteTime, false);
}

void UAbvaMovement::SetPostLandedPhysics(const FHitResult& result)
{
	MaxWallJumpCount = 1;
	LastWallranWallNormal = FVector::ZeroVector;
	Super::SetPostLandedPhysics(result);

	if (CanSlide())
	{
		BeginSlide();
	}
}

void UAbvaMovement::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* thisTickFunction)
{
	Super::TickComponent(deltaTime, tickType, thisTickFunction);

	if (IsSliding)
	{
		Sliding(deltaTime);
	}
	if (IsMoving)
	{
		MovePlayer(deltaTime);
	}
	
}

void UAbvaMovement::BeginPlay()
{
	Super::BeginPlay();

	BasePlayerHeight = GetPlayer()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

void UAbvaMovement::Jump()
{
	if (IsWallrunning)
	{
		WallrunAbort(true);
		return;
	}

	FVector targetLoc;
	bool crouch = false;

	if (CanVault(targetLoc))
	{
		Vault(targetLoc);
		return;
	}
	
	if (CanClimb(targetLoc, crouch))
	{
		Climb(targetLoc, crouch);
		return;
	}

	if (IsMovingOnGround() || GetWorld()->GetTimerManager().IsTimerActive(CoyoteTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(CoyoteTimer);
		
		if (IsCrouching())
		{
			JumpZVelocity = CrouchedJumpForce;
			Launch(FVector(0, 0, JumpZVelocity) + Velocity);
		}
		else
		{
			JumpZVelocity = BaseJumpForce;
		}

		GetPlayer()->Jump();

		if (!IsMovingOnGround())
		{
			Launch(FVector(Velocity.X, Velocity.Y, JumpZVelocity));
		}
	}
	else
	{
		WallJump();
	}
}
void UAbvaMovement::WallJump()
{
	if (!IsCrouching() && MaxWallJumpCount <= 0 && !IsMovingOnGround())
	{
		return;
	}

	const auto capsule = GetPlayer()->GetCapsuleComponent();
	const float extent = capsule->GetScaledCapsuleRadius() / 2;
	FCollisionShape box = FCollisionShape::MakeBox(FVector(extent, extent, 20.f));
	FHitResult hit;
	FVector start = capsule->GetComponentLocation() - (capsule->GetScaledCapsuleHalfHeight()) + 20.0 + MaxStepHeight;
	FVector end = start + capsule->GetForwardVector() * 100;
	FCollisionQueryParams param(FName("wallJump"), false, GetPlayer());

	if (GetWorld()->SweepSingleByChannel(hit, start, end, capsule->GetComponentQuat(), CollisionChannel, box, param))
	{
		Velocity = FVector(Velocity.X / 2, Velocity.Y / 2, WallJumpZHeight);
		MaxWallJumpCount--;
		return;
	}
	GetPlayer()->Jump();
}


void UAbvaMovement::BeginSlide()
{
	if (!IsSliding)
	{
		IsSliding = true;
		GroundFriction = 0.f;
		BrakingDecelerationWalking = 786.f;

		Velocity += GetPlayer()->GetCapsuleComponent()->GetForwardVector() * SlideStartBoost;
		OnSlide.Broadcast();
	}
}

void UAbvaMovement::Sliding(float deltaTime)
{
	if (Velocity.Length() > MaxSlideSpeed)
	{
		return;
	}
	else if (Velocity.Length() < MaxWalkSpeedCrouched)
	{
		EndSlide();
		return;
	}
	FHitResult floorHit = CurrentFloor.HitResult;
	FVector slideDir = CalcFloorInfluence(floorHit.ImpactNormal);

	Velocity += slideDir * FloorSlipperiness * deltaTime;
}

void UAbvaMovement::EndSlide()
{
	IsSliding = false;
	GroundFriction = 10;
	BrakingDecelerationWalking = 2048;
	OnSlideStop.Broadcast();
}

bool UAbvaMovement::CanSlide() const 
{
	return IsCrouching() && IsMovingOnGround() && Velocity.Length() > MinSpeedToSlide && !IsSliding;
}

FVector UAbvaMovement::CalcFloorInfluence(const FVector& floorNormal)
{
	if (!floorNormal.Equals(FVector::UpVector))
	{
		FVector V1 = FVector::CrossProduct(floorNormal, FVector::UpVector);
		FVector V2 = FVector::CrossProduct(floorNormal, V1);
		return V2;
	}
	else
	{
		return FVector::ZeroVector;
	}
}

void UAbvaMovement::Crouch(bool)
{
	Super::Crouch();
	const auto player = GetPlayer();
	player->Cam->SetRelativeLocation(FVector(0, 0, player->CrouchedViewHeight));
	AirControl = 0.1f;
	if (IsWallrunning)
	{
		WallrunAbort(false);
	}

	if (CanSlide())
	{
		BeginSlide();
	}
}

void UAbvaMovement::UnCrouch(bool)
{
	Super::UnCrouch();
	
	if (GetPlayer()->bIsCrouched)
	{
		return;
	}
	
	const auto player = GetPlayer();
	player->Cam->SetRelativeLocation(FVector(0, 0, player->StandingViewHeight));
	AirControl = 0.4f;
	EndSlide();
}

void UAbvaMovement::WallrunInit(const FVector& wallNormal, float distance, bool isRight)
{
	if (WallrunInitDoOnce)
	{
		if (LastWallranWallNormal == FVector::ZeroVector || !LastWallranWallNormal.Equals(wallNormal, 0.01))
		{
			GetPlayer()->StopJumping();
			LastWallranWallNormal = wallNormal;
			SetPlaneConstraintEnabled(true);
			SetPlaneConstraintNormal(wallNormal);

			WallrunInitDoOnce = false;
			IsWallrunning = true;
			GravityScale = 0.2f;
			//TODO: maybe we shouldnt cap the Z velocity if it's too big? otherwise it feels staggering.
			//either we dont cap or we dont initiate the wallrun
			Velocity.Z = 200.f;

   			auto velXY = FVector(Velocity.X, Velocity.Y, 0);
			//lmao this is stupid
			//TODO: maybe optimize the math here?
			//Velocity -= ((-wallNormal).Dot(velXY) * -wallNormal).RotateAngleAxis(180.0, wallNormal);
			Velocity = FVector::VectorPlaneProject(Velocity, wallNormal);

			GetWorld()->GetTimerManager().SetTimer(WallrunTimer, WallrunDuration, false);
			GetWorld()->GetTimerManager().SetTimer(WallrunCheckTimer, this, &UAbvaMovement::CheckIfCanStillWallrun, WallrunCheckInterval, true);
			FTimerManagerTimerParameters params;
			params.bMaxOncePerFrame = true;
			params.bLoop = true;
			FTimerDelegate dele;
			
			dele.BindUObject(this, &UAbvaMovement::FixPlayerPosToWall);
			FixPosRemainingDistance = distance - GetPlayer()->GetCapsuleComponent()->GetScaledCapsuleRadius();
			GetWorld()->GetTimerManager().SetTimer(FixPlayerPosTimer, dele, SMALL_NUMBER, params);

			OnWallRun.Broadcast(isRight);
		}
		else
		{
			WallrunAbort(false);
		}
	}
}

void UAbvaMovement::WallrunAbort(bool jump)
{
	if (!WallrunInitDoOnce)
	{
		WallrunInitDoOnce = true;

		SetPlaneConstraintEnabled(false);
		GetWorld()->GetTimerManager().ClearTimer(WallrunTimer);
		GetWorld()->GetTimerManager().ClearTimer(WallrunCheckTimer);
		GravityScale = 1.f;
		IsWallrunning = false;

		if (jump)
		{
			auto x = GetPlayer()->Cam->GetForwardVector();
			Velocity = FVector(x.X, x.Y, 0).GetSafeNormal() * Velocity.Length() + FVector::UpVector * 400;
		}
		OnWallRunStop.Broadcast();
	}
}

void UAbvaMovement::FixPlayerPosToWall()
{
	if (FixPosRemainingDistance >= KINDA_SMALL_NUMBER)
	{
   		constexpr float moveRate = 100.f; // 100cm/s
		float moveAmount = moveRate * GetWorld()->DeltaTimeSeconds;
		moveAmount = FMath::Clamp(moveAmount, 0, FixPosRemainingDistance);
		GetOwner()->AddActorWorldOffset(-LastWallranWallNormal * moveAmount);
		FixPosRemainingDistance -= moveAmount;

		GEngine->AddOnScreenDebugMessage(2, 0.1, FColor::Cyan, "Remaining Distance: " + FString::SanitizeFloat(FixPosRemainingDistance));
		return;
	}
	GetWorld()->GetTimerManager().ClearTimer(FixPlayerPosTimer);
}

void UAbvaMovement::CheckIfCanStillWallrun()
{
	const auto capsule = GetPlayer()->GetCapsuleComponent();
	FVector start = capsule->GetComponentLocation();
	FVector end = start - LastWallranWallNormal * 100.0;
	FCollisionShape shape = FCollisionShape::MakeCapsule(capsule->GetScaledCapsuleRadius(), capsule->GetScaledCapsuleHalfHeight() / 2);
	FCollisionQueryParams params(FName("WallrunCheck"), false, GetOwner());

	if (!GetWorld()->SweepTestByChannel(start, end, FQuat::Identity, CollisionChannel, shape, params))
	{
		WallrunAbort(false);
	}
}

bool UAbvaMovement::CanVault(FVector& outVaultLoc) const
{
	if (IsCrouching() || !IsMovingOnGround() || Velocity.SquaredLength() <= FMath::Square(300.f))
	{
		UE_LOG(LogTemp, Warning, TEXT("The initial conditions for vaulting weren't met. Aborting."));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("------------Starting vault check------------"));

	const auto capsule = GetPlayer()->GetCapsuleComponent();
	const float radius = capsule->GetScaledCapsuleRadius();
	const float halfHeight = capsule->GetScaledCapsuleHalfHeight();
	const auto capsuleShape = FCollisionShape::MakeCapsule(radius, halfHeight / 2.0f);

	FCollisionQueryParams params(FName("VaultTrace"), false, GetOwner());
	using namespace Dev::TraceDev;

	//Trace for the front wall
	FVector start = capsule->GetComponentLocation();
	FVector end = start + capsule->GetForwardVector() * 100.0f;
	FHitResult frontWallHit;

	if (!DevSweepSingleByChannel(GetWorld(), frontWallHit, start, end, FQuat::Identity, CollisionChannel, capsuleShape, params))
	{
		UE_LOG(LogTemp, Warning, TEXT("There wasn't any surface to vault on."));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Front wall was hit!"));

	//Trace for the floor (top) of the vault object
	end = frontWallHit.ImpactPoint - frontWallHit.ImpactNormal;
	start = end;
	start.Z = GetPlayer()->Cam->GetComponentLocation().Z - 10.0f;

	FCollisionShape boxShape = FCollisionShape::MakeBox(FVector(radius - 0.1f));
	FHitResult floorHit;

	if (!DevSweepSingleByChannel(GetWorld(), floorHit, start, end, capsule->GetComponentQuat(), CollisionChannel, boxShape, params))
	{
		UE_LOG(LogTemp, Warning, TEXT("The object isn't vaultable, climbing or jumping is preferred."));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Floor was hit!"));

	//Check if the path forward over the object is clear
	constexpr float vaultReach = 300.f;
	start = floorHit.ImpactPoint + floorHit.ImpactNormal * radius;
	end = start + capsule->GetForwardVector() * vaultReach;

	if (GetWorld()->SweepTestByChannel(start, end, FQuat::Identity, CollisionChannel, boxShape, params))
	{
		UE_LOG(LogTemp, Warning, TEXT("Something was in the way of vaulting."));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Path is clear!"));

	//Final downward trace to find the exact landing drop-off
	start = end;
	end = start - FVector(0, 0, 500.f);

	DevSweepSingleByChannel(GetWorld(), floorHit, start, end, FQuat::Identity, CollisionChannel, capsuleShape, params);

	outVaultLoc = floorHit.bBlockingHit
		? floorHit.ImpactPoint + FVector(0, 0, halfHeight)
		: FVector(end.X, end.Y, capsule->GetComponentLocation().Z);

	return true;
}

//TODO:add a check to see if there's a roof above the player so he wont phase through it when climbing
//TODO:Might wanna make it so the check takes the camera's angle into consideration.
//for example if the camera isnt look upwards then the player probably doesnt want to climb
bool UAbvaMovement::CanClimb(FVector& outClimbLoc, bool& outCrouch) const
{
	outCrouch = false;

	const auto capsule = GetPlayer()->GetCapsuleComponent();
	const float radius = capsule->GetScaledCapsuleRadius();
	const float halfHeight = capsule->GetScaledCapsuleHalfHeight();

	const float boxExtent = IsCrouching() ? GetCrouchedHalfHeight() : radius;
	FCollisionShape boxShape = FCollisionShape::MakeBox(FVector(boxExtent));

	FCollisionQueryParams params(FName("ClimbTrace"), false, GetOwner());
	using namespace Dev::TraceDev;

	//Sweep for the obstacle we want to climb
	FVector start = capsule->GetComponentLocation() + capsule->GetForwardVector() * radius;
	FVector end = start + capsule->GetForwardVector() * 100.f;
	FHitResult frontWallHit;

	if (!DevSweepSingleByChannel(GetWorld(), frontWallHit, start, end, FQuat::Identity, CollisionChannel, boxShape, params))
	{
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("FrontWall hit!"));

	//Trace downward to find the floor/ledge of the object
	end = frontWallHit.ImpactPoint - frontWallHit.ImpactNormal * 2.f;
	start = end;
	start.Z = GetPlayer()->Cam->GetComponentLocation().Z + 200.f;
	FHitResult floorHit;

	if (!DevLineTraceSingleByChannel(GetWorld(), floorHit, start, end, CollisionChannel, params))
	{
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Floor was hit!"));

	//Validate the floor normal and height
	if (FVector::DotProduct(floorHit.ImpactNormal, FVector::UpVector) < 1.0f - KINDA_SMALL_NUMBER)
	{
		UE_LOG(LogTemp, Warning, TEXT("The floor was too angled to climb. Normal: %s"), *floorHit.ImpactNormal.ToString());
		return false;
	}

	if (FMath::Abs(floorHit.ImpactPoint.Z - capsule->GetComponentLocation().Z) < 20.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("The floor wasn't high enough to be considered for climbing."));
		return false;
	}

	//Trace upwards to check for a roof
	start = floorHit.ImpactPoint + floorHit.ImpactNormal;
	end = start + FVector::UpVector * (halfHeight * 2.f) + 2.f;
	FHitResult roofHit;

	if (DevLineTraceSingleByChannel(GetWorld(), roofHit, start, end, CollisionChannel, params))
	{
		UE_LOG(LogTemp, Log, TEXT("Roof was hit!"));
		if (roofHit.Distance < GetCrouchedHalfHeight() * 2.f + 1.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("The space was too tight to climb."));
			return false;
		}
		outCrouch = true;
	}

	//Check if there's anything directly above the player blocking their upward climb
	FVector startLocal = capsule->GetComponentLocation();
	FVector endLocal = FVector(startLocal.X, startLocal.Y, roofHit.bBlockingHit ? roofHit.ImpactPoint.Z : floorHit.ImpactPoint.Z + halfHeight);

	if (DevSweepTestByChannel(GetWorld(), startLocal, endLocal, FQuat::Identity, CollisionChannel, boxShape, params))
	{
		UE_LOG(LogTemp, Warning, TEXT("Something is above the player blocking their climb."));
		return false;
	}

	//Check Side Clearances
	const float sideTraceDist = radius + 1.f;
	const auto relativeRight = frontWallHit.ImpactNormal.RotateAngleAxis(90.f, FVector::UpVector);

	float rDist = radius;
	float lDist = radius;
	FHitResult sideWallHit;

	end = start + relativeRight * sideTraceDist;
	if (DevLineTraceSingleByChannel(GetWorld(), sideWallHit, start, end, CollisionChannel, params))
	{
		rDist = sideWallHit.Distance;
	}

	end = start - relativeRight * sideTraceDist;
	if (DevLineTraceSingleByChannel(GetWorld(), sideWallHit, start, end, CollisionChannel, params))
	{
		lDist = sideWallHit.Distance;
		if (rDist + lDist <= radius)
		{
			UE_LOG(LogTemp, Warning, TEXT("The area was too tight horizontally to climb :("));
			return false;
		}
	}

	//Final calculations
	const float finalHeight = outCrouch ? GetCrouchedHalfHeight() : halfHeight;

	outClimbLoc = floorHit.ImpactPoint + relativeRight * (rDist - lDist) + FVector::UpVector * finalHeight;

#if !UE_BUILD_SHIPPING
	DrawDebugCapsule(GetWorld(), outClimbLoc, finalHeight, radius, FQuat::Identity, FColor::Red, false, 5.f, 0, 1.f);
#endif

	return true;
}

void UAbvaMovement::Vault(const FVector& vaultLoc)
{
	const auto player = GetPlayer();

	IsVaulting = true;
	StartMovingPlayer(vaultLoc, VaultSpeed, false);
}

void UAbvaMovement::Climb(const FVector& climbLoc, const bool crouch)
{
	const auto player = GetPlayer();
	Velocity = FVector::ZeroVector;
	IsClimbing = true;

	StartMovingPlayer(climbLoc, ClimbSpeed , crouch);
}

void UAbvaMovement::PauseMovement()
{
	PausedData = FMovementData(Velocity, Acceleration);
	SetMovementMode(EMovementMode::MOVE_None);
}

void UAbvaMovement::ContinueMovement()
{
	SetMovementMode(EMovementMode::MOVE_Falling);
	Velocity = PausedData.Velocity;
	Acceleration = PausedData.Acceleration;
}

void UAbvaMovement::StartMovingPlayer(const FVector& moveTarget, const float moveDuration ,const bool shouldCrouch)
{
	const auto player = GetPlayer();

	ElapsedMoveTime = 0;
	CameraStartPos = player->Cam->GetComponentLocation();
	MoveDuration = moveDuration;
	player->Cam->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	player->SetActorLocation(moveTarget);

	if (shouldCrouch)
	{
		Crouch();
		TWeakObjectPtr<UAbvaMovement> WeakThis = this;
		GetWorld()->GetTimerManager().SetTimerForNextTick([WeakThis]() {
			if (WeakThis.IsValid()) 
			{ 
				WeakThis->UnCrouch();
			}
			});
	}

	const auto eyeOffset = FVector(0, 0, shouldCrouch ? player->CrouchedViewHeight : player->StandingViewHeight);

	PauseMovement();

	IsMoving = true;
}

void UAbvaMovement::MovePlayer(float deltaTime)
{
	const auto p = GetPlayer();

	ElapsedMoveTime += deltaTime;
	const float alpha = ElapsedMoveTime / MoveDuration;
	const auto moveTarget = p->GetCapsuleComponent()->GetComponentLocation()
											+ FVector(0, 0, IsCrouching() ? p->CrouchedViewHeight : p->StandingViewHeight);
	auto target = FMath::Lerp(CameraStartPos, moveTarget, alpha);

	p->Cam->SetWorldLocation(target);

	if (alpha >= 1.f)
	{
		EndMovingPlayer();
	}
}

void UAbvaMovement::EndMovingPlayer()
{
	const auto p = GetPlayer();

	p->Cam->AttachToComponent(p->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	p->Cam->SetRelativeLocation(FVector(0, 0, p->BaseEyeHeight));

	ContinueMovement();

	IsMoving = false;
	if (p->GetJumpHoldTime() > 0.5 && IsVaulting)
	{
		const auto modifier = FVector(0.3, 0.3, 1.0);
		Velocity = Velocity * modifier + FVector::UpVector * VaultJumpForce;
		Acceleration *= modifier;
	}

	IsVaulting = false;

	IsClimbing = false;
}

AAbvaPlayer* UAbvaMovement::GetPlayer() const
{
	return CastChecked<AAbvaPlayer>(GetOwner());
}
