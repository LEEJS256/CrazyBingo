// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CB_BingoBoard.h"

#include "CB_BingoCell.h"
#include "CB_BingoScoreBoard.h"
#include "CB_HostPanel.h"
#include "CB_QuestionBoard.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "GameInstance/CB_GameInstance.h"

void UCB_BingoBoard::OnCellSelected(int32 SelectedIndex)
{
	if (!Cells.IsValidIndex(SelectedIndex) || !QuestionBoard) return;

	CurrentOpenedCellIndex = SelectedIndex;

	if (CurrentRoundQuestions.IsValidIndex(SelectedIndex))
	{
		FCB_DataTable_Question TargetQuestion = CurrentRoundQuestions[SelectedIndex];

		// 🌟 우측 만능 전광판 부품에게 구조체를 통째로 토스!!
		QuestionBoard->SetQuestion(TargetQuestion);

		UE_LOG(LogTemp, Log, TEXT("[보드] QuestionBoard 연동 완료: %s"), *TargetQuestion.QuestionText);

		if (IsValid(TargetHostPanel))
		{
			TargetHostPanel->SetCurrentQuestionInfo(SelectedIndex, TargetQuestion);
		}
	}


	// 4. 셀 상태가 바뀌었으니 빙고가 완성되었는지 실시간 체크
	CheckBingo();
}

void UCB_BingoBoard::InitBoard(int32 InBoardSize)
{
	if (!BingoGrid || !BingoCellClass) return;

	// 🌟 전달받은 크기(3, 4, 5)를 저장
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
	// Early Return 방어선
	if (!IsValid(BingoScoreBoard)) return;

	int32 TeamABingoCount = 0;
	int32 TeamBBingoCount = 0;

	// -------------------------------------------------------------------------
	// 1. A팀 (AssignedTeamNumber == 1) 빙고 줄 수 계산
	// -------------------------------------------------------------------------
	// 가로 체크
	for (int32 Row = 0; Row < BoardSize; Row++)
	{
		bool bLine = true;
		for (int32 Col = 0; Col < BoardSize; Col++)
		{
			UCB_BingoCell* Cell = Cells[Row * BoardSize + Col];
			// 세포가 선택 상태이며, 방금 만든 소유주 변수가 1(A팀)인지 확인
			if (!Cell->bSelected || Cell->AssignedTeamNumber != 1)
			{
				bLine = false;
				break;
			}
		}
		if (bLine) TeamABingoCount++;
	}
	// 세로 체크
	for (int32 Col = 0; Col < BoardSize; Col++)
	{
		bool bLine = true;
		for (int32 Row = 0; Row < BoardSize; Row++)
		{
			UCB_BingoCell* Cell = Cells[Row * BoardSize + Col];
			if (!Cell->bSelected || Cell->AssignedTeamNumber != 1)
			{
				bLine = false;
				break;
			}
		}
		if (bLine) TeamABingoCount++;
	}
	// 대각선 체크
	bool bDiag1_A = true, bDiag2_A = true;
	for (int32 i = 0; i < BoardSize; i++)
	{
		if (!Cells[i * BoardSize + i]->bSelected || Cells[i * BoardSize + i]->AssignedTeamNumber != 1) bDiag1_A = false;
		if (!Cells[i * BoardSize + ((BoardSize - 1) - i)]->bSelected || Cells[i * BoardSize + ((BoardSize - 1) - i)]->
			AssignedTeamNumber != 1) bDiag2_A = false;
	}
	if (bDiag1_A) TeamABingoCount++;
	if (bDiag2_A) TeamABingoCount++;


	// -------------------------------------------------------------------------
	// 2. B팀 (AssignedTeamNumber == 2) 빙고 줄 수 계산
	// -------------------------------------------------------------------------
	// 가로 체크
	for (int32 Row = 0; Row < BoardSize; Row++)
	{
		bool bLine = true;
		for (int32 Col = 0; Col < BoardSize; Col++)
		{
			UCB_BingoCell* Cell = Cells[Row * BoardSize + Col];
			if (!Cell->bSelected || Cell->AssignedTeamNumber != 2)
			{
				bLine = false;
				break;
			}
		}
		if (bLine) TeamBBingoCount++;
	}
	// 세로 체크
	for (int32 Col = 0; Col < BoardSize; Col++)
	{
		bool bLine = true;
		for (int32 Row = 0; Row < BoardSize; Row++)
		{
			UCB_BingoCell* Cell = Cells[Row * BoardSize + Col];
			if (!Cell->bSelected || Cell->AssignedTeamNumber != 2)
			{
				bLine = false;
				break;
			}
		}
		if (bLine) TeamBBingoCount++;
	}
	// 대각선 체크
	bool bDiag1_B = true, bDiag2_B = true;
	for (int32 i = 0; i < BoardSize; i++)
	{
		if (!Cells[i * BoardSize + i]->bSelected || Cells[i * BoardSize + i]->AssignedTeamNumber != 2) bDiag1_B = false;
		if (!Cells[i * BoardSize + ((BoardSize - 1) - i)]->bSelected || Cells[i * BoardSize + ((BoardSize - 1) - i)]->
			AssignedTeamNumber != 2) bDiag2_B = false;
	}
	if (bDiag1_B) TeamBBingoCount++;
	if (bDiag2_B) TeamBBingoCount++;


	// -------------------------------------------------------------------------
	// 3. 준식님이 만드신 최종 전광판 UI 새로고침 함수 바인딩 호출
	// -------------------------------------------------------------------------
	BingoScoreBoard->RefreshBingoUI(TeamABingoCount, true); // A팀 텍스트 줄 수 세팅
	BingoScoreBoard->RefreshBingoUI(TeamBBingoCount, false); // B팀 텍스트 줄 수 세팅

	UE_LOG(LogTemp, Log, TEXT("[빙고 라인 연동] A팀: %d줄 / B팀: %d줄 실시간 갱신 완료"), TeamABingoCount, TeamBBingoCount);
}

void UCB_BingoBoard::NativeConstruct()
{
	Super::NativeConstruct();
	// InitBoard();
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
