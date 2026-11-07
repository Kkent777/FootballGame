#include "FootballAIController.h" 
#include "FootballAICharacter.h" 
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
// #include "Perception/AISenseConfig_Hearing.h"
#include "Ball.h" 
#include "FootballGameCharacter.h" 


const FName AFootballAIController::BBKEY_TargetPlayer = TEXT("TargetPlayer");
const FName AFootballAIController::BBKEY_BallActor = TEXT("BallActor");
const FName AFootballAIController::BBKEY_TargetLocation = TEXT("TargetLocation");
const FName AFootballAIController::BBKEY_CanSeePlayer = TEXT("CanSeePlayer");
const FName AFootballAIController::BBKEY_HasBall = TEXT("HasBall");
const FName AFootballAIController::BBKEY_BallLocation = TEXT("BallLocation");


AFootballAIController::AFootballAIController()
{
    
    BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
 
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

  
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    if (SightConfig)
    {
        SightConfig->SightRadius = 2000.0f;
        SightConfig->LoseSightRadius = SightConfig->SightRadius + 500.0f;
        SightConfig->PeripheralVisionAngleDegrees = 90.0f;
        SightConfig->SetMaxAge(5.0f); 
        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = true; 
        SightConfig->DetectionByAffiliation.bDetectFriendlies = true; 

        AIPerceptionComponent->ConfigureSense(*SightConfig);
        AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
    }


}

void AFootballAIController::BeginPlay()
{
    Super::BeginPlay();
   
    if (AIPerceptionComponent)
    {
        AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AFootballAIController::OnTargetPerceptionUpdated);
    }
}

void AFootballAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    AFootballAICharacter* AIChar = Cast<AFootballAICharacter>(InPawn);
    if (AIChar && AIChar->BehaviorTreeAsset)
    {
        BlackboardComponent->InitializeBlackboard(*(AIChar->BehaviorTreeAsset->BlackboardAsset));
        RunBehaviorTree(AIChar->BehaviorTreeAsset);
        UE_LOG(LogTemp, Log, TEXT("AI Controller possessed %s and started behavior tree %s"), *InPawn->GetName(), *AIChar->BehaviorTreeAsset->GetName());
    }
    else if (!AIChar)
    {
        UE_LOG(LogTemp, Warning, TEXT("AI Controller possessed a non-FootballAICharacter pawn or pawn is null."));
    }
    else // AIChar valid but BehaviorTreeAsset is null
    {
        UE_LOG(LogTemp, Warning, TEXT("BehaviorTreeAsset is not set on %s."), *AIChar->GetName());
    }
}

void AFootballAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor || !BlackboardComponent) return;


    if (AFootballGameCharacter* PerceivedPlayer = Cast<AFootballGameCharacter>(Actor))
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            BlackboardComponent->SetValueAsObject(BBKEY_TargetPlayer, PerceivedPlayer);
            BlackboardComponent->SetValueAsVector(BBKEY_TargetLocation, PerceivedPlayer->GetActorLocation());
            BlackboardComponent->SetValueAsBool(BBKEY_CanSeePlayer, true);
            UE_LOG(LogTemp, Log, TEXT("AI Detected Player: %s"), *PerceivedPlayer->GetName());
        }
        else 
        {
    
            AActor* CurrentTargetPlayer = Cast<AActor>(BlackboardComponent->GetValueAsObject(BBKEY_TargetPlayer));
            if (CurrentTargetPlayer == PerceivedPlayer)
            {
                BlackboardComponent->SetValueAsBool(BBKEY_CanSeePlayer, false);
            
                UE_LOG(LogTemp, Log, TEXT("AI Lost Sight of Player: %s"), *PerceivedPlayer->GetName());
            }
        }
    }

    else if (ABall* PerceivedBall = Cast<ABall>(Actor))
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            BlackboardComponent->SetValueAsObject(BBKEY_BallActor, PerceivedBall);
            BlackboardComponent->SetValueAsVector(BBKEY_BallLocation, PerceivedBall->GetActorLocation());
            UE_LOG(LogTemp, Log, TEXT("AI Detected Ball: %s at %s"), *PerceivedBall->GetName(), *PerceivedBall->GetActorLocation().ToString());
        }
        else 
        {
            AActor* CurrentBall = Cast<AActor>(BlackboardComponent->GetValueAsObject(BBKEY_BallActor));
            if (CurrentBall == PerceivedBall)
            {
                BlackboardComponent->ClearValue(BBKEY_BallActor);
                BlackboardComponent->ClearValue(BBKEY_BallLocation);
                UE_LOG(LogTemp, Log, TEXT("AI Lost Sight of Ball: %s"), *PerceivedBall->GetName());
            }
        }
    }
}