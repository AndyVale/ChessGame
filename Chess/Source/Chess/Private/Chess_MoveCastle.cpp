// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_MoveCastle.h"
#include "ChessPiece.h"
#include "Square.h"
#include "Chessboard.h"
#include "CP_Bishop.h"
#include "CP_King.h"
#include "CP_Knight.h"
#include "CP_Pawn.h"
#include "CP_Queen.h"
#include "CP_Rook.h"

Chess_MoveCastle::Chess_MoveCastle(FVector2D f, FVector2D t, AChessboard* board, bool isQueenSide):
    Chess_Move(f, t, board)
{
    MoveClass = MoveType::CASTLE;
    CapturingPiece = board->GetPieceFromXY(f);
    if (CapturingPiece == nullptr)
    {
        int a = 5;
    }
    CapturedPiece = board->GetPieceFromXY(t);
    MoveValue = MoveValueCalculation();
    bIsQueenSide = isQueenSide;
}

void Chess_MoveCastle::MakeMove(bool simulate)
{
    Chess_Move::MakeMove(simulate);
    CastleMove(simulate);
    if (!simulate)
    {
        CalculateResult();
    }

    UpdateCastleVariables();
}

void Chess_MoveCastle::RollbackMove(bool simulate)
{
    CastleMoveRollback(simulate);
    Chess_Move::RollbackMove(simulate);
}

FString Chess_MoveCastle::ToString() const
{
    FString AlgebricNotation = bIsQueenSide ? "0-0-0" : "0-0";
    if (PlayerOnCheck != NAC)
    {
        if (PlayerOnCheckMate == NAC)//it's only a check
        {
            AlgebricNotation += '+';
        }
        else//it's checkmate
        {
            AlgebricNotation += '#';
        }
    }
    return AlgebricNotation;
}

//--------------------- Castle methods:

void Chess_MoveCastle::CastleMove(bool simulate)
{
    FVector2D newRookPosition;
    if (bIsQueenSide)
    {
        FVector2D   BlackLeftRookPosition = FVector2D(0, 0), WhiteLeftRookPosition = FVector2D(0, 7), rookBlackQueenSidePos = FVector2D(3, 0), rookWhiteQueenSidePos = FVector2D(3, 7);
        CastledRook = GetMoveColor() == ChessColor::WHITE ? ReferredBoard->GetPieceFromXY(WhiteLeftRookPosition) : ReferredBoard->GetPieceFromXY(BlackLeftRookPosition);
        newRookPosition = GetMoveColor() == ChessColor::WHITE ? rookWhiteQueenSidePos : rookBlackQueenSidePos;
    }
    else
    {
        FVector2D BlackRightRookPosition = FVector2D(7, 0), WhiteRightRookPosition = FVector2D(7, 7), rookBlackKingSidePos = FVector2D(5, 0), rookWhiteKingSidePos = FVector2D(5, 7);
        CastledRook = GetMoveColor() == ChessColor::WHITE ? ReferredBoard->GetPieceFromXY(WhiteRightRookPosition) : ReferredBoard->GetPieceFromXY(BlackRightRookPosition);
        newRookPosition = GetMoveColor() == ChessColor::WHITE ? rookWhiteKingSidePos : rookBlackKingSidePos;
    }
    ReferredBoard->RemovePiece(CastledRook);
    ReferredBoard->SetPieceFromXY(newRookPosition, CastledRook);

    if (!simulate)
    {
        CastledRook->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(newRookPosition.X, newRookPosition.Y));
    }
}

void Chess_MoveCastle::CastleMoveRollback(bool simulate)
{
    FVector2D oldRookPosition;
    if (bIsQueenSide)
    {
        FVector2D BlackLeftRookPosition = FVector2D(0, 0), WhiteLeftRookPosition = FVector2D(0, 7), rookBlackQueenSidePos = FVector2D(3, 0), rookWhiteQueenSidePos = FVector2D(3, 7);
        oldRookPosition = GetMoveColor() == ChessColor::WHITE ? WhiteLeftRookPosition : BlackLeftRookPosition;
    }
    else
    {
        FVector2D BlackRightRookPosition = FVector2D(7, 0), WhiteRightRookPosition = FVector2D(7, 7), rookBlackKingSidePos = FVector2D(5, 0), rookWhiteKingSidePos = FVector2D(5, 7);
        oldRookPosition = GetMoveColor() == ChessColor::WHITE ? WhiteRightRookPosition : BlackRightRookPosition;
    }
    ReferredBoard->RemovePiece(CastledRook);
    ReferredBoard->SetPieceFromXY(oldRookPosition, CastledRook);

    if (!simulate)
    {
        CastledRook->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(oldRookPosition.X, oldRookPosition.Y));
    }
}
