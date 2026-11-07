// Copyright Epic Games, Inc. All Rights Reserved.
#include "FootballGameCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "FootballInGameHUDNew.h" // Assuming correct path
#include "Ball.h"
#include "Curves/CurveFloat.h"


DEFINE_LOG_CATEGORY_STATIC(LogFootballGameCharacter, Log, All);

AFootballGameCharacter::AFootballGameCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity = 1500.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 600.f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // Tuned dash values
    MinDashChargeTime = 0.3f;
    MaxDashChargeTime = 1.2f;
    MinDashForce = 1500.0f;
    MaxDashForce = 3500.0f;
    // DashCooldown = 3.0f; // REMOVED
    DashForceCurve = nullptr;

    GlideMaxDuration = 2.5f;
    GlideGravityScale = 0.1f;
    MinSpeedForGlide = 100.0f;
    GlideHorizontalSpeedMultiplier = 1.3f;

    MinShootChargeTime = 0.3f;
    MaxShootChargeTime = 1.2f;
    MinShootPower = 2000.0f;
    MaxShootPower = 6000.0f;
    ShootDashPowerMultiplier = 1.5f;
    BallSpawnSocketName = TEXT("foot_r_ball");

    CurrentDashChargeTime = 0.0f;
    bIsDashCharging = false;
    // bCanDash = true; // REMOVED (no cooldown = always can dash, unless other state blocks it)

    CurrentGlideTime = 0.0f;
    bIsGliding = false;
    OriginalGravityScale = 1.0f;

    CurrentShootChargeTime = 0.0f;
    bIsShootCharging = false;

    CurrentMovementState = EPlayerMovementState::Grounded;
    SmoothedMovementInput = FVector2D::ZeroVector;
}

void AFootballGameCharacter::ExitDashingState()
{
    if (CurrentMovementState == EPlayerMovementState::Dashing)
    {
        UE_LOG(LogFootballGameCharacter, Log, TEXT("Dashing state active duration ended. State will re-evaluate on next Tick."));
        UpdateCurrentMovementState(); // Optional: force immediate re-evaluation
    }
}

void AFootballGameCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            if (DefaultMappingContext)
            {
                 Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
            else
            {
                UE_LOG(LogFootballGameCharacter, Warning, TEXT("DefaultMappingContext is not set in %s"), *GetName());
            }
        }
    }
    if (GetCharacterMovement())
    {
        OriginalGravityScale = GetCharacterMovement()->GravityScale;
    }
    UpdateCurrentMovementState();
    
    if (IsLocallyControlled())
    {
        APlayerController* PC = GetController<APlayerController>();
        if (PC && InGameHUDClassNew) 
        {
            InGameHUDInstanceNew = CreateWidget<UFootballInGameHUDNew>(PC, InGameHUDClassNew);
            if (InGameHUDInstanceNew)
            {
                InGameHUDInstanceNew->AddToViewport();
                UE_LOG(LogFootballGameCharacter, Log, TEXT("InGameHUDInstanceNew (%s) created and added to viewport."), *InGameHUDInstanceNew->GetName());
            }
            else
            {
                FString HudClassName = InGameHUDClassNew ? InGameHUDClassNew->GetName() : TEXT("nullptr");
                UE_LOG(LogFootballGameCharacter, Error, TEXT("Failed to create InGameHUDInstanceNew from InGameHUDClassNew '%s'!"), *HudClassName);
            }
        }
    }
}

void AFootballGameCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateCurrentMovementState();
    if (bIsDashCharging) UpdateDashCharging(DeltaTime);
    if (bIsGliding) UpdateGliding(DeltaTime);
    if (bIsShootCharging) UpdateShootCharging(DeltaTime);

    // REMOVED: Logic for updating HUD cooldown bar in Tick
    // if (!bCanDash && InGameHUDInstanceNew) ...
}

