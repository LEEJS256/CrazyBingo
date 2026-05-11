// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/CB_GameMode.h"

#include "Kismet/GameplayStatics.h"
#include "UI/CB_BingoBoard.h"

ACB_GameMode::ACB_GameMode()
{
}

void ACB_GameMode::BeginPlay()
{
	Super::BeginPlay();
	StartGame();
}

void ACB_GameMode::StartGame()
{
	if (!BingoBoardClass)
		return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	BingoBoard = CreateWidget<UCB_BingoBoard>(PC, BingoBoardClass);
	if (BingoBoard)
	{
		BingoBoard->AddToViewport();
		PC->SetShowMouseCursor(true);
	}
}

void ACB_GameMode::ResetGame()
{
	if (BingoBoard)
	{
		BingoBoard->InitBoard();
	}
}
