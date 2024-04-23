// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chess_Move.h"

/**
 * 
 */
class CHESS_API Chess_MoveCastle : public Chess_Move
{
public:
	Chess_MoveCastle() = default;
	//constructor for the castle move, f and t are the positions of the king
	Chess_MoveCastle(FVector2D f, FVector2D t, AChessboard* board, bool isQueenSide);

	virtual ~Chess_MoveCastle() = default;

	//Override of the make and rollback move methods:
	virtual void MakeMove(bool simulate) override;
	virtual void RollbackMove(bool simulate) override;

	//Override of the ToString method:
	virtual FString ToString() const override;
private:
	//bool used by Castle moves to identify the rook to move
	bool bIsQueenSide; 
	//saving the rook used in the castle
	AChessPiece* CastledRook;

	//Auxiliary methods used in make and rollback move
	void CastleMove(bool simulate);
	void CastleMoveRollback(bool simulate);
};