// In AFootballGameCharacter.cpp
void AFootballGameCharacter::UpdateCurrentMovementState()
{
    if (!GetCharacterMovement()) return;
    EPlayerMovementState PreviousState = CurrentMovementState;


    if (bIsGliding)
    {
        CurrentMovementState = EPlayerMovementState::Gliding;
    }
    else if (bIsDashCharging) // 正在蓄力冲刺
    {
        CurrentMovementState = EPlayerMovementState::ChargingDash;
    }
    else if (bIsShootCharging) // 正在蓄力射门
    {
        CurrentMovementState = EPlayerMovementState::ChargingShot;
    }

    else if (GetCharacterMovement()->IsFalling())
    {
        CurrentMovementState = EPlayerMovementState::Airborne;
    }
    else 
    {
        CurrentMovementState = EPlayerMovementState::Grounded;
    }

    if (PreviousState != CurrentMovementState)
    {
        UE_LOG(LogFootballGameCharacter, Log, TEXT("Movement State Changed from %s to %s"), *UEnum::GetValueAsString(PreviousState), *UEnum::GetValueAsString(CurrentMovementState));
    }
}

void AFootballGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Bindings remain the same
        if(MoveAction) EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFootballGameCharacter::MoveInput);
        if(LookAction) EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFootballGameCharacter::LookInput);
        if(JumpAction)
        {
            EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
            EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
        }
        if(DashAction)
        {
            EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AFootballGameCharacter::StartDashChargeInput);
            EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &AFootballGameCharacter::ReleaseDashInput);
        }
        if(GlideAction)
        {
             EnhancedInputComponent->BindAction(GlideAction, ETriggerEvent::Started, this, &AFootballGameCharacter::AttemptGlideInput);
             EnhancedInputComponent->BindAction(GlideAction, ETriggerEvent::Completed, this, &AFootballGameCharacter::StopGlideInput);
        }
        if(ShootAction)
        {
            EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &AFootballGameCharacter::StartShootChargeInput);
            EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, this, &AFootballGameCharacter::ReleaseShootInput);
        }
        if(PassInterceptAction) EnhancedInputComponent->BindAction(PassInterceptAction, ETriggerEvent::Triggered, this, &AFootballGameCharacter::PassOrIntercept);
        if(TackleAction) EnhancedInputComponent->BindAction(TackleAction, ETriggerEvent::Triggered, this, &AFootballGameCharacter::Tackle);
        if(ToggleLowGravityAction) EnhancedInputComponent->BindAction(ToggleLowGravityAction, ETriggerEvent::Triggered, this, &AFootballGameCharacter::ToggleLowGravityField);
        if(ToggleAdrenalineAction) EnhancedInputComponent->BindAction(ToggleAdrenalineAction, ETriggerEvent::Triggered, this, &AFootballGameCharacter::ToggleAdrenalineMode);
    }
    else
    {
        UE_LOG(LogFootballGameCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
    }
}

// MoveInput, LookInput, Landed remain largely the same
void AFootballGameCharacter::MoveInput(const FInputActionValue& Value)
{
    FVector2D RawInput = Value.Get<FVector2D>();
    SmoothedMovementInput = FMath::Vector2DInterpTo(
        SmoothedMovementInput,
        RawInput,
        GetWorld()->GetDeltaSeconds(),
        MovementInputSmoothingSpeed
    );
    const FVector2D MovementVector = SmoothedMovementInput;
    if (Controller != nullptr && CurrentMovementState != EPlayerMovementState::Dashing)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}

void AFootballGameCharacter::LookInput(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();
    if (Controller != nullptr)
    {
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void AFootballGameCharacter::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);
    StopActualGlide(true);
}


