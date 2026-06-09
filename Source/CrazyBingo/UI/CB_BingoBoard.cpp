// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CB_BingoBoard.h"

#include "CB_BingoCell.h"
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
	BingoCount = 0;

	for (int32 Row = 0; Row < BoardSize; Row++)
	{
		bool bLine = true;
		for (int32 Col = 0; Col < BoardSize; Col++)
		{
			if (!Cells[Row * BoardSize + Col]->bSelected) { bLine = false; break; }
		}
		if (bLine) BingoCount++;
	}

	// 세로 체크
	for (int32 Col = 0; Col < BoardSize; Col++)
	{
		bool bLine = true;
		for (int32 Row = 0; Row < BoardSize; Row++)
		{
			if (!Cells[Row * BoardSize + Col]->bSelected) { bLine = false; break; }
		}
		if (bLine) BingoCount++;
	}

	// 대각선 체크
	bool bDiag1 = true, bDiag2 = true;
	for (int32 i = 0; i < BoardSize; i++)
	{
		if (!Cells[i * BoardSize + i]->bSelected) bDiag1 = false;
		if (!Cells[i * BoardSize + ((BoardSize - 1) - i)]->bSelected) bDiag2 = false;
	}
	if (bDiag1) BingoCount++;
	if (bDiag2) BingoCount++;

	UE_LOG(LogTemp, Warning, TEXT("현재 빙고 수: %d"), BingoCount);
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

	// 1. 현재 열려있는 칸의 진짜 셀 객체를 가져옵니다.
	UCB_BingoCell* TargetCell = Cells[CurrentOpenedCellIndex];
	if (TargetCell)
	{
		// 2. 셀에 구현해둔 배경색 변경 함수 호출 (0: 기본, 1: A팀 보라, 2: B팀 주황)
		TargetCell->OccupyCell(TeamNumber);
		
		// 3. 진행자가 1번팀이나 2번팀 승리 판정을 내렸다면 해당 셀을 '체크 완료' 상태로 만듭니다.
		if (TeamNumber > 0)
		{
			TargetCell->bSelected = true;
			// 만약 셀 자체의 버튼 틴트 컬러 변경 비주얼이 필요하다면 아래도 주석 해제
			// TargetCell->SetSelected(true); 
		}
		else
		{
			// 오답 혹은 선택 취소(0번)인 경우 다시 미점령/미선택 상태로 롤백
			TargetCell->bSelected = false;
			// TargetCell->SetSelected(false);
		}

		UE_LOG(LogTemp, Log, TEXT("[진행자 제어] %d번 빙고 칸이 %d번 팀에 의해 점령되었습니다."), CurrentOpenedCellIndex + 1, TeamNumber);
	}

	// 4. 점령 상태가 바뀌었으므로 가로/세로/대각선 빙고 줄이 완성되었는지 실시간 계산!
	CheckBingo();

	// 5. 처리가 깔끔하게 끝났으니 열려있던 문제 인덱스는 다시 빈 칸(INDEX_NONE)으로 초기화합니다.
	CurrentOpenedCellIndex = INDEX_NONE;
}
