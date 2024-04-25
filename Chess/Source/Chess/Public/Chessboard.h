//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Square.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chess_Move.h"
#include "Chess_MoveCastle.h"
#include "Chess_MovePawnPromotion.h"
#include "Chess_MoveEnPassant.h"
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

	//float PositionValue;//sum of white material - black material; initialized at the spawn of the chesspiece and updated when a move is done
	float WhiteMaterial;

	float BlackMaterial;

	TArray<AChessPiece*> GetPieces(ChessColor C);

	UFUNCTION()
	void ResetBoard();

	//bool IsOnReplay();//return true if the board is on "replay-mode" (in this mode the player cannot do any move)

	//methods for data structure abstraction:
	FVector2D* GetXYFromPiece(AChessPiece* p);

	AChessPiece* GetPieceFromXY(FVector2D xy);

	ASquare* GetSquareFromXY(FVector2D xy);

	bool SetPieceFromXY(FVector2D xy, AChessPiece* p);//return false if xy is not valid

	ChessColor GetPieceColorFromXY(FVector2D xy);

	TSharedPtr<Chess_Move> GetMoveShowedOnBoard();

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

	void HandledMakeAMove(TSharedPtr<Chess_Move> move, bool simulate);

	void HandledRollbackAMove(TSharedPtr<Chess_Move> move, bool simulate);

	TArray<TSharedPtr<Chess_Move>> GetMovesAndShowHints(AChessPiece* Piece, bool Show);

	void CancelFeasibleSquares();

	void RestoreBoardColors();

	virtual void OnConstruction(const FTransform& Transform) override;

	void GenerateField();

	void RemovePiece(AChessPiece* p);

	FVector GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const;

	FVector2D GetXYPositionByRelativeLocation(const FVector& Location) const;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector2D EnPassantPossibleCapture = FVector2D(-10, -10);//setted to a legal position by makemove if there are conditions for enpassant

	//Castle variables, updated when a move is performed (chess_move class)
	bool bCastleBlackLong = false;//true;
	bool bCastleBlackShort = false;//true;
	bool bCastleWhiteLong = false;//true;
	bool bCastleWhiteShort = false;//true;

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

	UFUNCTION()
	void ReplayMove(int32 moveNumber);

	UFUNCTION()
	void RemoveUndoneMoves(int32 moveNumber);
protected:

private:

	TArray<TSharedPtr<Chess_Move>> StackMoves;

	TArray<TSharedPtr<Chess_Move>> StackUndoMoves;

	//UPROPERTY(VisibleAnywhere)
	AChessPiece* WhiteKing;

	//UPROPERTY(VisibleAnywhere)
	AChessPiece* BlackKing;

	//UPROPERTY(VisibleAnywhere)
	TArray<AChessPiece*> WhitePieces;

	//UPROPERTY(VisibleAnywhere)
	TArray<AChessPiece*> BlackPieces;

	AChessPiece* SpawnStarterPieceByXYPosition(const int32 InX, const int32 InY);

	//void FilterMovesAvoidCheck(AChessPiece* p, TArray<FVector2D>& moves);

	void PushAndPopUntilMove(int32 moveNumber);

};