void AFootballGameCharacter::StartDashChargeInput()
{
    // If no cooldown, bCanDash is effectively always true, unless another mechanic sets it to false.
    // For simplicity now, we remove the bCanDash check.
    // If you reintroduce cooldown or another resource (like stamina) for dash, add the check back.
    UE_LOG(LogFootballGameCharacter, Warning, TEXT("StartDashChargeInput CALLED!"));
    UE_LOG(LogFootballGameCharacter, Warning, TEXT("Condition Check: State=%s"), *UEnum::GetValueAsString(CurrentMovementState));

    if (CurrentMovementState != EPlayerMovementState::Gliding && CurrentMovementState != EPlayerMovementState::Dashing)
    {
        bIsDashCharging = true;
        CurrentDashChargeTime = 0.0f;
        UE_LOG(LogFootballGameCharacter, Log, TEXT("Dash Charging ACTUALLY Started."));
    }
    else
    {
         UE_LOG(LogFootballGameCharacter, Warning, TEXT("StartDashChargeInput CONDITIONS NOT MET (Currently Gliding or Dashing)."));
    }
}

void AFootballGameCharacter::ReleaseDashInput()
{
    UE_LOG(LogFootballGameCharacter, Warning, TEXT("ReleaseDashInput CALLED!"));
    if (bIsDashCharging)
    {
        bIsDashCharging = false; 
        if (CurrentDashChargeTime >= MinDashChargeTime)
        {
            float ChargeRatio = FMath::Clamp(CurrentDashChargeTime / MaxDashChargeTime, 0.0f, 1.0f);
            if (DashForceCurve)
            {
                ChargeRatio = DashForceCurve->GetFloatValue(ChargeRatio);
            }
            ApplyDash(ChargeRatio);
        }
        else 
        {
            UE_LOG(LogFootballGameCharacter, Log, TEXT("Dash charge too short. Dash cancelled."));
        }
        CurrentDashChargeTime = 0.0f; 
    }
    else { UE_LOG(LogFootballGameCharacter, Warning, TEXT("ReleaseDashInput: NOT bIsDashCharging")); }
}




void AFootballGameCharacter::ApplyDash(float ChargeRatio)
{
    // If no cooldown, bCanDash check is removed.
    // Consider adding other conditions if needed (e.g., stamina).
    if (!GetCharacterMovement())
    {
        UE_LOG(LogFootballGameCharacter, Warning, TEXT("ApplyDash: No CharacterMovementComponent."));
        return;
    }

    float ForceMagnitude = FMath::Lerp(MinDashForce, MaxDashForce, ChargeRatio);
    FVector DashDirection = GetActorForwardVector(); 
    FVector LastMovementInput = GetCharacterMovement()->GetLastInputVector();

    if (!LastMovementInput.IsNearlyZero())
    {
        DashDirection = LastMovementInput.GetSafeNormal();
    }
    else if (GetCharacterMovement()->IsFalling()) 
    {
         DashDirection = UKismetMathLibrary::GetForwardVector(GetControlRotation());
    }

    GetCharacterMovement()->Launch(DashDirection * ForceMagnitude);
    UE_LOG(LogFootballGameCharacter, Log, TEXT("Dash Applied! ForceRatio: %f, LaunchSpeed: %f, Direction: %s"), ChargeRatio, ForceMagnitude, *DashDirection.ToString());
    
    DashReleaseVelocity = GetCharacterMovement()->Velocity; 
    CurrentMovementState = EPlayerMovementState::Dashing;   
    // bCanDash = false; // REMOVED - No cooldown means dash is always available if conditions met
    
    // REMOVED - No cooldown timer to set
    // GetWorldTimerManager().SetTimer(DashCooldownTimerHandle, this, &AFootballGameCharacter::OnDashCooldownFinished, DashCooldown, false);
    
    GetWorldTimerManager().SetTimer(DashStateActiveTimerHandle, this, &AFootballGameCharacter::ExitDashingState, 0.2f, false); // Keep this to exit Dashing state
    
    if (InGameHUDInstanceNew)
    {
        InGameHUDInstanceNew->UpdateDashCharge(0.0f); 
        // REMOVED - No cooldown bar to show initially
        // InGameHUDInstanceNew->SetDashCooldown(true, 1.0f); 
    }
}


