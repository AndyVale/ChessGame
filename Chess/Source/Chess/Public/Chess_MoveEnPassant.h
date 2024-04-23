// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chess_Move.h"

/**
 * 
 */
class CHESS_API Chess_MoveEnPassant : public Chess_Move
{
public:
	Chess_MoveEnPassant() = default;

	//enPassantCapturePosition specify the position of the "special captured" piece
	Chess_MoveEnPassant(FVector2D f, FVector2D t, AChessboard* board, FVector2D enPassantCapturePosition);

	virtual ~Chess_MoveEnPassant() = default;

	//Override of the make and rollback move methods:
	virtual void MakeMove(bool simulate) override;
	virtual void RollbackMove(bool simulate) override;
private:
	//Auxiliary variable to restore captured-enpassant pieces. Position is not needed because can be calculated on "From and To"
	AChessPiece* EnPassantCapturedPiece = nullptr;
	
	//Auxiliary methods used in make and rollback move:
	void EnPassantCapture(bool simulate);
	void EnPassantCaptureRollback(bool simulate);
};
