// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CB_SaveDialog.generated.h"

class UListView;
class UEditableTextBox;
class UButton;

UCLASS()
class CRAZYBINGO_API UCB_SaveDialog : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY()
	class UCB_AddQuestion* OwningAddQuestionWidget;

	// 메인 화면에서 팝업을 띄운 후 이 함수로 주소를 넘겨줄 겁니다.
	void SetOwningWidget(UCB_AddQuestion* InParent);

protected:
	virtual void NativeConstruct() override;

	// UI 컴포넌트 변수들 (블루프린트 이름과 매칭)
	UPROPERTY(meta = (BindWidget))
	UListView* SaveFileList;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* NewFileNameInput;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Confirm;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Cancel;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_DeleteSelectedSave;


	//불러오기
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Load; //
	
	// 리스트뷰 항목 클릭 시 실행될 함수
	UFUNCTION()
	void OnSlotItemClicked(UObject* Item);

	//세이브 데이터 삭제
	UFUNCTION()
	void OnDeleteSelectedSaveClicked();
	
	//저장 ,덮어쓰기
	UFUNCTION()
	void OnConfirmClicked();
	
	UFUNCTION()
	void OnLoadClicked();
	
	UFUNCTION()
	void OnCancelClicked();

public:
	// 폴더를 뒤져 세이브 파일 목록을 리스트뷰에 뿌려주는 함수
	void RefreshSaveFileList();
};