void AFootballGameCharacter::AttemptGlideInput()
{
    UE_LOG(LogFootballGameCharacter, Warning, TEXT("AttemptGlideInput CALLED!"));
    if (!GetCharacterMovement())
    {
        UE_LOG(LogFootballGameCharacter, Warning, TEXT("AttemptGlideInput: No CharacterMovementComponent."));
        return;
    }


    bool bIsMovingFastEnough = GetCharacterMovement()->Velocity.SizeSquared() > FMath::Square(MinSpeedForGlide);
    FVector CurrentActualVelocity = GetCharacterMovement()->Velocity; // 用于日志

    UE_LOG(LogFootballGameCharacter, Warning, TEXT("Glide Attempt Conditions: IsFalling=%s, !bIsGliding=%s, bIsMovingFastEnough (from CURRENT Velocity)=%s (CurrentVelSq: %f, MinSpeedSq: %f)"),
        GetCharacterMovement()->IsFalling() ? TEXT("True") : TEXT("False"),
        !bIsGliding ? TEXT("True") : TEXT("False"),
        bIsMovingFastEnough ? TEXT("True") : TEXT("False"),
        CurrentActualVelocity.SizeSquared(), FMath::Square(MinSpeedForGlide));

    if (GetCharacterMovement()->IsFalling() && !bIsGliding && bIsMovingFastEnough)
    {

        StartActualGlide(CurrentActualVelocity); // <--- 修改 StartActualGlide 以接收初始速度
    }
    else
    {
        UE_LOG(LogFootballGameCharacter, Warning, TEXT("AttemptGlideInput: Conditions for glide not met."));
    }
}
void AFootballGameCharacter::StartActualGlide(FVector InitialGlideVelocity) // <--- 接收初始速度
{
    if (!GetCharacterMovement() || bIsGliding) return;
    bIsGliding = true;
    CurrentGlideTime = 0.0f;
    GetCharacterMovement()->GravityScale = GlideGravityScale;

    FVector HorizontalVelocityToMaintain = InitialGlideVelocity; // 使用传入的初始速度
    HorizontalVelocityToMaintain.Z = 0;

    float InitialZ = InitialGlideVelocity.Z * 0.1f; // 保留初始Z速度的一小部分

    GetCharacterMovement()->Velocity = (HorizontalVelocityToMaintain * GlideHorizontalSpeedMultiplier) + FVector(0,0,InitialZ);

    UE_LOG(LogFootballGameCharacter, Log, TEXT("Glide ACTUALLY Started. Gravity Scale: %f, Initial Glide Velocity: %s"), GlideGravityScale, *GetCharacterMovement()->Velocity.ToString());
    GetWorldTimerManager().SetTimer(GlideDurationTimerHandle, this, &AFootballGameCharacter::OnGlideDurationExpired, GlideMaxDuration, false);
}
void AFootballGameCharacter::StopGlideInput()
{
    UE_LOG(LogFootballGameCharacter, Warning, TEXT("StopGlideInput CALLED!"));
    StopActualGlide(false);
}

