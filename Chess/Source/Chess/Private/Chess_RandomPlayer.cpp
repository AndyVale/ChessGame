// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_RandomPlayer.h"
#include "ChessPiece.h"
#include <Chess_GameMode.h>

// Sets default values
AChess_RandomPlayer::AChess_RandomPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RandomPColor = BLACK;
}	

// Called when the game starts or when spawned
void AChess_RandomPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called to bind functionality to input
void AChess_RandomPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AChess_RandomPlayer::OnTurn()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Turn"));
	FTimerHandle TimerHandle;
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&] {AChess_RandomPlayer::MakeRandomMove();}, 1, false);
}
void AChess_RandomPlayer::MakeRandomMove() {
	AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	AChessboard* Board = GameMode->Board;
	TMap<AChessPiece*, FVector2D> ActualPieces = Board->GetPieces(RandomPColor);
	TArray<bool> actualIsVisited;
	TArray<AChessPiece*> actualPiece;
	TArray<FVector2D> actualXY;

	for (auto& p : ActualPieces) {
		actualIsVisited.Add(false);
		actualPiece.Add(p.Key);
		actualXY.Add(p.Value);
	}

	int32 piecesNumber = actualIsVisited.Num();
	//int32 size = Board->BoardSize;
	//AChessPiece* randomPiece;
	TArray<FVector2D> possibleMoves;
	int numSize = 0;
	int32 randomPieceIndx = 0;

	do {
		randomPieceIndx = FMath::Rand() % piecesNumber;
		if (!actualIsVisited[randomPieceIndx]) {
			possibleMoves = Board->GetFeasibleMoves(actualPiece[randomPieceIndx], false);
			numSize = possibleMoves.Num();
			actualIsVisited[randomPieceIndx] = true;
 		}
	} while (numSize == 0 && actualIsVisited.Find(false) != INDEX_NONE);//get a piece that can be moved

	if (numSize != 0)
	{
		int32 randomMove = FMath::Rand() % numSize;
		FVector2D newLoc = possibleMoves[randomMove];
		FVector2D oldLoc = *Board->GetXYFromPiece(actualPiece[randomPieceIndx]);
		Board->MakeAMove(oldLoc, newLoc, false);
	}
	else
	{
		//STALLO
	}
	//CHEK Control
	GameMode->TurnNextPlayer();
}

void AChess_RandomPlayer::OnWin()
{
}

void AChess_RandomPlayer::OnLose()
{
}
