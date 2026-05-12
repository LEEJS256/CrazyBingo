// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CB_MainMenu.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UCB_MainMenu::NativeConstruct()
{
	Super::NativeConstruct();
	if (StartButton)
		StartButton->OnClicked.AddDynamic(this, &UCB_MainMenu::OnStartClicked);
	if (EditButton)
		EditButton->OnClicked.AddDynamic(this, &UCB_MainMenu::OnEditClicked);
	if (SettingButton)
		SettingButton->OnClicked.AddDynamic(this, &UCB_MainMenu::OnQuitClicked);
	if (QuitButton)
		QuitButton->OnClicked.AddDynamic(this, &UCB_MainMenu::OnQuitClicked);
}

void UCB_MainMenu::OnStartClicked()
{
	UGameplayStatics::OpenLevel(this, FName("BingoLevel"));
}

void UCB_MainMenu::OnEditClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Edit Clicked"));
}

void UCB_MainMenu::OnSettingClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Setting  Clicked"));
}

void UCB_MainMenu::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}
