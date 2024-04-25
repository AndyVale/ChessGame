// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_MovePawnPromotion.h"
#include "ChessPiece.h"
#include "Square.h"
#include "Chessboard.h"
#include "CP_Bishop.h"
#include "CP_King.h"
#include "CP_Knight.h"
#include "CP_Pawn.h"
#include "CP_Queen.h"
#include "CP_Rook.h"

Chess_MovePawnPromotion::Chess_MovePawnPromotion(FVector2D f, FVector2D t, AChessboard* board, CP selectedPiece):
    Chess_Move(f, t, board)
{
    MoveClass = MoveType::PAWN_PROMOTION;
    if (CapturingPiece == nullptr)
    {
		UE_LOG(LogTemp, Error, TEXT("Chess_MovePawnPromotion:No pieces in old position in the constructor"));
	}
    lastSelectedPiece = selectedPiece;//save the selected piece for the promotion
}

void Chess_MovePawnPromotion::MakeMove(bool simulate)
{
    Chess_Move::MakeMove(simulate);
    PromotePawn(simulate, CP::KING);
    if (!simulate)
    {
        CalculateResult();
    }

    UpdateCastleVariables();
}

void Chess_MovePawnPromotion::RollbackMove(bool simulate)
{
    PromotePawnRollback(simulate);
    Chess_Move::RollbackMove(simulate);
}

FString Chess_MovePawnPromotion::ToString() const
{
    FString AlgebricNotation;
    FString Columns = "abcdefgh";
    FString Rows = "87654321";

    AlgebricNotation += Columns[From.X];
    AlgebricNotation += Rows[From.Y];
    if (CapturedPiece != nullptr)//eat
    {
        AlgebricNotation += 'x';
    }
    else {
        AlgebricNotation += '-';
    }
    AlgebricNotation += Columns[To.X];
    AlgebricNotation += Rows[To.Y];

    //add the piece in which the pawn is promoted
    if (ACP_Pawn* pawn = Cast<ACP_Pawn>(CapturingPiece))
    {
		AlgebricNotation += "=" + GetPieceLetter(pawn->PromotedPiece);
	}

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

//--------------------- PawnPromotion methods:
void Chess_MovePawnPromotion::PromotePawn(bool simulate, CP selectedPiece)
{
    if (selectedPiece != CP::PAWN && selectedPiece != CP::KING) {//if a new piece is selected for the promotion update the value
        lastSelectedPiece = selectedPiece;
    }
    if (ACP_Pawn* pawn = Cast<ACP_Pawn>(CapturingPiece))
    {
		pawn->PromoteIn(lastSelectedPiece);//promote the pawn to the selected piece
	}
}


void Chess_MovePawnPromotion::PromotePawnRollback(bool simulate)
{
    if (ACP_Pawn* pawn = Cast<ACP_Pawn>(CapturingPiece))
    {
        pawn->PromoteIn(CP::PAWN);//downgrade the promoted piece to a pawn, no matter what it was
    }
}