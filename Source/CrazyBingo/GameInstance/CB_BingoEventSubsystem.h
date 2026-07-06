// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/CB_DataTable_Question.h"
#include "CB_BingoEventSubsystem.generated.h"


DECLARE_MULTICAST_DELEGATE_TwoParams(FOnQuestionUpdatedSignature, int32, const FCB_DataTable_Question&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCellOwnerCommandSignature, uint8);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRerollRequestSignature, int32);

UCLASS()
class CRAZYBINGO_API UCB_BingoEventSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// 보드판 -> 호스트패널: "새로운 문제 정보가 최신화되었으니 화면 갱신해라"
	FOnQuestionUpdatedSignature OnQuestionUpdated;

	// 호스트패널 -> 보드판: "진행자가 특정 팀 정답/오답 판정 버튼을 눌렀다"
	FOnCellOwnerCommandSignature OnCellOwnerCommand;

	// 호스트패널 -> 보드판: "이 칸 문제 맘에 안 드니 리롤 돌려줘라"
	FOnRerollRequestSignature OnRerollRequest;
};
