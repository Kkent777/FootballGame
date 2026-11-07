#include "FootballInGameHUDNew.h" 
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UFootballInGameHUDNew::UpdateDashCharge(float NormalizedChargeAmount)
{
	if (DashChargeBar)
	{
		DashChargeBar->SetPercent(NormalizedChargeAmount);
	}

	OnDashChargeUpdated(NormalizedChargeAmount);
}

void UFootballInGameHUDNew::SetDashChargeVisibility(bool bIsVisible)
{
	if (DashChargeBar)
	{
		DashChargeBar->SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	OnDashChargeVisibilityChanged(bIsVisible); 
}



void UFootballInGameHUDNew::SetAdrenalineStatus(const FString& StatusMessage)
{
	if (AdrenalineStatusText)
	{
		AdrenalineStatusText->SetText(FText::FromString(StatusMessage));
	}
	OnAdrenalineStatusChanged(StatusMessage);
}