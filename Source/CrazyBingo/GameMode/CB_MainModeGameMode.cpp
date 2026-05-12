// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/CB_MainModeGameMode.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/CB_MainMenu.h"

void ACB_MainModeGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		PC->SetShowMouseCursor(true);
		FInputModeUIOnly InputMode;
		PC->SetInputMode(InputMode);
	}

	if (MainMenuClass)
	{
		UCB_MainMenu* MainMenu = CreateWidget<UCB_MainMenu>(
			UGameplayStatics::GetPlayerController(this, 0), MainMenuClass);
		if (MainMenu) MainMenu->AddToViewport();
	}
}
