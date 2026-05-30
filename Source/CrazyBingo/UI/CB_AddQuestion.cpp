// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CB_AddQuestion.h"

#include "CB_MainMenu.h"
#include "CB_QuestionListData.h"
#include "CB_SaveDialog.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableText.h"
#include "Components/EditableTextBox.h"
#include "Components/ListView.h"
#include "Components/MultiLineEditableText.h"
#include "Components/SpinBox.h"
#include "Components/WidgetSwitcher.h"
#include "GameInstance/CB_GameInstance.h"

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
	if (Btn_SaveQuiz)
		Btn_SaveQuiz->OnClicked.AddDynamic(this, &UCB_AddQuestion::OnSaveButtonClicked);
	if (Btn_LoadQuiz)
		Btn_LoadQuiz->OnClicked.AddDynamic(this, &UCB_AddQuestion::OnLoadButtonClicked);

	RefreshListView();
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
		if (ChoiceInput_1)
			NewQuestion.Choices.Add(ChoiceInput_1->GetText().ToString());
		if (ChoiceInput_2)
			NewQuestion.Choices.Add(ChoiceInput_2->GetText().ToString());
		if (ChoiceInput_3)
			NewQuestion.Choices.Add(ChoiceInput_3->GetText().ToString());
		if (ChoiceInput_4)
			NewQuestion.Choices.Add(ChoiceInput_4->GetText().ToString());
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

	ClearInputFields();
	
	UE_LOG(LogTemp, Log, TEXT("새로운 문제가 추가되었습니다. 총 개수: %d"), TemporaryQuestionList.Num());
}

void UCB_AddQuestion::OnDeleteClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("선택 항목 일괄 삭제 버튼 클릭됨"));

	if (!QuestionListView) return;

	bool bAnyDeleted = false;

	// 1. 리스트뷰가 현재 화면(스냅샷)에 들고 있는 가상 데이터 오브젝트 목록을 싹 가져옵니다.
	TArray<UObject*> AllListViewItems = QuestionListView->GetListItems();

	// 2. 인덱스가 당겨지는 것을 방지하기 위해 반드시 '뒤에서부터(Reverse)' 순회합니다.
	for (int32 i = AllListViewItems.Num() - 1; i >= 0; --i)
	{
		// 리스트뷰 아이템을 데이터 오브젝트 타입으로 캐스팅
		UCB_QuestionListData* DataObj = Cast<UCB_QuestionListData>(AllListViewItems[i]);
        
		if (DataObj)
		{
			// 3. 🌟 화면에서 체크박스를 눌러 '데이터 오브젝트'의 값이 바뀐 것을 검사합니다!
			if (DataObj->bIsCheckedForDelete)
			{
				// 4. 리스트뷰의 인덱스(i)는 원본 TemporaryQuestionList의 인덱스와 1:1 대응하므로
				// 원본 배열에서 안전하게 해당 인덱스를 날려버립니다.
				if (TemporaryQuestionList.IsValidIndex(i))
				{
					TemporaryQuestionList.RemoveAt(i);
					bAnyDeleted = true;
                    
					UE_LOG(LogTemp, Log, TEXT("체크 확인되어 원본 배열에서 %d번째 항목 삭제 성공"), i);
				}
			}
		}
	}

	// 5. 하나라도 지워졌다면 리스트뷰를 다시 깔끔하게 렌더링합니다.
	if (bAnyDeleted)
	{
		UE_LOG(LogTemp, Log, TEXT("체크된 문제들이 원본 배열에서 성공적으로 일괄 삭제되었습니다."));
		RefreshListView();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("삭제할 항목이 체크되지 않았거나 원본 배열 반영에 실패했습니다."));
	}
}

void UCB_AddQuestion::OnBackToMenuClicked()
{
	UCB_MainMenu* MasterMenu = Cast<UCB_MainMenu>(GetOuter()->GetOuter());
	if (MasterMenu)
	{
		MasterMenu->SwitchToPage(0);
	}
}

