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
public:
	// 외부(자식 위젯)에서 화면을 바꿀 수 있도록 열어주는 함수
	void SwitchToPage(int32 PageIndex);
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UImage* BackgroundImage;

	// UPROPERTY(meta = (BindWidget))
	// class UTextBlock* TitleText;


	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UCB_AddQuestion* WBP_QuestionEditor;

	UPROPERTY(meta = (BindWidget))
	class UCB_GameSetup* WBP_GameSetup;
	
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

private:
	void SetupButtonStyle(UButton* pButton , FLinearColor Normal, FLinearColor Hovered, FLinearColor Pressed);
};
