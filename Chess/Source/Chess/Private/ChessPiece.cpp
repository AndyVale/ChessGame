// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessPiece.h"

// Sets default values
AChessPiece::AChessPiece()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    ChessPieceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    ChessPieceMesh->SetWorldScale3D(FVector(0.60, 0.60, 0.001));//scaling the mesh so in the blueprints i have not to scale it
    PiecePosition = FVector2D(-1, -1);//initializing pieceposition to a non valid xy
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

void AChessPiece::GetFeasibleDiagonals(AChessPiece* piece, TArray<TSharedPtr<Chess_Move>>& moves)
{
    AChessboard* Board = piece->ReferredBoard;
    int max = Board->BoardSize;
    int x = piece->PiecePosition[0], y = piece->PiecePosition[1];

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

        while (!foundPiece && i >= 0 && i < max && j >= 0 && j < max)//until we find a piece or we reach the end of the board
        {
            if (Board->GetPieceFromXY(FVector2D(i, j)))
            {
                foundPiece = true;
                if (Board->GetPieceColorFromXY(FVector2D(i, j)) == piece->PieceColor)
                {
                    break;
                }
            }
            moves.Add(MakeShareable<Chess_Move>(new Chess_Move(piece->PiecePosition, FVector2D(i, j), piece->ReferredBoard)));
            i += move.X;
            j += move.Y;
        }
    }
}

void AChessPiece::GetFeasibleCross(AChessPiece* piece, TArray <TSharedPtr<Chess_Move>> & moves)
{
    AChessboard* Board = piece->ReferredBoard;
    int max = Board->BoardSize;
    int x = piece->PiecePosition[0], y = piece->PiecePosition[1];

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

        while (!foundPiece && i >= 0 && i < max && j >= 0 && j < max)//until we find a piece or we reach the end of the board
        {
            if (Board->GetPieceFromXY(FVector2D(i, j)))
            {
                foundPiece = true;
                if (Board->GetPieceColorFromXY(FVector2D(i, j)) == piece->PieceColor)
                {
                    break;
                }
            }
            moves.Add(MakeShareable<Chess_Move>(new Chess_Move(piece->PiecePosition, FVector2D(i, j), piece->ReferredBoard)));
            i += move.X;
            j += move.Y;
        }
    }

}

//(work in progress)
float AChessPiece::GetCorrectedPieceValue()
{
    float pv = GetPieceValue();
    float pov = GetPositionValue(PiecePosition);
    return pv + pov;
}

TArray<TSharedPtr<Chess_Move>> AChessPiece::GetPieceLegalMoves()
{
    TArray<TSharedPtr<Chess_Move>> feasibleMoves = this->GetPieceMoves();//get all the moves
    TArray<TSharedPtr<Chess_Move>> legalMoves = TArray<TSharedPtr<Chess_Move>>();

    for (TSharedPtr<Chess_Move> move : feasibleMoves)
    {
        if (move->IsLegal())//check if the move is legal
        {
            legalMoves.Add(move);
        }
    }

    return legalMoves;
}
