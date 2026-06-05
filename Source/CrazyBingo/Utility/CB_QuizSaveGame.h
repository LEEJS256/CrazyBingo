// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Data/CB_DataTable_Question.h"
#include "CB_QuizSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class CRAZYBINGO_API UCB_QuizSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	// 사용자가 입력/수정한 퀴즈 데이터 리스트를 통째로 저장합니다.
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	TArray<FCB_DataTable_Question> SavedQuestions;

	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	FString SaveDateTimeString;
	
};
