// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CB_BingoBoard.h"

#include "CB_BingoCell.h"
#include "CB_BingoScoreBoard.h"
#include "CB_HostPanel.h"
#include "CB_MainMenu.h"
#include "CB_QuestionBoard.h"
#include "CB_ResultPopup.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "GameInstance/CB_GameInstance.h"
#include "Kismet/GameplayStatics.h"

void UCB_BingoBoard::ReturnToMainMenu()
{
	UCB_MainMenu* MasterMenu = Cast<UCB_MainMenu>(GetOuter()->GetOuter());
	if (IsValid(MasterMenu))
	{

		MasterMenu->SwitchToPage(0); 
	}
}

void UCB_BingoBoard::HandleGameOver(uint8 WinningTeamID)
{
	if (!IsValid(ResultPopupClass)) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(PC)) return;

	// 🌟 [교정] 우리가 만든 C++ 결과창 클래스로 위젯을 생성하고 형변환합니다!
	UCB_ResultPopup* ResultWidget = CreateWidget<UCB_ResultPopup>(PC, ResultPopupClass);
	if (!IsValid(ResultWidget)) return;

	UCB_GameInstance* GI = Cast<UCB_GameInstance>(GetGameInstance());
	int32 ScoreA = IsValid(GI) ? GI->TeamAScore : 0;
	int32 ScoreB = IsValid(GI) ? GI->TeamBScore : 0;

	// 🌟 양방향 링크 통로 개통 및 데이터 세팅!
	ResultWidget->OwnerBoard = this; // 결과창에게 나(보드판)의 주소를 가리키게 함
	ResultWidget->SetupResultData(WinningTeamID, ScoreA, ScoreB); // 데이터 주입

	// 화면에 장착
	ResultWidget->AddToViewport(99);

	// 진행자용 호스트 패널 닫기
	if (IsValid(TargetHostPanel))
	{
		TargetHostPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCB_BingoBoard::OnCellSelected(int32 SelectedIndex)
{
	if (!Cells.IsValidIndex(SelectedIndex) || !QuestionBoard) return;

	CurrentOpenedCellIndex = SelectedIndex;

	if (CurrentRoundQuestions.IsValidIndex(SelectedIndex))
	{
		FCB_DataTable_Question TargetQuestion = CurrentRoundQuestions[SelectedIndex];
		
		QuestionBoard->SetQuestion(TargetQuestion);

		UE_LOG(LogTemp, Log, TEXT("[보드] QuestionBoard 연동 완료: %s"), *TargetQuestion.QuestionText);

		if (IsValid(TargetHostPanel))
		{
			TargetHostPanel->SetCurrentQuestionInfo(SelectedIndex, TargetQuestion);
		}
	}


	// 4. 셀 상태가 바뀌었으니 빙고가 완성되었는지 실시간 체크
	// CheckBingo();
}

void UCB_BingoBoard::InitBoard(int32 InBoardSize)
{
	if (!BingoGrid || !BingoCellClass) return;


	BoardSize = InBoardSize;
	int32 TotalCellCount = BoardSize * BoardSize;

	BingoGrid->ClearChildren();
	Cells.Empty();
	CurrentRoundQuestions.Empty();

	UCB_GameInstance* GI = Cast<UCB_GameInstance>(GetGameInstance());
	if (GI)
	{
		// 보드 크기에 딱 맞게 (예: 5x5면 25개) 랜덤 셔플된 문제를 뽑아옵니다.
		CurrentRoundQuestions = GI->GetRandomQuestions(TotalCellCount);
		GI->ResetScores();
	}

	if (IsValid(BingoScoreBoard) && IsValid(GI))
	{
		BingoScoreBoard->RefreshScoreUI();
		BingoScoreBoard->RefreshTeamColor();
	}

	TArray<int32> Numbers;
	for (int32 i = 1; i <= TotalCellCount; i++) Numbers.Add(i);

	for (int32 i = Numbers.Num() - 1; i > 0; i--)
	{
		int32 j = FMath::RandRange(0, i);
		Numbers.Swap(i, j);
	}

	// 4. 셀 생성 및 데이터 주입 루프
	for (int32 i = 0; i < TotalCellCount; i++)
	{
		UCB_BingoCell* Cell = CreateWidget<UCB_BingoCell>(this, BingoCellClass);
		if (!Cell) continue;

		// 🌟 뽑아온 이번 라운드 문제 리스트에서 카테고리 긁어오기 (안전성 체크 포함)
		FString TargetCategory = TEXT("일반");
		if (CurrentRoundQuestions.IsValidIndex(i))
		{
			TargetCategory = CurrentRoundQuestions[i].Category;
		}

		// 🌟 확장된 초기화 함수 호출 (번호, 진짜 카테고리, 부모 주소(this), 고유 인덱스(i) 주입)
		Cell->InitCellData(Numbers[i], TargetCategory, this, i);
		Cell->OccupyCell(0); // 기본 배경색 세팅

		// 5. 그리드 배치
		UUniformGridSlot* NewSlot = BingoGrid->AddChildToUniformGrid(Cell);

		NewSlot->SetRow(i / BoardSize);
		NewSlot->SetColumn(i % BoardSize);
		NewSlot->SetHorizontalAlignment(HAlign_Fill);
		NewSlot->SetVerticalAlignment(VAlign_Fill);

		Cells.Add(Cell);
	}
}

void UCB_BingoBoard::CheckBingo()
{
	if (!IsValid(BingoScoreBoard)) return;

	int32 TeamABingoCount = 0;
	int32 TeamBBingoCount = 0;

	// -------------------------------------------------------------------------
	// 1. A팀 (AssignedTeamNumber == 1) 및 B팀 (AssignedTeamNumber == 2) 빙고 계산
	// -------------------------------------------------------------------------
	
	// [가로 체크]
	for (int32 Row = 0; Row < BoardSize; Row++)
	{
		bool bLineA = true;
		bool bLineB = true;
		for (int32 Col = 0; Col < BoardSize; Col++)
		{
			UCB_BingoCell* Cell = Cells[Row * BoardSize + Col];
			if (!Cell->bSelected || Cell->AssignedTeamNumber != 1) bLineA = false;
			if (!Cell->bSelected || Cell->AssignedTeamNumber != 2) bLineB = false;
		}
		if (bLineA) TeamABingoCount++;
		if (bLineB) TeamBBingoCount++;
	}

	// [세로 체크]
	for (int32 Col = 0; Col < BoardSize; Col++)
	{
		bool bLineA = true;
		bool bLineB = true;
		for (int32 Row = 0; Row < BoardSize; Row++)
		{
			UCB_BingoCell* Cell = Cells[Row * BoardSize + Col];
			if (!Cell->bSelected || Cell->AssignedTeamNumber != 1) bLineA = false;
			if (!Cell->bSelected || Cell->AssignedTeamNumber != 2) bLineB = false;
		}
		if (bLineA) TeamABingoCount++;
		if (bLineB) TeamBBingoCount++;
	}

	// [대각선 체크]
	bool bDiag1_A = true, bDiag2_A = true;
	bool bDiag1_B = true, bDiag2_B = true;
	for (int32 i = 0; i < BoardSize; i++)
	{
		// 주대각선 (\)
		UCB_BingoCell* Cell1 = Cells[i * BoardSize + i];
		if (!Cell1->bSelected || Cell1->AssignedTeamNumber != 1) bDiag1_A = false;
		if (!Cell1->bSelected || Cell1->AssignedTeamNumber != 2) bDiag1_B = false;

		// 부대각선 (/)
		UCB_BingoCell* Cell2 = Cells[i * BoardSize + ((BoardSize - 1) - i)];
		if (!Cell2->bSelected || Cell2->AssignedTeamNumber != 1) bDiag2_A = false;
		if (!Cell2->bSelected || Cell2->AssignedTeamNumber != 2) bDiag2_B = false;
	}
	if (bDiag1_A) TeamABingoCount++;
	if (bDiag2_A) TeamABingoCount++;
	if (bDiag1_B) TeamBBingoCount++;
	if (bDiag2_B) TeamBBingoCount++;


	// -------------------------------------------------------------------------
	// 2. 🌟 [핵심 버그 수정 구역] 정답/오답 불문하고 '체크 완료된 모든 칸' 카운트
	// -------------------------------------------------------------------------
	int32 TotalSelectedCells = 0;
	for (UCB_BingoCell* Cell : Cells)
	{
		if (IsValid(Cell))
		{
			// 💡 포인트: 'bSelected'가 true라면 (팀A든, 팀B든, 혹은 호스트가 무효(0)로 풀었든 간에)
			// 진행자가 문제를 출제해서 '완료 처리'를 끝낸 칸이므로 무조건 숫자를 셉니다!
			if (Cell->bSelected)
			{
				TotalSelectedCells++;
			}
		}
	}

	// -------------------------------------------------------------------------
	// 3. 전광판 UI 실시간 새로고침 및 게임 오버 판정선 전송
	// -------------------------------------------------------------------------
	BingoScoreBoard->RefreshBingoUI(TeamABingoCount, true);
	BingoScoreBoard->RefreshBingoUI(TeamBBingoCount, false);

	UE_LOG(LogTemp, Log, TEXT("[빙고 라인 연동] A팀: %d줄 / B팀: %d줄 / 진행 완료된 칸: %d/%d"), 
		TeamABingoCount, TeamBBingoCount, TotalSelectedCells, Cells.Num());

	// 🌟 이제 중간에 오답이 껴서 빙고가 안 만들어져도, TotalSelectedCells가 전체 칸 개수에 도달하면 칼같이 종료 판단을 내립니다!
	CheckOutGameOver(TeamABingoCount, TeamBBingoCount, TotalSelectedCells);
}

void UCB_BingoBoard::OnToggleGridSizeClicked()
{
	if (!AN_ScaleGrid) return;

	if (bIsGridZoomed)
	{
		// 1. 이미 커진 상태라면 원래 크기로 줄입니다 (역재생)
		PlayAnimation(AN_ScaleGrid, 0.0f, 1, EUMGSequencePlayMode::Reverse, 1.0f);
		bIsGridZoomed = false;
		
		UE_LOG(LogTemp, Log, TEXT("[보드] 빙고판 크기 축소"));
	}
	else
	{
		// 2. 작은 상태라면 화면에서 크게 키웁니다 (정방향 재생)
		PlayAnimation(AN_ScaleGrid, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
		bIsGridZoomed = true;
		
		UE_LOG(LogTemp, Log, TEXT("[보드] 빙고판 크기 확대"));
	}
}

void UCB_BingoBoard::RerollCellQuestion(int32 TargetIndex)
{

	if (!Cells.IsValidIndex(TargetIndex) || !CurrentRoundQuestions.IsValidIndex(TargetIndex)) return;

	UCB_GameInstance* GI = Cast<UCB_GameInstance>(GetGameInstance());
	if (!GI) return;


	const TArray<FCB_DataTable_Question>& AllQuestions = GI->Questions; 
	
	if (AllQuestions.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[리롤 실패] GameInstance에 로드된 전체 문제가 없습니다."));
		return;
	}

	// ─── 아래 중복 필터링 및 교체 로직은 그대로 유지 ───
	TArray<FCB_DataTable_Question> AvailableCandidates;
	for (const FCB_DataTable_Question& TotalQ : AllQuestions)
	{
		bool bIsAlreadyUsed = false;
		for (const FCB_DataTable_Question& CurrentQ : CurrentRoundQuestions)
		{
			if (TotalQ.QuestionText.Equals(CurrentQ.QuestionText))
			{
				bIsAlreadyUsed = true;
				break;
			}
		}

		if (!bIsAlreadyUsed)
		{
			AvailableCandidates.Add(TotalQ);
		}
	}

	if (AvailableCandidates.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[리롤 실패] 중복되지 않는 남은 문제가 부족합니다!"));
		return;
	}

	int32 RandomIdx = FMath::RandRange(0, AvailableCandidates.Num() - 1);
	FCB_DataTable_Question NewQuestion = AvailableCandidates[RandomIdx];

	CurrentRoundQuestions[TargetIndex] = NewQuestion;

	if (UCB_BingoCell* TargetCell = Cells[TargetIndex])
	{
		TargetCell->InitCellData(TargetCell->Number, NewQuestion.Category, this, TargetIndex);
		UE_LOG(LogTemp, Log, TEXT("[리롤 성공] %d번 칸 문제 교체 완료 -> 카테고리: %s"), TargetIndex + 1, *NewQuestion.Category);
	}

	if (CurrentOpenedCellIndex == TargetIndex)
	{
		if (QuestionBoard)
			QuestionBoard->SetQuestion(NewQuestion);
		// if (IsValid(TargetHostPanel))
		// 	TargetHostPanel->SetCurrentQuestionInfo(TargetIndex, NewQuestion);
	}
}

void UCB_BingoBoard::NativeConstruct()
{
	Super::NativeConstruct();
	// InitBoard();

	if (Btn_ToggleGridSize)
	{
		Btn_ToggleGridSize->OnClicked.AddDynamic(this, &UCB_BingoBoard::OnToggleGridSizeClicked);
	}
}



void UCB_BingoBoard::CheckOutGameOver(int32 TeamA, int32 TeamB, int32 TotalSelectedCells)
{
	UCB_GameInstance* GI = Cast<UCB_GameInstance>(GetGameInstance());
	
	// GI가 유효하다면 세팅된 목표값을 쓰고, 없으면 안전빵으로 3줄을 기본값으로 씁니다.
	int32 FinalTargetLines = IsValid(GI) ? GI->GetTargetBingo() : 3;

	// -------------------------------------------------------------------------
	// 조건 1. [기존] 어느 한 팀이라도 목표 빙고 줄 수에 먼저 도달했을 때
	// -------------------------------------------------------------------------
	if (TeamA >= FinalTargetLines)
	{
		UE_LOG(LogTemp, Warning, TEXT("[게임 종료] 🔴 A팀이 목표치인 %d줄을 달성하여 최종 승리했습니다!"), FinalTargetLines);
		HandleGameOver(1); // A팀 승리 팝업
		return;
	}
	else if (TeamB >= FinalTargetLines)
	{
		UE_LOG(LogTemp, Warning, TEXT("[게임 종료] 🔵 B팀이 목표치인 %d줄을 달성하여 최종 승리했습니다!"), FinalTargetLines);
		HandleGameOver(2); // B팀 승리 팝업
		return;
	}

	// -------------------------------------------------------------------------
	// 조건 2. [추가] 목표 빙고에는 못 갔지만 모든 빙고판이 꽉 채워졌을 때 (점수 높은 팀 승리)
	// -------------------------------------------------------------------------
	if (Cells.Num() > 0 && TotalSelectedCells >= Cells.Num())
	{
		int32 ScoreA = IsValid(GI) ? GI->TeamAScore : 0;
		int32 ScoreB = IsValid(GI) ? GI->TeamBScore : 0;

		UE_LOG(LogTemp, Warning, TEXT("[게임 종료] 모든 빙고판이 채워졌습니다! 최종 점수를 비교합니다. (A팀: %d점 / B팀: %d점)"), ScoreA, ScoreB);

		if (ScoreA > ScoreB)
		{
			UE_LOG(LogTemp, Warning, TEXT("[결과] 점수가 더 높은 🔴 A팀 최종 승리!"));
			HandleGameOver(1);
		}
		else if (ScoreB > ScoreA)
		{
			UE_LOG(LogTemp, Warning, TEXT("[결과] 점수가 더 높은 🔵 B팀 최종 승리!"));
			HandleGameOver(2);
		}
		else
		{
			// 🤝 점수까지 완벽하게 동점일 경우 (레크리에이션 예외 처리)
			// 우선 1번(A팀) 혹은 별도의 무승부 플래그(예: 3)를 던져 연출하도록 설계합니다.
			UE_LOG(LogTemp, Warning, TEXT("[결과] 점수까지 동점입니다! 무승부 혹은 판정 승리 처리."));
			HandleGameOver(1); 
		}
	}
}

void UCB_BingoBoard::SetCellOwnerByHost(uint8 TeamNumber)
{
	if (CurrentOpenedCellIndex == INDEX_NONE || !Cells.IsValidIndex(CurrentOpenedCellIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("[진행자 조작 실패] 현재 선택되어 열린 빙고 칸이 없습니다!"));
		return;
	}
	UCB_GameInstance* GI = Cast<UCB_GameInstance>(GetGameInstance());

	int32 CurrentQuestionScore = 0;
	if (CurrentRoundQuestions.IsValidIndex(CurrentOpenedCellIndex))
	{
		// 🌟 데이터 테이블 구조체에서 문제 고유의 Score 값을 추출합니다!
		CurrentQuestionScore = CurrentRoundQuestions[CurrentOpenedCellIndex].Score;
	}

	UCB_BingoCell* TargetCell = Cells[CurrentOpenedCellIndex];
	if (TargetCell)
	{
		TargetCell->OccupyCell(TeamNumber);

		if (TeamNumber > 0)
		{
			TargetCell->bSelected = true;

			// 🌟 [4번 요구사항] 정답 판정을 내렸다면 해당 팀의 누적 점수를 증가시킵니다!
			if (IsValid(GI))
			{
				if (TeamNumber == 1)
				{
					GI->PlusScore(CurrentQuestionScore, true); // 정답 팀 점수 200점 가산
				}
				else if (TeamNumber == 2)
				{
					GI->PlusScore(CurrentQuestionScore, false); // 정답 팀 점수 200점 가산
				}
			}
		}
		else
		{
			TargetCell->bSelected = false;
		}

		UE_LOG(LogTemp, Log, TEXT("[진행자 제어] %d번 빙고 칸이 %d번 팀에 의해 점령되었습니다."), CurrentOpenedCellIndex + 1, TeamNumber);
	}

	// 🌟 [4번 계속] 바뀐 점수를 점수판 UI(BingoScoreBoard)에 노출시키는 연동 함수 호출 기점
	if (IsValid(BingoScoreBoard) && IsValid(GI))
	{
		BingoScoreBoard->RefreshScoreUI();
	}

	// 빙고 줄 수 체크
	CheckBingo();

	CurrentOpenedCellIndex = INDEX_NONE;
}
