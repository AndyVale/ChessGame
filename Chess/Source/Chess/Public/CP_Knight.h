// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChessPiece.h"
#include "CP_Knight.generated.h"

/**
 * 
 */
UCLASS()
class CHESS_API ACP_Knight : public AChessPiece
{
	GENERATED_BODY()
public:
    static inline const TArray<TArray<float>> WhiteEvalMatrix =
    {
     {-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0},
     {-4.0, -2.0,  0.0,  0.0,  0.0,  0.0, -2.0, -4.0},
     {-3.0,  0.0,  1.0,  1.5,  1.5,  1.0,  0.0, -3.0},
     {-3.0,  0.5,  1.5,  2.0,  2.0,  1.5,  0.5, -3.0},
     {-3.0,  0.0,  1.5,  2.0,  2.0,  1.5,  0.0, -3.0},
     {-3.0,  0.5,  1.0,  1.5,  1.5,  1.0,  0.5, -3.0},
     {-4.0, -2.0,  0.0,  0.5,  0.5,  0.0, -2.0, -4.0},
     {-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0}
    };

    // Matrice di valutazione per il cavallo nero
    static inline const TArray<TArray<float>> BlackEvalMatrix =
    {
    {-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0},
    {-4.0, -2.0,  0.0,  0.0,  0.0,  0.0, -2.0, -4.0},
    {-3.0,  0.0,  1.0,  1.5,  1.5,  1.0,  0.0, -3.0},
    {-3.0,  0.5,  1.5,  2.0,  2.0,  1.5,  0.5, -3.0},
    {-3.0,  0.0,  1.5,  2.0,  2.0,  1.5,  0.0, -3.0},
    {-3.0,  0.5,  1.0,  1.5,  1.5,  1.0,  0.5, -3.0},
    {-4.0, -2.0,  0.0,  0.5,  0.5,  0.0, -2.0, -4.0},
    {-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0}
    };

	TArray<Chess_Move>GetPieceMoves() override;
    float GetPieceValue() override;
    float GetCorrectedPieceValue() override;
    float GetPositionValue(FVector2D xy) override {
        return PieceColor == ChessColor::WHITE ? WhiteEvalMatrix[xy.Y][xy.X] : BlackEvalMatrix[xy.Y][xy.X];
    }
};
