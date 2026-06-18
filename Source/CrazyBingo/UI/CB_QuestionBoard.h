// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/CB_DataTable_Question.h"
#include "CB_QuestionBoard.generated.h"


class UTextBlock;
class UImage;
class UWidgetSwitcher;

UCLASS()
class CRAZYBINGO_API UCB_QuestionBoard : public UUserWidget
{
	GENERATED_BODY()
public:

	void SetQuestion(const FCB_DataTable_Question& QuestionData);

protected:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_QuestionType; 

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_QuestionBody; 

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_QuestionScore;
	
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* QuestionTypeSwitcher;


	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Choice_1;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Choice_2;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Choice_3;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Choice_4;


	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_InitialHint;

	// UPROPERTY(meta = (BindWidget))
	// class UWebBrowser* WebBrowser_Youtube;
	

	UPROPERTY(meta = (BindWidget))
	UImage* Image_QuestionPhoto;


};
