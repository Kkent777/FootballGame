#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FootballInGameHUDNew.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class FOOTBALLGAME_API UFootballInGameHUDNew : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateDashCharge(float NormalizedChargeAmount); // 0.0 to 1.0

	
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetAdrenalineStatus(const FString& StatusMessage); // e.g., "READY", "ACTIVE: 2.5s", "CD: 4.0s"

	// ...


	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetDashChargeVisibility(bool bIsVisible);
	// ...
protected:
	// ...
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
	void OnDashChargeVisibilityChanged(bool bIsVisible);
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget)) // BlueprintReadOnly so BP can read it if needed
	TObjectPtr<UProgressBar> DashChargeBar;

	

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> AdrenalineStatusText;

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
	void OnDashChargeUpdated(float NormalizedCharge);

	

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
	void OnAdrenalineStatusChanged(const FString& Status);
};