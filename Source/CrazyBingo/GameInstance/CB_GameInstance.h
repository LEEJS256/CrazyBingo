// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Data/CB_DataTable_Question.h"
#include "CB_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class CRAZYBINGO_API UCB_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	// 기본 카테고리 + 편집화면에서 추가 가능
	UPROPERTY(BlueprintReadWrite)
	TArray<FString> Categories = { TEXT("과학"), TEXT("사회"), TEXT("예능"), TEXT("애니") };
	
	// 전체 문제 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FCB_DataTable_Question> Questions;

	// DataTable에서 문제 로드
	UFUNCTION(BlueprintCallable)
	void LoadQuestionsFromDataTable(UDataTable* DataTable);

	// 문제 추가
	UFUNCTION(BlueprintCallable)
	void AddQuestion(FCB_DataTable_Question Question);
	
	// 랜덤 25개 뽑기
	UFUNCTION(BlueprintCallable)
	TArray<FCB_DataTable_Question> GetRandomQuestions(int32 Count = 25);
};
