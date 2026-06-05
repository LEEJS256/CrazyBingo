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
		FDateTime CurrentTime = FDateTime::Now();
		FString FormattedDate = FString::Printf(TEXT("%04d-%02d-%02d"), CurrentTime.GetYear(), CurrentTime.GetMonth(), CurrentTime.GetDay());
		
		// 🌟 마스터 리스트를 세이브 객체의 'SavedQuestions'에 복사
		SaveGameInstance->SavedQuestions = QuizMasterList;

		UGameplayStatics::SaveGameToSlot(SaveGameInstance, CurrentSaveSlotName, UserIndex);
	}
}

void UCB_GameInstance::LoadQuizDataFromFile()
{
	if (UGameplayStatics::DoesSaveGameExist(CurrentSaveSlotName, UserIndex))
	{
		UCB_QuizSaveGame* LoadedGame = Cast<UCB_QuizSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSaveSlotName, UserIndex));
		if (LoadedGame)
		{
			// 🌟 세이브 객체의 'SavedQuestions'에서 마스터 리스트로 복사
			QuizMasterList = LoadedGame->SavedQuestions;
			return;
		}
	}
	QuizMasterList.Empty();
}
