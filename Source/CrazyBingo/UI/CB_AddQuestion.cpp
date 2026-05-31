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
	if (ModifyButton)
		ModifyButton->OnClicked.AddDynamic(this, &UCB_AddQuestion::OnModifyButtonClicked);
	if (DeleteButton)
		DeleteButton->OnClicked.AddDynamic(this, &UCB_AddQuestion::OnDeleteClicked);
	if (BackToMenuButton)
		BackToMenuButton->OnClicked.AddDynamic(this, &UCB_AddQuestion::OnBackToMenuClicked);
	if (Btn_SaveQuiz)
		Btn_SaveQuiz->OnClicked.AddDynamic(this, &UCB_AddQuestion::OnSaveButtonClicked);
	// if (Btn_LoadQuiz)
	// 	Btn_LoadQuiz->OnClicked.AddDynamic(this, &UCB_AddQuestion::OnLoadButtonClicked);
	if (Btn_SelectAll)
		Btn_SelectAll->OnClicked.AddDynamic(this, &UCB_AddQuestion::OnSelectAllClicked);
	if (Btn_UnselectAll)
		Btn_UnselectAll->OnClicked.AddDynamic(this, &UCB_AddQuestion::OnUnselectAllClicked);
	if (Btn_ModifySelected)
		Btn_ModifySelected->OnClicked.AddDynamic(this, &UCB_AddQuestion::OnModifySelectedClicked);

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

void UCB_AddQuestion::OnModifySelectedClicked()
{
	if (!QuestionListView) return;

	TArray<UObject*> AllItems = QuestionListView->GetListItems();

	UCB_QuestionListData* SelectedDataObj = nullptr;
	int32 SelectedIndex = INDEX_NONE;
	int32 CheckedCount = 0;

	// 1. 체크박스 켜진 아이템 추적
	for (int32 i = 0; i < AllItems.Num(); ++i)
	{
		UCB_QuestionListData* DataObj = Cast<UCB_QuestionListData>(AllItems[i]);
		if (DataObj && DataObj->bIsCheckedForDelete)
		{
			SelectedDataObj = DataObj;
			SelectedIndex = i;
			CheckedCount++;
		}
	}

	// 2. 예외 처리 방어선
	if (CheckedCount == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[메인] 수정할 문제를 선택하지 않았습니다."));
		return;
	}
	if (CheckedCount > 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("[메인 경고] 문제는 한 번에 하나만 수정할 수 있습니다! 하나만 체크해 주세요."));
		return;
	}

	// 3. 데이터 역주입 시작
	if (SelectedDataObj)
	{
		const FCB_DataTable_Question& QData = SelectedDataObj->QuestionData;

		// 일반 텍스트 및 기본 정보 채우기
		if (CategoryInput) CategoryInput->SetText(FText::FromString(QData.Category));
		if (QuestionTextInput) QuestionTextInput->SetText(FText::FromString(QData.QuestionText));
		if (YoutubeURLInput) YoutubeURLInput->SetText(FText::FromString(QData.YoutubeURL));
		if (AnswerInput) AnswerInput->SetText(FText::FromString(QData.Answer));
		if (InitialSoundHintInput) InitialSoundHintInput->SetText(FText::FromString(QData.InitialSoundHint));
		if (HintInput) HintInput->SetText(FText::FromString(QData.Hint));
		if (ExplanationInput) ExplanationInput->SetText(FText::FromString(QData.Explanation));
		if (ScoreSpinBox) ScoreSpinBox->SetValue(static_cast<float>(QData.Score));

		// 🌟 [배열 맞춤 핵심] 객관식 보기 4개 역주입 (안전하게 인덱스 체크하면서 대입)
		if (ChoiceInput_1) ChoiceInput_1->SetText(QData.Choices.IsValidIndex(0)
			                                          ? FText::FromString(QData.Choices[0])
			                                          : FText::GetEmpty());
		if (ChoiceInput_2) ChoiceInput_2->SetText(QData.Choices.IsValidIndex(1)
			                                          ? FText::FromString(QData.Choices[1])
			                                          : FText::GetEmpty());
		if (ChoiceInput_3) ChoiceInput_3->SetText(QData.Choices.IsValidIndex(2)
			                                          ? FText::FromString(QData.Choices[2])
			                                          : FText::GetEmpty());
		if (ChoiceInput_4) ChoiceInput_4->SetText(QData.Choices.IsValidIndex(3)
			                                          ? FText::FromString(QData.Choices[3])
			                                          : FText::GetEmpty());

		// 퀴즈 타입 콤보박스 및 스위처 갱신
		if (QuestionTypeComboBox)
		{
			FString TypeStr = TEXT("객관식");
			switch (QData.QuestionType)
			{
			case ECB_QuestionType::MultipleChoice: TypeStr = TEXT("객관식");
				break;
			case ECB_QuestionType::InitialSound: TypeStr = TEXT("초성");
				break;
			case ECB_QuestionType::YoutubeLink: TypeStr = TEXT("유튜브");
				break;
			case ECB_QuestionType::ShortAnswer: TypeStr = TEXT("단답형");
				break;
			}
			QuestionTypeComboBox->SetSelectedOption(TypeStr);
			OnQuestionTypeChanged(TypeStr, ESelectInfo::Direct);
		}

		// 수정 모드 상태 저장
		CurrentEditingIndex = SelectedIndex;

		UE_LOG(LogTemp, Log, TEXT("[메인 성공] %d번 문제 데이터 복원 완료. 수정 후 등록을 누르세요."), SelectedIndex + 1);
	}
}

