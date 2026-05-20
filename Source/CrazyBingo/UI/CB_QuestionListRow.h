// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "CB_QuestionListRow.generated.h"

/**
 * 
 */
UCLASS()
class CRAZYBINGO_API UCB_QuestionListRow : public UUserWidget , public IUserObjectListEntry
{
	GENERATED_BODY()
protected:
	// 리스트뷰 목록에 간단하게 띄울 텍스트 (예: "[객관식] 1번 문제 제목입니다.")
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* QuestionTitleText;

	// --- IUserObjectListEntry 인터페이스 구현 ---
	// 데이터가 이 위젯에 할당될 때 자동으로 호출되는 함수입니다.
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};
