#pragma once

#include "GameFramework/Character.h"
#include "AbvaPlayer.generated.h"

class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UAbvaMovement;
class UItemManager;
class AAbvaController;
struct FInputActionValue;


UCLASS()
class AAbvaPlayer : public ACharacter
{
    GENERATED_BODY()

	friend AAbvaController;
	friend UAbvaMovement;

    AAbvaPlayer(const FObjectInitializer& initializer);

public:
	virtual void BeginPlay() override;

	virtual void Tick(float deltaTick) override;

protected://input
    virtual void SetupPlayerInputComponent(class UInputComponent* playerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputMappingContext> IMC_FpsPlayerMapping;

	//Movement
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> IA_Move;
	void MoveDir(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> IA_Look;
	void Look(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> IA_Sprint;
	void Sprint(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> IA_Crouch;
	void PCrouch(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> IA_Jump;
	void PJump(const FInputActionValue& Value);

	//Combat
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> IA_Reload;
	void Reload(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> IA_ADS;
	void ADS(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> IA_ChangeWeapon;
	void ChangeItem(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> IA_Attack;
	void Attack(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> IA_Interact;
	void Interact(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> IA_ThrowItem;
	void ThrowItem(const FInputActionValue& Value);
	//end input

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USceneComponent> WeaponLock;

protected://properties
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UItemManager> ItemManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UCameraComponent> Cam;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> SpeedLineMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> SpeedLineMat;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StandingViewHeight = 75.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CrouchedViewHeight = 25.f;
	//end properties

protected:
	FVector2D PlayerMoveInput = FVector2D::ZeroVector;

protected:
	virtual void Jump() override;
	virtual void StopJumping() override;
	virtual void Landed(const FHitResult& floor) override;

	double GetJumpHoldTime() const;
public:
	virtual void GetActorEyesViewPoint(FVector& outLocation, FRotator& outRotation) const override;
	
private:
	void ChangeFOVWithSpeed(float deltaTick) const;

private:
	double JumpTimeStamp = -1;

private:
	auto GetMovement() const;
	auto GetPlayerController() const;
	bool CanWallrun() const;
	void HandleWallrun();
	void HandleSpeedLines(float deltaTime) const;
};