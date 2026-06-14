// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/CB_BingoBoard.h"
#include "Components/TextBlock.h"
#include "WebBrowser.h"
#include "UI/CB_HostPanel.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"


void UCB_HostPanel::SetCurrentQuestionInfo(int32 CellIndex, const FCB_DataTable_Question& QuestionData)
{
	CachedCellIndex = CellIndex;

	// 문제 텍스트 반영
	if (IsValid(Text_CurrentQuestion))
	{
		Text_CurrentQuestion->SetText(FText::FromString(QuestionData.QuestionText));
	}

	// 해설 텍스트 반영
	if (IsValid(Text_Explanation))
	{
		Text_Explanation->SetText(FText::FromString(QuestionData.Explanation));
	}

	// 유튜브 링크 주소 브라우저에 LoadURL 시키기
	if (IsValid(WebBrowser_Youtube))
	{
		// 데이터 테이블의 멤버 변수명이 'YoutubeURL'인지 확인해 주세요!

		if (QuestionData.YoutubeURL.IsEmpty())
		{
			// 잔상이 남지 않도록 주소가 없을 땐 비워주기
			WebBrowser_Youtube->LoadURL(TEXT("about:blank"));
			return;
		}

		FString TargetURL = QuestionData.YoutubeURL;

		// QuestionBoard와 동일한 유튜브 임베드 필터링 적용
		if (TargetURL.Contains(TEXT("watch?v=")))
		{
			TargetURL = TargetURL.Replace(TEXT("watch?v="), TEXT("embed/"));
			TargetURL += TEXT("?autoplay=1");
		}

		WebBrowser_Youtube->LoadURL(TargetURL);
	}
}

void UCB_HostPanel::NativeConstruct()
{
	Super::NativeConstruct();
	if (Btn_TeamA_Win) Btn_TeamA_Win->OnClicked.AddDynamic(this, &UCB_HostPanel::OnTeamAWinClicked);
	if (Btn_TeamB_Win) Btn_TeamB_Win->OnClicked.AddDynamic(this, &UCB_HostPanel::OnTeamBWinClicked);
	if (Btn_WrongAnswer) Btn_WrongAnswer->OnClicked.AddDynamic(this, &UCB_HostPanel::OnWrongAnswerClicked);
}

void UCB_HostPanel::OnTeamAWinClicked()
{
	if (IsValid(TargetBingoBoard))
	{
	
		TargetBingoBoard->SetCellOwnerByHost(1);
		UE_LOG(LogTemp, Log, TEXT("[호스트패널] A팀 정답 처리를 독립 무전으로 보드에 통보했습니다."));
	}
}

void UCB_HostPanel::OnTeamBWinClicked()
{
	if (IsValid(TargetBingoBoard))
	{
		TargetBingoBoard->SetCellOwnerByHost(2);
	}
}

void UCB_HostPanel::OnWrongAnswerClicked()
{
	if (IsValid(TargetBingoBoard))
	{
		// A팀 점령 (1번 번호 넘기기)
		TargetBingoBoard->SetCellOwnerByHost(0);
		UE_LOG(LogTemp, Log, TEXT("[호스트패널] A팀 정답 처리를 독립 무전으로 보드에 통보했습니다."));
	}
}
