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
	TArray<FVector2D>GetFeasibleMoves(FVector2D*xy, AChessboard* Board) override;
};
