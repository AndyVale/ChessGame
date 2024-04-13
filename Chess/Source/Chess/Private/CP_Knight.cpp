// Fill out your copyright notice in the Description page of Project Settings.


#include "CP_Knight.h"



TArray<Chess_Move> ACP_Knight::GetPieceMoves()
{//TODO:Aggiungere impossibilità di automangiarsi
    TArray<Chess_Move> moves = TArray<Chess_Move>();

    int x = PiecePosition[0], y = PiecePosition[1];
    int max = ReferredBoard->BoardSize;
    //Possible moves:
    FVector2D knightMoves[8] = {
        FVector2D(1, 2),
        FVector2D(-1, 2),
        FVector2D(2, 1),
        FVector2D(-2, 1),
        FVector2D(1, -2),
        FVector2D(-1, -2),
        FVector2D(2, -1),
        FVector2D(-2, -1)
    };

    for (const FVector2D& move : knightMoves) {
        FVector2D newLoc = FVector2D(x + move.X, y + move.Y);
        if (newLoc.X >= 0 && newLoc.X < max && newLoc.Y >= 0 && newLoc.Y < max && ReferredBoard->GetPieceColorFromXY(newLoc) != PieceColor) {
            moves.Add(Chess_Move(PiecePosition, newLoc, ReferredBoard));
        }
    }
    return moves;
}

float ACP_Knight::GetPieceValue()
{
    return 30.0f;
}

float ACP_Knight::GetCorrectedPieceValue()
{
    if (PieceColor == ChessColor::WHITE)
    {
        return GetPieceValue() + WhiteEvalMatrix[PiecePosition.Y][PiecePosition.X];
    }
    if (PieceColor == ChessColor::BLACK)
    {
        return GetPieceValue() + BlackEvalMatrix[PiecePosition.Y][PiecePosition.X];
    }
    return -1;
}
