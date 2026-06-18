// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CB_HostPanel.h"
#include "UI/CB_BingoBoard.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"


void UCB_HostPanel::SetCurrentQuestionInfo(int32 CellIndex, const FCB_DataTable_Question& QuestionData)
{
	CachedCellIndex = CellIndex;

	// 정답
	if (IsValid(Text_CurrentQuestion))
	{
		Text_CurrentQuestion->SetText(FText::FromString(QuestionData.Hint));
	}

	// 해설 텍스트 반영
	if (IsValid(Text_Explanation))
	{
		Text_Explanation->SetText(FText::FromString(QuestionData.Answer));
	}

	FString TargetURL = QuestionData.YoutubeURL.TrimStartAndEnd();
	FString VideoID = TEXT("");
	FString TimeParam = TEXT("");

	// 2. 타임스탬프 추출
	if (TargetURL.Contains(TEXT("t=")))
	{
		int32 TimeIndex = TargetURL.Find(TEXT("t="));
		if (TimeIndex != INDEX_NONE)
		{
			FString SubTime = TargetURL.RightChop(TimeIndex);
			int32 AmpIndex = SubTime.Find(TEXT("&"));
			TimeParam = (AmpIndex != INDEX_NONE) ? SubTime.Left(AmpIndex) : SubTime;
		}
	}

	// 3. 영상 ID 정밀 추출
	if (TargetURL.Contains(TEXT("watch?v=")))
	{
		int32 ParamIndex = TargetURL.Find(TEXT("watch?v="));
		if (ParamIndex != INDEX_NONE) VideoID = TargetURL.Mid(ParamIndex + 8, 11);
	}
	else if (TargetURL.Contains(TEXT("youtu.be/")))
	{
		int32 SlashIndex = TargetURL.Find(TEXT("youtu.be/"));
		if (SlashIndex != INDEX_NONE) VideoID = TargetURL.Mid(SlashIndex + 9, 11);
	}

	// 4. 주소 조립 및 윈도우 외부 브라우저 실행
	// 🌟 [핵심 교정] VideoID가 정상적으로 파싱되었을 때'만' 중괄호 내부에서 실행되도록 격리합니다.
	if (!VideoID.IsEmpty())
	{
		TargetURL = FString::Printf(TEXT("https://www.youtube.com/v/%s?autoplay=1"), *VideoID);
		if (!TimeParam.IsEmpty())
		{
			TargetURL += FString::Printf(TEXT("&%s"), *TimeParam);
		}
       
		// 진짜 띄워야 할 유튜브 주소가 완성되었을 때만 창을 켭니다.
		FPlatformProcess::LaunchURL(*TargetURL, nullptr, nullptr);
		UE_LOG(LogTemp, Log, TEXT("[호스트패널] 유튜브 URL을 실행했습니다: %s"), *TargetURL);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[호스트패널] 입력된 주소에서 유효한 유튜브 VideoID를 찾을 수 없어 브라우저를 띄우지 않습니다."));
	}

	
}

void UCB_HostPanel::NativeConstruct()
{
	Super::NativeConstruct();
	if (Btn_TeamA_Win) Btn_TeamA_Win->OnClicked.AddDynamic(this, &UCB_HostPanel::OnTeamAWinClicked);
	if (Btn_TeamB_Win) Btn_TeamB_Win->OnClicked.AddDynamic(this, &UCB_HostPanel::OnTeamBWinClicked);
	if (Btn_WrongAnswer) Btn_WrongAnswer->OnClicked.AddDynamic(this, &UCB_HostPanel::OnWrongAnswerClicked);
	if (Btn_Reroll) Btn_Reroll->OnClicked.AddDynamic(this, &UCB_HostPanel::RerollCellQuestion);
}

void UCB_HostPanel::RerollCellQuestion()
{
	if (CachedCellIndex == INDEX_NONE || CachedCellIndex < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[호스트패널] 현재 선택된 빙고 칸이 없어 문제를 리롤할 수 없습니다!"));
		return;
	}

	// 2. 연결된 빙고 보드가 유효한지 체크 후 리롤 명령 전달
	if (IsValid(TargetBingoBoard))
	{
		// 이전에 설계했던 보드판의 리롤 함수를 호출하면서 현재 칸 인덱스를 던집니다.
		TargetBingoBoard->RerollCellQuestion(CachedCellIndex);
		
		UE_LOG(LogTemp, Log, TEXT("[호스트패널] %d번 칸에 대한 문제 리롤을 보드판에 요청했습니다."), CachedCellIndex + 1);
	}
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
		TargetBingoBoard->SetCellOwnerByHost(3);
		UE_LOG(LogTemp, Log, TEXT("[호스트패널] A팀 정답 처리를 독립 무전으로 보드에 통보했습니다."));
	}
}
