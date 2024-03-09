// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_RandomPlayer.h"
#include "ChessPiece.h"
#include <Chess_GameMode.h>

// Sets default values
AChess_RandomPlayer::AChess_RandomPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RandomPColor = BLACK;
}	

// Called when the game starts or when spawned
void AChess_RandomPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChess_RandomPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	int32 size = Board->BoardSize;
	FVector2D randomLoc;
	AChessPiece* randomPiece;
	TArray<FVector2D> possibleMoves;
	int numSize = 0;
	do {
		do
		{
			randomLoc = FVector2D(FMath::RandRange(0, size), FMath::RandRange(0, size));
		} while (Board->GetPieceColorFromXY(randomLoc) != RandomPColor);
		randomPiece = Board->GetPieceFromXY(randomLoc);
		possibleMoves = randomPiece->GetFeasibleMoves(&randomLoc, Board);//get a black piece
		numSize = possibleMoves.Num();
	} while (numSize == 0);//get a piece that can be moved

	int32 randoMove = FMath::Rand() % numSize;
	FVector2D newLoc = possibleMoves[randoMove];
	//CHEK Control
	Board->MakeAMove(randomLoc, newLoc);
	GameMode->TurnNextPlayer();
}
void AChess_RandomPlayer::OnWin()
{
}

void AChess_RandomPlayer::OnLose()
{
}

