//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Chessboard.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChessPiece.generated.h"


UCLASS()
class CHESS_API AChessPiece : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChessPiece();
	ChessColor PieceColor;
	void SetColorAndMaterial(ChessColor c);
	//virtual *** getFeasibleMoves();
	virtual TArray<FVector2D>GetFeasibleMoves(FVector2D* xy, AChessboard* Board);
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
	//Asuiliary methods for Queen, Rook and Bishop moves:
	void GetFeasibleDiagonals(FVector2D* xy, AChessboard* Board, TArray<FVector2D>& moves);
	void GetFeasibleCross(FVector2D* xy, AChessboard* Board, TArray<FVector2D>& moves);
};
