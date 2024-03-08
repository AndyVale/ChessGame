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

TArray<FVector2D> AChessPiece::GetFeasibleMoves(FVector2D* xy, AChessboard* Board)
{
	TArray<FVector2D> moves;
	//if (xy)
	//{
	//	moves.Add(*xy);//you can always click on the selected piece to undo the move
	//}
	return moves;//TODO:IMPLEMENTARE QUESTE SULLE VARIE PEDINE
}

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
    int x = (*xy)[0], y = (*xy)[1], i = 0, j = 0;
    bool foundPiece = false;

    //up right:
    i = x + 1;
    j = y + 1;
    foundPiece = false;
    while (!foundPiece && i < max && j < max)
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
        i++;
        j++;
    }

    //up left:
    i = x - 1;
    j = y + 1;
    foundPiece = false;
    while (!foundPiece && i >= 0 && j < max)
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
        i--;
        j++;
    }

    //down right:
    i = x + 1;
    j = y - 1;
    foundPiece = false;
    while (!foundPiece && i < max && j >= 0)
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
        i++;
        j--;
    }

    //down left:
    i = x - 1;
    j = y - 1;
    foundPiece = false;
    while (!foundPiece && i >= 0 && j >= 0)
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
        i--;
        j--;
    }
}

void AChessPiece::GetFeasibleCross(FVector2D* xy, AChessboard* Board, TArray<FVector2D>& moves)
{
    int max = Board->BoardSize;
    int x = (*xy)[0], y = (*xy)[1], i = 0;
    bool foundPiece = false;

    //up:
    i = y + 1;
    foundPiece = false;
    while (!foundPiece && i < max)
    {
        if (Board->GetPieceFromXY(FVector2D(x, i)))
        {
            foundPiece = true;
            if (Board->GetPieceColorFromXY(FVector2D(x, i)) == PieceColor)
            {
                break;
            }
        }
        moves.Add(FVector2D(x, i));
        i++;
    }

    //left:
    i = x + 1;
    foundPiece = false;
    while (!foundPiece && i < max)
    {
        if (Board->GetPieceFromXY(FVector2D(i, y)))
        {
            foundPiece = true;
            if (Board->GetPieceColorFromXY(FVector2D(i, y)) == PieceColor)
            {
                break;
            }
        }
        moves.Add(FVector2D(i, y));
        i++;
    }

    //Down:
    i = y - 1;
    foundPiece = false;
    while (!foundPiece && i >= 0)
    {
        if (Board->GetPieceFromXY(FVector2D(x, i)))
        {
            foundPiece = true;
            if (Board->GetPieceColorFromXY(FVector2D(x, i)) == PieceColor)
            {
                break;
            }
        }
        moves.Add(FVector2D(x, i));
        i--;
    }

    // right:
    i = x - 1;
    foundPiece = false;
    while (!foundPiece && i >= 0)
    {
        if (Board->GetPieceFromXY(FVector2D(i, y)))
        {
            foundPiece = true;
            if (Board->GetPieceColorFromXY(FVector2D(i, y)) == PieceColor)
            {
                break;
            }
        }
        moves.Add(FVector2D(i, y));
        i--;
    }
}

