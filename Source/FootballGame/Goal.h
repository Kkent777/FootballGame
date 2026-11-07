#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Goal.generated.h"

// Forward declare Ball class to avoid circular dependencies if Goal needs to know about Ball type specifically
class ABall;
class UBoxComponent; // Forward declare UBoxComponent
class UStaticMeshComponent; // Forward declare UStaticMeshComponent

UCLASS()
class FOOTBALLGAME_API AGoal : public AActor
{
	GENERATED_BODY()

public:
	AGoal();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* GoalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* GoalCollider; // This is the trigger volume [214]

	// W06: To respawn the ball. We'll adapt this later for scoring.
	UPROPERTY(EditAnywhere, Category = "Goal Settings")
	TSubclassOf<ABall> BallClassToSpawn; // [214]

	UPROPERTY(EditAnywhere, Category = "Goal Settings", Meta = (MakeEditWidget = true))
	FVector BallSpawnLocation; // [215]

	// Team ID or identifier for this goal
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal Settings")
	int32 TeamID; // Useful for knowing which team scored

public:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult); // [215]
};