#include "FootballInGameHUD.h" // 根据你的实际路径调整
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UFootballInGameHUD::UpdateDashCharge(float NormalizedChargeAmount)
{
	if (DashChargeBar)
	{
		DashChargeBar->SetPercent(NormalizedChargeAmount);
	}
	// 调用蓝图事件，如果蓝图中有更复杂的视觉逻辑
	OnDashChargeUpdated(NormalizedChargeAmount);
}

void UFootballInGameHUD::SetDashCooldown(bool bIsOnCooldown, float NormalizedCooldownProgress)
{
	if (DashCooldownBar)
	{
		DashCooldownBar->SetVisibility(bIsOnCooldown ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		if (bIsOnCooldown)
		{
			DashCooldownBar->SetPercent(NormalizedCooldownProgress);
		}
	}
	OnDashCooldownUpdated(bIsOnCooldown, NormalizedCooldownProgress);
}

void UFootballInGameHUD::SetAdrenalineStatus(const FString& StatusMessage)
{
	if (AdrenalineStatusText)
	{
		AdrenalineStatusText->SetText(FText::FromString(StatusMessage));
	}
	OnAdrenalineStatusChanged(StatusMessage);
}