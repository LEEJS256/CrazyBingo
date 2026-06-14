// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CB_GameSetup.h"

#include "CB_BingoBoard.h"
#include "CB_HostPanel.h"
#include "CB_MainMenu.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "GameInstance/CB_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/CB_QuizSaveGame.h"
#include "Utility/CB_SaveSlotData.h"
#include "Widgets/Layout/SDPIScaler.h"

void UCB_GameSetup::NativeConstruct()
{
	Super::NativeConstruct();

	// 1. 콤보박스 초기화
	if (BingoSizeComboBox)
	{
		BingoSizeComboBox->ClearOptions();
		BingoSizeComboBox->AddOption(TEXT("3 x 3"));
		BingoSizeComboBox->AddOption(TEXT("4 x 4"));
		BingoSizeComboBox->AddOption(TEXT("5 x 5"));
		BingoSizeComboBox->SetSelectedIndex(2); // 기본값 5x5
	}

	if (SaveFileListView)
		SaveFileListView->OnItemClicked().AddUObject(this, &UCB_GameSetup::OnSaveFileSelected);

	// 2. 버튼 이벤트 바인딩
	if (Btn_LaunchGame)
		Btn_LaunchGame->OnClicked.AddDynamic(this, &UCB_GameSetup::OnLaunchGameClicked);

	if (Btn_Cancel)
		Btn_Cancel->OnClicked.AddDynamic(this, &UCB_GameSetup::OnCancelClicked);

	// 3. 화면이 켜지자마자 세이브 파일 목록 로드
	SelectedFileQuestionCount = 0;
	RefreshSaveFileList();
	UpdateSetupValidation();
}


void UCB_GameSetup::OnLaunchGameClicked()
{
	// 1. 기본 UI 컴포넌트 체크
	if (!SaveFileListView || !BingoSizeComboBox) return;

	UObject* SelectedItem = SaveFileListView->GetSelectedItem();
	if (!IsValid(SelectedItem)) return;

	UCB_SaveSlotData* SelectedSlotData = Cast<UCB_SaveSlotData>(SelectedItem);
	if (!IsValid(SelectedSlotData)) return;

	// 2. 빙고 크기 가공
	FString SizeOption = BingoSizeComboBox->GetSelectedOption();
	int32 BingoSize = 5;
	if (SizeOption == TEXT("3 x 3"))
		BingoSize = 3;
	else if (SizeOption == TEXT("4 x 4"))
		BingoSize = 4;
	else if (SizeOption == TEXT("5 x 5"))
		BingoSize = 5;

	int32 RequiredQuestionCount = BingoSize * BingoSize;

	// 3. 세이브 파일 데이터 로드
	TArray<FCB_DataTable_Question> LoadedRawQuestions;
	FString DummyDate;
	if (!LoadQuestionsFromSlot(SelectedSlotData->SlotName, LoadedRawQuestions, DummyDate)) return;

	// 4. 게임 인스턴스 및 퀴즈 셔플 처리
	UCB_GameInstance* GI = Cast<UCB_GameInstance>(GetGameInstance());
	if (!IsValid(GI)) return;

	GI->Questions = LoadedRawQuestions;
	TArray<FCB_DataTable_Question> FinalBingoQuestions = GI->GetRandomQuestions(RequiredQuestionCount);

	// 5. 메인 메뉴 및 스위처 유효성 검사
	UCB_MainMenu* MasterMenu = Cast<UCB_MainMenu>(GetOuter()->GetOuter());
	if (!IsValid(MasterMenu)) return;

	int32 BingoPlayPageValue = 3;
	UWidgetSwitcher* MainSwitcher = Cast<UWidgetSwitcher>(MasterMenu->GetWidgetFromName(TEXT("MenuSwitcher")));
	if (!IsValid(MainSwitcher)) return;

	// 6. 빙고 보드 초기화
	UCB_BingoBoard* BingoBoard = Cast<UCB_BingoBoard>(MainSwitcher->GetWidgetAtIndex(BingoPlayPageValue));
	if (!IsValid(BingoBoard)) return;

	BingoBoard->InitBoard(BingoSize);

	// 7. 호스트 패널(외부 창) 생성 및 양방향 포인터 연동
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(PC)) return;

	UCB_HostPanel* HostPanel = CreateWidget<UCB_HostPanel>(PC, HostPanelClass);
	if (!IsValid(HostPanel)) return;

	HostPanel->TargetBingoBoard = BingoBoard;
	BingoBoard->TargetHostPanel = HostPanel;

	// 8. 독립 윈도우 창(SWindow) 띄우기
	TSharedRef<SWindow> NewWindow = SNew(SWindow)
		.Title(FText::FromString(TEXT("진행자 패널 (Host Panel)")))
		.ClientSize(FVector2D(800.f, 1200.f))
		.AutoCenter(EAutoCenter::PreferredWorkArea)
		.SupportsMaximize(false)
		.SupportsMinimize(true);

	NewWindow->SetContent(
		SNew(SDPIScaler)
		.DPIScale(1.0f)
		[
			HostPanel->TakeWidget()
		]
	);

	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().AddWindow(NewWindow);
	}

	// 9. 최종 페이지 전환
	MasterMenu->SwitchToPage(BingoPlayPageValue);
}

