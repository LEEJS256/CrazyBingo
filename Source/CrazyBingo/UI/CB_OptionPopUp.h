// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CB_OptionPopUp.generated.h"

/**
 * 
 */
class UEditableTextBox;
class UButton;

UCLASS()
class CRAZYBINGO_API UCB_OptionPopUp : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	class  UComboBoxString* Combo_TargetBingoCount;


	UPROPERTY(meta = (BindWidget))
	UButton* Btn_ColorA_Option;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_ColorB_Option;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_ApplyAndClose;


	UFUNCTION()
	void OnColorAOptionClicked();

	UFUNCTION()
	void OnColorBOptionClicked();

	void OnColorAChanged(FLinearColor NewColor);
	void OnColorBChanged(FLinearColor NewColor);

	
	UFUNCTION() void OnApplyAndCloseClicked();

private:
	// 임시로 들고 있을 현재 설정 중인 색상 (확인을 눌러야 GI로 전송됨)
	FLinearColor CurrentSelectedColorA = FLinearColor::Red;
	FLinearColor CurrentSelectedColorB = FLinearColor::Blue;
	
};
