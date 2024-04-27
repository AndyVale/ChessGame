// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_RandomPlayer.h"
#include "ChessPiece.h"
#include <Chess_GameMode.h>

// Sets default values
AChess_RandomPlayer::AChess_RandomPlayer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PlayerColor = BLACK;
	bIsMyTurn = false;
	GameInstanceRef = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!GameInstanceRef)
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstanceRef is null in AChess_RandomPlayer"));
	}
}

// Called when the game starts or when spawned
void AChess_RandomPlayer::BeginPlay()
{
	Super::BeginPlay();
	if (GameInstanceRef)
	{
		GameInstanceRef->OnResetEvent.AddDynamic(this, &AChess_RandomPlayer::ResetHandler);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstanceRef is null in AChess_RandomPlayer in begin play"));
	}
}

void AChess_RandomPlayer::ResetHandler()
{
	bIsMyTurn = PlayerColor == ChessColor::WHITE;
}
// Called to bind functionality to input
void AChess_RandomPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AChess_RandomPlayer::OnTurn()
{
	GameInstanceRef->SetTurnMessage(PlayerColor == ChessColor::WHITE ? TEXT("White Turn") : TEXT("Black Turn"));
	FTimerHandle TimerHandle;
	if (AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->PlayerSwapNotify(false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode is null in AChess_RandomPlayer OnTurn"));
	}
	bIsMyTurn = true;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&] {AChess_RandomPlayer::MakeRandomMove(); }, 2, false);
}
void AChess_RandomPlayer::MakeRandomMove() {
	if (AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode()))
	{
		AChessboard* Board = GameMode->Board;
		TArray<AChessPiece*> ActualPieces = Board->GetPieces(PlayerColor);//get the pieces of the player

		if (ActualPieces.IsEmpty())
		{//should never happen (at least the king should be there)
			UE_LOG(LogTemp, Error, TEXT("No pieces to move in AChess_RandomPlayer"));
			return;
		}

		int32 piecesNumber = ActualPieces.Num();
		TArray<bool> actualIsVisited;
		actualIsVisited.Init(false, piecesNumber);//initialize the array of visited pieces to false for all the pieces
		TArray<TSharedPtr<Chess_Move>> possibleMoves;
		int numSize = 0;
		int32 randomPieceIndx = 0;

		do {
			randomPieceIndx = FMath::Rand() % piecesNumber;
			if (!actualIsVisited[randomPieceIndx]) {//if the piece has not been visited
				possibleMoves = ActualPieces[randomPieceIndx]->GetPieceLegalMoves();//get the possible moves of the piece
				numSize = possibleMoves.Num();//get the number of possible moves
				actualIsVisited[randomPieceIndx] = true;
			}
		} while (numSize == 0 && actualIsVisited.Find(false) != INDEX_NONE);//until we find a piece with possible moves or we have visited all the pieces

		TSharedPtr<Chess_Move> randomMovePtr = nullptr;
		if (bIsMyTurn) {
			if (numSize != 0)//if there are possible moves (if not, the game is over)
			{
				int32 randomMoveIndx = FMath::Rand() % numSize;
				randomMovePtr = possibleMoves[randomMoveIndx];//get a random move
				Board->HandledMakeAMove(randomMovePtr, false);
			}
			else
			{
				//STALL or CHECKMATE
				UE_LOG(LogTemp, Error, TEXT("Random player has no moves to make"));
			}

			if (randomMovePtr) {
				GameMode->UpdateLastMove(randomMovePtr);//notify the HUD of the move
			}

			bIsMyTurn = false;
			GameMode->TurnNextPlayer();
		}
	}
}

void AChess_RandomPlayer::OnWin()
{
	GameInstanceRef->IncrementScoreAIPlayer();
}


