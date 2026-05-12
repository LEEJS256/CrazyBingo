// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_BingoCell.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCB_BingoCell::SetNumber(int32 InNumber)
{
	Number = InNumber;
	if (NumberText)
	{
		NumberText->SetText(FText::AsNumber(InNumber));

		// FSlateFontInfo FontInfo = NumberText->GetFont();
		// FontInfo.Size = 24;  // ← 원하는 크기
		// NumberText->SetFont(FontInfo);
	}
}

void UCB_BingoCell::SetSelected(bool bInSelected)
{
	bSelected = bInSelected;
	// if (CellButton)
	// {
	// 	// 선택 시 색상 변경
	// 	FLinearColor Color = bInSelected ? FLinearColor(1.f, 0.5f, 0.f) : FLinearColor::White;
	// 	CellButton->SetBackgroundColor(Color);
	// }

	if (CellImage)
	{
		FLinearColor Color = bInSelected 
			? FLinearColor(0.2f, 0.2f, 0.2f, 1.f)  // 선택 시 어둡게
			: FLinearColor(1.f, 1.f, 1.f, 1.f);     // 기본 밝게
        
		CellImage->SetColorAndOpacity(Color);
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
}