void UCB_GameSetup::OnCancelClicked()
{
	UCB_MainMenu* MasterMenu = Cast<UCB_MainMenu>(GetOuter()->GetOuter());
	if (MasterMenu)
	{
		MasterMenu->SwitchToPage(0); // 메인 메뉴 첫 페이지로 백업
	}
}

void UCB_GameSetup::InitHostPanel()
{
}

void UCB_GameSetup::RefreshSaveFileList()
{
	if (!SaveFileListView) return;
	SaveFileListView->ClearListItems();

	FString SaveDir = FPaths::ProjectSavedDir() + TEXT("SaveGames/");
	IFileManager& FileManager = IFileManager::Get();
	FString ExtensionMask = SaveDir + TEXT("*.sav");

	TArray<FString> FoundFiles;
	FileManager.FindFiles(FoundFiles, *ExtensionMask, true, false);

	for (const FString& FileName : FoundFiles)
	{
		FString SlotName = FPaths::GetBaseFilename(FileName);

		UCB_SaveSlotData* NewSlotObj = NewObject<UCB_SaveSlotData>(this);
		if (NewSlotObj)
		{
			NewSlotObj->SlotName = SlotName;
			NewSlotObj->bIsSelected = false;
			NewSlotObj->OwningGameSetup = this;

			SaveFileListView->AddItem(NewSlotObj);
		}
	}
}

bool UCB_GameSetup::LoadQuestionsFromSlot(const FString& SlotName, TArray<FCB_DataTable_Question>& OutQuestions,
                                          FString& OutSaveDate)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UCB_QuizSaveGame* LoadedGame = Cast<UCB_QuizSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if (LoadedGame)
		{
			OutQuestions = LoadedGame->SavedQuestions;

			OutSaveDate = LoadedGame->SaveDateTimeString.IsEmpty()
				              ? TEXT("2026-06-06")
				              : LoadedGame->SaveDateTimeString;
			return true;
		}
	}
	return false;
}


