// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CB_QuestionListRow.h"

#include "CB_AddQuestion.h"
#include "CB_QuestionListData.h"
#include "Components/CheckBox.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"


void UCB_QuestionListRow::OnDeleteCheckBoxChanged(bool bIsChecked)
{
	if (MyDataObj)
	{
		// 1. 데이터 주머니의 체크 상태 실시간 동기화
		MyDataObj->bIsCheckedForDelete = bIsChecked;
		MyDataObj->QuestionData.bIsCheckedForDelete = bIsChecked; // 구조체 원본에도 동기화


		if (MyDataObj->OwningAddQuestion)
		{
	
			MyDataObj->OwningAddQuestion->UpdateCountTextsOnly();
		}
	}
}
void UCB_QuestionListRow::NativeConstruct()
{
	Super::NativeConstruct();
	// if (Delete_CheckBox)
	// {
	// 	// 체크박스 이벤트 바인딩
	// 	Delete_CheckBox->OnCheckStateChanged.AddDynamic(this, &UCB_QuestionListRow::OnDeleteCheckBoxChanged);
	// }
}

void UCB_QuestionListRow::NativeOnItemSelectionChanged(bool bIsSelected)
{
	IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);
}




void UCB_QuestionListRow::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	MyDataObj = Cast<UCB_QuestionListData>(ListItemObject);
	if (!MyDataObj) return;

	// 🌟 [안전장치] 체크박스 이벤트 중복 방지 및 확실한 바인딩
	if (Delete_CheckBox)
	{
		Delete_CheckBox->OnCheckStateChanged.RemoveAll(this);
		Delete_CheckBox->OnCheckStateChanged.AddDynamic(this, &UCB_QuestionListRow::OnDeleteCheckBoxChanged);
        
		// 데이터 주머니의 상태를 UI에 강제 동기화
		Delete_CheckBox->SetIsChecked(MyDataObj->bIsCheckedForDelete);
	}

	// --- 이하 텍스트 세팅 로직은 조장님 코드 그대로 유지 ---
	if (QuestionTitleText)
	{
		QuestionTitleText->SetText(FText::FromString(MyDataObj->QuestionData.QuestionText));
	}

	FString TypeStr = TEXT("");
	switch (MyDataObj->QuestionData.QuestionType)
	{
	case ECB_QuestionType::MultipleChoice: TypeStr = TEXT("객관식"); break;
	case ECB_QuestionType::InitialSound: TypeStr = TEXT("초성"); break;
	case ECB_QuestionType::YoutubeLink: TypeStr = TEXT("유튜브"); break;
	case ECB_QuestionType::ShortAnswer: TypeStr = TEXT("단답형"); break;
	}

	if (QuestionTypeText) QuestionTypeText->SetText(FText::FromString(TypeStr));

	if (QuestionCategoryText)
	{
		QuestionCategoryText->SetText(FText::FromString(MyDataObj->QuestionData.Category));
	}

	if (QuestionScoreText)
	{
		FString ScoreStr = FString::Printf(TEXT("%d점"), MyDataObj->QuestionData.Score);
		QuestionScoreText->SetText(FText::FromString(ScoreStr));
	}
}