void AFootballGameCharacter::StartShootChargeInput()
{
    UE_LOG(LogFootballGameCharacter, Warning, TEXT("StartShootChargeInput CALLED!"));
    UE_LOG(LogFootballGameCharacter, Warning, TEXT("Condition Check: State=%s"), *UEnum::GetValueAsString(CurrentMovementState));
   
    if (CurrentMovementState != EPlayerMovementState::Dashing)
    {
        bIsShootCharging = true;
        CurrentShootChargeTime = 0.0f;
        UE_LOG(LogFootballGameCharacter, Log, TEXT("Shoot Charging ACTUALLY Started."));
    }
    else { UE_LOG(LogFootballGameCharacter, Warning, TEXT("StartShootChargeInput: CANNOT SHOOT, State is Dashing.")); }
}
void AFootballGameCharacter::ReleaseShootInput()
{
    UE_LOG(LogFootballGameCharacter, Warning, TEXT("ReleaseShootInput CALLED!"));
    if (bIsShootCharging)
    {
        bIsShootCharging = false;
        if (CurrentShootChargeTime >= MinShootChargeTime)
        {
            float ChargeRatio = FMath::Clamp(CurrentShootChargeTime / MaxShootChargeTime, 0.0f, 1.0f);
            PerformShot(ChargeRatio); // PerformShot 自身也需要有定义
        }
        else
        {
            UE_LOG(LogFootballGameCharacter, Log, TEXT("Shoot charge too short. Shoot cancelled."));
        }
        CurrentShootChargeTime = 0.0f;
    }
    else { UE_LOG(LogFootballGameCharacter, Warning, TEXT("ReleaseShootInput: NOT bIsShootCharging.")); }
}
void AFootballGameCharacter::UpdateDashCharging(float DeltaTime)
{
    CurrentDashChargeTime += DeltaTime;
    CurrentDashChargeTime = FMath::Min(CurrentDashChargeTime, MaxDashChargeTime);

    if (InGameHUDInstanceNew)
    {
        float NormalizedCharge = (MaxDashChargeTime > 0) ? (CurrentDashChargeTime / MaxDashChargeTime) : 0.0f;
        if (UFootballInGameHUDNew* HUD = Cast<UFootballInGameHUDNew>(InGameHUDInstanceNew))
        {
            HUD->UpdateDashCharge(NormalizedCharge); 
        }
    }
}
// ... (StartActualGlide, UpdateGliding, StopActualGlide, OnGlideDurationExpired, UpdateShootCharging, PerformShot functions remain as you provided them)
void AFootballGameCharacter::StartActualGlide()
{
    if (!GetCharacterMovement() || bIsGliding) return;
    bIsGliding = true;
    CurrentGlideTime = 0.0f;
    GetCharacterMovement()->GravityScale = GlideGravityScale;
    GetCharacterMovement()->StopMovementImmediately();
    FVector HorizontalVelocity = DashReleaseVelocity; HorizontalVelocity.Z = 0;
    float InitialGlideZVelocity = GetCharacterMovement()->Velocity.Z > 0 ? GetCharacterMovement()->Velocity.Z * 0.1f : 0.f;
    GetCharacterMovement()->Velocity = (HorizontalVelocity * GlideHorizontalSpeedMultiplier) + FVector(0,0,InitialGlideZVelocity);
    UE_LOG(LogFootballGameCharacter, Log, TEXT("Glide Started. Gravity Scale: %f"), GlideGravityScale); 
    GetWorldTimerManager().SetTimer(GlideDurationTimerHandle, this, &AFootballGameCharacter::OnGlideDurationExpired, GlideMaxDuration, false);
}

void AFootballGameCharacter::UpdateGliding(float DeltaTime) 
{ 
    CurrentGlideTime += DeltaTime; 
    if (GetCharacterMovement() && bIsGliding)
    {
        // Example: Simple air friction (adjust factor)
        // GetCharacterMovement()->Velocity.X *= (1.0f - (0.1f * DeltaTime));
        // GetCharacterMovement()->Velocity.Y *= (1.0f - (0.1f * DeltaTime));
    }
}

void AFootballGameCharacter::StopActualGlide(bool bForceStop)
{
    if (bIsGliding && GetCharacterMovement())
    {
        bIsGliding = false;
        GetCharacterMovement()->GravityScale = OriginalGravityScale;
        GetWorldTimerManager().ClearTimer(GlideDurationTimerHandle);
        UE_LOG(LogFootballGameCharacter, Log, TEXT("Glide Stopped. Forced: %s. New Gravity: %f"),
            bForceStop ? TEXT("True") : TEXT("False"), OriginalGravityScale);
    }
}

void AFootballGameCharacter::OnGlideDurationExpired() 
{ 
    StopActualGlide(true); 
}

void AFootballGameCharacter::UpdateShootCharging(float DeltaTime)
{
    CurrentShootChargeTime += DeltaTime;
    CurrentShootChargeTime = FMath::Min(CurrentShootChargeTime, MaxShootChargeTime);
}

