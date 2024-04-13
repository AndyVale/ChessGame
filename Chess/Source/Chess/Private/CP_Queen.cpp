// Fill out your copyright notice in the Description page of Project Settings.


#include "CP_Queen.h"



TArray<Chess_Move> ACP_Queen::GetPieceMoves()
{
    TArray<Chess_Move> moves = TArray<Chess_Move>();
    GetFeasibleDiagonals(this, moves);
    GetFeasibleCross(this, moves);
    return moves;
}

float ACP_Queen::GetPieceValue()
{
    return 90.0f;
}
