// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Math/Vector2D.h" // Included from your original code
#include "Blueprint/UserWidget.h" // Included from your original code
#include "FootballGameCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
// class UStaticMeshComponent; // Not used directly in the .h you provided, but fine to keep if needed elsewhere
class ABall;
class UCurveFloat;
class UFootballInGameHUDNew; // Forward declaration for InGameHUDInstanceNew

UENUM(BlueprintType)
enum class EPlayerMovementState : uint8
{
    Grounded UMETA(DisplayName = "Grounded"),
    Dashing UMETA(DisplayName = "Dashing"),
    Airborne UMETA(DisplayName = "Airborne"),
    Gliding UMETA(DisplayName = "Gliding"),
    ChargingDash UMETA(DisplayName = "ChargingDash"),
    ChargingShot UMETA(DisplayName = "ChargingShot")
};

UCLASS(config=Game)
class FOOTBALLGAME_API AFootballGameCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category="Movement|Input")
    float MovementInputSmoothingSpeed = 8.0f;
    
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UFootballInGameHUDNew> InGameHUDClassNew; 

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    TObjectPtr<UFootballInGameHUDNew> InGameHUDInstanceNew;

private:
    FVector2D SmoothedMovementInput;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> FollowCamera;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    // ... (Input Action UPROPERTY declarations for MoveAction, LookAction, JumpAction, DashAction, etc. remain the same) ...
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> DashAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> GlideAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> ShootAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> PassInterceptAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> TackleAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> ToggleLowGravityAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> ToggleAdrenalineAction;


public:
    AFootballGameCharacter();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerState|Movement")
    EPlayerMovementState CurrentMovementState;

    // Dash System Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerSettings|Dash")
    float MinDashChargeTime;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerSettings|Dash")
    float MaxDashChargeTime;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerSettings|Dash")
    float MinDashForce;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerSettings|Dash")
    float MaxDashForce;
    // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerSettings|Dash") // REMOVED
    // float DashCooldown;                                                           // REMOVED
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerSettings|Dash", meta = (AllowedClasses = "CurveFloat"))
    TObjectPtr<UCurveFloat> DashForceCurve;

    // ... (Glide and Shoot properties remain the same) ...
    // Glide System Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerSettings|Glide")
    float GlideMaxDuration;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerSettings|Glide")
    float GlideGravityScale;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerSettings|Glide")
    float MinSpeedForGlide;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerSettings|Glide")
    float GlideHorizontalSpeedMultiplier;

    // Shoot System Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerSettings|Shoot")
    float MinShootChargeTime;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerSettings|Shoot")
    float MaxShootChargeTime;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerSettings|Shoot")
    float MinShootPower;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerSettings|Shoot")
    float MaxShootPower;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerSettings|Shoot")
    float ShootDashPowerMultiplier;
    UPROPERTY(EditDefaultsOnly, Category="PlayerSettings|Shoot")
    TSubclassOf<ABall> BallProjectileClass;
    UPROPERTY(EditDefaultsOnly, Category="PlayerSettings|Shoot")
    FName BallSpawnSocketName;


protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void Landed(const FHitResult& Hit) override;

    // Input Handler Functions (declarations remain the same)
    void MoveInput(const FInputActionValue& Value);
    void LookInput(const FInputActionValue& Value);

    void StartDashChargeInput();
    void ReleaseDashInput();
    void AttemptGlideInput();
    void StopGlideInput();
    void StartShootChargeInput();
    void ReleaseShootInput();
    void PassOrIntercept();
    void Tackle();
    void ToggleLowGravityField();
    void ToggleAdrenalineMode();
    void StartActualGlide(FVector InitialGlideVelocity);
private:
    // Core Logic Functions
    void UpdateDashCharging(float DeltaTime);
    void ApplyDash(float ChargeRatio);
    FTimerHandle DashStateActiveTimerHandle; 
    void ExitDashingState();   

    void StartActualGlide();
    void UpdateGliding(float DeltaTime);
    void StopActualGlide(bool bForceStop = false);
    void OnGlideDurationExpired();
    FTimerHandle GlideDurationTimerHandle;

    void UpdateShootCharging(float DeltaTime);
    void PerformShot(float ChargeRatio);

    void UpdateCurrentMovementState();

    // State Variables
    float CurrentDashChargeTime;
    bool bIsDashCharging;
    // bool bCanDash; // REMOVED (If no cooldown, assume can always dash unless another condition prevents it)

    float CurrentGlideTime;
    bool bIsGliding;
    float OriginalGravityScale;
    FVector DashReleaseVelocity;

    float CurrentShootChargeTime;
    bool bIsShootCharging;
};