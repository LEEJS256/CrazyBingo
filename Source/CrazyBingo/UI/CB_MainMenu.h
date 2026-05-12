// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CB_MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class CRAZYBINGO_API UCB_MainMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UImage* BackgroundImage;

	// UPROPERTY(meta = (BindWidget))
	// class UTextBlock* TitleText;

	UPROPERTY(meta = (BindWidget))
	class UButton* StartButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* EditButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* SettingButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;

	UFUNCTION()
	void OnStartClicked();

	UFUNCTION()
	void OnEditClicked();

	UFUNCTION()
	void OnSettingClicked();

	UFUNCTION()
	void OnQuitClicked();
};
