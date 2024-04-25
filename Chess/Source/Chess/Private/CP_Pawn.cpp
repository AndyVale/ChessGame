// Fill out your copyright notice in the Description page of Project Settings.


#include "CP_Pawn.h"
#include "Chessboard.h"


TArray<TSharedPtr<Chess_Move>> ACP_Pawn::GetPieceMoves()
{
	if (PromotedPiece)//if the pawn is promoted, return the promoted piece moves
	{
		PromotedPiece->ReferredBoard = ReferredBoard;
		PromotedPiece->PiecePosition = PiecePosition;
		return PromotedPiece->GetPieceMoves();
	}
	//else return the pawn moves:

	TArray<TSharedPtr<Chess_Move>> moves = TArray<TSharedPtr<Chess_Move>>();
	int max = ReferredBoard->BoardSize;
	int x = PiecePosition[0], y = PiecePosition[1];

	if (this->PieceColor == BLACK)//go from 0 to max
	{
		if (y + 1 < max - 1) {//move forwards
			if (!ReferredBoard->GetPieceFromXY(FVector2D(x, y + 1)))
			{
				moves.Add(MakeShareable<Chess_Move>(new Chess_Move(PiecePosition, FVector2D(x, y + 1), ReferredBoard)));
				if (y == 1 && !ReferredBoard->GetPieceFromXY(FVector2D(x, y + 2)))//move by two at the beginning
				{
					moves.Add(MakeShareable<Chess_Move>(new Chess_Move(PiecePosition, FVector2D(x, y + 2), ReferredBoard)));
				}
			}
			if (x + 1 < max && ReferredBoard->GetPieceColorFromXY(FVector2D(x + 1, y + 1)) == WHITE)
			{//eat
				moves.Add(MakeShareable<Chess_Move>(new Chess_Move(PiecePosition, FVector2D(x + 1, y + 1), ReferredBoard)));
			}
			if (x - 1 >= 0 && ReferredBoard->GetPieceColorFromXY(FVector2D(x - 1, y + 1)) == WHITE)
			{//eat
				moves.Add(MakeShareable<Chess_Move>(new Chess_Move(PiecePosition, FVector2D(x - 1, y + 1), ReferredBoard)));
			}
		}
		else if(y + 1 == max - 1)
		{//promotion
			if (!ReferredBoard->GetPieceFromXY(FVector2D(x, y + 1)))//move forward
			{
				moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x, y + 1), ReferredBoard, CP::QUEEN)));
				moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x, y + 1), ReferredBoard, CP::KNIGHT)));
				//moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x, y + 1), ReferredBoard, CP::BISHOP))); //Commented to optimize the performance in minimax recursion
				//moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x, y + 1), ReferredBoard, CP::ROOK)));
			}
			if (x + 1 < max && ReferredBoard->GetPieceColorFromXY(FVector2D(x + 1, y + 1)) == WHITE)
			{//eat
				moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x + 1, y + 1), ReferredBoard, CP::QUEEN)));
				moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x + 1, y + 1), ReferredBoard, CP::KNIGHT)));
				//moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x + 1, y + 1), ReferredBoard, CP::BISHOP)));
				//moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x + 1, y + 1), ReferredBoard, CP::ROOK)));
			}
			if (x - 1 >= 0 && ReferredBoard->GetPieceColorFromXY(FVector2D(x - 1, y + 1)) == WHITE)
			{//eat
				moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x - 1, y + 1), ReferredBoard, CP::QUEEN)));
				moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x - 1, y + 1), ReferredBoard, CP::KNIGHT)));
				//moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x - 1, y + 1), ReferredBoard, CP::BISHOP)));
				//moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x - 1, y + 1), ReferredBoard, CP::ROOK)));
			}
		}
		
	}
	else//go from max downto 0
	{
		if (y - 1 > 0) {//move backwards
			if (!ReferredBoard->GetPieceFromXY(FVector2D(x, y - 1)))
			{
				moves.Add(MakeShareable<Chess_Move>(new Chess_Move(PiecePosition, FVector2D(x, y - 1), ReferredBoard)));
				if (y == 6 && !ReferredBoard->GetPieceFromXY(FVector2D(x, y - 2)))//move by two at the beginning
				{
					moves.Add(MakeShareable<Chess_Move>(new Chess_Move(PiecePosition, FVector2D(x, y - 2), ReferredBoard)));
				}
			}
			if (x + 1 < max && ReferredBoard->GetPieceColorFromXY(FVector2D(x + 1, y - 1)) == BLACK)
			{//eat
				moves.Add(MakeShareable<Chess_Move>(new Chess_Move(PiecePosition, FVector2D(x + 1, y - 1), ReferredBoard)));
			}
			if (x - 1 >= 0 && ReferredBoard->GetPieceColorFromXY(FVector2D(x - 1, y - 1)) == BLACK) {//eat
				moves.Add(MakeShareable<Chess_Move>(new Chess_Move(PiecePosition, FVector2D(x - 1, y - 1), ReferredBoard)));
			}
		}
		else if (y - 1 == 0)
		{//promotion
			if(!ReferredBoard->GetPieceFromXY(FVector2D(x, y - 1)))//move forward
			{
				moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x, y - 1), ReferredBoard, CP::QUEEN)));
				moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x, y - 1), ReferredBoard, CP::KNIGHT)));
				//moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x, y - 1), ReferredBoard, CP::BISHOP)));
				//moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x, y - 1), ReferredBoard, CP::ROOK)));
			}
			if(x + 1 < max && ReferredBoard->GetPieceColorFromXY(FVector2D(x + 1, y - 1)) == BLACK)
			{//eat
				moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x + 1, y - 1), ReferredBoard, CP::QUEEN)));
				moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x + 1, y - 1), ReferredBoard, CP::KNIGHT)));
				//moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x + 1, y - 1), ReferredBoard, CP::BISHOP)));
				//moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x + 1, y - 1), ReferredBoard, CP::ROOK)));
			}
			if(x - 1 >= 0 && ReferredBoard->GetPieceColorFromXY(FVector2D(x - 1, y - 1)) == BLACK)
			{//eat
				moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x - 1, y - 1), ReferredBoard, CP::QUEEN)));
				moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x - 1, y - 1), ReferredBoard, CP::KNIGHT)));
				//moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x - 1, y - 1), ReferredBoard, CP::BISHOP)));
				//moves.Add(MakeShareable<Chess_Move>(new Chess_MovePawnPromotion(PiecePosition, FVector2D(x - 1, y - 1), ReferredBoard, CP::ROOK)));
			}
		}

	}

	if (FMath::Abs(ReferredBoard->EnPassantPossibleCapture.X - PiecePosition.X) == 1 && ReferredBoard->EnPassantPossibleCapture.Y == PiecePosition.Y)//en passant capture check
	{
		FVector2D To = FVector2D(ReferredBoard->EnPassantPossibleCapture.X, ReferredBoard->EnPassantPossibleCapture.Y + (PieceColor == ChessColor::WHITE ? -1 : +1));//enpassant possible capture is at the position saved in chessboard
		moves.Add(MakeShareable<Chess_Move>(new Chess_MoveEnPassant(PiecePosition, To, ReferredBoard, ReferredBoard->EnPassantPossibleCapture)));
	}

	return moves;
}

