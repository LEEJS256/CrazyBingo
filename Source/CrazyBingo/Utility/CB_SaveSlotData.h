// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CB_SaveSlotData.generated.h"

/**
 * 
 */
UCLASS()
class CRAZYBINGO_API UCB_SaveSlotData : public UObject
{
	GENERATED_BODY()
public:
	
	UPROPERTY(BlueprintReadWrite, Category = "SaveData")
	FString SlotName;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData")
	bool bIsSelected = false;
};
