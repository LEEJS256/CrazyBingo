// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CB_AddQuestion.h"

#include "CB_MainMenu.h"
#include "CB_QuestionListData.h"
#include "CB_SaveDialog.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "ImageUtils.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableText.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/ListView.h"
#include "Components/MultiLineEditableText.h"
#include "Components/SpinBox.h"
#include "Components/TextBlock.h"
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

	if (Btn_SelectImage)
	{
		Btn_SelectImage->OnClicked.AddDynamic(this, &UCB_AddQuestion::OnSelectImageClicked);
	}

	if (FilterComboBox)
		FilterComboBox->OnSelectionChanged.AddDynamic(this, &UCB_AddQuestion::OnFilterChanged);


	if (Btn_LoadSelected)
	{
		Btn_LoadSelected->OnClicked.AddDynamic(this, &UCB_AddQuestion::OnLoadSelectedClicked);
	}
	
	RefreshListView();
}

void UCB_AddQuestion::OnLoadSelectedClicked()
{
	if (!QuestionListView) return;

	// 1. 리스트뷰의 모든 아이템을 돌며 체크박스가 켜진 대상을 찾습니다.
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

	// 2. 예외 처리 방어선 구축
	if (CheckedCount == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[불러오기 경고] 불러올 문제의 체크박스를 선택하지 않았습니다."));
		return;
	}
	if (CheckedCount > 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("[불러오기 경고] 불러오기는 한 번에 하나만 가능합니다. 하나만 체크해 주세요."));
		return;
	}

	// 3. 🎯 완벽하게 하나만 체크되었다면, 원본 배열에서 구조체를 꺼내 입력창에 세팅합니다!
	if (TemporaryQuestionList.IsValidIndex(TargetIndex))
	{
		SelectAndLoadQuestionData(TemporaryQuestionList[TargetIndex]);
		
		UE_LOG(LogTemp, Log, TEXT("[불러오기 완료] %d번 문제를 편집창으로 성공적으로 로드했습니다."), TargetIndex + 1);
	}
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

		EditedData.ImageAssetPath = SelectedAbsoluteImagePath;
		
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
		else if (CurrentType == TEXT("유튜브 링크")) NewData.QuestionType = ECB_QuestionType::YoutubeLink;
			// 🌟 보정: 콤보박스 스트링 매칭 일치화
		else if (CurrentType == TEXT("단답형")) NewData.QuestionType = ECB_QuestionType::ShortAnswer;
	}

	NewData.ImageAssetPath = SelectedAbsoluteImagePath;

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

void UCB_AddQuestion::OnFilterChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Log, TEXT("[필터 변경] %s 유형만 보기로 전환합니다."), *SelectedItem);

	// 필터 조건이 바뀌었으니 리스트뷰를 다시 그려줍니다.
	RefreshListView();
}

