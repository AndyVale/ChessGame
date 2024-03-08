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
	TArray<FVector2D>GetFeasibleMoves(FVector2D*, AChessboard* Board) override;
};
