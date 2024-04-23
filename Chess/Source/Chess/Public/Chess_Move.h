// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "ChessPiece.h"
//#include "Chessboard.h"
#include "CoreMinimal.h"

class ASquare;
class AChessPiece;
class AChessboard;
enum ChessColor;

//enum for the type of move, used to determine the class of the move
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

	//attributes modified after the move is done and calculate result method is called, defaults NAC (not a color)
	ChessColor PlayerOnCheck;		//if the move make opponent's king on check the color of the opponent is saved
	ChessColor PlayerOnCheckMate;	//if the move make opponent's king on mate the color of the opponent is saved
	ChessColor PlayerOnStall;		//if the move make opponent unable to move any piece the coloro of the opponent is saved

	AChessPiece* CapturingPiece;	//reference to the capturing piece at move creation time (piece at from)
	FVector2D From;

	AChessPiece* CapturedPiece;		//reference to the captured piece at move creation time (piece at to)
	FVector2D To;

	AChessboard* ReferredBoard;

	Chess_Move(FVector2D f, FVector2D t, AChessboard* board);//generic move constructor

	//Do - Undo methods for the move:
	virtual void MakeMove(bool simulate); //overrided by child class
	virtual void RollbackMove(bool simulate);

	//Method to check if the move is legal or not (used to filter the moves that put king on check)
	bool IsLegal();
	
	//Method to get the string representation of the move
	virtual FString ToString() const;//overrided by castle and pawnpromotion

	//Method to get the color of the player that made the move
	ChessColor GetMoveColor() const;
	virtual ~Chess_Move() = default;

protected:
	//Static method to get the letter of the piece (used to print the move)
	static FString GetPieceLetter(AChessPiece* piece);

	//Method used to update PlayerOnCheck, PlayerOnCheckMate and PlayerOnStall
	void CalculateResult();

	//Method to calculate the value of the move (value of eaten piece)
	float MoveValueCalculation();//higher value = best move for white; lower value = best move for black;

	//Methods used to update and rollback the castle variables of the board:
	void UpdateCastleVariables();
	void RollbackCastleVariables();

	//Auxiliar variables used to remember if the castle variables have to be set on RollbackCastleVariables
	bool bSetWhiteCastleLongOnUndo = false;
	bool bSetWhiteCastleShortOnUndo = false;
	bool bSetBlackCastleLongOnUndo = false; 
	bool bSetBlackCastleShortOnUndo = false;
};