// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "ChessPiece.h"
//#include "Chessboard.h"
#include "CoreMinimal.h"

/**
 * TODO: ADD CHECK AND CHECKMATE
 */
class ASquare;
class AChessPiece;
class AChessboard;
enum ChessColor;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMove, const FString, MovePerformed);

UENUM()
enum MoveType {
	STANDARD,
	PAWN_PROMOTION,
	EN_PASSANT,
	CASTLE
};

class CHESS_API Chess_Move
{
public:
	MoveType MoveClass;
	float MoveValue;
	//attributes modified after the move is done, defaults NAC (not a color)
	ChessColor PlayerOnCheck;		//if the move make opponent's king on check the color of the opponent is saved
	ChessColor PlayerOnCheckMate;	//if the move make opponent's king on mate the color of the opponent is saved
	ChessColor PlayerOnStall;		//if the move make opponent unable to move any piece the coloro of the opponent is saved

	AChessPiece* CapturingPiece;	//reference to the capturing piece at move creation time (piece at from)
	FVector2D From;

	AChessPiece* CapturedPiece;		//reference to the captured piece at move creation time (piece at to)
	FVector2D To;

	AChessboard* ReferredBoard;

	//bool bPromotionAfterMove = false;//setted by constructor if the moved piece is a pawn and reaches end of the board

	Chess_Move(FVector2D f, FVector2D t, AChessboard* board);//generic move constructor

	//TODO: Add pawn promotion constructor to avoid null actions in pawnpromotion

	//enpassant move constructor : enPassantCapturePosition specify the position of the "special captured" piece
	Chess_Move(FVector2D f, FVector2D t, AChessboard* board, FVector2D enPassantCapturePosition);
	
	Chess_Move(FVector2D f, FVector2D t, AChessboard* board, bool isQueenSide);

	void MakeMove(bool simulate);
	void RollbackMove(bool simulate);

	void PromotePawn(bool simulate, TSubclassOf<AChessPiece> selectedPiece);//promote the pawn, if selectedPiece is null the method uses PawnPromotionAusRef as input

	bool IsLegal();

	FString ToString() const;
	ChessColor GetMoveColor() const;
	~Chess_Move();
private:
	//FString PieceLetter;
	static FString GetPieceLetter(AChessPiece* piece);

	void CalculateResult();

	AChessPiece* PawnPromotionAusRef = nullptr;//aus variable for rollback purpose
	void PromotePawnRollback(bool simulate);


	AChessPiece* EnPassantCapturedPiece = nullptr;//as variable to restore captured-enpassant pieces. Position is not needed because can be calculated on "From and To"
	//FVector2D EnPassantCapturedPiecePosition;
	void EnPassantCapture(bool simulate);
	void EnPassantCaptureRollback(bool simulate);

	bool bIsQueenSide; //bool used by Castle moves to identify the rook to move
	AChessPiece* CastledRook;//saving the rook used in the castle
	void CastleMove(bool simulate);
	void CastleMoveRollback(bool simulate);

	float MoveValueCalculation();//higher value = best move for white; lower value = best move for black;

	void UpdateCastleVariables();
	void RollbackCastleVariables();

	//bool bCastleVariablesWereModified[4] = {false, false, false, false};//0 -> long white castle, 1 -> short white castle, 2 -> long black castle, 3 -> short black castle
	bool bSetWhiteCastleLongOnUndo = false;//variables used to remember if the castle variables have to be set on undo
	bool bSetWhiteCastleShortOnUndo = false;
	bool bSetBlackCastleLongOnUndo = false; 
	bool bSetBlackCastleShortOnUndo = false;
};