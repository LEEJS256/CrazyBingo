// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CB_ResultPopup.h"

#include "CB_BingoBoard.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UCB_ResultPopup::SetupResultData(uint8 WinningTeamID, int32 ScoreA, int32 ScoreB)
{
	if (IsValid(Text_WinnerBanner))
	{
		FString WinnerStr = (WinningTeamID == 1) ? TEXT("🔴 A팀 최종 승리!") : TEXT("🔵 B팀 최종 승리!");
		Text_WinnerBanner->SetText(FText::FromString(WinnerStr));
	}

	// 2. 최종 점수 문자열 가공 및 반영
	if (IsValid(Text_FinalScores))
	{
		FString ScoreStr = FString::Printf(TEXT("A팀: %d점  /  B팀: %d점"), ScoreA, ScoreB);
		Text_FinalScores->SetText(FText::FromString(ScoreStr));
	}
}

void UCB_ResultPopup::NativeConstruct()
{
	Super::NativeConstruct();

	// 🌟 확인 버튼 클릭 이벤트 연동
	if (IsValid(Btn_GoToMainMenu))
	{
		Btn_GoToMainMenu->OnClicked.AddDynamic(this, &UCB_ResultPopup::OnGoToMainMenuClicked);
	}
}

void UCB_ResultPopup::OnGoToMainMenuClicked()
{
	if (!IsValid(OwnerBoard))
	{
		UE_LOG(LogTemp, Error, TEXT("[오류] OwnerBoard 포인터가 비어있어 메인메뉴로 돌아갈 수 없습니다."));
		return;
	}

	// 1. 어제 보드판에 구현해 둔 메인 메뉴 복귀 함수를 트리거합니다!
	OwnerBoard->ReturnToMainMenu();

	// 2. 처리가 완전히 끝났으므로 팝업 자신을 화면(Viewport)에서 깔끔하게 제거하고 파괴합니다.
	RemoveFromParent();
	
	UE_LOG(LogTemp, Log, TEXT("[결과창 팝업] 메인 로비 복귀 완료 및 팝업 제거 성공."));
}