void UCB_AddQuestion::OnModifyButtonClicked()
{
	if (!QuestionListView) return;

	// 1. 현재 리스트뷰에 등록되어 있는 데이터 오브젝트들을 다 긁어모읍니다.
	TArray<UObject*> AllItems = QuestionListView->GetListItems();
	int32 TargetIndex = INDEX_NONE;
	int32 CheckedCount = 0;

	for (int32 i = 0; i < AllItems.Num(); ++i)
	{
		UCB_QuestionListData* DataObj = Cast<UCB_QuestionListData>(AllItems[i]);
		if (DataObj && DataObj->bIsCheckedForDelete)
		{
			TargetIndex = i;
			CheckedCount++;
		}
	}

	// 예외 처리 방어선
	if (CheckedCount == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[메인 경고] 수정(덮어쓰기)할 대상을 선택하지 않았습니다."));
		return;
	}
	if (CheckedCount > 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("[메인 경고] 수정은 한 번에 하나만 가능합니다."));
		return;
	}

	// 2. 입력창의 최신 데이터로 구조체 생성
	if (TemporaryQuestionList.IsValidIndex(TargetIndex))
	{
		FCB_DataTable_Question EditedData;

		if (CategoryInput) EditedData.Category = CategoryInput->GetText().ToString();
		if (QuestionTextInput) EditedData.QuestionText = QuestionTextInput->GetText().ToString();
		if (YoutubeURLInput) EditedData.YoutubeURL = YoutubeURLInput->GetText().ToString();
		if (AnswerInput) EditedData.Answer = AnswerInput->GetText().ToString();
		if (InitialSoundHintInput) EditedData.InitialSoundHint = InitialSoundHintInput->GetText().ToString();
		if (HintInput) EditedData.Hint = HintInput->GetText().ToString();
		if (ExplanationInput) EditedData.Explanation = ExplanationInput->GetText().ToString();
		if (ScoreSpinBox) EditedData.Score = FMath::RoundToInt(ScoreSpinBox->GetValue());

		if (ChoiceInput_1) EditedData.Choices.Add(ChoiceInput_1->GetText().ToString());
		if (ChoiceInput_2) EditedData.Choices.Add(ChoiceInput_2->GetText().ToString());
		if (ChoiceInput_3) EditedData.Choices.Add(ChoiceInput_3->GetText().ToString());
		if (ChoiceInput_4) EditedData.Choices.Add(ChoiceInput_4->GetText().ToString());

		if (QuestionTypeComboBox)
		{
			FString CurrentType = QuestionTypeComboBox->GetSelectedOption();
			if (CurrentType == TEXT("객관식")) EditedData.QuestionType = ECB_QuestionType::MultipleChoice;
			else if (CurrentType == TEXT("초성")) EditedData.QuestionType = ECB_QuestionType::InitialSound;
			else if (CurrentType == TEXT("유튜브 링크")) EditedData.QuestionType = ECB_QuestionType::YoutubeLink;
				// 🌟 보정: 콤보박스 아이템 등록명인 "유튜브 링크"와 일치해야 매핑됩니다!
			else if (CurrentType == TEXT("단답형")) EditedData.QuestionType = ECB_QuestionType::ShortAnswer;
		}

		// 덮어쓰기 후 체크박스는 깔끔하게 꺼지도록 false 처리
		EditedData.bIsCheckedForDelete = false;

		// 마스터 배열 데이터 완전히 덮어쓰기
		TemporaryQuestionList[TargetIndex] = EditedData;

		UE_LOG(LogTemp, Log, TEXT("[수정완료] %d번 문제 위치에 성공적으로 데이터를 덮어썼습니다."), TargetIndex + 1);

		// 3. 🌟 [핵심 수정] 입력란 비우고, 가상 리프레시가 아니라 "진짜 갱신(RefreshListView)"을 때려줍니다!
		ClearInputFields();
		RefreshListView();
	}
}

