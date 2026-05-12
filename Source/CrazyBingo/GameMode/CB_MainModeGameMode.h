// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CB_MainModeGameMode.generated.h"

/**
 * 
 */
class UCB_MainMenu;
UCLASS()
class CRAZYBINGO_API ACB_MainModeGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCB_MainMenu> MainMenuClass;
};