void UCB_AddQuestion::OnSaveButtonClicked()
{
	// UCB_GameInstance* GI = Cast<UCB_GameInstance>(GetGameInstance());
	// if (!GI)
	// 	return;
	//
	// // 디버깅 로그: 저장하려는 순간 임시 배열에 데이터가 있는지 확인
	// UE_LOG(LogTemp, Log, TEXT("[세이브] 현재 임시 배열(TemporaryQuestionList) 항목 개수: %d개"), TemporaryQuestionList.Num());
	//
	// // 1. 현재 UI 클래스에서 관리 중인 원본 배열 데이터를 GameInstance 마스터 리스트로 통째로 이관합니다.
	// GI->QuizMasterList = TemporaryQuestionList;
	//
	// // 2. GameInstance에 구현된 파일 저장 함수 호출 (.sav 파일로 쓰기)
	// GI->SaveQuizDataToFile();
 //    
	// UE_LOG(LogTemp, Log, TEXT("[세이브 성공] 총 %d개의 문제가 안전하게 세이브 파일로 저장되었습니다."), GI->QuizMasterList.Num());

	if (!SaveDialogClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[메인 에러] SaveDialogClass가 지정되지 않았습니다."));
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	// 1. 팝업창 생성 (타입을 UCB_SaveDialog로 캐스팅해서 가져옵니다)
	UCB_SaveDialog* SaveDialog = CreateWidget<UCB_SaveDialog>(PC, SaveDialogClass);
	if (SaveDialog)
	{
		// 2. 🌟 생성 직후 팝업창에게 메인 화면인 내 주소(this)를 넘겨줍니다.
		SaveDialog->SetOwningWidget(this);

		// 3. 화면 배치 및 목록 갱신
		SaveDialog->AddToViewport(100);
		SaveDialog->RefreshSaveFileList();

		UE_LOG(LogTemp, Log, TEXT("[메인] 팝업창에 주소 이관 완료 후 화면에 표시합니다."));
	}
}

void UCB_AddQuestion::OnLoadButtonClicked()
{
	UCB_GameInstance* GI = Cast<UCB_GameInstance>(GetGameInstance());
	if (!GI) return;

	// 1. 파일에서 데이터를 읽어와 GameInstance의 마스터 리스트를 채웁니다.
	GI->LoadQuizDataFromFile();

	// 2. 파일에서 가져온 데이터를 현재 UI의 원본 리스트인 TemporaryQuestionList에 통째로 복사합니다.
	TemporaryQuestionList = GI->QuizMasterList;

	// 3. 🌟 이미 만들어 두신 갱신 함수를 호출하면, 리스트뷰 화면이 알아서 완벽하게 새로 그려집니다!
	RefreshListView();

	UE_LOG(LogTemp, Log, TEXT("[로드 성공] 파일로부터 총 %d개의 문제를 불러와 화면을 갱신했습니다."), TemporaryQuestionList.Num());
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
	UListView* TargetListView = QuestionListView;
	if (!TargetListView)
	{
		TargetListView = Cast<UListView>(GetWidgetFromName(TEXT("QuestionListView")));
	}

	if (!TargetListView)
	{
		UE_LOG(LogTemp, Error, TEXT("[메인 에러] RefreshListView 시점에 QuestionListView를 찾을 수 없습니다!"));
		return;
	}

	// 화면 초기화
	TargetListView->ClearListItems();

	UE_LOG(LogTemp, Log, TEXT("[메인] 복원할 문제 데이터 개수: %d개"), TemporaryQuestionList.Num());

	// 가공해둔 런타임 구조체 배열을 순회하며 ListView용 UObject로 변환 및 등록
	for (int32 i = 0; i < TemporaryQuestionList.Num(); ++i)
	{
		UCB_QuestionListData* NewDataObj = NewObject<UCB_QuestionListData>(this);
		NewDataObj->QuestionData = TemporaryQuestionList[i];

		// 확실하게 찾아온 리스트뷰에 주입
		TargetListView->AddItem(NewDataObj);
	}
    
	UE_LOG(LogTemp, Log, TEXT("[메인] 리스트뷰 화면 갱신 완료."));
	
}

void UCB_AddQuestion::ClearInputFields()
{
	if (QuestionTextInput)
		QuestionTextInput->SetText(FText::GetEmpty());
	if (AnswerInput)
		AnswerInput->SetText(FText::GetEmpty());
	if (HintInput)
		HintInput->SetText(FText::GetEmpty());
	if (ExplanationInput)
		ExplanationInput->SetText(FText::GetEmpty());

	// 타입별 서브 인풋들 초기화
	if (ChoiceInput_1)
		ChoiceInput_1->SetText(FText::GetEmpty());
	if (ChoiceInput_2)
		ChoiceInput_2->SetText(FText::GetEmpty());
	if (ChoiceInput_3)
		ChoiceInput_3->SetText(FText::GetEmpty());
	if (ChoiceInput_4)
		ChoiceInput_4->SetText(FText::GetEmpty());
	
	if (InitialSoundHintInput)
		InitialSoundHintInput->SetText(FText::GetEmpty());
	if (YoutubeURLInput)
		YoutubeURLInput->SetText(FText::GetEmpty());
}
