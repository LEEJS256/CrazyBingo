// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CB_SaveSlotRow.h"

#include "CB_GameSetup.h"
#include "CB_SaveDialog.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Utility/CB_SaveSlotData.h"

void UCB_SaveSlotRow::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	MyItemData = Cast<UCB_SaveSlotData>(ListItemObject);
    
	if (MyItemData)
	{
		if (Txt_SlotName)
			Txt_SlotName->SetText(FText::FromString(MyItemData->SlotName));

		if (SlotCheckBox)
		{
			// 1. 기존 바인딩 제거 후 새로 바인딩 (이벤트 중복 방지)
			SlotCheckBox->OnCheckStateChanged.RemoveAll(this);
			SlotCheckBox->OnCheckStateChanged.AddDynamic(this, &UCB_SaveSlotRow::OnCheckBoxStateChanged);

			// 2. 데이터 주머니의 상태에 맞게 체크박스 UI 표현 (True면 체크, False면 해제)
			SlotCheckBox->SetIsChecked(MyItemData->bIsSelected);
		}
	}

}

void UCB_SaveSlotRow::OnCheckBoxStateChanged(bool bIsChecked)
{
	if (MyItemData)
	{
		// 화면의 체크 상태를 원본 데이터 주머니에 동기화해줍니다!
		MyItemData->bIsSelected = bIsChecked;

		if (bIsChecked)
		{

			if (MyItemData->OwningSaveDialog)
			{
				if (IsValid(MyItemData->OwningSaveDialog->NewFileNameInput))
				{
					// 체크박스를 켠 행의 파일 이름을 팝업창 텍스트 박스에 복사!
					MyItemData->OwningSaveDialog->NewFileNameInput->SetText(FText::FromString(MyItemData->SlotName));
				}
			}

			if (MyItemData->OwningGameSetup)
			{
				if (IsValid(MyItemData->OwningGameSetup->SaveFileListView))
				{
					MyItemData->OwningGameSetup->SaveFileListView->SetSelectedItem(MyItemData);
				}

				// 부모(GameSetup)의 우측 패널 정보 갱신 함수 호출
				MyItemData->OwningGameSetup->OnSaveFileSelected(MyItemData);
			}
		}
	}
}
