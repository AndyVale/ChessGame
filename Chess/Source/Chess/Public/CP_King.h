// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChessPiece.h"
#include "CP_King.generated.h"

/**
 * 
 */
UCLASS()
class CHESS_API ACP_King : public AChessPiece
{
	GENERATED_BODY()
public:
    static inline const TArray<TArray<float>> WhiteEvalMatrix =
    {
    {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
    {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
    {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
    {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
    {-2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0},
    {-1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0},
    { 2.0,  2.0,  0.0,  0.0,  0.0,  0.0,  2.0,  2.0 },
    { 2.0,  3.0,  1.0,  0.0,  0.0,  1.0,  3.0,  2.0 }
    };

    // Matrice di valutazione per il re nero
    static inline const TArray<TArray<float>> BlackEvalMatrix =
    {
    { 2.0,  3.0,  1.0,  0.0,  0.0,  1.0,  3.0,  2.0 },
    { 2.0,  2.0,  0.0,  0.0,  0.0,  0.0,  2.0,  2.0 },
    {-1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0},
    {-2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0},
    {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
    {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
    {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
    {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0}
    };

	TArray<TSharedPtr<Chess_Move>>GetPieceMoves() override;
	float GetPieceValue() override;
    float GetPositionValue(FVector2D xy) override {
        return PieceColor == ChessColor::WHITE ? WhiteEvalMatrix[xy.Y][xy.X] : BlackEvalMatrix[xy.Y][xy.X];
    }
};
