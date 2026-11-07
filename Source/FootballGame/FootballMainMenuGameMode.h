#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "FootballMainMenuGameMode.generated.h"

UCLASS()
class FOOTBALLGAME_API AFootballMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFootballMainMenuGameMode();

protected:
	virtual void BeginPlay() override;

	// UPROPERTY 用于在蓝图中指定主菜单的Widget类
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> MainMenuWidgetClass;
};