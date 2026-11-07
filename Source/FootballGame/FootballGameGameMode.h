#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FootballGameGameMode.generated.h"

UCLASS()
class FOOTBALLGAME_API AFootballGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFootballGameGameMode();
	virtual void BeginPlay() override; // 添加BeginPlay声明
};