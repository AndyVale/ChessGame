// Fill out your copyright notice in the Description page of Project Settings.


#include "CP_Knight.h"

TArray<FVector2D> ACP_Knight::GetFeasibleMoves(FVector2D* xy, AChessboard* Board)
{//TODO:Aggiungere impossibilità di automangiarsi
    TArray<FVector2D> moves = Super::GetFeasibleMoves(xy, Board);
    int x = (*xy)[0], y = (*xy)[1];
    int max = Board->BoardSize;

    if (x + 1 < max && y + 2 < max && Board->GetPieceColorFromXY(FVector2D(x + 1, y + 2)) != PieceColor)
    {
        moves.Add(FVector2D(x + 1, y + 2));
    }
    if (x - 1 >= 0 && y + 2 < max && Board->GetPieceColorFromXY(FVector2D(x - 1, y + 2)) != PieceColor)
    {
        moves.Add(FVector2D(x - 1, y + 2));
    }
    if (x + 2 < max && y + 1 < max && Board->GetPieceColorFromXY(FVector2D(x + 2, y + 1)) != PieceColor)
    {
        moves.Add(FVector2D(x + 2, y + 1));
    }
    if (x - 2 >= 0 && y + 1 < max && Board->GetPieceColorFromXY(FVector2D(x - 2, y + 1)) != PieceColor)
    {
        moves.Add(FVector2D(x - 2, y + 1));
    }

    if (x + 1 < max && y - 2 >= 0 && Board->GetPieceColorFromXY(FVector2D(x + 1, y - 2)) != PieceColor)
    {
        moves.Add(FVector2D(x + 1, y - 2));
    }
    if (x - 1 >= 0 && y - 2 >= 0 && Board->GetPieceColorFromXY(FVector2D(x - 1, y - 2)) != PieceColor)
    {
        moves.Add(FVector2D(x - 1, y - 2));
    }
    if (x + 2 < max && y - 1 >= 0 && Board->GetPieceColorFromXY(FVector2D(x + 2, y - 1)) != PieceColor)
    {
        moves.Add(FVector2D(x + 2, y - 1));
    }
    if (x - 2 >= 0 && y - 1 >= 0 && Board->GetPieceColorFromXY(FVector2D(x - 2, y - 1)) != PieceColor)
    {
        moves.Add(FVector2D(x - 2, y - 1));
    }

    return moves;
}
