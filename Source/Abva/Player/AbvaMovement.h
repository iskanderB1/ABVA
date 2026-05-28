#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbvaMovement.generated.h"

USTRUCT()
struct FMovementData
{
	GENERATED_BODY()
	FMovementData() {}
	inline constexpr explicit FMovementData(const FVector& velocity, const FVector& acceleration);

	FVector Velocity;
	FVector Acceleration;
};

inline constexpr FMovementData::FMovementData(const FVector& velocity, const FVector& acceleration) : Velocity(velocity), Acceleration(acceleration)
{}

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWallRunEvent, bool, IsRight);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWallRunStopEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlideEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlideStopEvent);

class AAbvaPlayer;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ABVA_API UAbvaMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintAssignable)
	FWallRunEvent OnWallRun;

	UPROPERTY(BlueprintAssignable)
	FWallRunStopEvent OnWallRunStop;

	UPROPERTY(BlueprintAssignable)
	FSlideEvent OnSlide;

	UPROPERTY(BlueprintAssignable)
	FSlideStopEvent OnSlideStop; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECollisionChannel> CollisionChannel = ECollisionChannel::ECC_WorldStatic;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CoyoteTime = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WallJumpZHeight = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float BaseJumpForce = 500.f;
	
	float BasePlayerHeight;
public:	
	// Sets default values for this component's properties
	UAbvaMovement();

	virtual void HandleWalkingOffLedge(const FVector& PreviousFloorImpactNormal, const FVector& PreviousFloorContactNormal, const FVector& PreviousLocation, float TimeDelta) override;
	
	virtual void SetPostLandedPhysics(const FHitResult& result) override;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void BeginPlay() override;

	//---------------------------------//

	void Jump();

	void WallJump();

	FTimerHandle CoyoteTimer;

public:
	//------------------------------//
	void BeginSlide();
	void Sliding(float deltaTime);
	void EndSlide();
	bool CanSlide() const;
	bool IsSliding = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sliding")
	float MinSpeedToSlide = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sliding")
	float MaxSlideSpeed = 3500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sliding")
	float FloorSlipperiness = 4000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sliding")
	float SlideStartBoost = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CrouchedJumpForce = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CrouchSpeed = 0.5f;

	static FVector CalcFloorInfluence(const FVector& floorNormal);
	virtual void Crouch(bool = false) override;
	virtual void UnCrouch(bool = false) override;
	bool bAnimateCrouch = false;
	float CrouchAlpha = 0.f;

public:

	void WallrunInit(const FVector& wallNormal, float distance, bool isRight);

	void WallrunAbort(bool jump);

	void FixPlayerPosToWall();

	void CheckIfCanStillWallrun();

	FTimerHandle FixPlayerPosTimer;

	FTimerHandle WallrunTimer;

	FTimerHandle WallrunCheckTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRunning")
	float WallrunDuration = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRunning")
	float WallrunCheckInterval = 0.2f;

	float FixPosRemainingDistance = 0.f;

	float TimeSinceJumped;

	FVector LastWallranWallNormal = FVector::ZeroVector;

	bool IsWallrunning = false;

	bool WallrunInitDoOnce = true;
protected:
	uint8 MaxWallJumpCount = 1;


public:
	bool IsClimbing = false;
	bool IsVaulting = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vault")
	float VaultJumpForce = 2000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vault")
	float VaultSpeed = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climb")
	float ClimbSpeed = 1.f;
	bool CanVault(FVector& outVaultLoc) const;
	bool CanClimb(FVector& outClimbLoc, bool& outClimb) const;
	bool IsClimbingOrVaulting() const { return IsClimbing || IsVaulting; }
	
	void Vault(const FVector& vaultLoc);
	void Climb(const FVector& climbLoc, const bool crouch);

	bool IsPaused = false;
	void PauseMovement();
	void ContinueMovement();
	FMovementData PausedData;

private:
	float ElapsedMoveTime;
	float MoveDuration;
	bool IsMoving;
	FVector MoveTarget;
	FVector CameraStartPos;
	void StartMovingPlayer(const FVector& moveTarget, const float moveDuration, const bool shouldCrouch);
	void MovePlayer(float deltaTime);
	void EndMovingPlayer();

private:
	AAbvaPlayer* GetPlayer() const;
};