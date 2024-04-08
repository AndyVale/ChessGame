// Fill out your copyright notice in the Description page of Project Settings.


#include "CP_Bishop.h"

TArray<FVector2D> ACP_Bishop::GetPieceMoves(FVector2D* xy, AChessboard* Board)
{
    TArray<FVector2D> moves = TArray<FVector2D>();
    GetFeasibleDiagonals(xy, Board, moves);
    return moves;
}

float ACP_Bishop::GetPieceValue()
{
    return 30.0f;
}

