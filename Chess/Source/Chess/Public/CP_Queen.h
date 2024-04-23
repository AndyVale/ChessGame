// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChessPiece.h"
#include "CP_Queen.generated.h"

/**
 * 
 */
UCLASS()
class CHESS_API ACP_Queen : public AChessPiece
{
	GENERATED_BODY()
public:
    static inline const TArray<TArray<float>> WhiteEvalMatrix =
    {
    { -20.0, -10.0, -10.0, -5.0, -5.0, -10.0, -10.0, -20.0 },
    { -10.0,  -10.0,   -10.0,   -10.0,  -10.0, -10.0, -10.0,  -10.0 },
    { -10.0,  0.0,   0.0,   0.0,  0.0,   0.0,   0.0,  -10.0 },
    {  -5.0,   0.0,   0.0,   0.0,  0.0,  0.0,   0.0,  0.0},
    { -5.0,   0.0,   0.0,   0.0,  0.0,   0.0,   0.0,  -5.0 },
    { -10.0,  0.0,   0.0,   0.0,  0.0,   0.0,   0.0,  -10.0 },
    { -10.0,  0.0,   0.0,   0.0,  0.0,   0.0,   0.0,  -10.0 },
    { -20.0, -10.0, -10.0, -5.0, -5.0, -10.0, -10.0, -20.0 }
    };


    // Matrice di valutazione per la regina nera
    static inline const TArray<TArray<float>> BlackEvalMatrix =
    {
    { -20.0, -10.0, -10.0, -5.0, -5.0, -10.0, -10.0, -20.0 },
    { -10.0,  0.0,   0.0,   0.0,  0.0,   0.0,   0.0,  -10.0 },
    { -10.0,  0.0,   0.0,   0.0,  0.0,   0.0,   0.0,  -10.0 },
    { -5.0,   0.0,   0.0,   0.0,  0.0,   0.0,   0.0,  -5.0 },
    {  0.0,   0.0,   0.0,   0.0,  0.0,  0.0,   0.0,  -5.0 },
    { -10.0,  0.0,   0.0,   0.0,  0.0,   0.0,   0.0,  -10.0 },
    { -10.0,  -10.0,   -10.0,   -10.0,  -10.0, -10.0, -10.0,  -10.0 },
    { -20.0, -10.0, -10.0, -5.0, -5.0, -10.0, -10.0, -20.0 }
    };

	TArray<TSharedPtr<Chess_Move>>GetPieceMoves() override;
	float GetPieceValue() override;
    float GetPositionValue(FVector2D xy) override {
        return PieceColor == ChessColor::WHITE ? WhiteEvalMatrix[xy.Y][xy.X] : BlackEvalMatrix[xy.Y][xy.X];
	}
};
