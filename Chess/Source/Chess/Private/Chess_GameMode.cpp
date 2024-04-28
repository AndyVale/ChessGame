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
	//Gamemodebase properties:
	PlayerControllerClass = AChess_PlayerController::StaticClass();
	DefaultPawnClass = AChess_HumanPlayer::StaticClass();
	UE_LOG(LogTemp, Display, TEXT("GameMode CREATA"));
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

	if (HumanPlayer == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("HumanPlayer is null, can not spawn HumanPlayer"));
		return;
	}

	if (BoardClass != nullptr)
	{
		Board = GetWorld()->SpawnActor<AChessboard>(BoardClass);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BoardClass is null, can not spawn Chessboard"));
	}

	float CenterOnBoard = (this->Board->SquareSize * (this->Board->BoardSize / 2) - (this->Board->SquareSize / 2));
	FVector CameraPos(CenterOnBoard, CenterOnBoard, 1000.0f);

	//set the rotation of the camera to see the board with the white player on the bottom:
	HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotator(-90, -90, 0));

	Players.Add(HumanPlayer);
	
	if (UChess_GameInstance* GameInstanceRef = Cast<UChess_GameInstance>(GetGameInstance()))
	{
		//Getting the game difficulty from the game instance (setted in the main menu)
		difficulty_level = GameInstanceRef->GameDifficulty;
	
		//Spawn the enemy player based on the difficulty level
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

		//Bind the reset event broadcasted by gameinstance
		GameInstanceRef->OnResetEvent.AddDynamic(this, &AChess_GameMode::ResetHandler);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance is null, can not get the game difficulty nor bind the reset handler"));
	}

	ChoosePlayerAndStartGame();//start the game
}

void AChess_GameMode::ChoosePlayerAndStartGame()
{
	//The current player is the human player (0), by default the white player is the human player
	CurrentPlayer = 0;
	bIsGameOver = false;
	Players[0]->/*PlayerColor == ChessColor::WHITE ? Players[0]->*/OnTurn();// : Players[1]->OnTurn();
}

void AChess_GameMode::ToggleCurrentPlayer()
{
	CurrentPlayer++;
	NextMoveNumber += 1;
	NextActualMoveNumber += 1;
	if (!Players.IsValidIndex(CurrentPlayer))
	{
		CurrentPlayer = 0;//if the index is out of range, reset to 0
		TurnNumber += 1;
	}
}

void AChess_GameMode::UpdateLastMove(TSharedPtr<Chess_Move> move)
{
	if (move)
	{
		const FString moveString = move->ToString();
		OnMoveUpdate.Execute(moveString, NextMoveNumber);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Move is null, can not update the last move"));
	}
}

void AChess_GameMode::ShowPromotionWidget(ChessColor playerColor)
{
	PlayerSwapNotify(false);//change player to avoid unexpected click on storyboard or "change opponent" button
	if (playerColor == Players[0]->PlayerColor) {//if is the human player, notify the HUD to show the promotion widget.
		OnShowPromotionWidget.Execute(playerColor);
		bMustSelectPiecePromotion = true;
	}
	//else the is done by the AI player and no need to show the promotion widget
}

void AChess_GameMode::GoBackToActualMove()
{
	NextMoveNumber = NextActualMoveNumber;//the next move number is the actual move number showed on board
	TurnNumber = (NextActualMoveNumber - 1) % 2 == 0 ? (NextActualMoveNumber - 1) / 2 + 1 : (NextActualMoveNumber - 1) / 2 + 1;
	CurrentPlayer = 0;//the current player is the white player (the white player is the human player and is the only one that can go back in moves)
	bIsOnReplay = false;
	bIsGameOver = false;//reset the game over flag (no resetting points)
	OnTurnGoBack.Broadcast(NextActualMoveNumber - 1);//broadcast the event to all the listeners
}

