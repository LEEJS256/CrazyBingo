// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_BingoCell.h"

#include "CB_BingoBoard.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"



void UCB_BingoCell::SetSelected(bool bInSelected)
{
	bSelected = bInSelected;

	// 버튼 자체의 틴트 컬러를 조절해 비활성화/선택 연출을 하고 싶을 때 사용
	if (CellButton)
	{
		FLinearColor Color = bInSelected ? FLinearColor(0.3f, 0.3f, 0.3f, 1.0f) : FLinearColor::White;
		CellButton->SetBackgroundColor(Color);
	}
}

void UCB_BingoCell::InitCellData(int32 InNumber, const FString& InCategory, class UCB_BingoBoard* InOwnerBoard,
	int32 InCellIndex)
{
	Number = InNumber;
	Category = InCategory;
	OwnerBoard = InOwnerBoard; // 🌟 부모 기억
	CellIndex = InCellIndex;   // 🌟 내 인덱스 기억

	if (NumberText)    \
		NumberText->SetText(FText::AsNumber(InNumber));
	if (Text_Category)
		Text_Category->SetText(FText::FromString(InCategory));
}

void UCB_BingoCell::InitCellData(int32 InNumber, const FString& InCategory)
{
	Number = InNumber;
	Category = InCategory;

	if (NumberText)
	{
		NumberText->SetText(FText::AsNumber(InNumber));
	}

	if (Text_Category)
	{
		Text_Category->SetText(FText::FromString(InCategory));
	}
}

void UCB_BingoCell::OccupyCell(uint8 TeamNumber)
{
	if (!CellBackgroundBorder) return;

	AssignedTeamNumber = TeamNumber;
	
	if (TeamNumber == 1)
	{
		// 🟣 팀 A 보라색
		CellBackgroundBorder->SetContentColorAndOpacity(FLinearColor(0.325f, 0.290f, 0.717f, 1.0f));
	}
	else if (TeamNumber == 2)
	{
		// 🟠 팀 B 주황색
		CellBackgroundBorder->SetContentColorAndOpacity(FLinearColor(0.729f, 0.458f, 0.090f, 1.0f));
	}
	else
	{
		// ⬛ 기본 어두운 배경색
		CellBackgroundBorder->SetContentColorAndOpacity(FLinearColor(0.164f, 0.145f, 0.250f, 1.0f));
	}
}

void UCB_BingoCell::NativeConstruct()
{
	Super::NativeConstruct();

	if (CellButton)
	{
		CellButton->OnClicked.AddDynamic(this, &UCB_BingoCell::OnCellClicked);
	}
}

void UCB_BingoCell::OnCellClicked()
{
	SetSelected(!bSelected);
	if (OwnerBoard)
	{
		OwnerBoard->OnCellSelected(CellIndex);
	}
}
