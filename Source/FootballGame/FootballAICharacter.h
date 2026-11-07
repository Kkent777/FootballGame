
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
// #include "FootballGameCharacter.h" 
#include "FootballAICharacter.generated.h"

class UBehaviorTree; 

UCLASS()
class FOOTBALLGAME_API AFootballAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFootballAICharacter();

	
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;


	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior")
	// float MaxAISpeed;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;


	// UFUNCTION(BlueprintCallable, Category = "AI Actions")
	// void AIShoot(FVector TargetLocation);

	// UFUNCTION(BlueprintCallable, Category = "AI Actions")
	// void AIAttemptTackle(AActor* TargetToTackle);
};