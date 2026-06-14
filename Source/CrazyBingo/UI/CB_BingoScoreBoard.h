// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CB_BingoScoreBoard.generated.h"

/**
 * 
 */
class UTextBlock;

UCLASS()
class CRAZYBINGO_API UCB_BingoScoreBoard : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateScoreBoard(int32 Team1Score, int32 Team2Score, int32 Team1Bingo, int32 Team2Bingo);
	
	void SetTeamNames(const FString& Team1Name, const FString& Team2Name);

	void RefreshTeamColor();
	void RefreshScoreUI();
	void RefreshBingoUI(int32 BingoNum , bool TeamA = true);
protected:

	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_BackToMenu;
	
	UFUNCTION()
	void OnBackToMenuClicked();
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_TeamName_1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_TeamName_2;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Team1Score;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Team2Score;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Team1BingoCount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Team2BingoCount;
};
