#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h" 
#include "FootballAIController.generated.h"

class UBehaviorTree;
class UBlackboardComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;


UCLASS()
class FOOTBALLGAME_API AFootballAIController : public AAIController
{
    GENERATED_BODY()

public:
    AFootballAIController();

protected:
    virtual void OnPossess(APawn* InPawn) override;
    virtual void BeginPlay() override; // 

private:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBlackboardComponent> BlackboardComponent;

   
    // GetBehaviorTreeComponent()
    // UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    // TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponentInstance; // 

   
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;


    UPROPERTY(VisibleDefaultsOnly, Category = "AI") // VisibleDefaultsOnly 
    TObjectPtr<UAISenseConfig_Sight> SightConfig;

  
    // UPROPERTY(VisibleDefaultsOnly, Category = "AI")
    // TObjectPtr<UAISenseConfig_Hearing> HearingConfig;

 
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

public:
 
    static const FName BBKEY_TargetPlayer;
    static const FName BBKEY_BallActor;
    static const FName BBKEY_TargetLocation;
    static const FName BBKEY_CanSeePlayer;
    static const FName BBKEY_HasBall;     
    static const FName BBKEY_BallLocation;
};