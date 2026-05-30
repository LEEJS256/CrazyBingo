// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "Utility/CB_SaveSlotData.h"
#include "CB_SaveSlotRow.generated.h"

/**
 * 
 */
class UTextBlock;
class UCheckBox;
UCLASS()
class CRAZYBINGO_API UCB_SaveSlotRow : public UUserWidget , public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	// 🌟 인터페이스가 제공하는 원본 함수를 오버라이드합니다.
	// 리스트뷰에 아이템이 세팅될 때 엔진에 의해 자동으로 호출됩니다.
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UFUNCTION()
	void OnCheckBoxStateChanged(bool bIsChecked);
	
	// 블루프린트에 배치할 텍스트 블록 (이름이 일치해야 합니다)
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_SlotName;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* SlotCheckBox;

private:
	// 현재 이 칸이 바라보고 있는 진짜 데이터 주머니의 주소
	UPROPERTY()
	UCB_SaveSlotData* MyItemData;
};
