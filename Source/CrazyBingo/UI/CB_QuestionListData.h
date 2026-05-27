// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/CB_DataTable_Question.h"
#include "UObject/NoExportTypes.h"
#include "CB_QuestionListData.generated.h"

/**
 * 
 */
UCLASS()
class CRAZYBINGO_API UCB_QuestionListData : public UObject
{
	GENERATED_BODY()

public:
	// 리스트뷰 한 칸이 들고 있을 실제 퀴즈 데이터 구조체
	FCB_DataTable_Question QuestionData;

	
	UPROPERTY(BlueprintReadWrite, Category = "Quiz")
	bool bIsCheckedForDelete = false;
	
	// // 삭제나 관리를 위해 보관할 배열 인덱스 (옵션)
	// int32 ListIndex;
};
