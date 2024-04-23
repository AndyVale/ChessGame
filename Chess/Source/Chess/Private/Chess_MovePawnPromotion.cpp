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
    FVector position = ReferredBoard->GetRelativeLocationByXYPosition(To.X, To.Y);
    FRotator rotation = FRotator(0, 0, 0);
    ChessColor pColor = GetMoveColor();
    lastSelectedPiece = selectedPiece;
    //PawnPromotionAusRef = ReferredBoard->GetWorld()->SpawnActor<AChessPiece>(selectedPiece, position, rotation);
    //PawnPromotionAusRef->SetColorAndMaterial(pColor);
    //PawnPromotionAusRef->SetActorHiddenInGame(true);
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
//PROMOZIONE NON FUNZIONA ---> HARD REFACTORING NEEDED (aggiungere metodi nella classe del pedone per la promozione)
void Chess_MovePawnPromotion::PromotePawn(bool simulate, CP selectedPiece)
{
    if (selectedPiece != CP::PAWN && selectedPiece != CP::KING) {
        lastSelectedPiece = selectedPiece;
    }
    if (ACP_Pawn* pawn = Cast<ACP_Pawn>(CapturingPiece))
    {
		pawn->PromoteIn(lastSelectedPiece);
	}
}


void Chess_MovePawnPromotion::PromotePawnRollback(bool simulate)
{
    if (ACP_Pawn* pawn = Cast<ACP_Pawn>(CapturingPiece))
    {
        pawn->PromoteIn(CP::PAWN);
    }
}