float ACP_Pawn::GetPieceValue()
{
	return 10.0f;
}

void ACP_Pawn::PromoteIn(CP promotedPiece)
{
	//I decided to save here the promoted pieces to avoid spawning a new piece in every Chess_MovePawnPromotion
	switch
		(promotedPiece)
	{//TODO: change spawning location and rotation to a fixed value 
		case CP::BISHOP:
			if (!BishopPromotion)//if the piece is not already spawned, spawn it and set its color, bind it as promoted piece
			{
				BishopPromotion = GetWorld()->SpawnActor<AChessPiece>(ReferredBoard->Bishop, FVector(FMath::Rand(), FMath::Rand(), FMath::Rand()), FRotator(FMath::Rand(), FMath::Rand(), FMath::Rand()));//random location and rotation to avoid overlapping
				BishopPromotion->SetColorAndMaterial(PieceColor);
			}
			PromotedPiece = BishopPromotion;
			break;
		case CP::KNIGHT:
			if (!KnightPromotion)
			{
				KnightPromotion = GetWorld()->SpawnActor<AChessPiece>(ReferredBoard->Knight, FVector(FMath::Rand(), FMath::Rand(), FMath::Rand()), FRotator(FMath::Rand(), FMath::Rand(), FMath::Rand()));
				KnightPromotion->SetColorAndMaterial(PieceColor);
			}
			PromotedPiece = KnightPromotion;
			break;
		case CP::ROOK:
			if (!RookPromotion)
			{
				RookPromotion = GetWorld()->SpawnActor<AChessPiece>(ReferredBoard->Rook, FVector(FMath::Rand(), FMath::Rand(), FMath::Rand()), FRotator(FMath::Rand(), FMath::Rand(), FMath::Rand()));
				RookPromotion->SetColorAndMaterial(PieceColor);
			}
			
			PromotedPiece = RookPromotion;
			break;
		case CP::QUEEN:
			if (!QueenPromotion)
			{
				QueenPromotion = GetWorld()->SpawnActor<AChessPiece>(ReferredBoard->Queen, FVector(FMath::Rand(), FMath::Rand(), FMath::Rand()), FRotator(FMath::Rand(), FMath::Rand(), FMath::Rand()));
				QueenPromotion->SetColorAndMaterial(PieceColor);
			}
			PromotedPiece = QueenPromotion;
			break;
		default:
			PromotedPiece = nullptr;
			this->SetColorAndMaterial(PieceColor);
			return;
	}
	this->ChessPieceMesh->SetMaterial(0, PromotedPiece->ChessPieceMesh->GetMaterial(0));
}
