#include "FootballMainMenuGameMode.h" 
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

AFootballMainMenuGameMode::AFootballMainMenuGameMode()
{

	DefaultPawnClass = nullptr;
	PlayerControllerClass = APlayerController::StaticClass();
}

void AFootballMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (MainMenuWidgetClass)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (PlayerController)
		{
			UUserWidget* MainMenuWidgetInstance = CreateWidget<UUserWidget>(PlayerController, MainMenuWidgetClass);
			if (MainMenuWidgetInstance)
			{
				MainMenuWidgetInstance->AddToViewport();

			
				FInputModeUIOnly InputModeData;
				InputModeData.SetWidgetToFocus(MainMenuWidgetInstance->TakeWidget());
				InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				PlayerController->SetInputMode(InputModeData);
				PlayerController->bShowMouseCursor = true;
				UE_LOG(LogTemp, Log, TEXT("Main Menu Widget Added to Viewport and Input Mode Set."));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to create MainMenuWidgetInstance!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get PlayerController in MainMenuGameMode!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MainMenuWidgetClass is not set in AFootballMainMenuGameMode!"));
	}
}