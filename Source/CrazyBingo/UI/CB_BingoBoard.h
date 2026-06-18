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

	UFUNCTION(BlueprintCallable, Category = "Bingo|Rules")
	void ReturnToMainMenu();
	
	// 최종 승리팀 번호를 받아 처리하는 함수
	void HandleGameOver(uint8 WinningTeamID);
	
	void OnCellSelected(int32 SelectedIndex);
	
	UFUNCTION(BlueprintCallable)
	void InitBoard(int32 InBoardSize);

	UFUNCTION(BlueprintCallable)
	void CheckBingo();

	UFUNCTION()
	void OnToggleGridSizeClicked();
	
	UFUNCTION(BlueprintCallable, Category = "Bingo|Host")
	void SetCellOwnerByHost(uint8 TeamNumber);

	UPROPERTY(BlueprintReadWrite, Category = "Host")
	class UCB_HostPanel* TargetHostPanel;

	// 호스트 패널이 호출해 줄 진짜 리롤 처리 함수
	void RerollCellQuestion(int32 TargetIndex);
	
protected:
	virtual void NativeConstruct() override;


	// 에디터에서 이름 맞춰줄 것
	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* BingoGrid;

	// 셀 위젯 클래스 (에디터에서 WBP_BingoCell 지정)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UCB_BingoCell> BingoCellClass;
	
	UPROPERTY()
	TArray<class UCB_BingoCell*> Cells;

	UPROPERTY(BlueprintReadOnly, Category = "Bingo")
	int32 BoardSize = 5;
	
	int32 BingoCount = 0;


	UPROPERTY(meta = (BindWidgetAnimation), Transient)
	UWidgetAnimation* AN_ScaleGrid;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_ToggleGridSize;

	bool bIsGridZoomed = false;
	
	
	UPROPERTY(meta = (BindWidget))
	class UCB_BingoScoreBoard* BingoScoreBoard; 

	UPROPERTY(meta = (BindWidget))
	class UCB_QuestionBoard*	QuestionBoard;
	
	UPROPERTY()
	TArray<FCB_DataTable_Question> CurrentRoundQuestions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bingo|Rules")
	TSubclassOf<class UUserWidget> ResultPopupClass;
	
	void CheckOutGameOver(int32 TeamA, int32 TeamB, int32 TotalSelectedCells);
	

	int32 CurrentOpenedCellIndex = INDEX_NONE;
};
