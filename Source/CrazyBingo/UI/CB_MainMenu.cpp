// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CB_MainMenu.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"

void UCB_MainMenu::SwitchToPage(int32 PageIndex)
{
	if (MenuSwitcher)
	{
		MenuSwitcher->SetActiveWidgetIndex(PageIndex);
	}
}

void UCB_MainMenu::NativeConstruct()
{
	Super::NativeConstruct();
	if (StartButton)
		StartButton->OnClicked.AddDynamic(this, &UCB_MainMenu::OnStartClicked);
	if (EditButton)
		EditButton->OnClicked.AddDynamic(this, &UCB_MainMenu::OnEditClicked);
	if (SettingButton)
		SettingButton->OnClicked.AddDynamic(this, &UCB_MainMenu::OnSettingClicked);
	if (QuitButton)
		QuitButton->OnClicked.AddDynamic(this, &UCB_MainMenu::OnQuitClicked);

	SetupButtonStyle(StartButton,
	                 FLinearColor(1.0f, 0.75f, 0.0f, 1.f), // Normal  - 골드
	                 FLinearColor(1.0f, 0.88f, 0.2f, 1.f), // Hovered - 밝은 골드
	                 FLinearColor(0.8f, 0.58f, 0.0f, 1.f) // Pressed - 어두운 골드
	); // Pressed


	SetupButtonStyle(EditButton,
	                 FLinearColor(1.0f, 0.75f, 0.0f, 1.f), // Normal  - 골드
	                 FLinearColor(1.0f, 0.88f, 0.2f, 1.f), // Hovered - 밝은 골드
	                 FLinearColor(0.8f, 0.58f, 0.0f, 1.f) // Pressed - 어두운 골드
	); // Pressed

	SetupButtonStyle(SettingButton,
	                 FLinearColor(1.0f, 0.75f, 0.0f, 1.f), // Normal  - 골드
	                 FLinearColor(1.0f, 0.88f, 0.2f, 1.f), // Hovered - 밝은 골드
	                 FLinearColor(0.8f, 0.58f, 0.0f, 1.f) // Pressed - 어두운 골드
	); // Pressed

	SetupButtonStyle(QuitButton,
	                 FLinearColor(1.0f, 0.75f, 0.0f, 1.f), // Normal  - 골드
	                 FLinearColor(1.0f, 0.88f, 0.2f, 1.f), // Hovered - 밝은 골드
	                 FLinearColor(0.8f, 0.58f, 0.0f, 1.f) // Pressed - 어두운 골드
	); // Pressed
}

void UCB_MainMenu::OnStartClicked()
{
	UGameplayStatics::OpenLevel(this, FName("BingoLevel"));
}

void UCB_MainMenu::OnEditClicked()
{
	SwitchToPage(1);
}

void UCB_MainMenu::OnSettingClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Setting  Clicked"));
}

void UCB_MainMenu::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}

void UCB_MainMenu::SetupButtonStyle(UButton* pButton, FLinearColor Normal, FLinearColor Hovered, FLinearColor Pressed)
{
	if (!pButton) return;

	FButtonStyle Style = pButton->GetStyle();
	Style.Normal.TintColor = FSlateColor(Normal);
	Style.Hovered.TintColor = FSlateColor(Hovered);
	Style.Pressed.TintColor = FSlateColor(Pressed);
	pButton->SetStyle(Style);
}
