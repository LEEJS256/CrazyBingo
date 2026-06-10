// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/CB_DataTable_Question.h"
#include "CB_BingoBoard.generated.h"

/**
 * 
 */
class UCB_BingoCell;

UCLASS()
class CRAZYBINGO_API UCB_BingoBoard : public UUserWidget
{
	GENERATED_BODY()

public:
	void OnCellSelected(int32 SelectedIndex);
	
	UFUNCTION(BlueprintCallable)
	void InitBoard(int32 InBoardSize);

	UFUNCTION(BlueprintCallable)
	void CheckBingo();
	
	UFUNCTION(BlueprintCallable, Category = "Bingo|Host")
	void SetCellOwnerByHost(uint8 TeamNumber);

	UPROPERTY(BlueprintReadWrite, Category = "Host")
	class UCB_HostPanel* TargetHostPanel;

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

	UPROPERTY(BlueprintReadOnly, Category = "Bingo")
	int32 BoardSize = 5;
	
	int32 BingoCount = 0;

	UPROPERTY(meta = (BindWidget))
	class UCB_BingoScoreBoard* BingoScoreBoard; 

	UPROPERTY(meta = (BindWidget))
	class UCB_QuestionBoard*	QuestionBoard;
	
	UPROPERTY()
	TArray<FCB_DataTable_Question> CurrentRoundQuestions;
	

	int32 CurrentOpenedCellIndex = INDEX_NONE;
};
