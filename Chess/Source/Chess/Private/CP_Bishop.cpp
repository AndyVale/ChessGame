// Fill out your copyright notice in the Description page of Project Settings.


#include "CP_Bishop.h"

TArray<Chess_Move> ACP_Bishop::GetPieceMoves()
{
    TArray<Chess_Move> moves = TArray<Chess_Move>();
    GetFeasibleDiagonals(this, moves);
    return moves;
}

float ACP_Bishop::GetPieceValue()
{
    return 30.0f;
}

