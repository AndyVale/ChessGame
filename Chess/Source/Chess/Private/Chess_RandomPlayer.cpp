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
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Turn"));
	FTimerHandle TimerHandle;
	bMyTurn = true;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&] {AChess_RandomPlayer::MakeRandomMove(); }, 1, false);
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
	TArray<FVector2D> possibleMoves;
	int numSize = 0;
	int32 randomPieceIndx = 0;

	do {
		randomPieceIndx = FMath::Rand() % piecesNumber;
		if (!actualIsVisited[randomPieceIndx]) {
			possibleMoves = Board->GetFeasibleSquares(actualPiece[randomPieceIndx], false);
			numSize = possibleMoves.Num();
			actualIsVisited[randomPieceIndx] = true;
		}
	} while (numSize == 0 && actualIsVisited.Find(false) != INDEX_NONE);//get a piece that can be moved
	TSharedPtr<Chess_Move> randomMove = nullptr;
	if (bMyTurn) {
		if (numSize != 0)
		{
			int32 randomMoveIndx = FMath::Rand() % numSize;
			FVector2D newLoc = possibleMoves[randomMoveIndx];
			FVector2D oldLoc = *Board->GetXYFromPiece(actualPiece[randomPieceIndx]);
			randomMove = MakeShareable(new Chess_Move(oldLoc, newLoc, Board));
			Board->MakeAMove(randomMove, false);
			//Chess_Move succMove = Chess_Move(oldLoc, newLoc);
		}
		else
		{
			//STALLO
		}
		if (randomMove && randomMove->bPromotionAfterMove) {
			TArray<CP> randomPromotedPiece = { CP::BISHOP, CP::KNIGHT, CP::ROOK, CP::QUEEN };
			int32 randomPromotionIndx = FMath::Rand() % randomPromotedPiece.Num();
			Board->PromoteLastMove(randomPromotedPiece[randomPromotionIndx]);
		}
		if (randomMove) {
			GameMode->UpdateLastMove(randomMove);//notify the HUD of the move
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

