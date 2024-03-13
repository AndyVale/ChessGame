// Fill out your copyright notice in the Description page of Project Settings.


#include "CP_Queen.h"

TArray<FVector2D> ACP_Queen::GetPieceMoves(FVector2D* xy, AChessboard* Board)
{
    TArray<FVector2D> moves = TArray<FVector2D>();
    GetFeasibleDiagonals(xy, Board, moves);
    GetFeasibleCross(xy, Board, moves);
    return moves;
}
