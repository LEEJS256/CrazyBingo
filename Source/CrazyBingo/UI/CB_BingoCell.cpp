// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_BingoCell.h"

#include "CB_BingoBoard.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameInstance/CB_GameInstance.h"


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
	CellIndex = InCellIndex; // 🌟 내 인덱스 기억

	if (NumberText)
	{
		FString FormattedNumber = FString::Printf(TEXT("%02d"), InNumber);

		NumberText->SetText(FText::FromString(FormattedNumber));
	}
	if (Text_Category)
		Text_Category->SetText(FText::FromString(InCategory));
}


void UCB_BingoCell::OccupyCell(uint8 TeamNumber)
{
	if (!CellBackgroundBorder) return;

	AssignedTeamNumber = TeamNumber;

	UCB_GameInstance* GI = Cast<UCB_GameInstance>(GetGameInstance());

	if (TeamNumber == 1)
	{
		FLinearColor ColorA = GI->GetTeamColor(true);
		CellBackgroundBorder->SetContentColorAndOpacity(ColorA);

		UE_LOG(LogTemp, Log, TEXT("[세포 갱신] %d번 세포가 A팀 색상으로 채워졌습니다."), CellIndex + 1);
	}
	else if (TeamNumber == 2)
	{
		FLinearColor ColorB = GI->GetTeamColor(false);
		CellBackgroundBorder->SetContentColorAndOpacity(ColorB);

		UE_LOG(LogTemp, Log, TEXT("[세포 갱신] %d번 세포가 B팀 색상으로 채워졌습니다."), CellIndex + 1);
	}
	else if (TeamNumber == 3)
	{
		CellBackgroundBorder->SetContentColorAndOpacity(FLinearColor(0.45f, 0.08f, 0.08f, 1.0f));
	}
	else
	{
		CellBackgroundBorder->SetContentColorAndOpacity(FLinearColor(0.25f, 0.28f, 0.35f, 1.0f));
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
