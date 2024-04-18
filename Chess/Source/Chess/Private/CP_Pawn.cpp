// Fill out your copyright notice in the Description page of Project Settings.


#include "CP_Pawn.h"
#include "Chessboard.h"


TArray<Chess_Move> ACP_Pawn::GetPieceMoves()
{
	TArray<Chess_Move> moves = TArray<Chess_Move>();
	
	int max = ReferredBoard->BoardSize;
	int x = PiecePosition[0], y = PiecePosition[1];
	if (this->PieceColor == BLACK)//go from 0 to max
	{
		if (y + 1 < max) {//move forwards
			if (!ReferredBoard->GetPieceFromXY(FVector2D(x, y + 1)))
			{
				moves.Add(Chess_Move(PiecePosition, FVector2D(x, y + 1), ReferredBoard));
				if (y == 1 && !ReferredBoard->GetPieceFromXY(FVector2D(x, y + 2)))
				{
					moves.Add(Chess_Move(PiecePosition, FVector2D(x, y + 2), ReferredBoard));
				}
			}
			if (x + 1 < max && ReferredBoard->GetPieceColorFromXY(FVector2D(x + 1, y + 1)) == WHITE) 
			{//eat
				moves.Add(Chess_Move(PiecePosition, FVector2D(x + 1, y + 1), ReferredBoard));
			}
			if (x - 1 >= 0 && ReferredBoard->GetPieceColorFromXY(FVector2D(x - 1, y + 1)) == WHITE) 
			{//eat
				moves.Add(Chess_Move(PiecePosition, FVector2D(x - 1, y + 1), ReferredBoard));
			}
		}

	}
	else//go from max downto 0
	{
		if (y - 1 >= 0) {//move backwards
			if (!ReferredBoard->GetPieceFromXY(FVector2D(x, y - 1)))
			{
				moves.Add(Chess_Move(PiecePosition, FVector2D(x, y - 1), ReferredBoard));
				if (y == 6 && !ReferredBoard->GetPieceFromXY(FVector2D(x, y - 2)))
				{
					moves.Add(Chess_Move(PiecePosition, FVector2D(x, y - 2), ReferredBoard));
				}
			}
			if (x + 1 < max && ReferredBoard->GetPieceColorFromXY(FVector2D(x + 1, y - 1)) == BLACK)
			{//eat
				moves.Add(Chess_Move(PiecePosition, FVector2D(x + 1, y - 1), ReferredBoard));
			}
			if (x - 1 >= 0 && ReferredBoard->GetPieceColorFromXY(FVector2D(x - 1, y - 1)) == BLACK) {//eat
				moves.Add(Chess_Move(PiecePosition, FVector2D(x - 1, y - 1), ReferredBoard));
			}
		}
	}

	if (FMath::Abs(ReferredBoard->EnPassantPossibleCapture.X - PiecePosition.X) == 1 && ReferredBoard->EnPassantPossibleCapture.Y == PiecePosition.Y)
	{
		FVector2D To = FVector2D(ReferredBoard->EnPassantPossibleCapture.X, ReferredBoard->EnPassantPossibleCapture.Y + (PieceColor == ChessColor::WHITE ? -1 : +1));
		moves.Add(Chess_Move(PiecePosition, To, ReferredBoard, ReferredBoard->EnPassantPossibleCapture));
	}

	return moves;
}

float ACP_Pawn::GetPieceValue()
{
	return 10.0f;
}

