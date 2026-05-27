// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/CB_GameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Utility/CB_QuizSaveGame.h"

void UCB_GameInstance::LoadQuestionsFromDataTable(UDataTable* DataTable)
{
	if (!DataTable)
		return;

	Questions.Empty();
	TArray<FCB_DataTable_Question*> Rows;
	DataTable->GetAllRows<FCB_DataTable_Question>(TEXT(""), Rows);

	for (FCB_DataTable_Question* Row : Rows)
	{
		if (Row) Questions.Add(*Row);
	}
}

void UCB_GameInstance::AddQuestion(FCB_DataTable_Question Question)
{
	Questions.Add(Question);
}


TArray<FCB_DataTable_Question> UCB_GameInstance::GetRandomQuestions(int32 Count)
{
	TArray<FCB_DataTable_Question> Shuffled = Questions;

	// 셔플
	for (int32 i = Shuffled.Num() - 1; i > 0; i--)
	{
		int32 j = FMath::RandRange(0, i);
		Shuffled.Swap(i, j);
	}

	// Count만큼 잘라서 반환
	int32 ActualCount = FMath::Min(Count, Shuffled.Num());
	return TArray<FCB_DataTable_Question>(Shuffled.GetData(), ActualCount);
}

void UCB_GameInstance::Init()
{
	Super::Init();
	
	LoadQuizDataFromFile();
}

void UCB_GameInstance::SaveQuizDataToFile()
{
	UCB_QuizSaveGame* SaveGameInstance = Cast<UCB_QuizSaveGame>(UGameplayStatics::CreateSaveGameObject(UCB_QuizSaveGame::StaticClass()));
    
	if (SaveGameInstance)
	{
		// 2. 현재 마스터 리스트 데이터를 세이브 객체에 복사
		SaveGameInstance->SavedQuestions = QuizMasterList;

		// 3. 디스크에 파일로 저장 (.sav)
		bool bSuccess = UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveSlotName, UserIndex);
		if (bSuccess)
		{
			UE_LOG(LogTemp, Log, TEXT("퀴즈 데이터가 성공적으로 저장되었습니다! 총 %d개"), QuizMasterList.Num());
		}
	}
}

void UCB_GameInstance::LoadQuizDataFromFile()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
	{
		// 2. 파일 로드 및 캐스팅
		UCB_QuizSaveGame* LoadedGame = Cast<UCB_QuizSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
        
		if (LoadedGame)
		{
			// 3. 파일에 있던 데이터를 게임 마스터 리스트로 복사
			QuizMasterList = LoadedGame->SavedQuestions;
			UE_LOG(LogTemp, Log, TEXT("저장된 퀴즈 데이터를 불러왔습니다. 총 %d개"), QuizMasterList.Num());
			return;
		}
	}

	// 저장된 파일이 없거나 로드 실패 시 비어있는 상태 유지 (혹은 기본 DataTable에서 기본값 채우기 가능)
	UE_LOG(LogTemp, Warning, TEXT("저장된 퀴즈 데이터 파일이 없습니다. 새로운 리스트를 시작합니다."));
	QuizMasterList.Empty();
}