void AChess_GameMode::SelectedPawnPromotionHandler(CP ChessPieceEnum)
{
	TSharedPtr<Chess_Move> move = Board->PromoteLastMove(ChessPieceEnum);//set the promotion in the last move
	UpdateLastMove(move);//write the promotion in the storyboard
	bMustSelectPiecePromotion = false;
	
	ToggleCurrentPlayer();//Calculate the next player
	
	if (ControlChecks())//Check if the game is over, a move can cause a checkmate
	{
		CurrentPlayer == 0 ? Players[1]->OnWin() : Players[0]->OnWin();//if the game is over, the winner is the other player, attention at the order of this instruction and the next one (gameoversignla modifies currentplayer)
		GameOverSignal(CurrentPlayer == 0 ? Players[1]->PlayerColor : Players[0]->PlayerColor);//if the game is over, the winner is the other player)
		return;
	}
	else if (ControlStall())//Check if the game is over, a move can cause a stall
	{
		GameOverSignal(ChessColor::NAC);//There is no winner, the game is a stall
		return;
	}
	else {
		Players[CurrentPlayer]->OnTurn();//Tell the next player to play
	}
}

void AChess_GameMode::ReplayMove(int32 moveNumber)
{
	NextActualMoveNumber = moveNumber + 1;//the next move in the replay is the move number + 1
	bIsOnReplay = (NextActualMoveNumber != NextMoveNumber);//if the next move is not the last move, the game is in replay mode
	OnReplayMove.Broadcast(moveNumber);
}

void AChess_GameMode::TurnNextPlayer()
{
	if (!bMustSelectPiecePromotion)//if the player has not to select the piece promotion
	{
		if (!bIsGameOver)
		{
			Board->RestoreBoardColors();
			ToggleCurrentPlayer();//Calculate the next player

			if (ControlChecks())//Check if the game is over, a move can cause a checkmate
			{
				CurrentPlayer == 0 ? Players[1]->OnWin() : Players[0]->OnWin();//if the game is over, the winner is the other player, attention at the order of this instruction and the next one (gameoversignla modifies currentplayer)
				GameOverSignal(CurrentPlayer == 0 ? Players[1]->PlayerColor : Players[0]->PlayerColor);//if the game is over, the winner is the other player)
				return;
			}
			else if (ControlStall())//Check if the game is over, a move can cause a stall
			{
				GameOverSignal(ChessColor::NAC);//There is no winner, the game is a stall
				return;
			}
			else {
				Players[CurrentPlayer]->OnTurn();//Tell the next player to play
			}
		}
	}
}

bool AChess_GameMode::ControlChecks() //TODO: Stall check
{
	bool mate = false;
	ChessColor colorToControl = CurrentPlayer == 0 ? WHITE : BLACK;

	if (Board->GetMoveShowedOnBoard() && Board->GetMoveShowedOnBoard()->PlayerOnCheck == colorToControl)//get the move showed and look at the move cache
	{
		if(Board->GetMoveShowedOnBoard()->PlayerOnCheckMate == colorToControl)
		{
			UE_LOG(LogTemp, Display, TEXT("--------MATTO!!--------"));
			mate = true;
		}
		if (Board->GetSquareFromXY(*Board->GetKingPosition(colorToControl)))//get the king position and set the danger color
		{
			Board->GetSquareFromXY(*Board->GetKingPosition(colorToControl))->SetDangerColor();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("King position not found in ControlChecks"));
		}
	}

	return mate;
}

bool AChess_GameMode::ControlStall()
{
	bool stall = false;
	if (Board->GetMoveShowedOnBoard() && Board->GetMoveShowedOnBoard()->PlayerOnStall != ChessColor::NAC)
	{
		stall = true;
		UE_LOG(LogTemp, Display, TEXT("--------STALLO!!--------"));
	}
	return stall;
}

void AChess_GameMode::GameOverSignal(ChessColor C)
{
	bIsGameOver = true;
	OnGameOver.Execute(C);//notify the HUD that the game is over and the winner is the C player

	//force the player swap to human player to notify the HUD to allow the player to use the buttons (Not an elegant solution because the timer is used to avoid )
	PlayerSwapNotify(true);
}

void AChess_GameMode::ResetHandler()
{
	//Reset the variable of game to the initial state and notify all the listeners
	TurnNumber = 1;
	NextMoveNumber = 1;
	NextActualMoveNumber = 1;
	bMustSelectPiecePromotion = false;
	ChoosePlayerAndStartGame();
}

void AChess_GameMode::PlayerSwapNotify(bool IsHumanPlayer)
{
	if (OnTurnSwap.IsBound())//avoid to call the event if no one is listening, otherwise the game may crash
	{
		OnTurnSwap.Execute(IsHumanPlayer);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnTurnSwap event is not bound"));
	}
}