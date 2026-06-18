// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/CB_DataTable_Question.h"
#include "CB_AddQuestion.generated.h"

/**
 * 
 */
UCLASS()
class CRAZYBINGO_API UCB_AddQuestion : public UUserWidget
{
	GENERATED_BODY()
protected:
    virtual void NativeConstruct() override;

    // =========================================================================
    // 1. 입력 UI 컴포넌트 (구조체 멤버 변수들과 매핑)
    // =========================================================================
    UPROPERTY(meta = (BindWidget))
    class UEditableText* CategoryInput;

    UPROPERTY(meta = (BindWidget))
    class UComboBoxString* QuestionTypeComboBox;

    UPROPERTY(meta = (BindWidget))
    class UMultiLineEditableText* QuestionTextInput; // 멀티라인은 UEditableText 사용 권장

    UPROPERTY(meta = (BindWidget))
    class UMultiLineEditableTextBox* YoutubeURLInput;

	// 🌟 1. 총 문제 개수만 깔끔하게 표시할 텍스트 (예: "등록된 총 문제: 15개")
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TotalCountText;

	// 🌟 2. 선택된 개수 / 전체 개수를 표시할 텍스트 (예: "선택됨: 3 / 15")
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SelectionRatioText;
	
    // 객관식 보기 4개 (배열로 묶어 다루면 편합니다)
    UPROPERTY(meta = (BindWidget))
    class UMultiLineEditableTextBox* ChoiceInput_1;
    UPROPERTY(meta = (BindWidget))
    class UMultiLineEditableTextBox* ChoiceInput_2;
    UPROPERTY(meta = (BindWidget))
    class UMultiLineEditableTextBox* ChoiceInput_3;
    UPROPERTY(meta = (BindWidget))
    class UMultiLineEditableTextBox* ChoiceInput_4;

    UPROPERTY(meta = (BindWidget))
    class UMultiLineEditableTextBox* AnswerInput;

    UPROPERTY(meta = (BindWidget))
    class UMultiLineEditableTextBox* InitialSoundHintInput;

    UPROPERTY(meta = (BindWidget))
    class UMultiLineEditableTextBox* HintInput;

    UPROPERTY(meta = (BindWidget))
    class USpinBox* ScoreSpinBox;

    UPROPERTY(meta = (BindWidget))
    class UMultiLineEditableTextBox* ExplanationInput;

    // 특정 타입 선택 시 관련 입력창만 켜고 끄기 위한 위젯 스위처
    UPROPERTY(meta = (BindWidget))
    class UWidgetSwitcher* TypeWidgetSwitcher;
	
    UPROPERTY(meta = (BindWidget))
    class UListView* QuestionListView;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_ResetImage;
	
    UPROPERTY(meta = (BindWidget))
    class UButton* AddButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ModifyButton;
	
    UPROPERTY(meta = (BindWidget))
    class UButton* DeleteButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_SelectImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* Image_Preview;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_SelectedImagePath;
	
    UPROPERTY(meta = (BindWidget))
    class UButton* BackToMenuButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_SaveQuiz;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_SelectAll;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_UnselectAll;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_LoadSelected;


	UFUNCTION()
	void OnLoadSelectedClicked();

	UFUNCTION()
	void OnResetImageClicked();
	
	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* FilterComboBox;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> SaveDialogClass;

    UFUNCTION()
    void OnQuestionTypeChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	
	UFUNCTION()
	void OnModifyButtonClicked();


    UFUNCTION()
    void OnAddClicked();

    UFUNCTION()
    void OnDeleteClicked();

    UFUNCTION()
    void OnBackToMenuClicked();

	UFUNCTION()
	void OnSaveButtonClicked();

	UFUNCTION()
	void OnSelectAllClicked();

	UFUNCTION()
	void OnUnselectAllClicked();

	UFUNCTION()
	void OnFilterChanged(FString SelectedItem, ESelectInfo::Type SelectionType);


	// 최종 저장할 이미지 경로를 담아둘 변수
	FString SelectedAbsoluteImagePath;

	UFUNCTION()
	void OnSelectImageClicked();
	
public:
    // 임시로 런타임 데이터를 들고 있을 배열 (나중에 SaveGame이나 파일로 저장할 타겟)
    TArray<FCB_DataTable_Question> TemporaryQuestionList;
    // 리스트뷰 갱신용 함수
    void RefreshListView();

	void UpdateCountTextsOnly();

	void SelectAndLoadQuestionData(const FCB_DataTable_Question& TargetData);
private:
    // UI 초기화용 (ComboBox 아이템 세팅 등)
    void InitializeUI();
	void ClearInputFields(); // 문제 추가 후 입력창을 비워주는 편의 함수

	int32 CurrentEditingIndex = INDEX_NONE;
	
};
