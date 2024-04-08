// Fill out your copyright notice in the Description page of Project Settings.


#include "CP_Pawn.h"
#include "Chessboard.h"


TArray<FVector2D> ACP_Pawn::GetPieceMoves(FVector2D* xy, AChessboard* Board)
{
	TArray<FVector2D> moves = TArray<FVector2D>();
	int max = Board->BoardSize;
	int x = (*xy)[0], y = (*xy)[1];
	if (this->PieceColor == BLACK)//go from 0 to max
	{
		if (y + 1 < max) {//move forwards
			if (!Board->GetPieceFromXY(FVector2D(x, y + 1))) {
				moves.Add(FVector2D(x, y + 1));
				if (y == 1 && !Board->GetPieceFromXY(FVector2D(x, y + 2)))
					moves.Add(FVector2D(x, y + 2));
			}
			if (x + 1 < max && Board->GetPieceColorFromXY(FVector2D(x + 1, y + 1)) == WHITE) {//eat
				moves.Add(FVector2D(x + 1, y + 1));
			}
			if (x - 1 >= 0 && Board->GetPieceColorFromXY(FVector2D(x - 1, y + 1)) == WHITE) {//eat
				moves.Add(FVector2D(x - 1, y + 1));
			}
		}

	}
	else//go from max downto 0
	{
		if (y - 1 >= 0) {//move backwards
			if (!Board->GetPieceFromXY(FVector2D(x, y - 1)))
			{
				moves.Add(FVector2D(x, y - 1));
				if (y == 6 && !Board->GetPieceFromXY(FVector2D(x, y - 2)))
					moves.Add(FVector2D(x, y - 2));
			}
			if (x + 1 < max && Board->GetPieceColorFromXY(FVector2D(x + 1, y - 1)) == BLACK) {//eat
				moves.Add(FVector2D(x + 1, y - 1));
			}
			if (x - 1 >= 0 && Board->GetPieceColorFromXY(FVector2D(x - 1, y - 1)) == BLACK) {//eat
				moves.Add(FVector2D(x - 1, y - 1));
			}
		}
	}
	return moves;
}

float ACP_Pawn::GetPieceValue()
{
	return 10.0f;
}

