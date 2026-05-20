// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "CB_GameState.generated.h"

/**
 * 
 */
UCLASS()
class CRAZYBINGO_API ACB_GameState : public AGameState
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	FString Team1Name = TEXT("Team 1");

	UPROPERTY(BlueprintReadWrite)
	FString Team2Name = TEXT("Team 2");

	UPROPERTY(BlueprintReadWrite)
	int32 Team1Score = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 Team2Score = 0;

	void AddScore(int32 TeamIndex, int32 Amount);
	void ResetScores();
	
};
