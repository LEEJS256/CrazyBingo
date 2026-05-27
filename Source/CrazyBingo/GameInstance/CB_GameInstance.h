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

	
#pragma region SaveData
public:
	// 게임 시작 시 호출되는 초기화 함수 (여기서 자동 로드)
	virtual void Init() override;

	// 현재 메모리(런타임)에서 관리할 문제 리스트 마스터 배열
	UPROPERTY(BlueprintReadWrite, Category = "Quiz")
	TArray<FCB_DataTable_Question> QuizMasterList;

	// 런타임 배열을 파일로 저장하는 함수
	UFUNCTION(BlueprintCallable, Category = "Quiz|Save")
	void SaveQuizDataToFile();

	// 파일에서 데이터를 읽어와 런타임 배열에 채우는 함수
	UFUNCTION(BlueprintCallable, Category = "Quiz|Save")
	void LoadQuizDataFromFile();
	
#pragma endregion 
private:
	const FString SaveSlotName = TEXT("CrazyBingo_QuizSlot");
	const int32 UserIndex = 0;
};
