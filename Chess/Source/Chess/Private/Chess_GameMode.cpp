// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_GameMode.h"
#include "Chess_PlayerController.h"
#include "Chess_HumanPlayer.h"
#include "Square.h"
#include "Engine.h"
#include "EngineUtils.h"
#include <Chess_RandomPlayer.h>

AChess_GameMode::AChess_GameMode()
{
	//Variabili di GameModeBase:
	PlayerControllerClass = AChess_PlayerController::StaticClass();
	DefaultPawnClass = AChess_HumanPlayer::StaticClass();
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
	AChess_HumanPlayer* HumanPlayer = Cast<AChess_HumanPlayer>(*TActorIterator<AChess_HumanPlayer>(GetWorld()));

	if (BoardClass != nullptr)
	{
		Board = GetWorld()->SpawnActor<AChessboard>(BoardClass);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BoardClass is null, can not spawn Chessboard"));
	}

	float CameraPosX = (this->Board->SquareSize * (this->Board->BoardSize / 2) - (this->Board->SquareSize / 2));
	FVector CameraPos(CameraPosX, CameraPosX, 1000.0f);
	HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotator::ZeroRotator);
	FRotator ActorRotation = HumanPlayer->GetActorRotation();
	ActorRotation.Pitch -= 90;//ruoto verso il basso
	ActorRotation.Yaw += 90;//in senso orario
	HumanPlayer->SetActorRotation(ActorRotation);

	CurrentPlayer = 0;

	Players.Add(HumanPlayer);

	AChess_RandomPlayer* RandomPlayer = GetWorld()->SpawnActor<AChess_RandomPlayer>(FVector(), FRotator());
	Players.Add(RandomPlayer);

	ChoosePlayerAndStartGame();
}

void AChess_GameMode::ChoosePlayerAndStartGame()
{
	Players[0]->OnTurn();
}

void AChess_GameMode::ToggleCurrentPlayer()
{
	CurrentPlayer++;
	if (!Players.IsValidIndex(CurrentPlayer))
	{
		CurrentPlayer = 0;
	}
}

void AChess_GameMode::TurnNextPlayer()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Cambio turno"));
	ToggleCurrentPlayer();
	Players[CurrentPlayer]->OnTurn();
	Board->RestoreBoardColors();
	ControlChecks();
}

void AChess_GameMode::ControlChecks() 
{
	Board->CheckControl(WHITE);
	Board->CheckControl(BLACK);
}