// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_GameMode.h"
#include "Chess_PlayerController.h"
#include "Chess_HumanPlayer.h"
#include "Chess_RandomPlayer.h"
#include "Chess_MinimaxPlayer.h"
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
	bIsGameOver = false;
	int32 difficulty_level = -1;
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
	FVector CameraPos(CenterOnBoard + 100, CenterOnBoard, 1000.0f);//TODO: Remove magic numbers
	HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotator::ZeroRotator);
	FRotator ActorRotation = HumanPlayer->GetActorRotation();
	ActorRotation.Pitch -= 90;//ruoto verso il basso
	ActorRotation.Yaw -= 90;//in senso orario
	HumanPlayer->SetActorRotation(ActorRotation);
	

	CurrentPlayer = 0;

	Players.Add(HumanPlayer);

	if (UChess_GameInstance* GI = Cast<UChess_GameInstance>(GetGameInstance()))
	{
		difficulty_level = GI->GameDifficulty;
	}

	switch (difficulty_level)
	{
	case 0:
	{
		AChess_RandomPlayer* RandomPlayer = GetWorld()->SpawnActor<AChess_RandomPlayer>(FVector(), FRotator());
		Players.Add(RandomPlayer);
		break;
	}
	case 1:
	{
		AChess_MinimaxPlayer* MinimaxPlayer = GetWorld()->SpawnActor<AChess_MinimaxPlayer>(FVector(), FRotator());
		Players.Add(MinimaxPlayer);
		break;
	}
	default://By default start with random player
		AChess_RandomPlayer* RandomPlayer = GetWorld()->SpawnActor<AChess_RandomPlayer>(FVector(), FRotator());
		Players.Add(RandomPlayer);
		UE_LOG(LogTemp, Error, TEXT("No Game difficulty found, base difficulty selected (random player)"));

		break;
	}

	if (UChess_GameInstance* GameInstanceRef = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))) {
		GameInstanceRef->OnResetEvent.AddDynamic(this, &AChess_GameMode::ResetHandler);
	}

	ChoosePlayerAndStartGame();
}

void AChess_GameMode::ChoosePlayerAndStartGame()
{
	bIsGameOver = false;
	Players[0]->Color == ChessColor::WHITE ? Players[0]->OnTurn() : Players[1]->OnTurn();
	//Players[0]->OnTurn();
}

void AChess_GameMode::ToggleCurrentPlayer()
{
	CurrentPlayer++;
	NextMoveNumber += 1;
	NextActualMoveNumber += 1;
	if (!Players.IsValidIndex(CurrentPlayer))
	{
		CurrentPlayer = 0;
		TurnNumber += 1;
	}
	OnPlayerSwap.Broadcast();
}

void AChess_GameMode::UpdateLastMove(TSharedPtr<Chess_Move> move)
{
	if (move)
	{
		const FString moveString = move->ToString();
		OnMoveUpdate.Execute(moveString, NextMoveNumber);
	}
}

void AChess_GameMode::ShowPromotionWidget(ChessColor playerColor)
{
	if (playerColor == Players[0]->Color) {//if is the human player
		OnShowPromotionWidget.Execute(playerColor);
		bMustSelectPiecePromotion = true;
	}
}

void AChess_GameMode::GoBackToActualMove()
{
	NextMoveNumber = NextActualMoveNumber;
	TurnNumber = (NextActualMoveNumber - 1) % 2 == 0 ? (NextActualMoveNumber - 1) / 2 + 1 : (NextActualMoveNumber - 1) / 2 + 1;
	bIsOnReplay = false;
	OnTurnGoBack.Broadcast(NextActualMoveNumber - 1);
}

void AChess_GameMode::SelectedPawnPromotionHandler(CP ChessPieceEnum)
{
	TSharedPtr<Chess_Move> move = Board->PromoteLastMove(ChessPieceEnum);
	UpdateLastMove(move);//write the promotion in the storyboard
	bMustSelectPiecePromotion = false;
	if (ControlChecks())//debug
	{
		bIsGameOver = true;
		CurrentPlayer == 0 ? Players[1]->OnWin() : Players[0]->OnWin();
	}
	else if (ControlStall())
	{
		bIsGameOver = true;
	}
	TurnNextPlayer();
}

void AChess_GameMode::ReplayMove(int32 moveNumber)
{
	NextActualMoveNumber = moveNumber + 1;
	bIsOnReplay = (NextActualMoveNumber != NextMoveNumber);
	OnReplayMove.Broadcast(moveNumber);
}

void AChess_GameMode::TurnNextPlayer()
{
	if (!bMustSelectPiecePromotion)
	{
		//Board->OnMove.Broadcast(Board->ToString());//TODO: Update pawn promotion 
		ToggleCurrentPlayer();
		if (!bIsGameOver)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Cambio turno"));
			Board->RestoreBoardColors();
			Players[CurrentPlayer]->OnTurn();
			UChess_GameInstance* GameInstanceRef = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

			if (ControlChecks())//debug
			{
				bIsGameOver = true;
				//ToggleCurrentPlayer();
				CurrentPlayer == 0 ? Players[1]->OnWin() : Players[0]->OnWin();

				//ToggleCurrentPlayer();
			}
			else if (ControlStall())
			{
				bIsGameOver = true;
			}

			//IsGameOver = mate ? mate : ControlStall();//first check for mate, then for stall
		}
	}
}

bool AChess_GameMode::ControlChecks() //TODO: Stall check
{
	bool mate = false;
	ChessColor colorToControl = CurrentPlayer == 0 ? WHITE : BLACK;

	if (Board->GetKingPosition(colorToControl)) {
		if (Board->CheckControl(colorToControl)) {
			if (Board->MateControl(colorToControl)) {
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("--------MATTO!!--------"));
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
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("--------STALLO!!--------"));
		UE_LOG(LogTemp, Error, TEXT("--------STALLO!!--------"));
	}
	return stall;
}

void AChess_GameMode::ResetHandler()
{
	TurnNumber = 1;
	NextMoveNumber = 1;
	ChoosePlayerAndStartGame();
}
