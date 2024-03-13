// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessPiece.h"

// Sets default values
AChessPiece::AChessPiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	ChessPieceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(Scene);
	ChessPieceMesh->SetupAttachment(Scene);
}

void AChessPiece::SetColorAndMaterial(ChessColor c)
{
	PieceColor = c;
    switch (c)
    {
    case WHITE:
        ChessPieceMesh->SetMaterial(0, WhiteMaterial);
        break;
    case BLACK:
        ChessPieceMesh->SetMaterial(0, BlackMaterial);
        break;
    case NAC:
        UE_LOG(LogTemp, Error, TEXT("SetColorAndMaterial on a NAC color"));
        break;
    }
}

/*
TArray<FVector2D> AChessPiece::GetPieceMoves(FVector2D* xy, AChessboard* Board)//TODO:Levare come mossa feasible il mangiare il RE
{
    TArray<FVector2D> moves;
    //if (xy)
   // {
   //     moves.Add(*xy);//you can always click on the selected piece to undo the move
   // }
return moves;
}
*/

// Called when the game starts or when spawned
void AChessPiece::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AChessPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AChessPiece::GetFeasibleDiagonals(FVector2D* xy, AChessboard* Board, TArray<FVector2D>& moves)
{
    int max = Board->BoardSize;
    int x = (*xy)[0], y = (*xy)[1];

    FVector2D bishopDirections[4] =
    {
        FVector2D(1, 1),  //up right
        FVector2D(-1, 1), //up left
        FVector2D(1, -1), //down right
        FVector2D(-1, -1) //down left
    };
    for (const FVector2D& move : bishopDirections) 
    {
        int i = x + move.X;
        int j = y + move.Y;
        bool foundPiece = false;

        while (!foundPiece && i >= 0 && i < max && j >= 0 && j < max) 
        {
            if (Board->GetPieceFromXY(FVector2D(i, j)))
            {
                foundPiece = true;
                if (Board->GetPieceColorFromXY(FVector2D(i, j)) == PieceColor)
                {
                    break;
                }
            }
            moves.Add(FVector2D(i, j));
            i += move.X;
            j += move.Y;
        }
    }
}

void AChessPiece::GetFeasibleCross(FVector2D* xy, AChessboard* Board, TArray<FVector2D>& moves)
{
    int max = Board->BoardSize;
    int x = (*xy)[0], y = (*xy)[1];

    FVector2D rookDirections[4] = 
    {
        FVector2D(0, 1),  // up
        FVector2D(1, 0),  // right
        FVector2D(0, -1), // down
        FVector2D(-1, 0)  // left
    };

    for (const FVector2D& move : rookDirections) 
    {
        int i = x + move.X;
        int j = y + move.Y;
        bool foundPiece = false;

        while (!foundPiece && i >= 0 && i < max && j >= 0 && j < max)
        {
            if (Board->GetPieceFromXY(FVector2D(i, j)))
            {
                foundPiece = true;
                if (Board->GetPieceColorFromXY(FVector2D(i, j)) == PieceColor)
                {
                    break;
                }
            }
            moves.Add(FVector2D(i, j));
            i += move.X;
            j += move.Y;
        }
    }

}
