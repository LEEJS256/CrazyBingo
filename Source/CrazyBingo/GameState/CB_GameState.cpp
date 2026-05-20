// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/CB_GameState.h"

void ACB_GameState::AddScore(int32 TeamIndex, int32 Amount)
{
	if (TeamIndex == 1)
		Team1Score += Amount;
	else if (TeamIndex == 2)
		Team2Score += Amount;
}

void ACB_GameState::ResetScores()
{
	Team1Score = 0;
	Team2Score = 0;
}
