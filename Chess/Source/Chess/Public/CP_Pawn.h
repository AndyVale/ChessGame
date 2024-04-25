// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChessPiece.h"
#include "CP_Pawn.generated.h"

/**
 * 
 */
UCLASS()
class CHESS_API ACP_Pawn : public AChessPiece
{
	GENERATED_BODY()
public:
	static inline const TArray<TArray<float>> WhiteEvalMatrix =
	{
		{0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0},
		{5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0},
		{1.0,  1.0,  2.0,  3.0,  3.0,  2.0,  1.0,  1.0},
		{0.5,  0.5,  1.0,  4.5,  4.5,  1.0,  0.5,  0.5},
		{0.0,  0.0,  0.0,  4.0,  4.0,  0.0,  0.0,  0.0},
		{0.5, -0.5, -1.0,  0.0,  0.0, -1.0, -0.5,  0.5},
		{1.0,  1.0, 1.0,  -2.0, -2.0,  1.0,  1.0,  1.0},
		{0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0}
	};

	static inline const TArray<TArray<float>> BlackEvalMatrix =
	{
		{0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0},
		{1.0,  1.0, 1.0,  -2.0, -2.0,  1.0,  1.0,  1.0},
		{0.5, -0.5, -1.0,  0.0,  0.0, -1.0, -0.5,  0.5},
		{0.0,  0.0,  0.0,  4,0,  4.0,  0.0,  0.0,  0.0},
		{0.5,  0.5,  1.0,  4.5,  4.5,  1.0,  0.5,  0.5},
		{1.0,  1.0,  2.0,  3.0,  3.0,  2.0,  1.0,  1.0},
		{5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0},
		{0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0}
	};
	TArray<TSharedPtr<Chess_Move>>GetPieceMoves() override;
	float GetPieceValue() override;
	float GetPositionValue(FVector2D xy) override {
		float tmp = PieceColor == ChessColor::WHITE ? WhiteEvalMatrix[xy.Y][xy.X] : BlackEvalMatrix[xy.Y][xy.X];
		return tmp;
	}
	void PromoteIn(CP promotedPiece);
	AChessPiece* PromotedPiece;//TODO: Spawn the pieces in the begin play and hide them
protected:
	// Called when the game starts or when spawned
private:
	AChessPiece* KnightPromotion;
	AChessPiece* BishopPromotion;
	AChessPiece* RookPromotion;
	AChessPiece* QueenPromotion;
};
