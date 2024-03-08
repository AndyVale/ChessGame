// Fill out your copyright notice in the Description page of Project Settings.


#include "CP_King.h"

TArray<FVector2D> ACP_King::GetFeasibleMoves(FVector2D* xy, AChessboard* Board)
{
    TArray<FVector2D> moves = Super::GetFeasibleMoves(xy, Board);
    int x = (*xy)[0], y = (*xy)[1];
    int max = Board->BoardSize;
    if (y + 1 < max)
    {
        if (Board->GetPieceColorFromXY(FVector2D(x, y + 1)) != PieceColor)
        {
            moves.Add(FVector2D(x, y + 1));
        }
        if(x + 1 < max && Board->GetPieceColorFromXY(FVector2D(x + 1, y + 1)) != PieceColor)
        {
            moves.Add(FVector2D(x + 1, y + 1));
        }
        if (x - 1 >= 0 && Board->GetPieceColorFromXY(FVector2D(x - 1, y + 1)) != PieceColor)
        {
            moves.Add(FVector2D(x - 1, y + 1));
        }
    }
    if (y - 1 >= 0)
    {
        if (Board->GetPieceColorFromXY(FVector2D(x, y - 1)) != PieceColor)
        {
            moves.Add(FVector2D(x, y - 1));
        }
        if (x + 1 < max && Board->GetPieceColorFromXY(FVector2D(x + 1, y - 1)) != PieceColor)
        {
            moves.Add(FVector2D(x + 1, y - 1));
        }
        if (x - 1 >= 0 && Board->GetPieceColorFromXY(FVector2D(x - 1, y - 1)) != PieceColor)
        {
            moves.Add(FVector2D(x - 1, y - 1));
        }
    }
    if (x + 1 < max && Board->GetPieceColorFromXY(FVector2D(x + 1, y)) != PieceColor)
    {
        moves.Add(FVector2D(x + 1, y));
    }
    if (x - 1 >= 0 && Board->GetPieceColorFromXY(FVector2D(x - 1, y)) != PieceColor)
    {
        moves.Add(FVector2D(x - 1, y));
    }
    return moves;
}