// Fill out your copyright notice in the Description page of Project Settings.


#include "CP_King.h"

TArray<FVector2D> ACP_King::GetPieceMoves(FVector2D* xy, AChessboard* Board)
{
    TArray<FVector2D> moves = TArray<FVector2D>();
    int x = (*xy)[0], y = (*xy)[1];
    int max = Board->BoardSize;
    //Possible moves:
    FVector2D kingMoves[8] = {
        FVector2D(0, 1),
        FVector2D(1, 1),
        FVector2D(-1, 1),
        FVector2D(0, -1),
        FVector2D(1, -1),
        FVector2D(-1, -1),
        FVector2D(1, 0),
        FVector2D(-1, 0)
    };

    for (const FVector2D& move : kingMoves) {
        FVector2D newLoc = FVector2D(x + move.X, y + move.Y);
        if (newLoc.X >= 0 && newLoc.X < max && newLoc.Y >= 0 && newLoc.Y < max && Board->GetPieceColorFromXY(newLoc) != PieceColor) {
            moves.Add(newLoc);
        }
    }

    return moves;
}

float ACP_King::GetPieceValue()
{
    return 900.0f;
}



