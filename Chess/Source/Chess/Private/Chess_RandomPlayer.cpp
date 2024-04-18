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
	GameInstanceRef = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
}

// Called when the game starts or when spawned
void AChess_RandomPlayer::BeginPlay()
{
	Super::BeginPlay();
	if (GameInstanceRef)
	{
		GameInstanceRef->OnResetEvent.AddDynamic(this, &AChess_RandomPlayer::ResetHandler);//TODO: this is a workaround, search for a better solution
	}
}

void AChess_RandomPlayer::ResetHandler()
{
	bMyTurn = false;
}
// Called to bind functionality to input
void AChess_RandomPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AChess_RandomPlayer::OnTurn()
{
	GameInstanceRef->SetTurnMessage(TEXT("Random player turn!"));
	FTimerHandle TimerHandle;
	bMyTurn = true;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&] {AChess_RandomPlayer::MakeRandomMove(); }, 1, false);
}
void AChess_RandomPlayer::MakeRandomMove() {
	AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	AChessboard* Board = GameMode->Board;
	TArray<AChessPiece*> ActualPieces = Board->GetPieces(RandomPColor);
	TArray<bool> actualIsVisited;
	TArray<AChessPiece*> actualPiece;
	TArray<FVector2D> actualXY;

	for (auto& p : ActualPieces) {
		actualIsVisited.Add(false);
		actualPiece.Add(p);
		actualXY.Add(p->PiecePosition);
	}

	int32 piecesNumber = actualIsVisited.Num();
	TArray<Chess_Move> possibleMoves;
	int numSize = 0;
	int32 randomPieceIndx = 0;

	do {
		randomPieceIndx = FMath::Rand() % piecesNumber;
		if (!actualIsVisited[randomPieceIndx]) {
			possibleMoves = actualPiece[randomPieceIndx]->GetPieceLegalMoves();
			numSize = possibleMoves.Num();
			actualIsVisited[randomPieceIndx] = true;
		}
	} while (numSize == 0 && actualIsVisited.Find(false) != INDEX_NONE);//get a piece that can be moved
	TSharedPtr<Chess_Move> randomMovePtr = nullptr;
	if (bMyTurn) {
		if (numSize != 0)
		{
			int32 randomMoveIndx = FMath::Rand() % numSize;
			randomMovePtr = MakeShareable(new Chess_Move(possibleMoves[randomMoveIndx]));
			Board->MakeAMove(randomMovePtr, false);
			//Chess_Move succMove = Chess_Move(oldLoc, newLoc);
		}
		else
		{
			//STALLO
		}
		if (randomMovePtr && randomMovePtr->MoveClass == MoveType::PAWN_PROMOTION) {
			TArray<CP> randomPromotedPiece = { CP::BISHOP, CP::KNIGHT, CP::ROOK, CP::QUEEN };
			int32 randomPromotionIndx = FMath::Rand() % randomPromotedPiece.Num();
			Board->PromoteLastMove(randomPromotedPiece[randomPromotionIndx]);
		}
		if (randomMovePtr) {
			GameMode->UpdateLastMove(randomMovePtr);//notify the HUD of the move
		}
	}

	bMyTurn = false;
	GameMode->TurnNextPlayer();
}

void AChess_RandomPlayer::OnWin()
{
	GameInstanceRef->SetTurnMessage(TEXT("Random player Wins!"));
	GameInstanceRef->IncrementScoreAIPlayer();
}

void AChess_RandomPlayer::OnLose()
{
	GameInstanceRef->SetTurnMessage(TEXT("Random player Lose!"));
}

