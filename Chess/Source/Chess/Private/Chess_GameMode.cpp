// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_GameMode.h"

AChess_GameMode::AChess_GameMode()
{
	//Variabili di GameModeBase:
	//PlayerControllerClass = AChess_PlayerController::StaticClass();
	//DefaultPawnClass = AChessHumanPlayer::StaticClass();

	//FieldSize = 8;
	UE_LOG(LogTemp, Error, TEXT("GameMode CREATA"));
}

AChess_GameMode::~AChess_GameMode()
{
}

void AChess_GameMode::BeginPlay()
{
	Super::BeginPlay();
	IsGameOver = false;

	if (BoardClass != nullptr)
	{
		Board = GetWorld()->SpawnActor<AChessboard>(BoardClass);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BoardClass is null, can not spawn Chessboard"));
	}

}

void AChess_GameMode::ChoosePlayerAndStartGame()
{
}
