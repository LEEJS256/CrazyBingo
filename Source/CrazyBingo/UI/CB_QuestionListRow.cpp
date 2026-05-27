// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CB_QuestionListRow.h"

#include "CB_QuestionListData.h"
#include "Components/CheckBox.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"


void UCB_QuestionListRow::OnDeleteCheckBoxChanged(bool bIsChecked)
{
	if (MyDataObj)
	{
		// 사용자가 체크를 켜고 끌 때 데이터 오브젝트에 실시간 반영
		MyDataObj->bIsCheckedForDelete = bIsChecked;
	}
}
void UCB_QuestionListRow::NativeConstruct()
{
	Super::NativeConstruct();
	if (Delete_CheckBox)
	{
		// 체크박스 이벤트 바인딩
		Delete_CheckBox->OnCheckStateChanged.AddDynamic(this, &UCB_QuestionListRow::OnDeleteCheckBoxChanged);
	}
}

void UCB_QuestionListRow::NativeOnItemSelectionChanged(bool bIsSelected)
{
	IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);
}




void UCB_QuestionListRow::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);


	MyDataObj = Cast<UCB_QuestionListData>(ListItemObject);
	if (!MyDataObj)
		return;

	UCB_QuestionListData* DataObj = Cast<UCB_QuestionListData>(ListItemObject);
	if (!DataObj || !QuestionTitleText) return;

	// 퀴즈 타입 이름 뽑아오기 (예: MultipleChoice -> 객관식)
	FString TypeStr = TEXT("");
	switch (DataObj->QuestionData.QuestionType)
	{
	case ECB_QuestionType::MultipleChoice: TypeStr = TEXT("객관식");
		break;
	case ECB_QuestionType::InitialSound: TypeStr = TEXT("초성");
		break;
	case ECB_QuestionType::YoutubeLink: TypeStr = TEXT("유튜브");
		break;
	case ECB_QuestionType::ShortAnswer: TypeStr = TEXT("단답형");
		break;
	}

	// // 텍스트 블록에 포맷 세팅 ("[객관식] 문제 내용...")
	// FString FormattedText = FString::Printf(TEXT("[%s] %s"), *TypeStr, *DataObj->QuestionData.QuestionText);
	// QuestionTitleText->SetText(FText::FromString(FormattedText));


	if (QuestionTitleText)
	{
		QuestionTitleText->SetText(FText::FromString(DataObj->QuestionData.QuestionText));
	}

	if (QuestionTypeText)
	{
		QuestionTypeText->SetText(FText::FromString(TypeStr));
	}

	if (QuestionCategoryText)
	{
		// FString 또는 FText 타입에 맞게 세팅 (여기서는 FString 혹은 FName 가정)
		FString CategoryStr = DataObj->QuestionData.Category;
		QuestionCategoryText->SetText(FText::FromString(CategoryStr));
	}

	if (QuestionScoreText)
	{
		// 정수형 점수를 문자열로 변환하여 포맷팅
		FString ScoreStr = FString::Printf(TEXT("%d점"), DataObj->QuestionData.Score);
		QuestionScoreText->SetText(FText::FromString(ScoreStr));
	}

	if (Delete_CheckBox)
	{
		Delete_CheckBox->SetIsChecked(MyDataObj->bIsCheckedForDelete);
	}
}
