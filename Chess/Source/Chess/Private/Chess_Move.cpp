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

Chess_Move::Chess_Move(FVector2D f, FVector2D t, AChessboard* board) : 
    PlayerOnCheck(ChessColor::NAC), PlayerOnCheckMate(ChessColor::NAC), PlayerOnStall(ChessColor::NAC), From(f), To(t), ReferredBoard(board)
{
    CapturingPiece = board->GetPieceFromXY(f);
    if (CapturingPiece == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("Chess_Move:No pieces in old position in the constructor"));
    }
    CapturedPiece = board->GetPieceFromXY(t);
    MoveClass = STANDARD;
    MoveValue = MoveValueCalculation();
}

void Chess_Move::CalculateResult()
{
    ChessColor OpponentColor = CapturingPiece->PieceColor == WHITE ? BLACK : WHITE;
    PlayerOnCheck = PlayerOnCheckMate = PlayerOnStall = ChessColor::NAC;

    //Calculate if the opponent is in check, checkmate or stall and change the variables accordingly (a sort of cache to avoid recalculating the same thing multiple times)
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
    if (!CapturingPiece)
    {
        UE_LOG(LogTemp, Error, TEXT("MakeAMove:Impossible move, no pieces in old position"));
        return;
    }

    //Making the move:
    ReferredBoard->RemovePiece(CapturingPiece);
    if (CapturedPiece)
    {
        ReferredBoard->RemovePiece(CapturedPiece);//remove NewPiece
    }
    ReferredBoard->SetPieceFromXY(To, CapturingPiece);

    //Moving the piece
    if (!simulate)//not a simulation, move the piece
    {
        if (CapturedPiece)
        {
            if (CapturedPiece->PieceColor == BLACK)
                CapturedPiece->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(-1, -1));
            else
                CapturedPiece->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(8, 8));
            CapturedPiece->SetActorHiddenInGame(true);
        }
        (CapturingPiece)->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(To[0], To[1]));
    }

    if (!simulate)
    {
        CalculateResult();
    }

    UpdateCastleVariables();
}


void Chess_Move::RollbackMove(bool simulate)
{
    ReferredBoard->RemovePiece(CapturingPiece);//remove the moved piece

    ReferredBoard->SetPieceFromXY(To, CapturedPiece);//put back the captured piece
    ReferredBoard->SetPieceFromXY(From, CapturingPiece);//put back the capturing piece


    if (CapturedPiece)
    {
        if (!simulate)
        {
            CapturedPiece->SetActorHiddenInGame(false);
            CapturedPiece->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(To[0], To[1]));
        }
    }

    if (CapturingPiece)
    {
        if (!simulate)
        {
            CapturingPiece->SetActorHiddenInGame(false);
            CapturingPiece->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(From[0], From[1]));
        }
    }

    RollbackCastleVariables();
}

bool Chess_Move::IsLegal()
{
    MakeMove(true);//a move is legal if the king is not in check after the move
    bool isLegal = !ReferredBoard->CheckControl(GetMoveColor());
    RollbackMove(true);
    return isLegal;
}

float Chess_Move::MoveValueCalculation()
{
    //move value is calculated based on heuristics
    float value = 0;
    if (CapturingPiece)
    {
        if (GetMoveColor() == ChessColor::WHITE)
        {//positive counting for white
            //value -= PieceFrom->GetPositionValue(From);//move value = newPiecePositionValue - oldPiecePositionValue
            //value += PieceFrom->GetPositionValue(To);
            if (CapturedPiece)
            {
                value += CapturedPiece->GetPieceValue();
            }
        }
        else
        {//negative counting for black
            //value += PieceFrom->GetPositionValue(From);//move value = newPiecePositionValue - oldPiecePositionValue
            //value -= PieceFrom->GetPositionValue(To);
            if (CapturedPiece)
            {
                value -= CapturedPiece->GetPieceValue();
            }
        }
    }
    return value;
}


