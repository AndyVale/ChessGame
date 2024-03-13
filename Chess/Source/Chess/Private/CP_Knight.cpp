// Fill out your copyright notice in the Description page of Project Settings.


#include "CP_Knight.h"

TArray<FVector2D> ACP_Knight::GetPieceMoves(FVector2D* xy, AChessboard* Board)
{//TODO:Aggiungere impossibilità di automangiarsi
    TArray<FVector2D> moves = TArray<FVector2D>();
    int x = (*xy)[0], y = (*xy)[1];
    int max = Board->BoardSize;
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
        if (newLoc.X >= 0 && newLoc.X < max && newLoc.Y >= 0 && newLoc.Y < max && Board->GetPieceColorFromXY(newLoc) != PieceColor) {
            moves.Add(newLoc);
        }
    }

    return moves;
}
