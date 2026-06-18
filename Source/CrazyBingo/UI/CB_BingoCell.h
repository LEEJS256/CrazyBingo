// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CB_BingoCell.generated.h"

/**
 * 
 */
class UButton;
class UTextBlock;
UCLASS()
class CRAZYBINGO_API UCB_BingoCell : public UUserWidget
{
	GENERATED_BODY()
public:

	// 🌟 셀을 초기화할 때 부모 보드의 주소와 고유 인덱스도 함께 기억시킵니다.
	void InitCellData(int32 InNumber, const FString& InCategory, class UCB_BingoBoard* InOwnerBoard, int32 InCellIndex);

	UPROPERTY()
	class UCB_BingoBoard* OwnerBoard;

	UPROPERTY(BlueprintReadOnly, Category = "Bingo")
	uint8 AssignedTeamNumber = 0;
	
	int32 CellIndex = 0;


	void OccupyCell(uint8 TeamNumber);

	// 선택됐는지 여부
	UFUNCTION(BlueprintCallable)
	void SetSelected(bool bInSelected);

	UPROPERTY(BlueprintReadOnly)
	int32 Number = 0;
	
	UPROPERTY(BlueprintReadOnly, Category = "Bingo")
	FString Category = TEXT("");

	UPROPERTY(BlueprintReadOnly)
	bool bSelected = false;
protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnCellClicked();
	
	UPROPERTY(meta = (BindWidget))
	class UButton* CellButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NumberText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Category;
	
	UPROPERTY(meta = (BindWidget))
	class UBorder* CellBackgroundBorder;

	
};
