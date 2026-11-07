#include "Ball.h"
#include "Components/StaticMeshComponent.h" // Make sure this is included
#include "Perception/AISense_Sight.h"

ABall::ABall()
{
	PrimaryActorTick.bCanEverTick = true; // Enable tick for updating CurrentVelocity if needed
	
	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
	
	StimuliSource->bAutoRegister = true;
	
	StimuliSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
	
	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	RootComponent = BallMesh;

	BallMesh->SetSimulatePhysics(true); // Enable physics simulation [204]
	BallMesh->SetCollisionProfileName(TEXT("PhysicsActor")); // Ensure it collides

	// Game Design specific defaults
	bAffectedByLowGravity = false;
	bIsPossessed = false;
	CurrentVelocity = FVector::ZeroVector;
}

void ABall::BeginPlay()
{
	Super::BeginPlay();
	
	// Optional: Set initial physics properties like mass, restitution if needed
	// BallMesh->SetMassScale(NAME_None, 0.5f); // Example
	// BallMesh->SetLinearDamping(0.1f);
	// BallMesh->SetAngularDamping(0.1f);
}

void ABall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (BallMesh && BallMesh->IsSimulatingPhysics())
	{
		CurrentVelocity = BallMesh->GetPhysicsLinearVelocity();
	}
}

void ABall::ApplyKickForce(FVector ForceDirection, float ForceStrength)
{
	if (BallMesh && BallMesh->IsSimulatingPhysics())
	{
		// Normalize direction to ensure consistent force application
		ForceDirection.Normalize();
		BallMesh->AddImpulse(ForceDirection * ForceStrength, NAME_None, true);
		bIsPossessed = false; // Kicking typically means losing possession
	}
}