// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CB_BingoScoreBoard.h"

#include "Components/TextBlock.h"

void UCB_BingoScoreBoard::UpdateScoreBoard(int32 Team1Score, int32 Team2Score, int32 Team1Bingo, int32 Team2Bingo)
{
	// 1팀 데이터 갱신
	if (Text_Team1Score)
	{
		// 숫자에 천 단위 콤보(,)를 자동으로 넣어주는 AsNumber를 쓰면 좋습니다.
		Text_Team1Score->SetText(FText::AsNumber(Team1Score));
	}
	if (Text_Team1BingoCount)
	{
		Text_Team1BingoCount->SetText(FText::FromString(FString::Printf(TEXT("빙고 %d줄"), Team1Bingo)));
	}

	// 2팀 데이터 갱신
	if (Text_Team2Score)
	{
		Text_Team2Score->SetText(FText::AsNumber(Team2Score));
	}
	if (Text_Team2BingoCount)
	{
		Text_Team2BingoCount->SetText(FText::FromString(FString::Printf(TEXT("빙고 %d줄"), Team2Bingo)));
	}
}

void UCB_BingoScoreBoard::SetTeamNames(const FString& Team1Name, const FString& Team2Name)
{
	if (Text_TeamName_1)
	{
		Text_TeamName_1->SetText(FText::FromString(Team1Name));
	}
	if (Text_TeamName_2)
	{
		Text_TeamName_2->SetText(FText::FromString(Team2Name));
	}
}
