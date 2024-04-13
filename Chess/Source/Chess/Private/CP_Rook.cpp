// Fill out your copyright notice in the Description page of Project Settings.


#include "CP_Rook.h"


TArray<Chess_Move> ACP_Rook::GetPieceMoves()//TODO:Creare un metodo di ChessPiece che faccia sta roba in modo da usarlo nella regina
{
    TArray<Chess_Move> moves = TArray<Chess_Move>();
    GetFeasibleCross(this, moves);
    return moves;
}

float ACP_Rook::GetPieceValue()
{
    return 50.0f;
}