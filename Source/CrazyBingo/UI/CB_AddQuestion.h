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
    class UEditableTextBox* YoutubeURLInput;

    // 객관식 보기 4개 (배열로 묶어 다루면 편합니다)
    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox* ChoiceInput_1;
    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox* ChoiceInput_2;
    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox* ChoiceInput_3;
    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox* ChoiceInput_4;

    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox* AnswerInput;

    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox* InitialSoundHintInput;

    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox* HintInput;

    UPROPERTY(meta = (BindWidget))
    class USpinBox* ScoreSpinBox;

    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox* ExplanationInput;

    // 특정 타입 선택 시 관련 입력창만 켜고 끄기 위한 위젯 스위처
    UPROPERTY(meta = (BindWidget))
    class UWidgetSwitcher* TypeWidgetSwitcher;


    // =========================================================================
    // 2. 리스트 및 기능 제어 버튼
    // =========================================================================
    // 문제 리스트를 보여줄 뷰 (ListView를 쓰려면 별도의 리스트 항목용 위젯이 필요합니다)
    UPROPERTY(meta = (BindWidget))
    class UListView* QuestionListView;

    UPROPERTY(meta = (BindWidget))
    class UButton* AddButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* DeleteButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* BackToMenuButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_SaveQuiz;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_LoadQuiz;

    // =========================================================================
    // 3. 내부 이벤트 처리 함수
    // =========================================================================
    UFUNCTION()
    void OnQuestionTypeChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void OnAddClicked();

    UFUNCTION()
    void OnDeleteClicked();

    UFUNCTION()
    void OnBackToMenuClicked();

	UFUNCTION()
	void OnSaveButtonClicked();

	UFUNCTION()
	void OnLoadButtonClicked();

private:
    // UI 초기화용 (ComboBox 아이템 세팅 등)
    void InitializeUI();

    // 임시로 런타임 데이터를 들고 있을 배열 (나중에 SaveGame이나 파일로 저장할 타겟)
    TArray<FCB_DataTable_Question> TemporaryQuestionList;

    // 리스트뷰 갱신용 함수
    void RefreshListView();

	void ClearInputFields(); // 문제 추가 후 입력창을 비워주는 편의 함수
	
};
