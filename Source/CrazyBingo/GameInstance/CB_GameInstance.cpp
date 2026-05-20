// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/CB_GameInstance.h"

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