void UCB_GameSetup::UpdateSetupValidation()
{
	FString SizeOption = BingoSizeComboBox ? BingoSizeComboBox->GetSelectedOption() : TEXT("5 x 5");
	int32 BingoSize = 5;
	if (SizeOption == TEXT("3 x 3")) BingoSize = 3;
	else if (SizeOption == TEXT("4 x 4")) BingoSize = 4;
	else if (SizeOption == TEXT("5 x 5")) BingoSize = 5;

	int32 RequiredCount = BingoSize * BingoSize; // 필요한 칸 수 계산 (9, 16, 25)

	// 우측 상세 패널 "빙고 크기" 문자열 연동
	if (Text_InfoSize)
	{
		FString SizeStr = FString::Printf(TEXT("%d x %d (%d칸)"), BingoSize, BingoSize, RequiredCount);
		Text_InfoSize->SetText(FText::FromString(SizeStr));
	}
	
	UObject* SelectedItem = SaveFileListView ? SaveFileListView->GetSelectedItem() : nullptr;

	if (!SelectedItem)
	{

		if (Text_InfoCheck) Text_InfoCheck->SetText(FText::FromString(TEXT("플레이할 세이브 파일을 왼쪽에서 골라주세요.")));
		if (Text_InfoStatus) Text_InfoStatus->SetText(FText::FromString(TEXT("대기 중")));
		if (Btn_LaunchGame) Btn_LaunchGame->SetIsEnabled(false); // 버튼 잠금
		return;
	}

	bool bIsEnough = (SelectedFileQuestionCount >= RequiredCount);

	// 3. 목업 조건부 스타일 출력 이식
	if (bIsEnough)
	{
		// 문제 수 충분할 때
		if (Text_InfoCheck)
		{
			FString CheckStr = FString::Printf(TEXT("%dx%d 빙고는 최소 %d문제가 필요합니다.\n현재 %d문제 로드됨  ✓ 충분"),
			                                   BingoSize, BingoSize, RequiredCount, SelectedFileQuestionCount);
			Text_InfoCheck->SetText(FText::FromString(CheckStr));
		}

		if (Text_InfoStatus) Text_InfoStatus->SetText(FText::FromString(TEXT("● 준비 완료")));
		if (Btn_LaunchGame) Btn_LaunchGame->SetIsEnabled(true);
	}
	else
	{
		// 문제 수 부족해서 대참사 날 위험이 감지되었을 때
		if (Text_InfoCheck)
		{
			FString CheckStr = FString::Printf(TEXT("%dx%d 빙고는 최소 %d문제가 필요합니다.\n현재 %d문제 로드됨  ✗ 부족"),
			                                   BingoSize, BingoSize, RequiredCount, SelectedFileQuestionCount);
			Text_InfoCheck->SetText(FText::FromString(CheckStr));
		}

		if (Text_InfoStatus) Text_InfoStatus->SetText(FText::FromString(TEXT("● 문제 부족")));
		if (Btn_LaunchGame) Btn_LaunchGame->SetIsEnabled(false); 
	}
}

void UCB_GameSetup::OnSaveFileSelected(UObject* Item)
{
	UCB_SaveSlotData* SelectedSlotData = Cast<UCB_SaveSlotData>(Item);
	if (!SelectedSlotData) return;

	if (Text_InfoName) Text_InfoName->SetText(FText::FromString(SelectedSlotData->SlotName));

	TArray<FCB_DataTable_Question> TempQuestions;
	FString TargetDate = TEXT(""); // 날짜를 받아올 변수


	if (LoadQuestionsFromSlot(SelectedSlotData->SlotName, TempQuestions, TargetDate))
	{
		SelectedFileQuestionCount = TempQuestions.Num();

		if (Text_InfoCount)
		{
			FString CountStr = FString::Printf(TEXT("%d문제"), SelectedFileQuestionCount);
			Text_InfoCount->SetText(FText::FromString(CountStr));
		}
	}
	else
	{
		SelectedFileQuestionCount = 0;
		if (Text_InfoCount) Text_InfoCount->SetText(FText::FromString(TEXT("0문제")));
		TargetDate = TEXT("---- -- --");
	}
	
	if (Text_InfoDate)
	{
		Text_InfoDate->SetText(FText::FromString(TargetDate));
	}

	UpdateSetupValidation();
}

void UCB_GameSetup::OnBingoSizeChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UpdateSetupValidation();
}
