// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CB_OptionPopUp.h"

#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Components/EditableTextBox.h"

#include "GameInstance/CB_GameInstance.h"

void UCB_OptionPopUp::NativeConstruct()
{
	Super::NativeConstruct();

	UCB_GameInstance* GI = Cast<UCB_GameInstance>(GetGameInstance());
	if (IsValid(Combo_TargetBingoCount))
	{
		Combo_TargetBingoCount->ClearOptions(); // 기존 옵션 청소
		Combo_TargetBingoCount->AddOption(TEXT("1"));
		Combo_TargetBingoCount->AddOption(TEXT("2"));
		Combo_TargetBingoCount->AddOption(TEXT("3"));
		Combo_TargetBingoCount->AddOption(TEXT("4"));
		Combo_TargetBingoCount->AddOption(TEXT("5"));

		// GameInstance에 저장되어 있던 기존 목표 빙고수가 있다면 콤보 박스 초기 선택값으로 매칭
		if (IsValid(GI))
		{
			FString DefaultOption = FString::FromInt(GI->GetTargetBingo());
			Combo_TargetBingoCount->SetSelectedOption(DefaultOption);
		}
	}

	Btn_ColorA_Option->OnClicked.AddDynamic(
		this,
		&UCB_OptionPopUp::OnColorAOptionClicked);

	Btn_ColorB_Option->OnClicked.AddDynamic(
		this,
		&UCB_OptionPopUp::OnColorBOptionClicked);

	if (IsValid(Btn_ApplyAndClose)) Btn_ApplyAndClose->OnClicked.AddDynamic(
		this, &UCB_OptionPopUp::OnApplyAndCloseClicked);
}


void UCB_OptionPopUp::OnColorAOptionClicked()
{
	FColorPickerArgs PickerArgs;
	
	PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateUObject(this, &UCB_OptionPopUp::OnColorAChanged);
	PickerArgs.InitialColor = CurrentSelectedColorA;
	
	OpenColorPicker(PickerArgs);
	UE_LOG(LogTemp, Log, TEXT("[C++ 설정창] 팀A 컬러 피커 소환 성공!"));


}

void UCB_OptionPopUp::OnColorBOptionClicked()
{
	FColorPickerArgs PickerArgs;
	
	PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateUObject(this, &UCB_OptionPopUp::OnColorBChanged);
	PickerArgs.InitialColor = CurrentSelectedColorA;
	
	OpenColorPicker(PickerArgs);
}

void UCB_OptionPopUp::OnColorAChanged(FLinearColor NewColor)
{
	CurrentSelectedColorA = NewColor;

	UCB_GameInstance* GI = Cast<UCB_GameInstance>(GetGameInstance());
	if (IsValid(GI))
	{
		GI->SetTeamColors2(CurrentSelectedColorA,true);
	}

	if (IsValid(Btn_ColorA_Option))
	{
		Btn_ColorA_Option->SetBackgroundColor(NewColor);
	}
	UE_LOG(LogTemp, Log, TEXT("[설정창] 컬러피커 수신 - 팀A 색상 변경: %s"), *NewColor.ToString());
}

void UCB_OptionPopUp::OnColorBChanged(FLinearColor NewColor)
{
	CurrentSelectedColorB = NewColor;

	UCB_GameInstance* GI = Cast<UCB_GameInstance>(GetGameInstance());
	if (IsValid(GI))
	{
		GI->SetTeamColors2(CurrentSelectedColorA,false);
	}
	
	if (IsValid(Btn_ColorB_Option))
	{
		Btn_ColorB_Option->SetBackgroundColor(NewColor);
	}
	UE_LOG(LogTemp, Log, TEXT("[설정창] 컬러피커 수신 - 팀B 색상 변경: %s"), *NewColor.ToString());
}

void UCB_OptionPopUp::OnApplyAndCloseClicked()
{
	UCB_GameInstance* GI = Cast<UCB_GameInstance>(GetGameInstance());
	if (!IsValid(GI))
	{
		RemoveFromParent();
		return;
	}

	if (IsValid(Combo_TargetBingoCount))
	{
		FString SelectedStr = Combo_TargetBingoCount->GetSelectedOption();
				
		int32 NewTargetCount = SelectedStr.IsNumeric() ? FCString::Atoi(*SelectedStr) : 3;
		
		GI->SetTargetBingoCount(NewTargetCount);
	}

	GI->SetTeamColors(CurrentSelectedColorA, CurrentSelectedColorB);

	RemoveFromParent();

}
