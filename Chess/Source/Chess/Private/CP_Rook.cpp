// Fill out your copyright notice in the Description page of Project Settings.


#include "CP_Rook.h"


TArray<FVector2D> ACP_Rook::GetPieceMoves(FVector2D* xy, AChessboard* Board)//TODO:Creare un metodo di ChessPiece che faccia sta roba in modo da usarlo nella regina
{
    TArray<FVector2D> moves = TArray<FVector2D>();
    GetFeasibleCross(xy, Board, moves);
    return moves;
}

float ACP_Rook::GetPieceValue()
{
    return 50.0f;
}