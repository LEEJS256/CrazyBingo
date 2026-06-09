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
	// 1. 공통 사용 컴포넌트
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_QuestionType; // "객관식", "주관식" 등 배지 텍스트

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_QuestionBody; // "대한민국의 수도는?" 실제 질문

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* QuestionTypeSwitcher;

	// 3. 객관식 전용 보기 텍스트들 (필요시)
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Choice_1;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Choice_2;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Choice_3;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Choice_4;

	// 1번 페이지: 초성 힌트 전용
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_InitialHint;

	UPROPERTY(meta = (BindWidget))
	class UWebBrowser* WebBrowser_Youtube;
	
	// 📸 동적 이미지 사진을 띄울 컴포넌트 추가!
	UPROPERTY(meta = (BindWidget))
	UImage* Image_QuestionPhoto;

	// // 4. 주관식/미디어 등 타 유형 정답 가리기용 (호스트가 정답공개 버튼 누르면 보이기 등)
	// UPROPERTY(meta = (BindWidget))
	// UTextBlock* Text_AnswerHidden;
};