void UCB_AddQuestion::OnAddClicked()
{
	FCB_DataTable_Question NewData;
    
	// 기본 데이터 수집
	if (CategoryInput) NewData.Category = CategoryInput->GetText().ToString();
	if (QuestionTextInput) NewData.QuestionText = QuestionTextInput->GetText().ToString();
	if (YoutubeURLInput) NewData.YoutubeURL = YoutubeURLInput->GetText().ToString();
	if (AnswerInput) NewData.Answer = AnswerInput->GetText().ToString();
	if (InitialSoundHintInput) NewData.InitialSoundHint = InitialSoundHintInput->GetText().ToString();
	if (HintInput) NewData.Hint = HintInput->GetText().ToString();
	if (ExplanationInput) NewData.Explanation = ExplanationInput->GetText().ToString();
	if (ScoreSpinBox) NewData.Score = FMath::RoundToInt(ScoreSpinBox->GetValue());
    
	if (ChoiceInput_1) NewData.Choices.Add(ChoiceInput_1->GetText().ToString());
	if (ChoiceInput_2) NewData.Choices.Add(ChoiceInput_2->GetText().ToString());
	if (ChoiceInput_3) NewData.Choices.Add(ChoiceInput_3->GetText().ToString());
	if (ChoiceInput_4) NewData.Choices.Add(ChoiceInput_4->GetText().ToString());

	// 콤보박스 열거형 대입
	if (QuestionTypeComboBox)
	{
		FString CurrentType = QuestionTypeComboBox->GetSelectedOption();
		if (CurrentType == TEXT("객관식")) NewData.QuestionType = ECB_QuestionType::MultipleChoice;
		else if (CurrentType == TEXT("초성")) NewData.QuestionType = ECB_QuestionType::InitialSound;
		else if (CurrentType == TEXT("유튜브 링크")) NewData.QuestionType = ECB_QuestionType::YoutubeLink; // 🌟 보정: 콤보박스 스트링 매칭 일치화
		else if (CurrentType == TEXT("단답형")) NewData.QuestionType = ECB_QuestionType::ShortAnswer;
	}

	// 🌟 [핵심 수정] 복잡한 분기 걷어내고 기획대로 무조건 새 데이터 추가만 처리!
	TemporaryQuestionList.Add(NewData);
	UE_LOG(LogTemp, Log, TEXT("[메인] 새로운 문제가 신규 추가되었습니다."));

	// 청소 및 화면 새로고침
	ClearInputFields();
	RefreshListView();
}

