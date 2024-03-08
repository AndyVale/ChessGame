//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Square.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chessboard.generated.h"

UENUM()
enum ChessColor {
	WHITE,
	BLACK,
	NAC,
};

class AChessPiece;

UCLASS()
class CHESS_API AChessboard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChessboard();

	AChessPiece* GetPieceFromXY(FVector2D xy);

	bool SetPieceFromXY(FVector2D xy, AChessPiece* p);//return false if xy is not valid

	ChessColor GetPieceColorFromXY(FVector2D xy);

	UPROPERTY(Transient)
	TMap<AChessPiece*, FVector2D> Piece_XY;

	UPROPERTY(Transient)
	TMap<FVector2D, AChessPiece*> XY_Piece;

	UPROPERTY(Transient)
	TMap<FVector2D, ASquare*> XY_Square;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASquare> SquareClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 BoardSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SquareSize;

	//UFUNCTION(BluePrintCallable)
	//void ResetField();

	void MakeASafeMove(FVector2D, FVector2D);//for human player

	void MakeAMove(FVector2D, FVector2D);//for AI

	void ShowFeasibleMoves(AChessPiece* Piece);

	void restoreBoardColors();

	virtual void OnConstruction(const FTransform& Transform) override;
	void GenerateField();

	FVector2D GetPositionFromHit(const FHitResult& Hit);

	FVector GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const;

	FVector2D GetXYPositionByRelativeLocation(const FVector& Location) const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TSubclassOf<AChessPiece> Bishop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TSubclassOf<AChessPiece> King;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TSubclassOf<AChessPiece> Knight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TSubclassOf<AChessPiece> Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TSubclassOf<AChessPiece> Queen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TSubclassOf<AChessPiece> Rook;

private:

	AChessPiece* SpawnStarterPieceByXYPosition(const int32 InX, const int32 InY) const;
};
