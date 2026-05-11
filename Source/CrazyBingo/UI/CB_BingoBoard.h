// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CB_BingoBoard.generated.h"

/**
 * 
 */
class UCB_BingoCell;
class UUniformGridPanel;
UCLASS()
class CRAZYBINGO_API UCB_BingoBoard : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitBoard();

	UFUNCTION(BlueprintCallable)
	void CheckBingo();

protected:
	virtual void NativeConstruct() override;

	// 에디터에서 이름 맞춰줄 것
	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* BingoGrid;

	// 셀 위젯 클래스 (에디터에서 WBP_BingoCell 지정)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UCB_BingoCell> BingoCellClass;

	// 25개 셀 배열
	UPROPERTY()
	TArray<class UCB_BingoCell*> Cells;

	int32 BingoCount = 0;
};
