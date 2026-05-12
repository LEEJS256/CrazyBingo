// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CB_BingoBoard.h"

#include "CB_BingoCell.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

void UCB_BingoBoard::InitBoard()
{
	if (!BingoGrid || !BingoCellClass) return;

	BingoGrid->ClearChildren();
	Cells.Empty();

	// 1~25 숫자 랜덤 배치
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 25; i++) Numbers.Add(i);

	// 셔플
	for (int32 i = Numbers.Num() - 1; i > 0; i--)
	{
		int32 j = FMath::RandRange(0, i);
		Numbers.Swap(i, j);
	}

	// 5x5 그리드에 셀 배치
	for (int32 i = 0; i < 25; i++)
	{
		UCB_BingoCell* Cell = CreateWidget<UCB_BingoCell>(this, BingoCellClass);
		if (!Cell) continue;

		Cell->SetNumber(Numbers[i]);

		UUniformGridSlot* NewSlot = BingoGrid->AddChildToUniformGrid(Cell);
		NewSlot->SetRow(i / 5);
		NewSlot->SetColumn(i % 5);
		NewSlot->SetHorizontalAlignment(HAlign_Fill);
		NewSlot->SetVerticalAlignment(VAlign_Fill);

		Cells.Add(Cell);
	}
}

void UCB_BingoBoard::CheckBingo()
{
	BingoCount = 0;

	// 가로 체크
	for (int32 Row = 0; Row < 5; Row++)
	{
		bool bLine = true;
		for (int32 Col = 0; Col < 5; Col++)
		{
			if (!Cells[Row * 5 + Col]->bSelected) { bLine = false; break; }
		}
		if (bLine) BingoCount++;
	}

	// 세로 체크
	for (int32 Col = 0; Col < 5; Col++)
	{
		bool bLine = true;
		for (int32 Row = 0; Row < 5; Row++)
		{
			if (!Cells[Row * 5 + Col]->bSelected) { bLine = false; break; }
		}
		if (bLine) BingoCount++;
	}

	// 대각선 체크
	bool bDiag1 = true, bDiag2 = true;
	for (int32 i = 0; i < 5; i++)
	{
		if (!Cells[i * 5 + i]->bSelected) bDiag1 = false;
		if (!Cells[i * 5 + (4 - i)]->bSelected) bDiag2 = false;
	}
	if (bDiag1) BingoCount++;
	if (bDiag2) BingoCount++;

	UE_LOG(LogTemp, Warning, TEXT("빙고 수: %d"), BingoCount);
}

void UCB_BingoBoard::NativeConstruct()
{
	Super::NativeConstruct();
	InitBoard();
}
