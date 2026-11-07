// Copyright Epic Games, Inc. All Rights Reserved.

#include "FootballGameGameMode.h"
#include "FootballGameCharacter.h" 
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerController.h" 
#include "Kismet/GameplayStatics.h"       

AFootballGameGameMode::AFootballGameGameMode()
{
	// set default pawn class to our Blueprinted character

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != nullptr) // 
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
		UE_LOG(LogTemp, Log, TEXT("AFootballGameGameMode: DefaultPawnClass set to %s"), *DefaultPawnClass->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AFootballGameGameMode: Failed to find PlayerPawnBPClass at path /Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter. Character will not spawn correctly."));
	}
}

void AFootballGameGameMode::BeginPlay()
{
	Super::BeginPlay();


	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController)
	{

		FInputModeGameOnly InputModeData;
		PlayerController->SetInputMode(InputModeData);

	
		PlayerController->bShowMouseCursor = false;

		UE_LOG(LogTemp, Warning, TEXT("AFootballGameGameMode: Input mode set to GameOnly and mouse cursor hidden. Player can now control character."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AFootballGameGameMode: PlayerController not found in BeginPlay. Input mode NOT set."));
	}
}