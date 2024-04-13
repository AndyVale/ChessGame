// Fill out your copyright notice in the Description page of Project Settings.


#include "CP_King.h"

TArray<Chess_Move> ACP_King::GetPieceMoves()
{
    TArray<Chess_Move> moves = TArray<Chess_Move>();

    int x = PiecePosition[0], y = PiecePosition[1];
    int max = ReferredBoard->BoardSize;
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
        if (newLoc.X >= 0 && newLoc.X < max && newLoc.Y >= 0 && newLoc.Y < max && ReferredBoard->GetPieceColorFromXY(newLoc) != PieceColor) {
            moves.Add(Chess_Move(PiecePosition, newLoc, ReferredBoard));
        }
    }
    return moves;
}

float ACP_King::GetPieceValue()
{
    return 900.0f;
}



