// Fill out your copyright notice in the Description page of Project Settings.


#include "CP_Bishop.h"

TArray<FVector2D> ACP_Bishop::GetFeasibleMoves(FVector2D* xy, AChessboard* Board)
{
    TArray<FVector2D> moves = Super::GetFeasibleMoves(xy, Board);
    GetFeasibleDiagonals(xy, Board, moves);
    return moves;
}
