// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CB_BingoScoreBoard.h"

#include "CB_BingoBoard.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameInstance/CB_GameInstance.h"

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

void UCB_BingoScoreBoard::RefreshTeamColor()
{
	
	UCB_GameInstance* GI = Cast<UCB_GameInstance>(GetGameInstance());

	Text_TeamName_1->SetColorAndOpacity(GI->GetTeamColor(true));
	
	Text_TeamName_2->SetColorAndOpacity(GI->GetTeamColor(false));
}


void UCB_BingoScoreBoard::RefreshScoreUI()
{
	UCB_GameInstance* GI = Cast<UCB_GameInstance>(GetGameInstance());

	if (IsValid(Text_Team1Score))
		Text_Team1Score->SetText(FText::AsNumber(GI->GetScore(true)));
	if (IsValid(Text_Team2Score))
		Text_Team2Score->SetText(FText::AsNumber(GI->GetScore(false)));
}

void UCB_BingoScoreBoard::RefreshBingoUI(int32 BingoNum, bool TeamA)
{
	if (TeamA)
	{
		Text_Team1BingoCount->SetText(FText::FromString(FString::Printf(TEXT("빙고 %d줄"), BingoNum)));
	}

	else
		Text_Team2BingoCount->SetText(FText::FromString(FString::Printf(TEXT("빙고 %d줄"), BingoNum)));
}

void UCB_BingoScoreBoard::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(Btn_BackToMenu))
	{
		Btn_BackToMenu->OnClicked.AddDynamic(this, &UCB_BingoScoreBoard::OnBackToMenuClicked);
	}
	
}

void UCB_BingoScoreBoard::OnBackToMenuClicked()
{
	UCB_BingoBoard* OwnerBoard = Cast<UCB_BingoBoard>(GetOuter());
	

	if (!IsValid(OwnerBoard))
	{

		OwnerBoard = Cast<UCB_BingoBoard>(GetParent()->GetOuter());
	}

	if (IsValid(OwnerBoard))
	{
		OwnerBoard->ReturnToMainMenu(); 
		UE_LOG(LogTemp, Log, TEXT("[점수판] 메인 메뉴 복귀 명령 전송 완료!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[오류] 부모 보드판(OwnerBoard)을 찾을 수 없어 탈출할 수 없습니다."));
	}
}
