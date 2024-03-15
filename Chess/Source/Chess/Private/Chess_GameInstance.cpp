// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_GameInstance.h"
#include <Chess_GameMode.h>

void UChess_GameInstance::ResetSignal()
{
	OnResetEvent.Broadcast();
}

void UChess_GameInstance::UpdateScoreSignal()
{
	OnScoreUpdate.Broadcast();
}


void UChess_GameInstance::IncrementScoreHumanPlayer()
{
	ScoreHumanPlayer += 1;
	UpdateScoreSignal();
}

void UChess_GameInstance::IncrementScoreAIPlayer()
{
	ScoreAIPlayer += 1;
	UpdateScoreSignal();
}

int32 UChess_GameInstance::GetScoreHumanPlayer()
{
	//return FMath::Rand();
	return ScoreHumanPlayer;
}

int32 UChess_GameInstance::GetScoreAIPlayer()
{
	//return FMath::Rand();
	return ScoreAIPlayer;
}

FString UChess_GameInstance::GetTurnMessage()
{
	return CurrentTurnMessage;
}

void UChess_GameInstance::SetTurnMessage(FString Message)
{
	CurrentTurnMessage = Message;
}

void UChess_GameInstance::ResetGame()
{
	//ScoreAIPlayer = 0;
	//ScoreHumanPlayer = 0;
	ScoreAIPlayer = FMath::Rand();
	ScoreHumanPlayer = FMath::Rand();
	UpdateScoreSignal();
	ResetSignal();
}
