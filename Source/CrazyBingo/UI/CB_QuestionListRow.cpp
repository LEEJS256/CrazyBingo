// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CB_QuestionListRow.h"

#include "CB_QuestionListData.h"
#include "Components/TextBlock.h"

void UCB_QuestionListRow::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	UCB_QuestionListData* DataObj = Cast<UCB_QuestionListData>(ListItemObject);
	if (!DataObj || !QuestionTitleText) return;

	// 퀴즈 타입 이름 뽑아오기 (예: MultipleChoice -> 객관식)
	FString TypeStr = TEXT("");
	switch (DataObj->QuestionData.QuestionType)
	{
	case ECB_QuestionType::MultipleChoice: TypeStr = TEXT("객관식"); break;
	case ECB_QuestionType::InitialSound:   TypeStr = TEXT("초성"); break;
	case ECB_QuestionType::YoutubeLink:    TypeStr = TEXT("유튜브"); break;
	case ECB_QuestionType::ShortAnswer:     TypeStr = TEXT("단답형"); break;
	}

	// 텍스트 블록에 포맷 세팅 ("[객관식] 문제 내용...")
	FString FormattedText = FString::Printf(TEXT("[%s] %s"), *TypeStr, *DataObj->QuestionData.QuestionText);
	QuestionTitleText->SetText(FText::FromString(FormattedText));
}