void UCB_AddQuestion::OnDeleteClicked()
{
	UListView* TargetListView = Cast<UListView>(GetWidgetFromName(TEXT("QuestionListView")));
	if (!TargetListView) return;

	TArray<UObject*> AllItems = TargetListView->GetListItems();

	// 1. 역순(뒤에서부터)으로 삭제해야 배열 인덱스가 밀리지 않고 안전하게 삭제됩니다.
	for (int32 i = AllItems.Num() - 1; i >= 0; --i)
	{
		UCB_QuestionListData* DataObj = Cast<UCB_QuestionListData>(AllItems[i]);
		if (DataObj && DataObj->bIsCheckedForDelete)
		{
			if (TemporaryQuestionList.IsValidIndex(i))
			{
				TemporaryQuestionList.RemoveAt(i); // 원본 배열에서 컷!
			}
		}
	}

	// 2. 삭제되고 남은 데이터들로 화면을 깨끗하게 다시 그립니다.
	RefreshListView();

	UE_LOG(LogTemp, Log, TEXT("[선택 삭제 완료] 체크된 항목들이 정상적으로 제거되었습니다."));
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


void UCB_AddQuestion::OnSelectAllClicked()
{
	UListView* TargetListView = Cast<UListView>(GetWidgetFromName(TEXT("QuestionListView")));
	if (!TargetListView) return;

	// 1. 런타임 마스터 배열(TemporaryQuestionList)의 원본 값들을 확실하게 true로 변경 (& 필수)
	for (FCB_DataTable_Question& Question : TemporaryQuestionList)
	{
		Question.bIsCheckedForDelete = true;
	}

	// 2. 현재 살아있는 리스트뷰 데이터 객체들도 동기화 (안전장치)
	TArray<UObject*> AllItems = TargetListView->GetListItems();
	for (UObject* Item : AllItems)
	{
		UCB_QuestionListData* DataObj = Cast<UCB_QuestionListData>(Item);
		if (DataObj)
		{
			DataObj->bIsCheckedForDelete = true;
			DataObj->QuestionData.bIsCheckedForDelete = true;
		}
	}

	// 3. 위에서 수정한 확실한 true 값들을 기반으로 새 주머니들을 그리도록 호출!
	RefreshListView();
}

void UCB_AddQuestion::OnUnselectAllClicked()
{
	UListView* TargetListView = Cast<UListView>(GetWidgetFromName(TEXT("QuestionListView")));
	if (!TargetListView) return;

	// 1. 리스트뷰의 모든 데이터 주머니를 돌며 체크를 false로 변경
	TArray<UObject*> AllItems = TargetListView->GetListItems();
	for (UObject* Item : AllItems)
	{
		UCB_QuestionListData* DataObj = Cast<UCB_QuestionListData>(Item);
		if (DataObj)
		{
			DataObj->bIsCheckedForDelete = false;
			DataObj->QuestionData.bIsCheckedForDelete = false;
		}
	}

	// 2. 런타임 마스터 배열 동기화
	TemporaryQuestionList.Empty();
	for (UObject* Item : AllItems)
	{
		UCB_QuestionListData* DataObj = Cast<UCB_QuestionListData>(Item);
		if (DataObj)
		{
			TemporaryQuestionList.Add(DataObj->QuestionData);
		}
	}

	// 3. 화면 새로고침
	RefreshListView();
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
	UListView* TargetListView = Cast<UListView>(GetWidgetFromName(TEXT("QuestionListView")));
	if (!TargetListView) return;

	TargetListView->ClearListItems();

	// 마스터 배열(TemporaryQuestionList)을 돌면서 화면에 뿌릴 주머니를 새로 만듭니다.
	for (int32 i = 0; i < TemporaryQuestionList.Num(); ++i)
	{
		UCB_QuestionListData* NewDataObj = NewObject<UCB_QuestionListData>(this);

		// 1. 구조체 데이터 원본 복사
		NewDataObj->QuestionData = TemporaryQuestionList[i];

		// 🌟 [핵심 연결 코드가 빠졌을 확률 99%]
		// 구조체 원본에 들어있는 체크 상태(일괄 선택 시 true로 바뀐 값)를 
		// 새로 태어나는 데이터 주머니 변수(bIsCheckedForDelete)에도 명시적으로 꽂아줍니다!
		NewDataObj->bIsCheckedForDelete = TemporaryQuestionList[i].bIsCheckedForDelete;

		TargetListView->AddItem(NewDataObj);
	}

	TargetListView->RequestRefresh();
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
