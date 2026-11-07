#pragma once
#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindPatrolPoint.generated.h"

UCLASS()
class FOOTBALLGAME_API UBTTask_FindPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_FindPatrolPoint();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetLocationKey; 

	UPROPERTY(EditAnywhere, Category = "AI")
	float PatrolRadius;
};