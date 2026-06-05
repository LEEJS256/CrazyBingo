// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/CB_DataTable_Question.h"
#include "CB_GameSetup.generated.h"

/**
 * 
 */
UCLASS()
class CRAZYBINGO_API UCB_GameSetup : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UListView* SaveFileListView;

	// 하드디스크의 SaveGames 폴더에서 .sav 목록을 긁어와 리스트뷰에 채우는 함수
	void RefreshSaveFileList();

	
	// UFUNCTION()
	void OnSaveFileSelected(UObject* Item); // 리스트뷰 클릭 감지 이벤트 함수
protected:
	virtual void NativeConstruct() override;

	// =========================================================================
	// 1. UI 컴포넌트 바인딩
	// =========================================================================
	// 세이브 파일 목록을 보여줄 리스트뷰 (UCB_SaveSlotData 주머니 재사용)

	// 빙고 크기 선택 (3x3, 4x4, 5x5)
	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* BingoSizeComboBox;


	// [오른쪽 영역: 선택된 파일 상세 정보 및 체크 패널]
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_InfoName;    // id="info-name" (파일명)

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_InfoCount;   // id="info-count" (총 문제 수)

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_InfoDate;    // id="info-date" (저장일)

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_InfoSize;    // id="info-size" (빙고 크기 텍스트)

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_InfoStatus;  // id="info-status" (준비 완료 / 문제 부족 상태 문구)

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_InfoCheck;   // id="info-check" (최소 X문제 필요 알림 가이드 문구)

	// [하단 제어 버튼]
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_LaunchGame;      // id="btnLaunch" (게임 시작)

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Cancel;          // (메인 메뉴로)

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UCB_BingoBoard> BingoBoardClass;
	
	
	bool LoadQuestionsFromSlot(const FString& SlotName, TArray<FCB_DataTable_Question>& OutQuestions, FString& OutSaveDate);
	void UpdateSetupValidation();

	
	
	UFUNCTION()
	void OnBingoSizeChanged(FString SelectedItem, ESelectInfo::Type SelectionType); // 콤보박스 변경 이벤트

	UFUNCTION()
	void OnLaunchGameClicked();

	UFUNCTION()
	void OnCancelClicked();


private:
	// 현재 내부 메모리에 임시 캐싱해 둔 선택된 파일의 총 문제 수
	int32 SelectedFileQuestionCount = 0;

};
