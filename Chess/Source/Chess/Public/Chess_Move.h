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

class CHESS_API Chess_Move
{
public:
	//attributes modified after the move is done, defaults NAC (not a color)
	ChessColor PlayerOnCheck;		//if the move make opponent's king on check the color of the opponent is saved
	ChessColor PlayerOnCheckMate;	//if the move make opponent's king on mate the color of the opponent is saved
	ChessColor PlayerOnStall;		//if the move make opponent unable to move any piece the coloro of the opponent is saved

	AChessPiece* PieceFrom;
	FVector2D From;

	AChessPiece* PieceTo;
	FVector2D To;

	AChessboard* ReferredBoard;

	Chess_Move(FVector2D f, FVector2D t, AChessboard* board);

	void MakeMove(bool simulate);
	void RollbackMove(bool simulate);

	FString ToString() const;
	ChessColor GetMoveColor() const;
	~Chess_Move();
private:
	FString PieceLetter;
	FString GetPieceLetter();

	void CalculateResult();
	//static bool CalculateCheck(FVector2D f, FVector2D t, AChessboard* board);
	//static bool CalculateMate(FVector2D f, FVector2D t, AChessboard* board);
	//static bool CalculateStall(FVector2D f, FVector2D t, AChessboard* board);
};
