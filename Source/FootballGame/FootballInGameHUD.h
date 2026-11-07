#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FootballInGameHUD.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class FOOTBALLGAME_API UFootballInGameHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateDashCharge(float NormalizedChargeAmount); // 0.0 to 1.0

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetDashCooldown(bool bIsOnCooldown, float NormalizedCooldownProgress); // Progress 0.0 (ready) to 1.0 (just started cd)

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetAdrenalineStatus(const FString& StatusMessage); // e.g., "READY", "ACTIVE: 2.5s", "CD: 4.0s"


protected:


	UPROPERTY(BlueprintReadOnly, meta = (BindWidget)) // BlueprintReadOnly so BP can read it if needed
	TObjectPtr<UProgressBar> DashChargeBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> DashCooldownBar; // Can use a progress bar or text for cooldown

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> AdrenalineStatusText;


	UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
	void OnDashChargeUpdated(float NormalizedCharge);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
	void OnDashCooldownUpdated(bool bIsOnCooldown, float NormalizedCooldownProgress);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
	void OnAdrenalineStatusChanged(const FString& Status);
};