void UCB_AddQuestion::OnSelectImageClicked()
{
	if (GEngine && GEngine->GameViewport)
	{
		void* ParentWindowHandle = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
		IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

		if (DesktopPlatform)
		{
			TArray<FString> OpenFileNames;
			FString FileTypes = TEXT("Image Files (*.png;*.jpg;*.jpeg)|*.png;*.jpg;*.jpeg");

			// 🌟 Windows 파일 탐색기 팝업 열기
			bool bOpened = DesktopPlatform->OpenFileDialog(
				ParentWindowHandle,
				TEXT("문제에 사용할 이미지 선택"),
				TEXT(""),
				TEXT(""),
				FileTypes,
				EFileDialogFlags::None,
				OpenFileNames
			);

			if (bOpened && OpenFileNames.Num() > 0)
			{
				// 유저가 선택한 이미지의 실제 PC 절대 경로 획득!
				SelectedAbsoluteImagePath = OpenFileNames[0];

				if (Text_SelectedImagePath)
				{
					Text_SelectedImagePath->SetText(FText::FromString(SelectedAbsoluteImagePath));
				}

				// 📸 런타임에 외부 이미지를 동적으로 로드하여 UI 미리보기(Preview)에 띄우기
				UTexture2D* LoadedTexture = FImageUtils::ImportFileAsTexture2D(SelectedAbsoluteImagePath);
				if (LoadedTexture && Image_Preview)
				{
					Image_Preview->SetBrushFromTexture(LoadedTexture);
				}
			}
		}
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
	if (FilterComboBox)
	{
		FilterComboBox->ClearOptions();
		FilterComboBox->AddOption(TEXT("전체보기")); // 기본값
		FilterComboBox->AddOption(TEXT("객관식"));
		FilterComboBox->AddOption(TEXT("초성"));
		FilterComboBox->AddOption(TEXT("유튜브 링크"));
		FilterComboBox->AddOption(TEXT("단답형"));
		FilterComboBox->SetSelectedIndex(0); // "전체보기"가 맨 처음 선택되도록 설정
	}
}

void UCB_AddQuestion::RefreshListView()
{
	UListView* TargetListView = Cast<UListView>(GetWidgetFromName(TEXT("QuestionListView")));
	if (!TargetListView) return;

	if (!FilterComboBox) return;

	FString SelectedFilter = FilterComboBox->GetSelectedOption().TrimStartAndEnd();

	if (SelectedFilter.IsEmpty())
	{
		SelectedFilter = TEXT("전체보기");
	}

	TargetListView->ClearListItems();

	for (int32 i = 0; i < TemporaryQuestionList.Num(); ++i)
	{
		// 1. 🌟 현재 데이터 구조체의 퀴즈 타입(Enum)을 문자열로 치환합니다.
		FString QuestionTypeStr = TEXT("");
		switch (TemporaryQuestionList[i].QuestionType)
		{
		case ECB_QuestionType::MultipleChoice: QuestionTypeStr = TEXT("객관식");
			break;
		case ECB_QuestionType::InitialSound: QuestionTypeStr = TEXT("초성");
			break;
		case ECB_QuestionType::YoutubeLink: QuestionTypeStr = TEXT("유튜브 링크");
			break; // 🌟 보정 일치화
		case ECB_QuestionType::ShortAnswer: QuestionTypeStr = TEXT("단답형");
			break;
		default: break;
		}

		// 2. 🌟 카테고리가 아니라, 방금 변환한 '문제 타입 문자열'과 콤보박스 필터를 비교합니다!
		if (SelectedFilter != TEXT("전체보기") && QuestionTypeStr != SelectedFilter)
		{
			continue; // 필터와 일치하지 않는 타입의 문제는 패스
		}

		UCB_QuestionListData* NewDataObj = NewObject<UCB_QuestionListData>(this);
		if (NewDataObj)
		{
			NewDataObj->QuestionData = TemporaryQuestionList[i];
			NewDataObj->bIsCheckedForDelete = TemporaryQuestionList[i].bIsCheckedForDelete;

			// [여기서 부모 주소 각인!] 주머니에게 내 주소(this)를 전달합니다.
			NewDataObj->OwningAddQuestion = this;

			TargetListView->AddItem(NewDataObj);
		}
	}
	TargetListView->RequestRefresh();

	// 첫 화면 텍스트 세팅
	UpdateCountTextsOnly();
}

void UCB_AddQuestion::UpdateCountTextsOnly()
{
	UListView* TargetListView = Cast<UListView>(GetWidgetFromName(TEXT("QuestionListView")));
	if (!TargetListView) return;

	TArray<UObject*> AllItems = TargetListView->GetListItems();
	int32 TotalQuestions = TemporaryQuestionList.Num();
	int32 CheckedCount = 0;

	for (UObject* Item : AllItems)
	{
		UCB_QuestionListData* DataObj = Cast<UCB_QuestionListData>(Item);
		if (DataObj && DataObj->bIsCheckedForDelete) CheckedCount++;
	}

	if (TotalCountText) TotalCountText->SetText(FText::FromString(FString::Printf(TEXT("%d개"), TotalQuestions)));
	if (SelectionRatioText)
		SelectionRatioText->SetText(
			FText::FromString(FString::Printf(TEXT("%d / %d"), CheckedCount, TotalQuestions)));
}

void UCB_AddQuestion::SelectAndLoadQuestionData(const FCB_DataTable_Question& TargetData)
{
	// 1. 기존 텍스트 박스들에 데이터 채우기
	if (CategoryInput) CategoryInput->SetText(FText::FromString(TargetData.Category));
	if (QuestionTextInput) QuestionTextInput->SetText(FText::FromString(TargetData.QuestionText));
	if (YoutubeURLInput) YoutubeURLInput->SetText(FText::FromString(TargetData.YoutubeURL));
	if (AnswerInput) AnswerInput->SetText(FText::FromString(TargetData.Answer));
	if (InitialSoundHintInput) InitialSoundHintInput->SetText(FText::FromString(TargetData.InitialSoundHint));
	if (HintInput) HintInput->SetText(FText::FromString(TargetData.Hint));
	if (ExplanationInput) ExplanationInput->SetText(FText::FromString(TargetData.Explanation));

	if (ScoreSpinBox) ScoreSpinBox->SetValue(TargetData.Score);

	// 2. 객관식 보기 채우기 (배열 안전성 체크)
	if (ChoiceInput_1) ChoiceInput_1->SetText(TargetData.Choices.IsValidIndex(0)
		                                          ? FText::FromString(TargetData.Choices[0])
		                                          : FText::GetEmpty());
	if (ChoiceInput_2) ChoiceInput_2->SetText(TargetData.Choices.IsValidIndex(1)
		                                          ? FText::FromString(TargetData.Choices[1])
		                                          : FText::GetEmpty());
	if (ChoiceInput_3) ChoiceInput_3->SetText(TargetData.Choices.IsValidIndex(2)
		                                          ? FText::FromString(TargetData.Choices[2])
		                                          : FText::GetEmpty());
	if (ChoiceInput_4) ChoiceInput_4->SetText(TargetData.Choices.IsValidIndex(3)
		                                          ? FText::FromString(TargetData.Choices[3])
		                                          : FText::GetEmpty());

	// 3. 콤보박스 선택 전환
	if (QuestionTypeComboBox)
	{
		FString TypeString = TEXT("객관식");
		switch (TargetData.QuestionType)
		{
		case ECB_QuestionType::MultipleChoice: TypeString = TEXT("객관식");
			break;
		case ECB_QuestionType::InitialSound: TypeString = TEXT("초성");
			break;
		case ECB_QuestionType::YoutubeLink: TypeString = TEXT("유튜브 링크");
			break;
		case ECB_QuestionType::ShortAnswer: TypeString = TEXT("단답형");
			break;
		}
		QuestionTypeComboBox->SetSelectedOption(TypeString);
	}

	// 📸 4. 이미지 미리보기 창도 채워주기
	// 탐색기로 고른 임시 주소가 남아있다면 런타임 로드해서 프리뷰 띄움
	SelectedAbsoluteImagePath = TargetData.ImageAssetPath;
	if (Image_Preview)
	{
		if (!SelectedAbsoluteImagePath.IsEmpty())
		{
			UTexture2D* LoadedTexture = FImageUtils::ImportFileAsTexture2D(SelectedAbsoluteImagePath);
			if (LoadedTexture) Image_Preview->SetBrushFromTexture(LoadedTexture);
		}
		else
		{
			Image_Preview->SetBrushFromTexture(nullptr);
		}
	}

	if (Text_SelectedImagePath)
	{
		Text_SelectedImagePath->SetText(SelectedAbsoluteImagePath.IsEmpty()
			                                ? FText::FromString(TEXT("선택된 파일 없음"))
			                                : FText::FromString(SelectedAbsoluteImagePath));
	}
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
