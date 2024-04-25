//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Chessboard.h"
#include "Chess_Move.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChessPiece.generated.h"

UCLASS(Abstract)
class CHESS_API AChessPiece : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AChessPiece();

	ChessColor PieceColor;

	FVector2D PiecePosition;

	AChessboard* ReferredBoard;

	void SetColorAndMaterial(ChessColor c);

	//GetPieceMoves get all the possible moves of a piece, no regarding at check situations or similar
	virtual TArray<TSharedPtr<Chess_Move>>GetPieceMoves() PURE_VIRTUAL(AChessPiece::GetPieceMoves, return TArray<TSharedPtr<Chess_Move>>(););

	//GetPieceLegalMoves get only the legal moves of a piece, much more heavy to compute than GetPieceMoves so it should be used only when needed
	TArray<TSharedPtr<Chess_Move>> GetPieceLegalMoves();

	virtual float GetPieceValue() PURE_VIRTUAL(AChessPiece::GetPieceValue, return float(););//return piece value

	virtual float GetCorrectedPieceValue();//return piece value corrected on piece position

	//return piece value on the specified xy position (work in progress)
	virtual float GetPositionValue(FVector2D xy) PURE_VIRTUAL(AChessPiece::GetPositionValue, return float(););

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* ChessPieceMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UMaterialInstance* BlackMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UMaterialInstance* WhiteMaterial;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//Asuiliary methods for Queen and Rook:
	static void GetFeasibleDiagonals(AChessPiece* piece, TArray<TSharedPtr<Chess_Move>>& moves);
	//Asuiliary methods for Queen and Bishop moves:
	static void GetFeasibleCross(AChessPiece* piece, TArray<TSharedPtr<Chess_Move>>& moves);
};
