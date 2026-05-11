// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CB_GameMode.generated.h"

/**
 * 
 */
class UCB_BingoBoard;
UCLASS()
class CRAZYBINGO_API ACB_GameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ACB_GameMode();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void StartGame();

	UFUNCTION(BlueprintCallable)
	void ResetGame();

protected:
	// 에디터에서 WBP_BingoBoard 지정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UCB_BingoBoard> BingoBoardClass;

	UPROPERTY()
	class UCB_BingoBoard* BingoBoard;
};
