// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChessPiece.h"
#include "CP_Rook.generated.h"

/**
 * 
 */
UCLASS()
class CHESS_API ACP_Rook : public AChessPiece
{
	GENERATED_BODY()
public:
	TArray<FVector2D>GetPieceMoves(FVector2D*, AChessboard* Board) override;
};
