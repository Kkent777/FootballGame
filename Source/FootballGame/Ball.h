#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"  
#include "Ball.generated.h"

UCLASS()
class FOOTBALLGAME_API ABall : public AActor
{
	GENERATED_BODY()

public:
	ABall();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, Category="AI")
	UAIPerceptionStimuliSourceComponent* StimuliSource;
public:
	// UPROPERTY for the ball's mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BallMesh;

	// Game Design specific: To be used by PhysicsManager later for Low-Gravity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball Physics")
	bool bAffectedByLowGravity;

	// Game Design specific: Will store velocity for momentum calculations
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ball Physics")
	FVector CurrentVelocity; // GameBall class in your design has 'velocity' [37]

	// Game Design specific: isPossessed (though direct possession might be player-centric)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ball Gameplay")
	bool bIsPossessed; // GameBall class in your design [37]

public:
	// Function to apply kick force, to be called by player/AI
	// GameBall class in your design has ApplyKickForce() [37]
	UFUNCTION(BlueprintCallable, Category = "Ball Gameplay")
	void ApplyKickForce(FVector ForceDirection, float ForceStrength);

	// Function to check possession status
	// GameBall class in your design has CheckPossession() [37]
	UFUNCTION(BlueprintPure, Category = "Ball Gameplay")
	bool IsCurrentlyPossessed() const { return bIsPossessed; }

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};