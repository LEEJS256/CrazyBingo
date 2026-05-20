// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CB_AddQuestion.h"

#include "CB_MainMenu.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableText.h"
#include "Components/EditableTextBox.h"
#include "Components/SpinBox.h"
#include "Components/WidgetSwitcher.h"

void UCB_AddQuestion::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeUI();

	if (QuestionTypeComboBox)
		QuestionTypeComboBox->OnSelectionChanged.AddDynamic(this, &UCB_AddQuestion::OnQuestionTypeChanged);

	if (AddButton)
		AddButton->OnClicked.AddDynamic(this, &UCB_AddQuestion::OnAddClicked);
	if (DeleteButton)
		DeleteButton->OnClicked.AddDynamic(this, &UCB_AddQuestion::OnDeleteClicked);
	if (BackToMenuButton)
		BackToMenuButton->OnClicked.AddDynamic(this, &UCB_AddQuestion::OnBackToMenuClicked);
}

void UCB_AddQuestion::OnQuestionTypeChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (!TypeWidgetSwitcher)
		return;

	// 선택한 콤보박스 텍스트에 따라 인풋창 스위처 인덱스 변경
	if (SelectedItem == TEXT("객관식"))
		TypeWidgetSwitcher->SetActiveWidgetIndex(0);
	else if (SelectedItem == TEXT("초성"))
		TypeWidgetSwitcher->SetActiveWidgetIndex(1);
	else if (SelectedItem == TEXT("유튜브 링크"))
		TypeWidgetSwitcher->SetActiveWidgetIndex(2);
	else if (SelectedItem == TEXT("단답형"))
		TypeWidgetSwitcher->SetActiveWidgetIndex(3); // 단답형은 가변영역 비워둠
}

void UCB_AddQuestion::OnAddClicked()
{// 1. 현재 UI에 입력된 값들을 기반으로 구조체 생성
	FCB_DataTable_Question NewQuestion;
	NewQuestion.Category = CategoryInput ? CategoryInput->GetText().ToString() : TEXT("");
	NewQuestion.QuestionText = QuestionTextInput ? QuestionTextInput->GetText().ToString() : TEXT("");
	NewQuestion.Answer = AnswerInput ? AnswerInput->GetText().ToString() : TEXT("");
	NewQuestion.Hint = HintInput ? HintInput->GetText().ToString() : TEXT("");
	NewQuestion.Score = ScoreSpinBox ? FMath::RoundToInt(ScoreSpinBox->GetValue()) : 1;
	NewQuestion.Explanation = ExplanationInput ? ExplanationInput->GetText().ToString() : TEXT("");

	// 현재 콤보박스 인덱스로 타입 파싱
	int32 TypeIndex = QuestionTypeComboBox ? QuestionTypeComboBox->GetSelectedIndex() : 0;
	NewQuestion.QuestionType = static_cast<ECB_QuestionType>(TypeIndex);

	// 타입별 가변 데이터 수집
	if (NewQuestion.QuestionType == ECB_QuestionType::MultipleChoice)
	{
		if (ChoiceInput_1) NewQuestion.Choices.Add(ChoiceInput_1->GetText().ToString());
		if (ChoiceInput_2) NewQuestion.Choices.Add(ChoiceInput_2->GetText().ToString());
		if (ChoiceInput_3) NewQuestion.Choices.Add(ChoiceInput_3->GetText().ToString());
		if (ChoiceInput_4) NewQuestion.Choices.Add(ChoiceInput_4->GetText().ToString());
	}
	else if (NewQuestion.QuestionType == ECB_QuestionType::InitialSound)
	{
		NewQuestion.InitialSoundHint = InitialSoundHintInput ? InitialSoundHintInput->GetText().ToString() : TEXT("");
	}
	else if (NewQuestion.QuestionType == ECB_QuestionType::YoutubeLink)
	{
		NewQuestion.YoutubeURL = YoutubeURLInput ? YoutubeURLInput->GetText().ToString() : TEXT("");
	}

	// 2. 임시 리스트에 추가 후 리스트뷰 갱신
	TemporaryQuestionList.Add(NewQuestion);
	RefreshListView();

	UE_LOG(LogTemp, Log, TEXT("새로운 문제가 추가되었습니다. 총 개수: %d"), TemporaryQuestionList.Num());
}

void UCB_AddQuestion::OnDeleteClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("문제 삭제 버튼 클릭됨"));
}

void UCB_AddQuestion::OnBackToMenuClicked()
{
	UCB_MainMenu* MasterMenu = Cast<UCB_MainMenu>(GetOuter()->GetOuter());
	if (MasterMenu)
	{
		MasterMenu->SwitchToPage(0);
	}
}

void UCB_AddQuestion::InitializeUI()
{
	if (QuestionTypeComboBox)
	{
		QuestionTypeComboBox->ClearOptions();
		QuestionTypeComboBox->AddOption(TEXT("객관식"));
		QuestionTypeComboBox->AddOption(TEXT("초성"));
		QuestionTypeComboBox->AddOption(TEXT("유튜브 링크"));
		QuestionTypeComboBox->AddOption(TEXT("단답형"));
		QuestionTypeComboBox->SetSelectedIndex(0);
	}
}

void UCB_AddQuestion::RefreshListView()
{
	if (!QuestionListView)
		return;
}
