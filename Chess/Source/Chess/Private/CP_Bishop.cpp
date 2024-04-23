// Fill out your copyright notice in the Description page of Project Settings.


#include "CP_Bishop.h"

TArray<TSharedPtr<Chess_Move>> ACP_Bishop::GetPieceMoves()
{
    TArray<TSharedPtr<Chess_Move>> moves = TArray<TSharedPtr<Chess_Move>>();
    GetFeasibleDiagonals(this, moves);
    return moves;
}

float ACP_Bishop::GetPieceValue()
{
    return 30.0f;
}

