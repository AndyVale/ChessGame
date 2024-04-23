// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chess_Move.h"

enum CP;//forward declaration of the ChessPiece enum, declared in Chessboard.h (to avoid circular dependencies)

class CHESS_API Chess_MovePawnPromotion : public Chess_Move
{
public:
	Chess_MovePawnPromotion() = default;

	//constructor for the pawn promotion move, f and t are the positions of the pawn, selectedPiece is the piece to promote the pawn
	Chess_MovePawnPromotion(FVector2D f, FVector2D t, AChessboard* board, CP selectedPiece);

	virtual ~Chess_MovePawnPromotion() = default;

	//Override of the make and rollback move methods:
	virtual void MakeMove(bool simulate) override;
	virtual void RollbackMove(bool simulate) override;

	//Method to promote the pawn, selectedPiece is the piece to promote the pawn into (king and pawn are not permitted), it calls the ACP_Pawn::Promote method to change the pawn into the selected piece
	void PromotePawn(bool simulate, CP selectedPiece);

	//Override of the ToString method:
	virtual FString ToString() const override;
private:
	//Auxiliary methods used in make and rollback move, it calls the ACP_Pawn::Promote method to change the PromotedPawn into a pawn
	void PromotePawnRollback(bool simulate);

	//Enum rappresenting the piece to promote the pawn into (king and pawn are not permitted)
	CP lastSelectedPiece;
};
