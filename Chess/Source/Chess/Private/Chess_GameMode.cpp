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

	float CenterOnBoard = (this->Board->SquareSize * (this->Board->BoardSize / 2) - (this->Board->SquareSize / 2));
	FVector CameraPos(CenterOnBoard - 100, CenterOnBoard, 1000.0f);//TODO: Remove magic numbers
	HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotator::ZeroRotator);
	FRotator ActorRotation = HumanPlayer->GetActorRotation();
	ActorRotation.Pitch -= 90;//ruoto verso il basso
	ActorRotation.Yaw += 90;//in senso orario
	HumanPlayer->SetActorRotation(ActorRotation);

	CurrentPlayer = 0;

	Players.Add(HumanPlayer);

	AChess_RandomPlayer* RandomPlayer = GetWorld()->SpawnActor<AChess_RandomPlayer>(FVector(), FRotator());
	Players.Add(RandomPlayer);

	if (UChess_GameInstance* GameInstanceRef = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))) {
		GameInstanceRef->OnResetEvent.AddDynamic(this, &AChess_GameMode::ResetHandler);
	}

	ChoosePlayerAndStartGame();
}

void AChess_GameMode::ChoosePlayerAndStartGame()
{
	IsGameOver = false;
	Players[0]->OnTurn();
}

void AChess_GameMode::ToggleCurrentPlayer()
{
	CurrentPlayer++;
	MoveNumber += 1;
	if (!Players.IsValidIndex(CurrentPlayer))
	{
		CurrentPlayer = 0;
		TurnNumber += 1;
	}
	OnPlayerSwap.Broadcast();
}

void AChess_GameMode::ReplayMove(int32 moveNumber)
{
	OnReplayMove.Broadcast(moveNumber);
}

void AChess_GameMode::TurnNextPlayer()
{
	ToggleCurrentPlayer();
	if (!IsGameOver)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Cambio turno"));
		Board->RestoreBoardColors();
		Players[CurrentPlayer]->OnTurn();

		if(ControlChecks())//debug
		{
			IsGameOver = true;
		}
		else if (ControlStall())
		{
			IsGameOver = true;
		}

		//IsGameOver = mate ? mate : ControlStall();//first check for mate, then for stall
	}
	else
	{
		Players[CurrentPlayer]->OnWin();
	}
}

bool AChess_GameMode::ControlChecks() //TODO: Stall check
{
	bool mate = false;
	ChessColor colorToControl = CurrentPlayer == 0 ? WHITE : BLACK;

	if (Board->GetKingPosition(colorToControl)) {
		if (Board->CheckControl(colorToControl)) {
			if (Board->MateControl(colorToControl)) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("--------MATTO!!--------"));
				UE_LOG(LogTemp, Error, TEXT("--------MATTO!!--------"));
				mate = true;
			}
			if (Board->GetSquareFromXY(*Board->GetKingPosition(colorToControl)))
			{
				Board->GetSquareFromXY(*Board->GetKingPosition(colorToControl))->SetDangerColor();
			}
		}
	}
	
	return mate;
}

bool AChess_GameMode::ControlStall()
{
	ChessColor colorToControl = NAC;
	colorToControl = CurrentPlayer == 0 ? WHITE : BLACK;
	bool stall = Board->StallControl(colorToControl);
	if (stall) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("--------STALLO!!--------"));
		UE_LOG(LogTemp, Error, TEXT("--------STALLO!!--------"));
	}
	return stall;
}

void AChess_GameMode::ResetHandler()
{
	TurnNumber = 1;
	MoveNumber = 1;
	ChoosePlayerAndStartGame();
}