void AFootballGameCharacter::PerformShot(float ChargeRatio)
{
    if (!BallProjectileClass)
    {
        UE_LOG(LogFootballGameCharacter, Warning, TEXT("BallProjectileClass not set for shooting!"));
        return;
    }

    float BasePower = FMath::Lerp(MinShootPower, MaxShootPower, ChargeRatio);
    float FinalPower = BasePower;

    bool bIsMovingVeryFast = GetCharacterMovement() && (GetCharacterMovement()->Velocity.SizeSquared() > FMath::Square(MinSpeedForGlide * 0.7f));

    if (bIsMovingVeryFast || CurrentMovementState == EPlayerMovementState::Dashing) // Keep Dashing check for multiplier if Dashing state is brief
    {
        FinalPower *= ShootDashPowerMultiplier;
        UE_LOG(LogFootballGameCharacter, Log, TEXT("Dash Shot Power Multiplier Applied! Base: %f, Final: %f"), BasePower, FinalPower);
    }

    APlayerController* PC = Cast<APlayerController>(GetController()); 
    if (!PC) 
    {
        UE_LOG(LogFootballGameCharacter, Warning, TEXT("PerformShot: No PlayerController found."));
        return;
    }

    FVector CameraLocation_Unused; 
    FRotator CameraRotation;
    PC->GetPlayerViewPoint(CameraLocation_Unused, CameraRotation); 

    FVector SpawnLocation;
    FRotator SpawnRotation = CameraRotation; 

    if (GetMesh() && GetMesh()->DoesSocketExist(BallSpawnSocketName))
    {
        SpawnLocation = GetMesh()->GetSocketLocation(BallSpawnSocketName);
    }
    else
    {
        FVector ForwardVec = CameraRotation.Vector();
        float CapsuleHalfHeight = GetCapsuleComponent() ? GetCapsuleComponent()->GetScaledCapsuleHalfHeight() : 50.f;
        float CapsuleRadius = GetCapsuleComponent() ? GetCapsuleComponent()->GetScaledCapsuleRadius() : 30.f;
        SpawnLocation = GetActorLocation() + ForwardVec * (CapsuleRadius + 20.f) + FVector(0,0, CapsuleHalfHeight * 0.5f);
        UE_LOG(LogFootballGameCharacter, Warning, TEXT("BallSpawnSocketName '%s' not found or GetMesh() is null. Using fallback spawn location."), *BallSpawnSocketName.ToString());
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    ABall* SpawnedBall = GetWorld()->SpawnActor<ABall>(BallProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

    if (SpawnedBall && SpawnedBall->BallMesh && SpawnedBall->BallMesh->IsSimulatingPhysics())
    {
        FVector LaunchDirection = CameraRotation.Vector(); 
        SpawnedBall->BallMesh->AddImpulse(LaunchDirection * FinalPower, NAME_None, true);
        UE_LOG(LogFootballGameCharacter, Log, TEXT("Ball shot with effective power: %f, Direction: %s from location %s"), FinalPower, *LaunchDirection.ToString(), *SpawnLocation.ToString());
    }
    else if (SpawnedBall)
    {
        UE_LOG(LogFootballGameCharacter, Warning, TEXT("SpawnedBall ( %s ) has no BallMesh or is not simulating physics."), *SpawnedBall->GetName());
    }
    else
    {
        UE_LOG(LogFootballGameCharacter, Error, TEXT("Failed to spawn BallProjectile."));
    }
}


// Stubs for other actions
void AFootballGameCharacter::PassOrIntercept() { UE_LOG(LogFootballGameCharacter, Log, TEXT("Pass/Intercept Action (Q)")); }
void AFootballGameCharacter::Tackle() { UE_LOG(LogFootballGameCharacter, Log, TEXT("Tackle Action (E)")); }
void AFootballGameCharacter::ToggleLowGravityField() { UE_LOG(LogFootballGameCharacter, Log, TEXT("Toggle Low Gravity (F)")); }
void AFootballGameCharacter::ToggleAdrenalineMode() { UE_LOG(LogFootballGameCharacter, Log, TEXT("Toggle Adrenaline (Z)")); }