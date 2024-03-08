// Fill out your copyright notice in the Description page of Project Settings.


#include "CP_Rook.h"

TArray<FVector2D> ACP_Rook::GetFeasibleMoves(FVector2D* xy, AChessboard* Board)//TODO:Creare un metodo di ChessPiece che faccia sta roba in modo da usarlo nella regina
{
    TArray<FVector2D> moves = Super::GetFeasibleMoves(xy, Board);
    GetFeasibleCross(xy, Board, moves);
    return moves;
}