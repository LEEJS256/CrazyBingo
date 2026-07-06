// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/CB_DataTable_Question.h"
#include "CB_HostPanel.generated.h"

/**
 * 
 */
class UButton;
UCLASS()
class CRAZYBINGO_API UCB_HostPanel : public UUserWidget
{
	GENERATED_BODY()
public:
	// UPROPERTY(BlueprintReadWrite, Category = "Host")
	// class UCB_BingoBoard* TargetBingoBoard;
	
	void SetCurrentQuestionInfo(int32 CellIndex, const FCB_DataTable_Question& QuestionData);
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Reroll;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_TeamA_Win;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_TeamB_Win;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_WrongAnswer;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_CurrentQuestion;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Explanation;

	// UPROPERTY(meta = (BindWidget))
	// class UWebBrowser* WebBrowser_Youtube; 

	UFUNCTION()
	void RerollCellQuestion();
	
	UFUNCTION()
	void OnTeamAWinClicked();

	UFUNCTION()
	void OnTeamBWinClicked();

	UFUNCTION()
	void OnWrongAnswerClicked();

private:
	int32 CachedCellIndex = -1;

	FVector2D DragOffset;
};
