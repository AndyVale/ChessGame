// Fill out your copyright notice in the Description page of Project Settings.

#include "Chess_Move.h"
#include "ChessPiece.h"
#include "Square.h"
#include "Chessboard.h"
#include "CP_Bishop.h"
#include "CP_King.h"
#include "CP_Knight.h"
#include "CP_Pawn.h"
#include "CP_Queen.h"
#include "CP_Rook.h"

Chess_Move::Chess_Move(FVector2D f, FVector2D t, AChessboard* board) : From(f), To(t), ReferredBoard(board)
{
    PlayerOnCheck = ChessColor::NAC;		
    PlayerOnCheckMate = ChessColor::NAC;
    PlayerOnStall = ChessColor::NAC;
    PieceFrom = board->GetPieceFromXY(f);
    PieceTo = board->GetPieceFromXY(t);
    PieceLetter = GetPieceLetter();
}

void Chess_Move::CalculateResult()
{
    UE_LOG(LogTemp, Error, TEXT("CalculateResult"));
    //TODO: FARLO FUNZIONARE
    //Calculate move resultTODO

    AChessPiece* XOpponent = PieceFrom;
    ChessColor OpponentColor = XOpponent->PieceColor == WHITE? BLACK : WHITE;

    if (ReferredBoard->CheckControl(OpponentColor))
    {
        PlayerOnCheck = OpponentColor;
        if (ReferredBoard->MateControl(OpponentColor))
        {
            PlayerOnCheckMate = OpponentColor;
        }
    }
    else
    {
        if (ReferredBoard->StallControl(OpponentColor))
        {
            PlayerOnStall = OpponentColor;
        }
    }
}

void Chess_Move::MakeMove(bool simulate)
{
    AChessPiece* eatingPiece = PieceFrom;
    AChessPiece* eatenPiece = PieceTo;

    if (!eatingPiece)
    {
        UE_LOG(LogTemp, Error, TEXT("MakeAMove:Impossible move, no pieces in old position"));
        return;
    }

    //Making the move:
    ReferredBoard->RemovePiece(eatingPiece);
    if (eatenPiece)
    {
        ReferredBoard->RemovePiece(eatenPiece);//remove NewPiece
    }
    ReferredBoard->SetPieceFromXY(To, eatingPiece);

    //Moving the piece
    if (!simulate)//not a simulation, move the piece
    {
        if (eatenPiece)
        {
            if (eatenPiece->PieceColor == BLACK)//TODO:RIARRANGIA
                eatenPiece->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(-1, -1));
            else
                eatenPiece->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(8, 8));
            eatenPiece->SetActorHiddenInGame(true);
        }
        (eatingPiece)->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(To[0], To[1]));
    }

    //CalculateResult test for check, checkmate and stall. Is importat to avoid this procedure in case it's a simulative move (for performance reasons)
    if (!simulate)
    {
        CalculateResult();
    }
}


void Chess_Move::RollbackMove(bool simulate)
{
    ReferredBoard->SetPieceFromXY(To, PieceTo);
    ReferredBoard->SetPieceFromXY(From, PieceFrom);
    if (!simulate) {
        if (PieceTo)
        {
            PieceTo->SetActorHiddenInGame(false);
            PieceTo->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(To[0], To[1]));
        }
        PieceFrom->SetActorHiddenInGame(false);
        PieceFrom->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(From[0], From[1]));
        //rimetti in gioco la pedina mangiata
    }
}

FString Chess_Move::ToString() const
{
    FString AlgebraicNotation;
    FString Columns = "hgfedcba";
    FString Rows = "12345678";
    // Converti le coordinate 'From' e 'To' in notazione algebrica
    AlgebraicNotation += PieceLetter;
    AlgebraicNotation += Columns[From.X];
    AlgebraicNotation += Rows[From.Y];
    if (PieceTo != nullptr)//eat
    {
        AlgebraicNotation += 'x';
    }
    else {
        AlgebraicNotation += '-';
    }
    AlgebraicNotation += Columns[To.X];
    AlgebraicNotation += Rows[To.Y];
    if (PlayerOnCheck != NAC)
    {
        if (PlayerOnCheckMate == NAC)//it's only a check
        {
            AlgebraicNotation += '+';
        }
        else//it's checkmate
        {
            AlgebraicNotation += '#';
        }
    }

    return AlgebraicNotation;
}

ChessColor Chess_Move::GetMoveColor() const
{
    UE_LOG(LogTemp, Error, TEXT("MoveColor"));

    if (PieceFrom)
    {
        return PieceFrom->PieceColor;
    }
    return ChessColor::NAC;
}

Chess_Move::~Chess_Move()
{
}

FString Chess_Move::GetPieceLetter()
{
    if (const ACP_Bishop* tmp = Cast<ACP_Bishop>(PieceFrom)) {
        return FString("B");
    }
    if (const ACP_King* tmp = Cast<ACP_King>(PieceFrom)) {
        return FString("K");
    }
    if (const ACP_Knight* tmp = Cast<ACP_Knight>(PieceFrom)) {
        return FString("N");
    }
    if (const ACP_Pawn* tmp = Cast<ACP_Pawn>(PieceFrom)) {
        return FString("");
    }
    if (const ACP_Queen* tmp = Cast<ACP_Queen>(PieceFrom)) {
        return FString("Q");
    }
    if (const ACP_Rook* tmp = Cast<ACP_Rook>(PieceFrom)) {
        return FString("R");
    }
    return FString("");
}
