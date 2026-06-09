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
	if (!SaveFileListView || !BingoSizeComboBox) return;

	UObject* SelectedItem = SaveFileListView->GetSelectedItem();
	if (!SelectedItem) return;

	UCB_SaveSlotData* SelectedSlotData = Cast<UCB_SaveSlotData>(SelectedItem);
	if (!SelectedSlotData) return;

	// 최종 크기 가공
	FString SizeOption = BingoSizeComboBox->GetSelectedOption();
	int32 BingoSize = 5;
	if (SizeOption == TEXT("3 x 3")) BingoSize = 3;
	else if (SizeOption == TEXT("4 x 4")) BingoSize = 4;
	else if (SizeOption == TEXT("5 x 5")) BingoSize = 5;

	int32 RequiredQuestionCount = BingoSize * BingoSize;


	TArray<FCB_DataTable_Question> LoadedRawQuestions;
	FString DummyDate;

	if (!LoadQuestionsFromSlot(SelectedSlotData->SlotName, LoadedRawQuestions, DummyDate))
		return;


	UCB_GameInstance* GI = Cast<UCB_GameInstance>(GetGameInstance());
	if (GI)
	{
		// 1. 불러온 퀴즈 팩을 게임 인스턴스 배열에 이관
		GI->Questions = LoadedRawQuestions;

		// 2. 인스턴스 내부에 미리 만들어두신 전매특허 함수를 호출하여 셔플 및 칼자르기 적용!
		TArray<FCB_DataTable_Question> FinalBingoQuestions = GI->GetRandomQuestions(RequiredQuestionCount);

		UE_LOG(LogTemp, Log, TEXT("[게임 세팅 최종 통과] %s 파일에서 무작위로 추출한 %d개의 퀴즈 패키징 완료!"),
		       *SelectedSlotData->SlotName, FinalBingoQuestions.Num());


		UCB_MainMenu* MasterMenu = Cast<UCB_MainMenu>(GetOuter()->GetOuter());
		if (MasterMenu)
		{
			int32 BingoPlayPageValue = 3;

			// 메인 메뉴의 위젯 스위처에서 스위처 컴포넌트를 찾아옵니다.
			if (UWidgetSwitcher* MainSwitcher = Cast<UWidgetSwitcher>(
				MasterMenu->GetWidgetFromName(TEXT("MenuSwitcher"))))
			{
				if (UCB_BingoBoard* BingoBoard = Cast<UCB_BingoBoard>(
					MainSwitcher->GetWidgetAtIndex(BingoPlayPageValue)))
				{
					// 🌟 [핵심 변경] 콤보박스 선택 가공 결과인 BingoSize(3, 4, 5)를 넘겨주며 보드를 조립합니다!
					BingoBoard->InitBoard(BingoSize);

					if (HostPanelClass)
					{
						APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
						if (PC)
						{
							// 1. 먼저 기존처럼 UMG 위젯을 생성하고 데이터를 연동합니다.
							UCB_HostPanel* HostPanel = CreateWidget<UCB_HostPanel>(PC, HostPanelClass);
							if (HostPanel)
							{
								HostPanel->TargetBingoBoard = BingoBoard;

								// 2. 🌟 진짜 독립된 OS 윈도우 창(SWindow)을 생성합니다.
								TSharedRef<SWindow> NewWindow = SNew(SWindow)
									.Title(FText::FromString(TEXT("진행자 패널 (Host Panel)"))) // 창 제목
									.ClientSize(FVector2D(600.f, 800.f))                    // 창 초기 크기
									.AutoCenter(EAutoCenter::PreferredWorkArea)            // 모니터 화면 중앙에 띄우기
									.SupportsMaximize(false)                               // 최대화 버튼 비활성화 (선택)
									.SupportsMinimize(true);                               // 최소화 버튼 활성화 (선택)

								// 3. 🌟 생성한 OS 창 내부에 우리 UMG 위젯(Slate 변환본)을 쏙 집어넣습니다!
								NewWindow->SetContent(HostPanel->TakeWidget());

								// 4. 🌟 엔진의 슬레이트 애플리케이션에 이 창을 등록하여 화면에 최종 출력합니다.
								if (FSlateApplication::IsInitialized())
								{
									FSlateApplication::Get().AddWindow(NewWindow);
								}

								UE_LOG(LogTemp, Log, TEXT("[호스트 패널] 게임 화면과 완전히 분리된 외부 독점 윈도우 창으로 팝업되었습니다."));
							}
						}
					}
				}
			}

			MasterMenu->SwitchToPage(BingoPlayPageValue);

			UE_LOG(LogTemp, Log, TEXT("[화면 전환] 빙고 플레이 화면(페이지 %d)으로 이동합니다. 빙고 보드가 스스로 조립됩니다."), BingoPlayPageValue);
		}
	}
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

			// 🌟 [추가] 데이터 주머니에게 메인 세팅창인 내 주소(this)를 전달합니다!
			// (이를 위해 UCB_SaveSlotData 클래스 내부에 OwningGameSetup 변수가 선언되어 있어야 합니다)
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

			// 🌟 파일에 저장되어 있던 진짜 날짜 문자열을 바깥으로 빼내줍니다!
			// 만약 옛날에 저장해서 날짜가 비어있다면 디폴트 날짜를 제공하는 안전장치 배치
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

	// 2. 🌟 유효성 체크 분기선 (충분한지 부족한지 실시간 판정)
	// 아무것도 선택하지 않았을 때의 방어선 예외 처리 포함
	UObject* SelectedItem = SaveFileListView ? SaveFileListView->GetSelectedItem() : nullptr;

	if (!SelectedItem)
	{
		// 파일 무선택 상태 가이드
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
		if (Btn_LaunchGame) Btn_LaunchGame->SetIsEnabled(false); // 🌟 [버튼 강제 차단!] 시작 방지
	}
}

void UCB_GameSetup::OnSaveFileSelected(UObject* Item)
{
	UCB_SaveSlotData* SelectedSlotData = Cast<UCB_SaveSlotData>(Item);
	if (!SelectedSlotData) return;

	if (Text_InfoName) Text_InfoName->SetText(FText::FromString(SelectedSlotData->SlotName));

	TArray<FCB_DataTable_Question> TempQuestions;
	FString TargetDate = TEXT(""); // 날짜를 받아올 변수

	// 🌟 로드 함수를 통해 문제와 날짜를 동시에 수집합니다.
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

	// 🌟 [하드코딩 탈출!] 세이브 파일이 가지고 있던 진짜 날짜를 UI에 꽂아줍니다!
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
