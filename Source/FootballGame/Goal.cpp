#include "Goal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Ball.h" // Include your Ball class
#include "Kismet/GameplayStatics.h" // For SpawnActor if still using W06 respawn

AGoal::AGoal()
{
    PrimaryActorTick.bCanEverTick = false; // Goal doesn't need to tick usually [221]

    GoalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GoalMesh"));
    RootComponent = GoalMesh;

    GoalCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("GoalCollider"));
    GoalCollider->SetupAttachment(RootComponent);
    GoalCollider->SetCollisionProfileName(TEXT("Trigger")); // Set to trigger
    GoalCollider->SetBoxExtent(FVector(100.f, 400.f, 200.f)); // Adjust as needed
    GoalCollider->SetRelativeLocation(FVector(0.f, 0.f, 200.f)); // Adjust

    TeamID = 0; // Default Team ID
}

void AGoal::BeginPlay()
{
    Super::BeginPlay();
    GoalCollider->OnComponentBeginOverlap.AddDynamic(this, &AGoal::OnOverlapBegin); 
}

void AGoal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ABall* ScoredBall = Cast<ABall>(OtherActor);
    if (ScoredBall && OtherActor != this) // [224]
    {
        // Prevent multiple scoring events from the same ball quickly
        if (ScoredBall->GetVelocity().SizeSquared() > 0.1f) // Only score if ball is moving
        {
            UE_LOG(LogTemp, Warning, TEXT("Goal Scored by Team %d!"), TeamID == 0 ? 1 : 0); // Example scoring logic
            // Later, this will interact with a GameState/UIManager to update score.
            // For now, using W06 logic to destroy and respawn:
            // Make sure BallClassToSpawn is set in BP_Goal
            if (BallClassToSpawn)
            {
                FVector SpawnLoc = BallSpawnLocation;
                // If BallSpawnLocation is relative to the goal, transform it to world space:
                // SpawnLoc = GetActorTransform().TransformPosition(BallSpawnLocation);

                // Or find the center of the arena to respawn the ball
                // For now, let's assume BallSpawnLocation is a world coordinate set in Blueprint
                // Or, if BallSpawnLocation is meant to be an offset FROM the goal, adjust.
                // The W06 lab has the BallSpawn as a property of the Goal itself.
                // We'll use the simple spawn at a pre-defined point for now.

                ScoredBall->Destroy(); // [225]
                FActorSpawnParameters SpawnParams;
                GetWorld()->SpawnActor<ABall>(BallClassToSpawn, BallSpawnLocation, FRotator::ZeroRotator, SpawnParams); // [225]
            }
        }
    }
}