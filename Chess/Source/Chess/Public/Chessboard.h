//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Square.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chess_Move.h"
#include "Chessboard.generated.h"

UENUM()
enum ChessColor {
	WHITE,
	BLACK,
	NAC,
};

UENUM()
enum CP {
	BISHOP,
	KING,
	KNIGHT,
	PAWN,
	QUEEN,
	ROOK
};

class AChessPiece;
//TODO: Use chess_move class: move delegate in chess_move and make a move and rollback (keep the interface but move the logic)

UCLASS()
class CHESS_API AChessboard : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AChessboard();

	//TSharedPtr<Chess_Move> PopLastMove();

	TMap<AChessPiece*, FVector2D> GetPieces(ChessColor C);

	UFUNCTION()
	void ResetBoard();

	bool IsOnReplay();//return true if the board is on "replay-mode" (in this mode the player cannot do any move)

	//methods for data structure abstraction:
	FVector2D* GetXYFromPiece(AChessPiece* p);

	AChessPiece* GetPieceFromXY(FVector2D xy);

	ASquare* GetSquareFromXY(FVector2D xy);

	bool SetPieceFromXY(FVector2D xy, AChessPiece* p);//return false if xy is not valid

	ChessColor GetPieceColorFromXY(FVector2D xy);

	UPROPERTY(Transient)
	TMap<FVector2D, ASquare*> XY_Square;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASquare> SquareClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 BoardSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SquareSize;

	//promote the piece in last move with the corrispondent chesspiece. returns a pointer to the updated move
	TSharedPtr<Chess_Move> PromoteLastMove(CP ChessPieceEnum);

	bool CheckControl(ChessColor C);
	
	bool MateControl(ChessColor C);

	bool StallControl(ChessColor C);

	FVector2D* GetKingPosition(ChessColor C);

	bool MakeASafeMove(TSharedPtr<Chess_Move> move);

	void MakeAMove(TSharedPtr<Chess_Move> move, bool simulate);

	void RollbackMove(TSharedPtr<Chess_Move> move, bool simulate);

	TArray<FVector2D> GetFeasibleSquares(AChessPiece* Piece, bool Show);

	void CancelFeasibleSquares();

	void RestoreBoardColors();

	virtual void OnConstruction(const FTransform& Transform) override;

	void GenerateField();

	void RemovePiece(AChessPiece* p);

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

	TArray<TSharedPtr<Chess_Move>> StackMoves;

	TArray<TSharedPtr<Chess_Move>> StackUndoMoves;

	//UPROPERTY(VisibleAnywhere)
	AChessPiece* WhiteKing;

	//UPROPERTY(VisibleAnywhere)
	AChessPiece* BlackKing;

	//UPROPERTY(VisibleAnywhere)
	TMap<AChessPiece*, FVector2D> WhitePieces;

	//UPROPERTY(VisibleAnywhere)
	TMap<AChessPiece*, FVector2D> BlackPieces;

	AChessPiece* SpawnStarterPieceByXYPosition(const int32 InX, const int32 InY);

	//void RollbackMove(FVector2D, FVector2D, AChessPiece*);

	void FilterMovesAvoidCheck(AChessPiece* p, TArray<FVector2D>& moves);

	UFUNCTION()
	void PushAndPopUntilMove(int32 moveNumber);
};
