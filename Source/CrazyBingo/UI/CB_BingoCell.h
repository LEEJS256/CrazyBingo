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
	// 숫자 세팅
	UFUNCTION(BlueprintCallable)
	void SetNumber(int32 InNumber);

	// 선택됐는지 여부
	UFUNCTION(BlueprintCallable)
	void SetSelected(bool bInSelected);

	UPROPERTY(BlueprintReadOnly)
	int32 Number = 0;

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

	
};