void Chess_Move::UpdateCastleVariables()
{
    //Method check if in the move the castle variables have to be updated and set the auxiliar variables to remember if they have to be set on RollbackCastleVariables
    bool isWhite = GetMoveColor() == ChessColor::WHITE;

    if ((isWhite && ReferredBoard->bCastleWhiteLong && From == FVector2D(0, 7)) || (!isWhite && ReferredBoard->bCastleWhiteLong && To == FVector2D(0, 7)))//long castle was feasible and moved the rook/captured the rook
    {
        ReferredBoard->bCastleWhiteLong = false;
        bSetWhiteCastleLongOnUndo = true;
    }
    else if ((isWhite && ReferredBoard->bCastleWhiteShort && From == FVector2D(7, 7)) || (!isWhite && ReferredBoard->bCastleWhiteShort && To == FVector2D(7, 7)))//same for king side rook
    {
        ReferredBoard->bCastleWhiteShort = false;
        bSetWhiteCastleShortOnUndo = true;
    }
    else
    {
        if (isWhite && ReferredBoard->bCastleWhiteLong && From == FVector2D(4, 7))
        {
            ReferredBoard->bCastleWhiteLong = false;
            bSetWhiteCastleLongOnUndo = true;
        }
        if (isWhite && ReferredBoard->bCastleWhiteShort && From == FVector2D(4, 7))
        {
            ReferredBoard->bCastleWhiteShort = false;
            bSetWhiteCastleShortOnUndo = true;
        }
    }
    
    if ((!isWhite && ReferredBoard->bCastleBlackLong && From == FVector2D(0, 0)) || (isWhite && ReferredBoard->bCastleBlackLong && To == FVector2D(0, 0)))//long castle was feasible and moved the rook/captured the rook
    {
        ReferredBoard->bCastleBlackLong = false;
        bSetBlackCastleLongOnUndo = true;
    }
    else if ((!isWhite && ReferredBoard->bCastleBlackShort && From == FVector2D(7, 0)) || (isWhite && ReferredBoard->bCastleBlackShort && To == FVector2D(7, 0)))//same for king side rook
    {
        ReferredBoard->bCastleBlackShort = false;
        bSetBlackCastleShortOnUndo = true;
    }
    else
    {
        if (!isWhite && ReferredBoard->bCastleBlackLong && From == FVector2D(4, 0))
        {
            ReferredBoard->bCastleBlackLong = false;
            bSetBlackCastleLongOnUndo = true;
        }
        if (!isWhite && ReferredBoard->bCastleBlackShort && From == FVector2D(4, 0))
        {
            ReferredBoard->bCastleBlackShort = false;
            bSetBlackCastleShortOnUndo = true;
        }
    }
}

void Chess_Move::RollbackCastleVariables()
{
    if (bSetWhiteCastleLongOnUndo)
    {
        ReferredBoard->bCastleWhiteLong = true;
    }
    if (bSetBlackCastleLongOnUndo)
    {
        ReferredBoard->bCastleBlackLong = true;
    }
    if (bSetWhiteCastleShortOnUndo)
    {
        ReferredBoard->bCastleWhiteShort = true;
    }
    if (bSetBlackCastleShortOnUndo)
    {
        ReferredBoard->bCastleBlackShort = true;
    }
}

FString Chess_Move::ToString() const
{
    FString AlgebricNotation;
    FString Columns = "abcdefgh";
    FString Rows = "87654321";

    AlgebricNotation += GetPieceLetter(CapturingPiece);
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

ChessColor Chess_Move::GetMoveColor() const
{
    if (CapturingPiece)
    {
        return CapturingPiece->PieceColor;
    }
    return ChessColor::NAC;
}

FString Chess_Move::GetPieceLetter(AChessPiece* piece)
{
    if (const ACP_Bishop* tmp = Cast<ACP_Bishop>(piece)) {
        return FString("B");
    }
    if (const ACP_King* tmp = Cast<ACP_King>(piece)) {
        return FString("K");
    }
    if (const ACP_Knight* tmp = Cast<ACP_Knight>(piece)) {
        return FString("N");
    }
    if (const ACP_Pawn* tmp = Cast<ACP_Pawn>(piece)) {
        if (tmp->PromotedPiece)//if the pawn has been promoted return the letter of the promoted piece
        {
			return GetPieceLetter(tmp->PromotedPiece);
		}
        return FString("");
    }
    if (const ACP_Queen* tmp = Cast<ACP_Queen>(piece)) {
        return FString("Q");
    }
    if (const ACP_Rook* tmp = Cast<ACP_Rook>(piece)) {
        return FString("R");
    }
    return FString("");
}
