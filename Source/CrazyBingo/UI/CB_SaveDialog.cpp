// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CB_SaveDialog.h"

#include "CB_AddQuestion.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/ListView.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/CB_GameInstance.h"
#include "Utility/CB_SaveSlotData.h"

void UCB_SaveDialog::SetOwningWidget(UCB_AddQuestion* InParent)
{ OwningAddQuestionWidget = InParent; }

void UCB_SaveDialog::NativeConstruct()
{
	Super::NativeConstruct();

	if (SaveFileList)
		SaveFileList->OnItemClicked().AddUObject(this, &UCB_SaveDialog::OnSlotItemClicked);
    
	if (Btn_Confirm)
		Btn_Confirm->OnClicked.AddDynamic(this, &UCB_SaveDialog::OnConfirmClicked);

	if (Btn_Cancel)
		Btn_Cancel->OnClicked.AddDynamic(this, &UCB_SaveDialog::OnCancelClicked);

	if (Btn_Load)
		Btn_Load->OnClicked.AddDynamic(this, &UCB_SaveDialog::OnLoadClicked);
	// 창이 켜질 때 목록 새로고침
	RefreshSaveFileList();
}

void UCB_SaveDialog::OnSlotItemClicked(UObject* Item)
{
	UCB_SaveSlotData* ClickedData = Cast<UCB_SaveSlotData>(Item);
	if (ClickedData && NewFileNameInput)
	{
		// 클릭한 기존 파일명이 텍스트박스에 자동으로 입력됩니다. (이 상태로 저장하면 덮어쓰기가 됨!)
		NewFileNameInput->SetText(FText::FromString(ClickedData->SlotName));
	}
}

void UCB_SaveDialog::OnConfirmClicked()
{
	if (!NewFileNameInput || NewFileNameInput->GetText().IsEmpty()) return;

	UCB_GameInstance* GI = Cast<UCB_GameInstance>(GetGameInstance());
	if (!GI) return;

	FString TargetSlotName = NewFileNameInput->GetText().ToString();
	GI->CurrentSaveSlotName = TargetSlotName;

	// 🌟 꼬이기 쉬운 GetOuter() 대신 확실하게 등록된 메인 위젯 주소를 사용합니다.
	if (OwningAddQuestionWidget)
	{
		GI->QuizMasterList = OwningAddQuestionWidget->TemporaryQuestionList;
		GI->SaveQuizDataToFile();
		UE_LOG(LogTemp, Log, TEXT("[팝업] 메인 화면의 데이터를 %s.sav 파일에 성공적으로 저장했습니다!"), *TargetSlotName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[팝업 에러] OwningAddQuestionWidget 주소가 없습니다!"));
	}

	RemoveFromParent();
}

void UCB_SaveDialog::OnLoadClicked()
{
	if (!SaveFileList) return;

	UCB_GameInstance* GI = Cast<UCB_GameInstance>(GetGameInstance());
	if (!GI) return;

	TArray<UObject*> AllItems = SaveFileList->GetListItems();
	FString SelectedSlotName = TEXT("");

	// 1. 체크박스가 켜진 아이템을 찾습니다.
	for (UObject* Item : AllItems)
	{
		UCB_SaveSlotData* SlotData = Cast<UCB_SaveSlotData>(Item);
		if (SlotData && SlotData->bIsSelected)
		{
			SelectedSlotName = SlotData->SlotName;
			break; 
		}
	}

	// [방어 코드] 만약 체크박스가 안 켜져 있다면, 인풋 텍스트 박스에 적힌 이름이라도 대안으로 가져옵니다.
	if (SelectedSlotName.IsEmpty() && NewFileNameInput && !NewFileNameInput->GetText().IsEmpty())
	{
		SelectedSlotName = NewFileNameInput->GetText().ToString();
	}

	// 2. 만약 어떻게든 선택된 파일 이름이 없다면 함수를 종료합니다.
	if (SelectedSlotName.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[팝업 경고] 불러올 세이브 파일 이름이 선택되거나 입력되지 않았습니다!"));
		return;
	}

	// 3. ★ 중요: 엔진에게 "우리가 방금 저장한 그 파일 이름을 읽어라"라고 명확히 지시합니다.
	GI->CurrentSaveSlotName = SelectedSlotName;
    
	// 4. 이제 파일에서 진짜 데이터를 긁어옵니다.
	GI->LoadQuizDataFromFile();

	// 5. 메인 화면 리스트뷰에 갱신 주입
	if (OwningAddQuestionWidget)
	{
		OwningAddQuestionWidget->TemporaryQuestionList = GI->QuizMasterList;
		OwningAddQuestionWidget->RefreshListView();
       
		UE_LOG(LogTemp, Log, TEXT("[팝업 성공] 최종적으로 [%s.sav] 파일을 타겟팅하여 %d개의 문제를 로드했습니다!"), 
			   *SelectedSlotName, OwningAddQuestionWidget->TemporaryQuestionList.Num());
	}

	RemoveFromParent();
}


void UCB_SaveDialog::OnCancelClicked()
{
	RemoveFromParent(); // 취소 시 그냥 닫기
}

void UCB_SaveDialog::RefreshSaveFileList()
{
	if (!SaveFileList) return;
	SaveFileList->ClearListItems();

	// 1. 언리얼 세이브 폴더의 상대 경로를 가져옵니다. (Saved/SaveGames/)
	FString RelativeSaveDir = FPaths::ProjectSavedDir() + TEXT("SaveGames/");
    
	// 2. ★ 중요: 상대 경로를 컴퓨터가 확실히 인식할 수 있는 '절대 경로'로 완벽하게 변환합니다.
	FString AbsoluteSaveDir = FPaths::ConvertRelativePathToFull(RelativeSaveDir);
    
	// 디버깅용 로그: 폴더가 실제로 어디를 가리키고 있는지 로그창에 찍어줍니다.
	UE_LOG(LogTemp, Log, TEXT("[팝업] 탐색할 세이브 파일 실제 절대 경로: %s"), *AbsoluteSaveDir);

	IFileManager& FileManager = IFileManager::Get();
    
	// 3. 해당 절대 경로에 폴더가 없다면 자동으로 만들어 줍니다. (안전장치)
	if (!FileManager.DirectoryExists(*AbsoluteSaveDir))
	{
		FileManager.MakeDirectory(*AbsoluteSaveDir, true);
	}

	// 4. 폴더 내의 모든 .sav 파일을 검색합니다.
	TArray<FString> Files;
	//FileManager.FindFiles 함수는 찾을 파일들의 확장자 필터(예: *.sav)를 포함한 경로를 요구합니다.
	FString FilterPath = AbsoluteSaveDir + TEXT("*.sav");
	FileManager.FindFiles(Files, *FilterPath, true, false);

	UE_LOG(LogTemp, Log, TEXT("[팝업] 발견된 총 세이브 파일 개수: %d개"), Files.Num());

	// 5. 찾아낸 파일 이름들을 리스트뷰 데이터로 래핑해서 넣어주기
	for (const FString& File : Files)
	{
		// 확장자(.sav)를 떼고 순수 파일명만 추출 (예: "MyQuiz.sav" -> "MyQuiz")
		FString SlotName = FPaths::GetBaseFilename(File);
        
		UCB_SaveSlotData* NewData = NewObject<UCB_SaveSlotData>(this);
		NewData->SlotName = SlotName;
        
		SaveFileList->AddItem(NewData);
	}
}
