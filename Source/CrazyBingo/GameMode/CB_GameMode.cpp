// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/CB_GameMode.h"

#include "GameState/CB_GameState.h"
#include "Kismet/GameplayStatics.h"
#include "UI/CB_BingoBoard.h"

ACB_GameMode::ACB_GameMode()
{
	GameStateClass = ACB_GameState::StaticClass();
}

void ACB_GameMode::BeginPlay()
{
	Super::BeginPlay();
	StartGame();
}

void ACB_GameMode::StartGame()
{

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		// 마우스 커서 보이게
		PC->SetShowMouseCursor(true);
        
		// 마우스로 카메라 이동 막기
		PC->SetIgnoreLookInput(true);
		PC->SetIgnoreMoveInput(true);

		// UI 인풋 모드로 변경
		FInputModeUIOnly InputMode;
		PC->SetInputMode(InputMode);
	}
	
	if (!BingoBoardClass)
		return;
	

	BingoBoard = CreateWidget<UCB_BingoBoard>(PC, BingoBoardClass);
	if (BingoBoard)
	{
		BingoBoard->AddToViewport();
		PC->SetShowMouseCursor(true);
	}
}

void ACB_GameMode::ResetGame()
{
	// if (BingoBoard)
	// {
	// 	BingoBoard->InitBoard();
	// }
}
