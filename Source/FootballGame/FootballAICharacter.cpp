#include "FootballAICharacter.h" 
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h" 
#include "Components/CapsuleComponent.h"

AFootballAICharacter::AFootballAICharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f); 
    GetCharacterMovement()->MaxWalkSpeed = 500.f; 


    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;


    BehaviorTreeAsset = nullptr;
}

void AFootballAICharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AFootballAICharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}