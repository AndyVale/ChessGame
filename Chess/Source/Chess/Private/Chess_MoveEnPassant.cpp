// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_MoveEnPassant.h"
#include "ChessPiece.h"
#include "Square.h"
#include "Chessboard.h"
#include "CP_Bishop.h"
#include "CP_King.h"
#include "CP_Knight.h"
#include "CP_Pawn.h"
#include "CP_Queen.h"
#include "CP_Rook.h"

Chess_MoveEnPassant::Chess_MoveEnPassant(FVector2D f, FVector2D t, AChessboard* board, FVector2D enPassantCapturePosition):
    Chess_Move(f, t, board)
{
    MoveClass = MoveType::EN_PASSANT;
    CapturingPiece = board->GetPieceFromXY(f);
    if (CapturingPiece == nullptr)
    {
        int a = 5;
    }
    CapturedPiece = board->GetPieceFromXY(t);
    EnPassantCapturedPiece = board->GetPieceFromXY(enPassantCapturePosition);
    MoveValue = MoveValueCalculation();
}

void Chess_MoveEnPassant::MakeMove(bool simulate)
{
    Chess_Move::MakeMove(simulate);
    EnPassantCapture(simulate);
    if (!simulate)
    {
        CalculateResult();
    }

    UpdateCastleVariables();
}

void Chess_MoveEnPassant::RollbackMove(bool simulate)
{
    EnPassantCaptureRollback(simulate);
    Chess_Move::RollbackMove(simulate);
}

//--------------------- EnPassant methods:

void Chess_MoveEnPassant::EnPassantCapture(bool simulate)
{
    //capture piece:
    if (EnPassantCapturedPiece)
    {
        ReferredBoard->RemovePiece(EnPassantCapturedPiece);
        if (!simulate)
        {
            if (EnPassantCapturedPiece->PieceColor == BLACK)//TODO:RIARRANGIA
                EnPassantCapturedPiece->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(-1, -1));
            else
                EnPassantCapturedPiece->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(8, 8));
            EnPassantCapturedPiece->SetActorHiddenInGame(true);
        }
    }
}

void Chess_MoveEnPassant::EnPassantCaptureRollback(bool simulate)
{
    //restore piece:
    if (EnPassantCapturedPiece)
    {
        FVector2D EnPassantCapturedPiecePosition = FVector2D(To.X, From.Y);
        ReferredBoard->SetPieceFromXY(EnPassantCapturedPiecePosition, EnPassantCapturedPiece);
        if (!simulate)
        {
            EnPassantCapturedPiece->SetActorHiddenInGame(false);
            EnPassantCapturedPiece->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(EnPassantCapturedPiecePosition.X, EnPassantCapturedPiecePosition.Y));
        }
    }
}
