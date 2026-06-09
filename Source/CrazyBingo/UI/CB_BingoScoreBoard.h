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
	// 🌟 외부에서 점수와 빙고 수가 바뀔 때마다 전광판을 갱신해 줄 함수
	void UpdateScoreBoard(int32 Team1Score, int32 Team2Score, int32 Team1Bingo, int32 Team2Bingo);

	// 🌟 게임 시작 시 팀 이름을 초기화 세팅해 줄 함수
	void SetTeamNames(const FString& Team1Name, const FString& Team2Name);

protected:
	// 💡 UMG 디자이너에서 이 이름들과 정확히 일치하게 텍스트 블록을 만들어야 합니다.
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
