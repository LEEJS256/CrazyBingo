// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CB_DataTable_Question.generated.h"


UENUM(BlueprintType)
enum class ECB_QuestionType : uint8
{
	MultipleChoice  UMETA(DisplayName = "객관식"),
	InitialSound    UMETA(DisplayName = "초성"),
	YoutubeLink     UMETA(DisplayName = "유튜브 링크"),
	ShortAnswer     UMETA(DisplayName = "단답형")
};


USTRUCT(BlueprintType)
struct FCB_DataTable_Question : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Quiz|UI")
	bool bIsCheckedForDelete = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Category = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECB_QuestionType QuestionType = ECB_QuestionType::MultipleChoice;
	
	// 문제 내용
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString QuestionText = TEXT("");

	// 유튜브 링크 (YoutubeLink 타입일 때만 사용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString YoutubeURL = TEXT("");

	// 객관식 보기 (최대 4개)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Choices;

	// 정답
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Answer = TEXT("");

	// 초성 힌트 (InitialSound 타입일 때만 사용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString InitialSoundHint = TEXT("");

	// 추가 힌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Hint = TEXT("");

	// 점수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Score = 1;

	// 해설 (진행자 화면에서만 표시)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Explanation = TEXT("");

};