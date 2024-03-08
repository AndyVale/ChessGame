// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_RandomPlayer.h"
#include "ChessPiece.h"
#include <Chess_GameMode.h>

// Sets default values
AChess_RandomPlayer::AChess_RandomPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
	AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	AChessboard* Board = GameMode->Board;
	int32 size = Board->BoardSize;
	FTimerHandle TimerHandle;
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("SIZE %lld"), size));

	FVector2D randomLoc;
	AChessPiece* randomPiece;
	TArray<FVector2D> possibleMoves;
	//TODO: è un modo stupido per selezionare la mossa
	do {//get a piece that can be moved
		do//get a black piece
		{
			randomLoc = FVector2D(FMath::RandRange(0, size), FMath::RandRange(0, size));
		} while (Board->GetPieceColorFromXY(randomLoc)!=BLACK);

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Black piece: %llf, %llf "), randomLoc.Component(0), randomLoc.Component(1)));
		randomPiece = Board->GetPieceFromXY(randomLoc);
		possibleMoves = randomPiece->GetFeasibleMoves(&randomLoc, Board);

	} while (possibleMoves.Num() == 0);

	int32 randoMove = FMath::Rand() % ((possibleMoves).Num());
	FVector2D newLoc = possibleMoves[randoMove];

			Board->MakeAMove(randomLoc, newLoc);
			GameMode->TurnNextPlayer();
	
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
	//	{
	//	}, 3, false);

	
}

void AChess_RandomPlayer::OnWin()
{
}

void AChess_RandomPlayer::OnLose()
{
}

